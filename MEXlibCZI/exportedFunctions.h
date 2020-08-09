#pragma once

// if linking with the static wllczi-library, the variable "_WLLCZISTATICLIB" should be defined.
#if !defined(_WLLCZISTATICLIB)

#ifdef MEXlibCZI_EXPORTS
#ifdef __GNUC__
#define MEXLIBCZI_API __attribute__ ((visibility ("default")))
#else
#define MEXLIBCZI_API __declspec(dllexport)
#endif
#else
#ifdef __GNUC__
#define MEXLIBCZI_API
#else
#define MEXLIBCZI_API __declspec(dllimport)
#endif
#endif

#else

#define MEXLIBCZI_API 

#endif

#ifndef EXTERN_C
#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C extern
#endif
#endif



//EXTERN_C MEXLIBCZI_API void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);
