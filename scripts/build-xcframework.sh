#!/usr/bin/env bash
# Builds ReceiptsCore.xcframework (macOS arm64) for a SwiftUI app: the core
# library and every static dependency merged into one archive, plus the
# public headers in include/receipts. Swift calls it through C++ interop.
#
#   scripts/build-xcframework.sh        -> core/build/ReceiptsCore.xcframework
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD="$ROOT/core/build/release"
OUT="$ROOT/core/build/ReceiptsCore.xcframework"

[[ "$(uname)" == Darwin ]] || { echo "xcframeworks are built on macOS" >&2; exit 1; }
"$ROOT/scripts/bootstrap-vcpkg.sh" >/dev/null
cmake -S "$ROOT/core" --preset release -DRECEIPTS_BUILD_TESTS=OFF -DRECEIPTS_BUILD_CLI=OFF >/dev/null
cmake --build "$BUILD" --target receipts_core

# One self-contained archive: our code plus Tesseract, Leptonica, llama.cpp,
# ggml, curl, OpenSSL, SQLite and their image codecs.
libraries=("$BUILD/libreceipts_core.a")
while IFS= read -r library; do libraries+=("$library"); done \
  < <(find "$BUILD/vcpkg_installed/arm64-osx/lib" -maxdepth 1 -name '*.a' | sort)
libtool -static -o "$BUILD/libReceiptsCore.a" "${libraries[@]}" 2>/dev/null

rm -rf "$OUT"
xcodebuild -create-xcframework -library "$BUILD/libReceiptsCore.a" -headers "$ROOT/core/include" -output "$OUT" >/dev/null
echo "built $OUT"
