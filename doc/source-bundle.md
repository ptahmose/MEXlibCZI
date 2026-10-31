# Rebuilding a MEXlibCZI source bundle

The matching binary package's SOURCE.md identifies this archive and its SHA-256.
`project/` contains the tracked project at the release commit; `deps/` contains
the pinned libCZI, Eigen, Zstandard and RapidJSON sources used for that build.
RapidJSON is intentionally limited to its header directory and license text.
Zstandard includes its library sources, CMake build files and license/readme
files; unused CLI and test trees are excluded.
libCZI's embedded JPEG XR, pugixml and MD5-derived sources are included intact.

Install Python 3.11 or later, CMake and a C/C++ compiler compatible with the host
you intend to use. Those general-purpose tools are not included in the archive.

From this directory, run:

```text
python rebuild.py --test
```

This installs the bundled header-only Eigen into a local rebuild directory,
configures MEXlibCZI with local dependency paths and FetchContent disconnected,
builds the shared core/Octave companion and runs native tests. No MATLAB is
needed. No dependency is downloaded. The build is not bit-for-bit reproducible
unless the toolchain and external environment are also reproduced.

To also build a MATLAB MEX against your own native MATLAB installation:

```text
python rebuild.py --matlab-root /path/to/MATLAB --build
```

On Windows use a Visual Studio developer shell and the appropriate MATLAB path.
On Apple Silicon use a native ARM64 shell/toolchain. The release builds target
macOS 15 or later; set `MACOSX_DEPLOYMENT_TARGET=15.0` before running the helper
when matching that target. Build configuration details are in build-info.json.

To build the Octave stub, install a compatible Octave and follow
`project/doc/building.markdown`; keep the stub and rebuilt companion together.
MATLAB, Octave, their SDK/runtime dependencies and general-purpose compiler/OS
sources are not included. See `project/doc/legal-review.md` for licensing matters
that still require review. Complete source trees can contain licenses for files
not compiled into the binary; preserve their per-file notices.
