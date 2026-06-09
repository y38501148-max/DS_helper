#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${WINDOWS_BUILD_DIR:-/tmp/ds-helper-win-build}"

rm -rf "${BUILD_DIR}"
mkdir -p "${BUILD_DIR}"

rsync -a --delete \
  --exclude node_modules \
  --exclude dist \
  --exclude build \
  --exclude src-tauri/target \
  --exclude .git \
  "${ROOT_DIR}/" "${BUILD_DIR}/"

cd "${BUILD_DIR}"
npm ci
npm run tauri -- build --bundles nsis --target x86_64-pc-windows-gnu

mkdir -p "${ROOT_DIR}/src-tauri/target/x86_64-pc-windows-gnu/release/bundle/nsis"
rsync -a \
  "${BUILD_DIR}/src-tauri/target/x86_64-pc-windows-gnu/release/bundle/nsis/" \
  "${ROOT_DIR}/src-tauri/target/x86_64-pc-windows-gnu/release/bundle/nsis/"
