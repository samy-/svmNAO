include_directories(".")

qi_create_lib(kmeans
  SRC kmeans/kmeans.hpp
      kmeans/kmeans.cpp
      src/KCtree.cpp
      src/KCtree.h
      src/KCutil.cpp
      src/KCutil.h
      src/KM_ANN.cpp
      src/KM_ANN.h
      src/KMcenters.cpp
      src/KMcenters.h
      src/KMdata.cpp
      src/KMdata.h
      src/KMeans.cpp
      src/KMeans.h
      src/KMfilterCenters.cpp
      src/KMfilterCenters.h
      src/KMlocal.cpp
      src/KMlocal.h
      src/KMrand.cpp
      src/KMrand.h
      src/KMterm.cpp
      src/KMterm.h	
  SHARED
)

qi_use_lib(kmeans)

qi_install_header(KEEP_RELATIVE_PATHS kmeans/kmeans.hpp)

qi_stage_lib(kmeans)

#add_subdirectory(test)
