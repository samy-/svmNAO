cmake_minimum_required(VERSION 2.8)
project(lib_1)

find_package(qibuild)

set(_srcs
  main.cpp
  lib_1.h
  lib_1.cpp
  svm-train.h
  svm-predict.h
  svm-train.c
  svm-predict.c
  svm.h
  svm.cpp
)

qi_create_lib(lib_1 SHARED ${_srcs} SUBFOLDER naoqi)

qi_use_lib(lib_1 ALCOMMON) # BOOST)
