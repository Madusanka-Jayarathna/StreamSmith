# StreamSmith

StreamSmith is a lightweight command-line application for downloading online media and converting local video/audio files with fine-grained codec and quality controls.

---

## Quick Start (For End Users - Zero Prerequisites)

No installation or external tools (like Python or FFmpeg) are required. Pre-built standalone bundles include everything needed.

### 1. Download
Download the latest archive for your operating system from the **[Releases](https://github.com/)** page:
- **Windows:** `streamsmith-windows-x64.zip`
- **macOS:** `streamsmith-macos.zip`
- **Linux:** `streamsmith-linux-x64.tar.gz`

### 2. Extract and Run

#### Windows
1. Extract `streamsmith-windows-x64.zip`.
2. Open PowerShell or Command Prompt inside the extracted folder.
3. Run:
   ```powershell
   .\streamsmith.exe
   ```

#### macOS
1. Extract `streamsmith-macos.zip`.
2. Open Terminal and navigate to the extracted folder:
   ```sh
   cd path/to/streamsmith-macos
   ```
3. (First time only) Allow the standalone binaries if prompted by macOS Gatekeeper:
   ```sh
   xattr -d com.apple.quarantine streamsmith ffmpeg yt-dlp 2>/dev/null || true
   chmod +x streamsmith ffmpeg yt-dlp
   ```
4. Run:
   ```sh
   ./streamsmith
   ```

#### Linux
1. Extract the archive and enter the folder:
   ```sh
   tar -xzvf streamsmith-linux-x64.tar.gz
   cd streamsmith-linux-x64
   ```
2. Make sure binaries are executable and run:
   ```sh
   chmod +x streamsmith ffmpeg yt-dlp
   ./streamsmith
   ```

---

## Features & Usage

Run `streamsmith` and choose an option from the menu:

- `1` — **Downloader**
  - Download media from any URL supported by yt-dlp.
  - Choose between Highest Quality, Video Only, Audio Only (extracted to MP3), or Custom stream combinations.
- `2` — **Converter**
  - Inspect existing local media container, codecs, dimensions, and bitrate.
  - Convert between formats (`MKV`, `MP4`, `MP3`, `AAC`, `M4A`).
  - Choose video codecs (`H.264`, `HEVC`, `AV1`, `VP9`, `MPEG2`) and audio codecs (`AAC`, `MP3`, `Opus`).
  - Adjust compression (CRF) and scale video dimensions.
- `0` — **Back**
- `-1` — **Exit**

---

## Building from Source (For Developers)

If you wish to compile StreamSmith from source code rather than using pre-built binaries:

### Build Requirements
- CMake 3.16 or later
- A C++17 compatible compiler (Clang, GCC, or MSVC)
- FFmpeg development libraries (`libavformat`, `libavcodec`, `libavutil`)

### Build Steps

```sh
# Configure and build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

The compiled binary will be located at:
- **Linux / macOS:** `build/main/streamsmith`
- **Windows:** `build/main/Release/streamsmith.exe`

### Creating a Local Standalone Bundle
To bundle the compiled binary together with standalone copies of `ffmpeg` and `yt-dlp`:
```sh
./scripts/bundle_local.sh
```
This generates a self-contained `dist/` directory ready for distribution.

---

## Author & License

**Author:** Madusanka Jayarathna (2026)  
**License:** [MIT License](LICENSE)

### Third-Party Licenses
- **[yt-dlp](https://github.com/yt-dlp/yt-dlp):** Released under [The Unlicense](https://github.com/yt-dlp/yt-dlp/blob/master/LICENSE) (Public Domain).
- **[FFmpeg](https://ffmpeg.org/):** Licensed under the [LGPL v2.1+](https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html) or [GPL v2+/v3](https://www.gnu.org/licenses/gpl-3.0.html).
- **[nlohmann/json](https://github.com/nlohmann/json):** Licensed under the [MIT License](https://github.com/nlohmann/json/blob/develop/LICENSE.MIT).
