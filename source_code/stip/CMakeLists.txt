cmake_minimum_required(VERSION 2.8)
project(stip)

find_package(qibuild)

add_subdirectory(libKmeans)

set(_srcs
  main.cpp
  stip.h
  stip.cpp
  pcheader.cpp
  pcheader.h
  cvutil.cpp
  cvutil.h
  harrisbuffer.h
  harrisbuffer.cpp
  msharrisbuffer.h
  msharrisbuffer.cpp
  cmdline.h
  cmdline.cpp
  stbuffer.cpp
  stbuffer.h
  

)

qi_create_lib(stip SHARED ${_srcs} SUBFOLDER naoqi)

qi_use_lib(stip ALCOMMON kmeans OPENCV2_IMGPROC OPENCV2_HIGHGUI OPENCV2_VIDEO)#OPENCV2_CORE OPENCV2_HIGHGUI) # BOOST)
