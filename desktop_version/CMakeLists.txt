# CMake File for VVVVVV
# Written by Ethan "flibitijibibo" Lee

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)
PROJECT(VVVVVV)

# Architecture Flags
IF(APPLE)
	# Wow, Apple is a huge jerk these days huh?
	SET(CMAKE_OSX_SYSROOT /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk)
	SET(CMAKE_OSX_DEPLOYMENT_TARGET 10.9)
	LINK_DIRECTORIES(/usr/local/lib)
	ADD_COMPILE_OPTIONS(-Werror=partial-availability)
ENDIF()

# Compiler Flags
ADD_DEFINITIONS(-DPHYSFS_SUPPORTS_DEFAULT=0 -DPHYSFS_SUPPORTS_ZIP=1)

# Executable Suffix
IF(APPLE)
	SET(CMAKE_EXECUTABLE_SUFFIX ".osx")
ELSEIF(WIN32)
	# Nothing!
ELSEIF(CMAKE_SIZEOF_VOID_P MATCHES "8")
	SET(CMAKE_EXECUTABLE_SUFFIX ".x86_64")
ELSE()
	SET(CMAKE_EXECUTABLE_SUFFIX ".x86")
ENDIF()

# Include Directories
INCLUDE_DIRECTORIES(src tinyxml physfs lodepng)

# Source Lists
SET(VVV_SRC
	src/BinaryBlob.cpp
	src/BlockV.cpp
	src/editor.cpp
	src/Ent.cpp
	src/Entity.cpp
	src/FileSystemUtils.cpp
	src/Finalclass.cpp
	src/Game.cpp
	src/Graphics.cpp
	src/GraphicsResources.cpp
	src/GraphicsUtil.cpp
	src/Input.cpp
	src/KeyPoll.cpp
	src/Labclass.cpp
	src/Logic.cpp
	src/Map.cpp
	src/Music.cpp
	src/Otherlevel.cpp
	src/preloader.cpp
	src/Screen.cpp
	src/Script.cpp
	src/Scripts.cpp
	src/SoundSystem.cpp
	src/Spacestation2.cpp
	src/TerminalScripts.cpp
	src/Textbox.cpp
	src/titlerender.cpp
	src/Tower.cpp
	src/UtilityClass.cpp
	src/WarpClass.cpp
	src/main.cpp
	src/SteamNetwork.c
)
SET(XML_SRC
	tinyxml/tinystr.cpp
	tinyxml/tinyxml.cpp
	tinyxml/tinyxmlerror.cpp
	tinyxml/tinyxmlparser.cpp
)
SET(PFS_SRC
	physfs/physfs.c
	physfs/physfs_archiver_dir.c
	physfs/physfs_archiver_unpacked.c
	physfs/physfs_archiver_zip.c
	physfs/physfs_byteorder.c
	physfs/physfs_unicode.c
	physfs/physfs_platform_posix.c
	physfs/physfs_platform_unix.c
	physfs/physfs_platform_windows.c
)
IF(APPLE)
	# Are you noticing a pattern with this Apple crap yet?
	SET(PFS_SRC ${PFS_SRC} physfs/physfs_platform_apple.m)
ENDIF()
SET(PNG_SRC lodepng/lodepng.c)

# Executable information
IF(WIN32)
	ADD_EXECUTABLE(vvvvvv WIN32 ${VVV_SRC})
ELSE()
	ADD_EXECUTABLE(vvvvvv ${VVV_SRC})
ENDIF()

# Library information
ADD_LIBRARY(tinyxml-static STATIC ${XML_SRC})
ADD_LIBRARY(physfs-static STATIC ${PFS_SRC} ${PFSP_SRC})
ADD_LIBRARY(lodepng-static STATIC ${PNG_SRC})

# Static Dependencies
TARGET_LINK_LIBRARIES(vvvvvv physfs-static tinyxml-static lodepng-static)

# SDL2 Dependency (Detection pulled from FAudio)
if (DEFINED SDL2_INCLUDE_DIRS AND DEFINED SDL2_LIBRARIES)
	message(STATUS "Using pre-defined SDL2 variables SDL2_INCLUDE_DIRS and SDL2_LIBRARIES")
	target_include_directories(vvvvvv PUBLIC "$<BUILD_INTERFACE:${SDL2_INCLUDE_DIRS}>")
	target_link_libraries(vvvvvv ${SDL2_LIBRARIES})
else()
	# Only try to autodetect if both SDL2 variables aren't explicitly set
	find_package(SDL2 CONFIG)
	if (TARGET SDL2::SDL2)
		message(STATUS "Using TARGET SDL2::SDL2")
		target_link_libraries(vvvvvv SDL2::SDL2 SDL2_mixer)
	elseif (TARGET SDL2)
		message(STATUS "Using TARGET SDL2")
		target_link_libraries(vvvvvv SDL2 SDL2_mixer)
	else()
		message(STATUS "No TARGET SDL2::SDL2, or SDL2, using variables")
		target_include_directories(vvvvvv PUBLIC "$<BUILD_INTERFACE:${SDL2_INCLUDE_DIRS}>")
		target_link_libraries(vvvvvv ${SDL2_LIBRARIES} SDL2_mixer)
	endif()
endif()

# Yes, more Apple Crap
IF(APPLE)
	FIND_LIBRARY(FOUNDATION NAMES Foundation)
	FIND_LIBRARY(IOKIT NAMES IOKit)
	TARGET_LINK_LIBRARIES(vvvvvv objc ${IOKIT} ${FOUNDATION})
ENDIF()
