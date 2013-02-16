# Autogenerated file, run build/tools/setup_cmake.py to regenerate

if(NOT DEFINED RMF_LIBRARIES)
message(STATUS "Checking for RMF")

include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(RMF_PKGCONF RMF)

# Include dir
find_path(RMF_INCLUDE_DIR
  NAMES RMF/FileHandle.h
  PATHS ${RMF_PKGCONF_INCLUDE_DIRS}
)

# Finally the library itself
find_library(RMF_LIBRARY
  NAMES RMF
  PATHS ${RMF_PKGCONF_LIBRARY_DIRS}
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(RMF_PROCESS_INCLUDES RMF_INCLUDE_DIR)
set(RMF_PROCESS_LIBS RMF_LIBRARY)
libfind_process(RMF)

if (${RMF_LIBRARY} MATCHES "RMF_LIBRARY-NOTFOUND"
    OR ${RMF_INCLUDE_DIR} MATCHES "RMF_INCLUDE_DIR-NOTFOUND")
  message(STATUS "RMF not found")
  set(RMF_INTERNAL 1 CACHE INTERNAL "" FORCE)
        message(STATUS "Building internal RMF")

set(RMF_BINARY_DIR ${PROJECT_BINARY_DIR}/src/dependency/RMF)

add_subdirectory(${PROJECT_SOURCE_DIR}/modules/rmf/dependency/RMF ${RMF_BINARY_DIR})

file(WRITE "${PROJECT_BINARY_DIR}/data/build_info/RMF" "ok=True")

set(RMF_INCLUDE_PATH ${PROJECT_SOURCE_DIR}/modules/rmf/dependency/RMF/include ${RMF_BINARY_DIR}/include  CACHE INTERNAL "" FORCE)
set(RMF_SWIG_PATH ${PROJECT_SOURCE_DIR}/modules/rmf/dependency/RMF/swig CACHE INTERNAL "" FORCE)
set(RMF_LIBRARIES RMF CACHE INTERNAL "" FORCE)
file(WRITE "${PROJECT_BINARY_DIR}/data/build_info/RMF" "ok=True")
else()
  message(STATUS "RMF found " ${RMF_INCLUDE_DIR} " " ${RMF_LIBRARY})
  file(WRITE "${PROJECT_BINARY_DIR}/data/build_info/RMF" "ok=True")
  #set(RMF_LINK_PATH ${RMF_LIBRARY_DIRS} CACHE INTERNAL ""  FORCE)
  set(RMF_INCLUDE_PATH ${RMF_INCLUDE_DIR} CACHE INTERNAL "" FORCE)
  set(RMF_LIBRARIES ${RMF_LIBRARY} CACHE INTERNAL "" FORCE)
endif()

else()
message(STATUS "RMF already setup")
if(DEFINED RMF_INTERNAL)
message(STATUS "Building internal RMF")

set(RMF_BINARY_DIR ${PROJECT_BINARY_DIR}/src/dependency/RMF)

add_subdirectory(${PROJECT_SOURCE_DIR}/modules/rmf/dependency/RMF ${RMF_BINARY_DIR})

file(WRITE "${PROJECT_BINARY_DIR}/data/build_info/RMF" "ok=True")

set(RMF_INCLUDE_PATH ${PROJECT_SOURCE_DIR}/modules/rmf/dependency/RMF/include ${RMF_BINARY_DIR}/include  CACHE INTERNAL "" FORCE)
set(RMF_SWIG_PATH ${PROJECT_SOURCE_DIR}/modules/rmf/dependency/RMF/swig CACHE INTERNAL "" FORCE)
set(RMF_LIBRARIES RMF CACHE INTERNAL "" FORCE)
endif(DEFINED RMF_INTERNAL)
endif(NOT DEFINED RMF_LIBRARIES)