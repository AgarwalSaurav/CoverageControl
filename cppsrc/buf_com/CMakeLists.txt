cmake_minimum_required(VERSION 3.15)

project(CoverageControlMain VERSION 0.3 LANGUAGES CUDA CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_COLOR_DIAGNOSTICS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS} -Wall -Wextra -Wshadow -Wformat=2 -Wunused -pedantic")

find_package(Eigen3 REQUIRED)
find_package(CGAL REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(CoverageControlCore REQUIRED)
find_package(CoverageControlTorch REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options)


include_directories("/usr/local/include/eigen3")
include_directories("/root/CoverageControl_ws/src/CoverageControl/cppsrc/core/include/CoverageControl/")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/buffer)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/telecom/tx)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/telecom/rx)



add_library(compiler_flags INTERFACE)
target_compile_features(compiler_flags INTERFACE cxx_std_20)

set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")
target_compile_options(compiler_flags INTERFACE
	"$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused;-pedantic>>"
	"$<${msvc_cxx}:$<BUILD_INTERFACE:-W3>>"
)

set(dependencies_list Eigen3::Eigen m stdc++fs yaml-cpp OpenMP::OpenMP_CXX CGAL::CGAL pthread ${GeographicLib_LIBRARIES} boost_iostreams boost_system boost_filesystem)
add_subdirectory(buffer)
add_subdirectory(telecom/tx)
add_subdirectory(telecom/rx)

#add_library(buffer_handler buffer_handler.cpp)

add_executable(main main.cpp)
# after private add eigen
target_link_libraries(main PRIVATE Boost::program_options Eigen3::Eigen compiler_flags buffer_handler udp_tx udp_rx)