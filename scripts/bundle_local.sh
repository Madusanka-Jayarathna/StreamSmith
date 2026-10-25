#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
DIST_DIR="${ROOT_DIR}/dist"

echo "==> Building StreamSmith..."
cmake -S "${ROOT_DIR}" -B "${ROOT_DIR}/build" -DCMAKE_BUILD_TYPE=Release
cmake --build "${ROOT_DIR}/build"

echo "==> Creating distribution folder at ${DIST_DIR}..."
rm -rf "${DIST_DIR}"
mkdir -p "${DIST_DIR}"

cp "${ROOT_DIR}/build/main/streamsmith" "${DIST_DIR}/streamsmith"

# Fetch or copy yt-dlp
if command -v yt-dlp >/dev/null 2>&1; then
    echo "==> Bundling yt-dlp from $(which yt-dlp)..."
    cp "$(which yt-dlp)" "${DIST_DIR}/yt-dlp"
    chmod +x "${DIST_DIR}/yt-dlp"
else
    echo "==> Downloading standalone yt-dlp..."
    OS="$(uname -s)"
    if [ "$OS" = "Darwin" ]; then
        curl -L -o "${DIST_DIR}/yt-dlp" https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp_macos || true
    else
        curl -L -o "${DIST_DIR}/yt-dlp" https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp || true
    fi
    if [ -f "${DIST_DIR}/yt-dlp" ]; then
        chmod +x "${DIST_DIR}/yt-dlp"
    else
        echo "WARNING: Could not download yt-dlp. Please place a yt-dlp binary in ${DIST_DIR}/"
    fi
fi

# Locate or copy FFmpeg
if command -v ffmpeg >/dev/null 2>&1; then
    echo "==> Bundling ffmpeg from $(which ffmpeg)..."
    cp "$(which ffmpeg)" "${DIST_DIR}/ffmpeg"
    chmod +x "${DIST_DIR}/ffmpeg"
else
    echo "WARNING: ffmpeg not found on PATH. Please place a static ffmpeg executable in ${DIST_DIR}/"
fi

echo ""
echo "=========================================================="
echo " Standalone bundle created successfully in:"
echo "   ${DIST_DIR}"
echo ""
echo " Contents:"
ls -lh "${DIST_DIR}"
echo "=========================================================="
