cmake_minimum_required(VERSION 3.15)
project(CAPSULE_cpp)

set(CMAKE_CXX_STANDARD 17)

add_executable(SURF_SRP main.cpp Extractor.cpp Extractor.h srpHash.cpp srpHash.h Reservoir.h Reservoir.cpp LSH.h LSH.cpp)

OPTION(USE_OPENMP "Use OpenMP to enable <omp.h>" ON)

set("OpenCV_DIR" "/Users/henryzjy/Desktop/Projects/OpenCV/build_nonfree_opencv")
set(CMAKE_C_COMPILER /usr/local/opt/llvm/bin/clang)
set(CMAKE_CXX_COMPILER /usr/local/opt/llvm/bin/clang++)
set(OPENMP_LIBRARIES "/usr/local/Cellar/llvm/10.0.0_3/lib")
set(OPENMP_INCLUDES "/usr/local/opt/llvm/include")

include_directories("/usr/local/include" "/usr/local/opt/llvm/include")
link_directories("/usr/local/lib" "/usr/local/opt/llvm/lib")
set(CMAKE_LIBRARY_PATH "/usr/local/opt/llvm/lib")
# Find OpenMP
if(APPLE AND USE_OPENMP)
    if(CMAKE_C_COMPILER_ID MATCHES "Clang")
        set(OpenMP_C "${CMAKE_C_COMPILER}")
        set(OpenMP_C_FLAGS "-fopenmp=libomp -Wno-unused-command-line-argument")
        set(OpenMP_C_LIB_NAMES "libomp" "libgomp" "libiomp5")
        set(OpenMP_libomp_LIBRARY ${OpenMP_C_LIB_NAMES})
        set(OpenMP_libgomp_LIBRARY ${OpenMP_C_LIB_NAMES})
        set(OpenMP_libiomp5_LIBRARY ${OpenMP_C_LIB_NAMES})
#        set(OpenMP_lomp_LIBRARY ${OpenMP_C_LIB_NAMES})
    endif()
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(OpenMP_CXX "${CMAKE_CXX_COMPILER}")
        set(OpenMP_CXX_FLAGS "-fopenmp=libomp -Wno-unused-command-line-argument")
        set(OpenMP_CXX_LIB_NAMES "libomp" "libgomp" "libiomp5")
        set(OpenMP_libomp_LIBRARY ${OpenMP_CXX_LIB_NAMES})
        set(OpenMP_libgomp_LIBRARY ${OpenMP_CXX_LIB_NAMES})
        set(OpenMP_libiomp5_LIBRARY ${OpenMP_CXX_LIB_NAMES})
#        set(OpenMP_lomp_LIBRARY ${OpenMP_CXX_LIB_NAMES})
    endif()
endif()

if(USE_OPENMP)
    find_package(OPENMP REQUIRED)
endif(USE_OPENMP)

if (OPENMP_FOUND)
    message("FOUND OPENMP")
    include_directories("${OPENMP_INCLUDES}")
    link_directories("${OPENMP_LIBRARIES}")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    # set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif(OPENMP_FOUND)


set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lpthread")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")



find_package(OpenCV REQUIRED)
if (NOT OpenCV_FOUND)
    MESSAGE(FATAL_ERROR "OpenCV not found")
endif (NOT OpenCV_FOUND)

include_directories(
        ${OpenCV_INCLUDE_DIRS}
)
#link_directories(
#        ${OpenCV_LIB_DIR}
#)
#FILE(GLOB opencv_dlls "${OpenCV_DIR}/bin/*.dll")
target_link_libraries(SURF_SRP ${OpenCV_LIBS})


