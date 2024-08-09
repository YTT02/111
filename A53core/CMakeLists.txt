cmake_minimum_required(VERSION 3.22)
project(helmet)

set(CMAKE_CXX_STANDARD 17)
find_package(Threads REQUIRED)

set(CMAKE_TOOLCHAIN_FILES /home/jaimevan/imx8mm/cross-tool-chain/toolchain.cmake)
include(/home/jaimevan/imx8mm/cross-tool-chain/toolchain.cmake)

message(STATUS "Cross compile message:")
message(STATUS "    toolchain path: ${CMAKE_TOOLCHAIN_FILES}")
message(STATUS "    system root: ${CMAKE_SYSROOT}")
message(STATUS "    c compiler: ${CMAKE_C_COMPILER}")
message(STATUS "    cpp compiler: ${CMAKE_CXX_COMPILER}")

set(OpenCV_DIR /home/jaimevan/imx8mm/third-party/opencv3.16/install/share/OpenCV)
list(APPEND CMAKE_MODULE_PATH /home/jaimevan/imx8mm/third-party/opencv3.16/install/share/OpenCV)

find_package(OpenCV 3.4.16 REQUIRED)

# If the package has been found, several variables will
# be set, you can find the full list with descriptions
# in the OpenCVConfig.cmake file.
# Print some message showing some of them
message(STATUS "OpenCV library status:")
message(STATUS "    config: ${OpenCV_DIR}")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

include_directories(Inc/ Thirdparty/CppLinuxSerial/Inc/ )

file(GLOB_RECURSE SOURCES "Src/*.*" Thirdparty/CppLinuxSerial/Src/*.*)

add_executable(${PROJECT_NAME} main.cpp ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCV_LIBS} Threads::Threads)
