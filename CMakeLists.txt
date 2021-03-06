cmake_minimum_required(VERSION 2.8)
set(FASTHOPCROFT_VERSION_MAJOR 1)
set(FASTHOPCROFT_VERSION_MINOR 3)

project(FastHopcroft)
configure_file(src/config.h.in config.h)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.49 REQUIRED COMPONENTS system filesystem timer chrono)
include_directories(
	"${Boost_INCLUDE_DIR}"
	"${PROJECT_BINARY_DIR}")

link_directories("${Boost_LIBRARY_DIRS}")

if (MSVC)
	set(CMAKE_CXX_FLAGS_RELEASE        "/MD /Ox /Ot /Gr /GL /D NDEBUG" CACHE STRING "" FORCE)
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MD /Ox /Ot /Gr /GL /D NDEBUG /Zi" CACHE STRING "" FORCE)
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE            "/LTCG /INCREMENTAL:NO" CACHE STRING "" FORCE)
	set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "/LTCG /INCREMENTAL:NO /DEBUG" CACHE STRING "" FORCE)
endif(MSVC)

if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
	set(CMAKE_CXX_COMPILER_IS_CLANG TRUE)
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
endif()

if(CMAKE_CXX_COMPILER_IS_CLANG)
    set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++ ${CMAKE_CXX_FLAGS}")
endif()

	
add_subdirectory(src/determinize)
add_subdirectory(src/generate_nfa)
add_subdirectory(src/minimize)
add_subdirectory(src/transcode)
add_subdirectory(src/test)

#export(PACKAGE FastHopcroftExecutable)
# build a CPack driven installer package
#include (InstallRequiredSystemLibraries)
#set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
#set(CPACK_PACKAGE_VERSION_MAJOR "${FASTHOPCROFT_VERSION_MAJOR}")
#set(CPACK_PACKAGE_VERSION_MINOR "${FASTHOPCROFT_VERSION_MINOR}")
#include(CPack)

