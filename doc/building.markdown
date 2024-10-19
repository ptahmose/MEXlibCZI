# Building

## General Remarks

In this project, mex-files for MATLAB and Octave are built. Those mex-files are built using the respective
environments (i.e. a MATLAB or Octave installation, providing the required headers and libraries).   
Note that the actual binaries are specific to the MATLAB or Octave. I.e. it is not possible to use the mex-file
build for Octave with MATLAB, and vice versa.  
The mex-file for MATLAB is built with the CMake build system, while the mex-file for Octave is built from
within Octave itself.

* The actual implementation is found in a static library "lib". This does not directly call any mex-functions, but
   uses an interface.
* For MATLAB, the project MEXlibczi is a dynamic libary, linking to the MATLAB mex-library (and is providing an interface
   implementation with it) and the static library "lib". It is self-contained.
* For Octave, we build a dynamic library "libmexlibczi" which contains the static "lib" library. Then we build
   a mex-file "octavelibczi", which is merely a stub and at runtime loads the dynamic library "libmexlibczi".

This way, the code for MATLAB and Octave is shared, but the build process is different.

![Building overview](pictures/build_architecture.png)]

## CMake

Executing the following command in the root directory of the project will build the mex-file for MATLAB   
and the dynamic library for the Octave-mex-file:

```bash
mkdir build
cd build
cmake ..
cmake --build . -j
```

The CMake build system will try to located the MATLAB installation automatically. It is also possible to specify the paths for the folders containing the necessary headers and libraries
manually. This can be done by setting the CMake variables `MEXLIBCZI_HEADERS` and `MEXLIBCZI_LIBS`.

## Building the Octave mex-file

The Octave-mex-file is built from within Octave itself. The following commands are executed in the root directory of the project:

```bash
cd D:\dev\MyGitHub\MEXlibCZI\OctaveMex
mkoctfile --mex octavelibczi.c
```

This will create the mex-file "octavelibczi.mex" in the current directory. This mex-file is a stub and at runtime loads the dynamic library "libmexlibczi".  
For the Octave-mex-file to work, the dynamic library "libmexlibczi" must be present in the same directory. This library is built using CMake, and must be copied 
manually to the directory where the mex-file is located.
