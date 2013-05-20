#ifndef KMEANS_KMEANS_H
#define KMEANS_KMEANS_H
#include<src/KMlocal.h>
#include<src/KM_ANN.h>
#include <cstdlib>			// C standard includes
#include <iostream>			// C++ I/O
#include <string.h>			// C++ strings
#include <fstream>
// For shared library
#if defined _WIN32 || defined __CYGWIN__
#  define _EXPORT_API __declspec(dllexport)
#  if defined _WINDLL
#    define _IMPORT_API __declspec(dllimport)
#  else
#    define _IMPORT_API
#  endif
#elif __GNUC__ >= 4
#  define _EXPORT_API __attribute__ ((visibility("default")))
#  define _IMPORT_API __attribute__ ((visibility("default")))
#else
#  define _EXPORT_API
#  define _IMPORT_API
#endif


//kmeans_EXPORTS controls which symbols are exported when libqi
// is compiled as a SHARED lib.
// DO NOT USE OUTSIDE libkmeans, and make sure the prefix
// of _EXPORTS define matches the CMake target name
#ifdef kmeans_EXPORTS
# define KMEANS_API _EXPORT_API
#elif defined(kmeans_IMPORTS)
# define KMEANS_API _IMPORT_API
#else
# define KMEANS_API
#endif

KMEANS_API int  nao_kmeans();
KMEANS_API void getArgs();	// get command-line arguments
KMEANS_API void printSummary(const KMlocal& theAlg,const KMdata& dataPts,KMfilterCenters& ctrs);
KMEANS_API bool readPt(istream& in,KMpoint& p);
KMEANS_API void laptevParser(istream& in, const KMdata& d, int *nPts);
KMEANS_API void trainingMeansParser(istream& in,const KMfilterCenters& ctrs);
KMEANS_API void printPt(ostream& out,const KMpoint& p);



#endif
