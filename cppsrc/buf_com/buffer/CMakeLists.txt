#include_directories("/usr/local/include/eigen3")
#include_directories("/root/CoverageControl_ws/src/CoverageControl/cppsrc/core/include/CoverageControl/")

#add_library(compiler_flags INTERFACE)
#target_compile_features(compiler_flags INTERFACE cxx_std_20)

#set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
#set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")
#target_compile_options(compiler_flags INTERFACE
#	"$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused;-pedantic>>"
#	"$<${msvc_cxx}:$<BUILD_INTERFACE:-W3>>"
#)

####################
## Buffer library ##
####################
#set(dependencies_list Eigen3::Eigen m stdc++fs yaml-cpp OpenMP::OpenMP_CXX CGAL::CGAL pthread ${GeographicLib_LIBRARIES} boost_iostreams boost_system boost_filesystem)

add_library(buffer_handler buffer_handler.cpp)
target_include_directories(buffer_handler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

#add_executable(buffer_test test/buffer_test.cpp)
# after private add eigen
#target_link_libraries(buffer_test PRIVATE Eigen3::Eigen compiler_flags buffer_handler)