# StreamSmith

StreamSmith is a C++ command-line application for downloading online media and converting local media files. The application is organized into a small menu-driven executable and reusable library components.

This README describes the C++ project in `main/` and `libs/`. The ignored `pythonImpl/` directory is not part of this documented build or usage.

## Features

- Download media from URLs supported by [yt-dlp](https://github.com/yt-dlp/yt-dlp).
- Inspect available video and audio formats before choosing a download.
- Download the highest available quality, a selected video or audio format, or a custom video/audio combination.
- Limit downloaded title filenames to 30 characters and restrict them to filenames without spaces.
- Inspect a local media file and convert it using FFmpeg.
- Select an output container and available audio/video codecs, choose compression settings, and retain or change video dimensions.

## Requirements

- CMake 3.16 or later
- A C++17 compiler
- FFmpeg development libraries and headers (libavformat, libavcodec, and libavutil)
- Network access during the initial CMake configure, which fetches `nlohmann/json`

### macOS with Homebrew

Install development dependencies with:

```sh
brew install cmake ffmpeg yt-dlp
```

The current CMake configuration looks for FFmpeg libraries and uses `/opt/homebrew/include` for headers, matching the default Apple Silicon Homebrew prefix. If your Homebrew installation is under a different prefix, update the include path in `libs/CMakeLists.txt` before building.

## Build

Run these commands from the repository root:

```sh
cmake -S . -B build
cmake --build build
```

The executable is created at:

```text
build/main/streamsmith
```

## Run

Start the interactive application from the repository root:

```sh
./build/main/streamsmith
```

Choose a product from the main menu:

- `1` — Downloader
- `2` — Converter
- `0` — Back
- `-1` — Exit

### Download media

1. Select the downloader.
2. Enter a download directory, or press Enter to use the current working directory.
3. Enter the media URL.
4. Choose a download mode:
   - **Highest quality** — downloads the best available video and audio, with MP4 as the merge format.
   - **Video only** — choose a listed video format ID.
   - **Audio only** — choose a listed audio format ID and extract it as MP3 at 192 Kbps.
   - **Custom** — choose video and audio format IDs and merge them into MKV.

The downloader uses yt-dlp, so available sites and formats depend on yt-dlp support and the source URL. Some sites may require authentication or may restrict access.

### Convert a local media file

1. Select the converter.
2. Enter the media directory, or press Enter to use the current working directory.
3. Enter the media filename within that directory.
4. Select an output container and the available video/audio codecs.
5. Enter compression settings. For video, the application maps the 0–100 compression input to an integer CRF value. Leave width or height blank to keep the corresponding source dimension.
6. The converted file is written to the selected media directory as `convertedFile.<extension>`.

The available container and codec combinations are defined by the converter implementation and may not be supported by every FFmpeg build. FFmpeg must include the selected encoders.

## Project layout

```text
main/                         C++ application entry point
libs/
  libMain/                    Main menu and product selection
  libDownloader/              yt-dlp download workflow
  libConverter/               FFmpeg media inspection and conversion
  libUtils/                    Shared input, path, and command utilities
```

## Standalone Distribution (Zero Prerequisites)

StreamSmith automatically resolves `ffmpeg` and `yt-dlp` executables placed in the same folder as the `streamsmith` binary without requiring them to be installed in the user's system `PATH`.

To create a local standalone bundle:
```sh
./scripts/bundle_local.sh
```

This creates a `dist/` directory containing the `streamsmith` executable alongside standalone copies of `ffmpeg` and `yt-dlp`. You can zip this folder and share it directly with users.

For cross-platform automated builds (Windows, Linux, macOS), the repository includes a GitHub Actions workflow in `.github/workflows/release.yml` that builds and bundles the release archives whenever a git release tag (e.g. `v1.0.0`) is pushed.

## Troubleshooting

- **CMake cannot find FFmpeg:** install FFmpeg and check that library and header locations are discoverable.
- **`yt-dlp` or `ffmpeg` cannot be found:** either place them in the same directory as the `streamsmith` executable or ensure they are accessible on the system `PATH`.
- **A download fails:** check the URL, network access, source-site restrictions, and yt-dlp's error output. Not every URL or format is downloadable.
- **A conversion fails:** check that the input file is readable and that the installed FFmpeg build supports the selected codecs and output container.

## Author & License

**Author:** Madusanka Jayarathna (2026)  
**License:** [MIT License](LICENSE)

### Third-Party Licenses

StreamSmith bundles or interacts with the following third-party software:
- **[yt-dlp](https://github.com/yt-dlp/yt-dlp):** Released under [The Unlicense](https://github.com/yt-dlp/yt-dlp/blob/master/LICENSE) (Public Domain).
- **[FFmpeg](https://ffmpeg.org/):** Licensed under the [LGPL v2.1+](https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html) or [GPL v2+/v3](https://www.gnu.org/licenses/gpl-3.0.html) depending on enabled build options.
- **[nlohmann/json](https://github.com/nlohmann/json):** Licensed under the [MIT License](https://github.com/nlohmann/json/blob/develop/LICENSE.MIT).
