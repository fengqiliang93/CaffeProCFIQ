cmake_minimum_required(VERSION 3.16)

project(CaffeProCFIQ LANGUAGES CXX)

set(CFIQ_TARGET_ARCH "" CACHE STRING "Target architecture: x86_64 or aarch64")
set_property(CACHE CFIQ_TARGET_ARCH PROPERTY STRINGS x86_64 aarch64)

if(NOT CFIQ_TARGET_ARCH)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64)$")
        set(CFIQ_TARGET_ARCH aarch64 CACHE STRING "Target architecture" FORCE)
    else()
        set(CFIQ_TARGET_ARCH x86_64 CACHE STRING "Target architecture" FORCE)
    endif()
endif()

set(CFIQ_CPU_ONLY ON CACHE BOOL "Build CPU-only Caffe runtime")
set(CFIQ_BUILD_TEST ON CACHE BOOL "Build FingerQualityCtrlTest")
set(CFIQ_STAGE_RUNTIME ON CACHE BOOL "Enable runtime staging target")

set(CFIQ_DEFAULT_DEPS_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/deps/${CFIQ_TARGET_ARCH}")

set(CFIQ_DEPS_ROOT "${CFIQ_DEFAULT_DEPS_ROOT}" CACHE PATH "Dependency root for the selected architecture")
set(CFIQ_STAGE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/out/${CFIQ_TARGET_ARCH}-cmake" CACHE PATH "Runtime staging directory")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

find_package(Threads REQUIRED)

set(CFIQ_INCLUDE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(CFIQ_PROTO_INCLUDE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/include/caffe/proto")
set(CFIQ_DEPS_INCLUDE_ROOT "${CFIQ_DEPS_ROOT}/include")
set(CFIQ_PUBLIC_INCLUDE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/include_cfiq")
set(CFIQ_THIRDPART_LIB_DIR "${CFIQ_DEPS_ROOT}/thirdpart_lib")
set(CFIQ_OPENCV_LIB_DIR "${CFIQ_DEPS_ROOT}/opencv_lib")

set(CFIQ_PROTOBUF_LIB "${CFIQ_THIRDPART_LIB_DIR}/libprotobuf.so.26.0.6" CACHE FILEPATH "protobuf shared library")
set(CFIQ_GFORTRAN_LIB "${CFIQ_THIRDPART_LIB_DIR}/libgfortran.so" CACHE FILEPATH "libgfortran shared library")
set(CFIQ_OPENCV_CORE_LIB "${CFIQ_OPENCV_LIB_DIR}/libopencv_core.so.3.2" CACHE FILEPATH "OpenCV core library")
set(CFIQ_OPENCV_IMGCODECS_LIB "${CFIQ_OPENCV_LIB_DIR}/libopencv_imgcodecs.so.3.2" CACHE FILEPATH "OpenCV imgcodecs library")
set(CFIQ_OPENCV_IMGPROC_LIB "${CFIQ_OPENCV_LIB_DIR}/libopencv_imgproc.so.3.2" CACHE FILEPATH "OpenCV imgproc library")

set(CFIQ_NIRVANA_KERNEL_LIB "${CFIQ_DEPS_ROOT}/nirvana_lib/libnirvana-kernel.so.8.15.1" CACHE FILEPATH "nirvana kernel library")

if(CFIQ_TARGET_ARCH STREQUAL "aarch64")
    set(CFIQ_MATH_LIBS
        -L/usr/lib/aarch64-linux-gnu/openblas-pthread -lopenblas
        -L/usr/lib/aarch64-linux-gnu/blas -lblas
        -L/usr/lib/aarch64-linux-gnu/lapack -llapack
    )
    set(CFIQ_GFORTRAN_LIB "/usr/lib/aarch64-linux-gnu/libgfortran.so.5" CACHE FILEPATH "libgfortran shared library" FORCE)
else()
    set(CFIQ_MATH_LIBS
        "${CFIQ_THIRDPART_LIB_DIR}/cblas_LINUX.a"
        "${CFIQ_THIRDPART_LIB_DIR}/blas_LINUX.a"
    )
endif()

foreach(required_file
        "${CFIQ_PROTOBUF_LIB}"
        "${CFIQ_OPENCV_CORE_LIB}"
        "${CFIQ_OPENCV_IMGCODECS_LIB}"
        "${CFIQ_OPENCV_IMGPROC_LIB}"
        "${CFIQ_NIRVANA_KERNEL_LIB}")
    if(NOT EXISTS "${required_file}")
        message(FATAL_ERROR "Missing required dependency for ${CFIQ_TARGET_ARCH}: ${required_file}")
    endif()
endforeach()

file(GLOB CFIQ_CAFFE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/caffe/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/caffe/layers/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/caffe/util/*.cpp"
)
file(GLOB CFIQ_RUNTIME_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src_cfiq/*.cpp"
)

add_library(QualityCtrl SHARED
    "${CMAKE_CURRENT_SOURCE_DIR}/src/caffe.pb.cc"
    ${CFIQ_CAFFE_SOURCES}
    ${CFIQ_RUNTIME_SOURCES}
)
set_target_properties(QualityCtrl PROPERTIES
    OUTPUT_NAME "QualityCtrl"
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "$ORIGIN"
)
target_include_directories(QualityCtrl PRIVATE
    "${CFIQ_PROTO_INCLUDE_ROOT}"
    "${CFIQ_INCLUDE_ROOT}"
    "${CFIQ_PUBLIC_INCLUDE_ROOT}"
    "${CFIQ_DEPS_INCLUDE_ROOT}"
)
if(CFIQ_CPU_ONLY)
    target_compile_definitions(QualityCtrl PRIVATE CPU_ONLY)
else()
    target_compile_definitions(QualityCtrl PRIVATE USE_CUDNN)
endif()
target_link_libraries(QualityCtrl PRIVATE
    ${CFIQ_MATH_LIBS}
    Threads::Threads
    "${CFIQ_PROTOBUF_LIB}"
    "${CFIQ_OPENCV_CORE_LIB}"
    "${CFIQ_OPENCV_IMGCODECS_LIB}"
    "${CFIQ_OPENCV_IMGPROC_LIB}"
    "${CFIQ_GFORTRAN_LIB}"
    dl
)

if(CFIQ_BUILD_TEST)
    add_executable(FingerQualityCtrlTest
        "${CMAKE_CURRENT_SOURCE_DIR}/test_cfiq/src/main.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/test_cfiq/src/ExtractMnt.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/test_cfiq/src/ScanNSortDirectory.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/test_cfiq/src/MakeBmp.cpp"
    )
    set_target_properties(FingerQualityCtrlTest PROPERTIES
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_RPATH "$ORIGIN;$ORIGIN/../lib"
    )
    target_include_directories(FingerQualityCtrlTest PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/test_cfiq/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/test_cfiq/include/nirvana"
    )
    target_link_libraries(FingerQualityCtrlTest PRIVATE
        "${CFIQ_NIRVANA_KERNEL_LIB}"
        QualityCtrl
        "${CFIQ_GFORTRAN_LIB}"
        ${CFIQ_MATH_LIBS}
        "${CFIQ_PROTOBUF_LIB}"
        "${CFIQ_OPENCV_CORE_LIB}"
        "${CFIQ_OPENCV_IMGPROC_LIB}"
        "${CFIQ_OPENCV_IMGCODECS_LIB}"
    )
    if(CFIQ_TARGET_ARCH STREQUAL "aarch64")
        target_link_options(FingerQualityCtrlTest PRIVATE
            -Wl,--allow-shlib-undefined
            -Wl,-rpath-link,${CFIQ_STAGE_DIR}/lib
            -Wl,-rpath-link,${CFIQ_THIRDPART_LIB_DIR}
            -Wl,-rpath-link,${CFIQ_OPENCV_LIB_DIR}
        )
    endif()
endif()

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/StageRuntime.cmake")
cfiq_configure_stage_runtime()

message(STATUS "CFIQ_TARGET_ARCH=${CFIQ_TARGET_ARCH}")
message(STATUS "CFIQ_DEPS_ROOT=${CFIQ_DEPS_ROOT}")
message(STATUS "CFIQ_STAGE_DIR=${CFIQ_STAGE_DIR}")
