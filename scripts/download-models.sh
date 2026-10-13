#!/usr/bin/env bash
# Downloads the scanner's model files into a directory laid out the way the
# core library expects (<dir>/tessdata/*.traineddata, <dir>/*.gguf), and
# writes <dir>/manifest.json (name, size, sha256) for publishing them.
#
#   scripts/download-models.sh core/testdata/models   # for tests and receipts-cli
#
# Sources are pinned to exact revisions so every download is identical:
#   Tesseract "best" LSTM models, tessdata_best @ e12c65a (more accurate than
#   tessdata_fast on photos: 16/16 vs 14/16 on core/testdata/receipts)
#   EmbeddingGemma 2, Q8_0 GGUF (Apache 2.0), ggml-org/embeddinggemma-2-GGUF @ bfcd298
set -euo pipefail

DIR="${1:?usage: download-models.sh <directory>}"
TESSDATA_REV=e12c65a915945e4c28e237a9b52bc4a8f39a0cec
GGUF_REV=bfcd298762cc34d0357ece5ebdd31791a3a374d8
LANGUAGES=(eng deu rus)
GGUF=embeddinggemma-2-Q8_0.gguf

mkdir -p "$DIR/tessdata"
fetch() {  # $1 url, $2 destination; skips files that are already complete
  [[ -s "$2" ]] && return
  echo "downloading $(basename "$2")"
  curl --fail --location --retry 3 --continue-at - --output "$2.part" "$1"
  mv "$2.part" "$2"
}
for lang in "${LANGUAGES[@]}"; do
  fetch "https://github.com/tesseract-ocr/tessdata_best/raw/${TESSDATA_REV}/${lang}.traineddata" "$DIR/tessdata/${lang}.traineddata"
done
fetch "https://huggingface.co/ggml-org/embeddinggemma-2-GGUF/resolve/${GGUF_REV}/${GGUF}" "$DIR/${GGUF}"

# The manifest accounts-api serves at /v1/assets (see deploy/README.md, "Publish models").
sha() { shasum -a 256 "$1" | cut -d' ' -f1; }
size() { wc -c < "$1" | tr -d ' '; }
{
  echo "["
  first=true
  for file in "$DIR"/tessdata/*.traineddata "$DIR/${GGUF}"; do
    $first || echo ","
    first=false
    printf '  {"name": "%s", "size": %s, "sha256": "%s"}' "$(basename "$file")" "$(size "$file")" "$(sha "$file")"
  done
  echo; echo "]"
} > "$DIR/manifest.json"
echo "models ready in $DIR"
