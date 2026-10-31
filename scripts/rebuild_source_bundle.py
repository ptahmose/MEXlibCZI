#!/usr/bin/env python3
"""Configure and optionally build the project using only bundled dependencies."""
import argparse
from pathlib import Path
import subprocess


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--matlab-root", type=Path)
    parser.add_argument("--build", action="store_true")
    parser.add_argument("--test", action="store_true")
    args = parser.parse_args()
    root = Path(__file__).resolve().parent
    build = root / "rebuild"
    eigen_install = build / "eigen-install"
    subprocess.run(["cmake", "-S", str(root / "deps/eigen"), "-B", str(build / "eigen"),
                    "-DBUILD_TESTING=OFF", "-DEIGEN_BUILD_DOC=OFF", "-DCMAKE_POLICY_VERSION_MINIMUM=3.5",
                    "-DCMAKE_INSTALL_PREFIX=" + str(eigen_install)], check=True)
    subprocess.run(["cmake", "--build", str(build / "eigen"), "--target", "install", "--config", "Release"], check=True)
    command = ["cmake", "-S", str(root / "project"), "-B", str(build / "project"),
               "-DCMAKE_BUILD_TYPE=Release", "-DMEXLIBCZI_BUILD_HELPER_TESTS=ON",
               "-DFETCHCONTENT_FULLY_DISCONNECTED=ON",
               "-DFETCHCONTENT_SOURCE_DIR_LIBCZI=" + str(root / "deps/libczi"),
               "-DFETCHCONTENT_SOURCE_DIR_ZSTD=" + str(root / "deps/zstd"),
               "-DMEXLIBCZI_RAPIDJSON_SOURCE_DIR=" + str(root / "deps/rapidjson"),
               "-DLIBCZI_BUILD_PREFER_EXTERNALPACKAGE_EIGEN3=ON",
               "-DEigen3_DIR=" + str(eigen_install / "share/eigen3/cmake")]
    if args.matlab_root:
        command.append("-DMatlab_ROOT_DIR=" + str(args.matlab_root.resolve()))
    else:
        command.append("-DMEXLIBCZI_BUILD_MATLABMEX=OFF")
    subprocess.run(command, check=True)
    if args.build or args.test:
        subprocess.run(["cmake", "--build", str(build / "project"), "--config", "Release", "--parallel", "2"], check=True)
    if args.test:
        subprocess.run(["ctest", "--test-dir", str(build / "project"), "-C", "Release", "--output-on-failure", "--no-tests=error"], check=True)


if __name__ == "__main__":
    main()
