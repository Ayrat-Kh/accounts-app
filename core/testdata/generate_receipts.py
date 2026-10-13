#!/usr/bin/env python3
"""Renders the synthetic receipt photos in testdata/receipts/ used by ScanAccuracyTest.

Real receipts contain personal data, so the test set is generated: realistic
layouts in English, German and Russian, printed in a monospace font on
slightly grey paper, then rotated, blurred and noised a little like a phone
photo. Some receipts label the total with words that keyword matching misses
("Betrag", "Amount payable") to show what the embedding model adds.

    python3 -m venv .venv && .venv/bin/pip install pillow==11.3.0
    .venv/bin/python core/testdata/generate_receipts.py [path/to/monospace.ttf]

Writes receipts/*.jpg and receipts/expected.json (file -> total). The output
is committed, so CI never needs Pillow or fonts.
"""

import json
import random
import sys
from pathlib import Path

from PIL import Image, ImageDraw, ImageFilter, ImageFont

HERE = Path(__file__).resolve().parent
OUT = HERE / "receipts"
FONT_CANDIDATES = [
    "/System/Library/Fonts/Menlo.ttc",
    "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf",
]

# (file name, lines, expected total in minor units, printed currency or None)
RECEIPTS = [
    ("de_rewe", ["REWE Markt GmbH", "Hauptstr. 12, Berlin", "", "Milch 3,5%         1,29", "Vollkornbrot       2,49",
                 "Bananen            1,99", "-----------------------", "SUMME EUR          5,77", "",
                 "Geg. Bar          10,00", "Rueckgeld          4,23", "MwSt 7%            0,38", "13.10.2026 18:42"],
     577, "EUR"),
    ("de_betrag", ["Baeckerei Schmidt", "", "2 x Broetchen      0,90", "Kaffee             2,80",
                   "Zimtschnecke       2,40", "", "BETRAG EUR         6,10", "Kartenzahlung      6,10",
                   "Datum: 02.10.2026"], 610, "EUR"),
    ("de_zu_bezahlen", ["Drogerie Mueller", "Filiale 0412", "Zahnpasta          2,95", "Shampoo            3,45",
                        "Taschentuecher     1,15", "Zwischensumme      7,55", "Rabatt            -0,75",
                        "Zu bezahlen        6,80", "EC-Karte           6,80", "05.10.2026"], 680, None),
    ("de_gesamt", ["Edeka Center", "Aepfel 1kg         2,99", "Kaese              4,49", "Wein              12,99",
                   "Gesamtbetrag      20,47", "Gegeben           50,00", "Zurueck           29,53",
                   "Netto             17,61", "11.10.2026"], 2047, None),
    ("en_total", ["CORNER STORE", "123 Main St", "", "Coffee              3.50", "Bagel               2.25",
                  "Orange juice        4.10", "SUBTOTAL            9.85", "TAX 8%              0.79",
                  "TOTAL              10.64", "CASH               20.00", "CHANGE              9.36",
                  "10/07/2026"], 1064, None),
    ("en_amount_due", ["City Pharmacy", "Vitamins           12.99", "Plasters            4.49",
                       "Sub-total          17.48", "VAT 20%             2.91", "Amount due         17.48",
                       "Card               17.48", "2026-10-09 14:02"], 1748, None),
    ("en_amount_payable", ["GREEN GROCER", "Tomatoes            2.40", "Basil               1.20",
                           "Mozzarella          3.30", "Olive oil           8.90", "Amount payable     15.80",
                           "Paid by card       15.80", "Thank you!", "01/10/2026"], 1580, None),
    ("en_grand_total", ["THE BURGER BAR", "Burger x2          23.00", "Fries               4.50",
                        "Soda x2             6.00", "Subtotal           33.50", "Service 10%         3.35",
                        "Grand total        36.85", "Tip                 5.00", "Visa               41.85",
                        "12/10/2026"], 3685, None),
    ("en_balance", ["HARDWARE DEPOT", "Screws (100)        6.99", "Hammer             14.49",
                    "Tape                3.25", "Balance due        24.73", "Debit              24.73",
                    "Store #18 2026-10-03"], 2473, None),
    ("ru_itogo", ["ПЯТЁРОЧКА", "ООО Агроторг", "Хлеб               45,90", "Молоко             89,99",
                  "Сыр               349,00", "ИТОГО             484,89", "НАЛИЧНЫМИ         500,00",
                  "СДАЧА              15,11", "НДС 10%            44,08", "13.10.2026 09:15"], 48489, None),
    ("ru_k_oplate", ["ВКУСВИЛЛ", "Творог             129,00", "Кефир               99,00",
                     "Яблоки             159,90", "Скидка             -20,00", "К ОПЛАТЕ           367,90",
                     "Безналичными       367,90", "04.10.2026"], 36790, None),
    ("ru_vsego", ["АПТЕКА 36,6", "Аспирин           215,00", "Пластырь           64,50",
                  "Витамины          520,00", "ВСЕГО             799,50", "Получено          1000,00",
                  "Сдача             200,50", "08.10.2026"], 79950, None),
    ("ru_summa", ["КОФЕЙНЯ ЗЁРНА", "Капучино          210,00", "Круассан          180,00",
                  "Сумма покупки     390,00", "Оплата картой     390,00", "06.10.2026 08:31"], 39000, None),
    ("de_big", ["Elektro Markt", "Kopfhoerer       129,00", "Ladekabel         19,99",
                "Garantie          24,99", "SUMME EUR        173,98", "Kreditkarte      173,98",
                "inkl. MwSt 19%    27,78", "07.10.2026"], 17398, "EUR"),
    ("en_restaurant", ["LA TRATTORIA", "Table 7", "Pizza Margherita   11.50", "Lasagne            13.90",
                       "Tiramisu            6.50", "Water               3.00", "TOTAL EUR          34.90",
                       "Tip                 3.10", "Paid               38.00", "29/09/2026"], 3490, "EUR"),
    ("de_tankstelle", ["ARAL Tankstelle", "Super E10 41,2 l   73,73", "Snickers           1,49",
                       "Summe              75,22", "Girocard           75,22", "Netto              63,21",
                       "MwSt 19%           12,01", "10.10.2026"], 7522, None),
]


def find_font(argument):
    for candidate in ([argument] if argument else []) + FONT_CANDIDATES:
        if candidate and Path(candidate).exists():
            return candidate
    sys.exit("no monospace font found; pass one: generate_receipts.py /path/to/font.ttf")


def render(lines, font, rng):
    line_height = 34
    paper = Image.new("L", (560, 60 + line_height * len(lines)), color=rng.randint(232, 248))
    draw = ImageDraw.Draw(paper)
    for i, line in enumerate(lines):
        draw.text((24, 30 + i * line_height), line, fill=rng.randint(20, 60), font=font)

    # A phone photo: paper on a darker table, slightly rotated, a bit soft and noisy.
    photo = Image.new("L", (paper.width + 120, paper.height + 120), color=rng.randint(70, 110))
    photo.paste(paper, (60, 60))
    photo = photo.rotate(rng.uniform(-2.5, 2.5), resample=Image.BICUBIC, fillcolor=90)
    photo = photo.filter(ImageFilter.GaussianBlur(radius=rng.uniform(0.3, 0.8)))
    noise = Image.effect_noise(photo.size, rng.uniform(6, 12))
    return Image.blend(photo, noise, 0.08)


def main():
    rng = random.Random(42)  # deterministic output
    font = ImageFont.truetype(find_font(sys.argv[1] if len(sys.argv) > 1 else None), 24)
    OUT.mkdir(exist_ok=True)
    expected = {}
    for name, lines, amount_minor, currency in RECEIPTS:
        render(lines, font, rng).save(OUT / f"{name}.jpg", quality=85)
        expected[f"{name}.jpg"] = {"amount_minor": amount_minor, "currency": currency}  # None: not printed
    (OUT / "expected.json").write_text(json.dumps(expected, indent=2, ensure_ascii=False) + "\n")
    print(f"wrote {len(RECEIPTS)} receipts to {OUT}")


if __name__ == "__main__":
    main()
