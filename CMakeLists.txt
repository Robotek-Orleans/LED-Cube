cmake_minimum_required(VERSION 3.13)    # CMake version check
set(PROJECT_NAME Led-Cube)
project(${PROJECT_NAME})                # Create project "Led-Cube" 
set(CMAKE_CXX_STANDARD 17)              # Enable c++17 standard (g++ 7-8)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/cpp/include)

# require at least g++ 8
if (CMAKE_CXX_COMPILER MATCHES "clang\\+\\+(\.exe)?$")
elseif(CMAKE_CXX_COMPILER MATCHES "g\\+\\+(\.exe)?$")
        if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8)
                message(FATAL_ERROR "g++ version must be at least 8! " ${CMAKE_CXX_COMPILER_VERSION})
        endif()
else()
        message(WARNING "Unknown CMAKE_CXX_COMPILER " ${CMAKE_CXX_COMPILER})
endif()

# add the source files
set(SOURCE_FILES
	cpp/main.cpp
	cpp/server/HardwareMonitor.cpp
	cpp/server/WebServer.cpp
	cpp/server/ServerFriend.cpp
	cpp/server/ClientManager.cpp
	cpp/server/ClientAuth.cpp
	cpp/server/ClientChallenge.cpp
	cpp/server/JsonMessage.cpp
	cpp/server/JsonReader.cpp
	cpp/include/Mongoose/mongoose.c
	cpp/include/zip/base64.cpp
	cpp/include/zip/gzip_cpp.cpp
	cpp/include/zip/json-zip.cpp
	cpp/include/unistd-compat.cpp
	cpp/include/Bcrypt.cpp/src/bcrypt.cpp
	cpp/include/Bcrypt.cpp/src/blowfish.cpp
	cpp/Thread.cpp
	cpp/ledcube/LEDCube.cpp
	cpp/ledcube/LEDCubeManager.cpp
	cpp/ledcube/Animation.cpp
	cpp/ledcube/Frame.cpp
	cpp/ledcube/AnimationLoader/AnimationLoader.cpp
	cpp/ledcube/AnimationLoader/v1.cpp
	cpp/ledcube/AnimationLoader/v2.cpp
)

if(NOBCM2835)
	# If there is no BCM2835 library create the build folder with the following command
	# cmake -DNOBCM2835=1 ..
	set(SOURCE_FILES ${SOURCE_FILES}
		cpp/no_bcm2835/Sender-no.cpp
		cpp/no_bcm2835/TLCSin-no.cpp)
	message("BCM2835 library disabled, using dummy library")
elseif(UNIX)
	set(SOURCE_FILES ${SOURCE_FILES}
		cpp/ledcube/Sender.cpp
		cpp/ledcube/TLCSin.cpp
		cpp/include/bcm2835-1.70/src/bcm2835.c)
else()
	set(SOURCE_FILES ${SOURCE_FILES}
		cpp/windows/Sender-w.cpp
		cpp/windows/TLCSin-w.cpp
		cpp/windows/ledcubewindow.cpp)
endif()

# Add executable target with source files listed in SOURCE_FILES variable
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

if(MSVC)
    target_compile_options(${PROJECT_NAME}
        PRIVATE
            "/wd4244;" # cast
			"/wd4267;" # cast
			"/wd4251;"
			"/wd4003;"
			"-Wdeprecated-non-prototype"
			"-Wdeprecated-declarations"
    )
endif()

# Add Library
find_package(ZLIB QUIET)
if (ZLIB_FOUND)
    include_directories(${ZLIB_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${ZLIB_LIBRARIES})
else()
	target_link_libraries(${PROJECT_NAME} z)
endif(ZLIB_FOUND)

if(WIN32)
	target_link_libraries(${PROJECT_NAME} wsock32 ws2_32 gdi32 bcrypt)
else()
	target_link_libraries(${PROJECT_NAME} pthread stdc++fs)
endif()
