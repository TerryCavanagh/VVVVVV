# CMake File for VVVVVV
# Written by Ethan "flibitijibibo" Lee

cmake_minimum_required(VERSION 2.8.12)

# CMake Options
option(ENABLE_WARNINGS "Enable compilation warnings" ON)
option(ENABLE_WERROR "Treat compilation warnings as errors" OFF)

option(BUNDLE_DEPENDENCIES "Use bundled TinyXML-2, PhysicsFS, and UTF8-CPP (if disabled, TinyXML-2 and PhysicsFS will be dynamically linked, LodePNG and UTF8-CPP will still be statically linked)" ON)

set(CUSTOM_LEVEL_SUPPORT ENABLED CACHE STRING "Optionally disable playing and/or editing of custom levels")
set_property(CACHE CUSTOM_LEVEL_SUPPORT PROPERTY STRINGS ENABLED NO_EDITOR DISABLED)

option(STEAM "Use the Steam API" OFF)
option(GOG "Use the GOG API" OFF)

option(OFFICIAL_BUILD "Compile an official build of the game" OFF)

option(MAKEANDPLAY "Compile a version of the game without the main campaign (provided for convenience; consider modifying MakeAndPlay.h instead" OFF)

if(OFFICIAL_BUILD AND NOT MAKEANDPLAY)
	set(STEAM ON)
	set(GOG ON)
endif()

if(${CMAKE_VERSION} VERSION_LESS "3.1.3")
	message(WARNING "Your CMake version is too old; set -std=c90 -std=c++98 yourself!")
else()
	set(CMAKE_C_STANDARD 90)
	set(CMAKE_C_EXTENSIONS OFF)

	set(CMAKE_CXX_STANDARD 98)
	set(CMAKE_CXX_EXTENSIONS OFF)
endif()

# Architecture Flags
if(APPLE)
	# Wow, Apple is a huge jerk these days huh?
	set(OSX_10_9_SDK_PATH /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk)
	if(NOT CMAKE_OSX_SYSROOT)
		if(IS_DIRECTORY ${OSX_10_9_SDK_PATH})
			set(CMAKE_OSX_SYSROOT ${OSX_10_9_SDK_PATH})
		else()
			message(WARNING "CMAKE_OSX_SYSROOT not set and macOS 10.9 SDK not found! Using default one.")
		endif()
	endif()
	set(CMAKE_OSX_DEPLOYMENT_TARGET 10.9)
	link_directories(/usr/local/lib)
	add_compile_options(-Werror=partial-availability)
endif()

project(VVVVVV)

if(APPLE)
	message(STATUS "Using macOS SDK at ${CMAKE_OSX_SYSROOT}")
endif()

# RPATH
if(NOT WIN32)
	if(APPLE)
		set(BIN_LIBROOT "osx")
		set(BIN_RPATH "@executable_path/osx")
	elseif(CMAKE_SIZEOF_VOID_P MATCHES "8")
		set(BIN_LIBROOT "lib64")
		set(BIN_RPATH "\$ORIGIN/lib64")
	else()
		set(BIN_LIBROOT "lib")
		set(BIN_RPATH "\$ORIGIN/lib")
	endif()
	set(CMAKE_SKIP_BUILD_RPATH TRUE)
	set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
	set(CMAKE_INSTALL_RPATH ${BIN_RPATH})
	set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)
endif()

# Source Lists
set(VVV_SRC
	src/BinaryBlob.cpp
	src/BlockV.cpp
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
	src/Render.cpp
	src/RenderFixed.cpp
	src/Screen.cpp
	src/Script.cpp
	src/Scripts.cpp
	src/SoundSystem.cpp
	src/Spacestation2.cpp
	src/TerminalScripts.cpp
	src/Textbox.cpp
	src/Tower.cpp
	src/UtilityClass.cpp
	src/WarpClass.cpp
	src/XMLUtils.cpp
	src/main.cpp
	src/DeferCallbacks.c
	src/GlitchrunnerMode.c
	src/Network.c
	src/ThirdPartyDeps.c
)
if(NOT CUSTOM_LEVEL_SUPPORT STREQUAL "DISABLED")
	list(APPEND VVV_SRC src/editor.cpp)
endif()
if(STEAM)
	list(APPEND VVV_SRC src/SteamNetwork.c)
endif()
if(GOG)
	list(APPEND VVV_SRC src/GOGNetwork.c)
endif()

# Executable information
if(WIN32)
	add_executable(VVVVVV WIN32 ${VVV_SRC})
else()
	add_executable(VVVVVV ${VVV_SRC})
endif()

# Include Directories
if(BUNDLE_DEPENDENCIES)
	target_include_directories(
		VVVVVV PRIVATE
		src
		../third_party/tinyxml2
		../third_party/physfs
		../third_party/lodepng
		../third_party/utfcpp/source
	)
else()
	target_include_directories(
		VVVVVV PRIVATE
		src
		../third_party/lodepng
	)
endif()

if(MAKEANDPLAY)
	target_compile_definitions(VVVVVV PRIVATE -DMAKEANDPLAY)
endif()

if(STEAM)
	target_compile_definitions(VVVVVV PRIVATE -DSTEAM_NETWORK)
endif()
if(GOG)
	target_compile_definitions(VVVVVV PRIVATE -DGOG_NETWORK)
endif()

set(XML2_SRC
	../third_party/tinyxml2/tinyxml2.cpp
)
set(PFS_SRC
	../third_party/physfs/physfs.c
	../third_party/physfs/physfs_archiver_dir.c
	../third_party/physfs/physfs_archiver_unpacked.c
	../third_party/physfs/physfs_archiver_zip.c
	../third_party/physfs/physfs_byteorder.c
	../third_party/physfs/physfs_unicode.c
	../third_party/physfs/physfs_platform_posix.c
	../third_party/physfs/physfs_platform_unix.c
	../third_party/physfs/physfs_platform_windows.c
	../third_party/physfs/physfs_platform_haiku.cpp
)
if(APPLE)
	# Are you noticing a pattern with this Apple crap yet?
	set(PFS_SRC ${PFS_SRC} ../third_party/physfs/physfs_platform_apple.m)
endif()
set(PNG_SRC ../third_party/lodepng/lodepng.c)

if(NOT OFFICIAL_BUILD)
	# Add interim commit hash and its date to the build

	# find_package sets GIT_FOUND and GIT_EXECUTABLE
	find_package(Git)

	if(GIT_FOUND)
		# These filenames have to be qualified, because when we run
		# the CMake script, its work dir gets set to the build folder
		set(VERSION_INPUT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/Version.h.in)
		set(VERSION_OUTPUT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/Version.h.out)

		add_custom_target(
			GenerateVersion ALL
			# This BYPRODUCTS line is required for this to be ran every time
			BYPRODUCTS ${VERSION_OUTPUT_FILE}
			COMMAND ${CMAKE_COMMAND}
			# These args have to be passed through, otherwise the script can't see them
			# Also, these args have to come BEFORE `-P`! (Otherwise it fails with an unclear error)
			-DGIT_EXECUTABLE=${GIT_EXECUTABLE}
			-DINPUT_FILE=${VERSION_INPUT_FILE}
			-DOUTPUT_FILE=${VERSION_OUTPUT_FILE}
			-P ${CMAKE_CURRENT_SOURCE_DIR}/version.cmake
		)

		add_dependencies(VVVVVV GenerateVersion)

		# This lets Version.h know that Version.h.out exists
		add_definitions(-DVERSION_H_OUT_EXISTS)
	endif()
endif()

# Build options
if(ENABLE_WARNINGS)
	# The weird syntax is due to CMake generator expressions.
	# Saves quite a few lines and boilerplate at the price of readability.
	target_compile_options(VVVVVV PRIVATE
		$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:
			-Wall -Wpedantic $<$<BOOL:${ENABLE_WERROR}>:-Werror>>
		$<$<CXX_COMPILER_ID:MSVC>:
			/W4 $<$<BOOL:${ENABLE_WERROR}>:/WX>>)
endif()

if(CUSTOM_LEVEL_SUPPORT STREQUAL "NO_EDITOR")
	add_definitions(-DNO_EDITOR)
elseif(CUSTOM_LEVEL_SUPPORT STREQUAL "DISABLED")
	add_definitions(-DNO_CUSTOM_LEVELS -DNO_EDITOR)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	set(SUPPORTS_IMPLICIT_FALLTHROUGH TRUE)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 7.0)
		set(SUPPORTS_IMPLICIT_FALLTHROUGH TRUE)
	else()
		set(SUPPORTS_IMPLICIT_FALLTHROUGH FALSE)
	endif()
else()
	set(SUPPORTS_IMPLICIT_FALLTHROUGH FALSE)
endif()


if(SUPPORTS_IMPLICIT_FALLTHROUGH)
	target_compile_options(VVVVVV PRIVATE -Werror=implicit-fallthrough)
endif()

if(MSVC)
	# Disable MSVC warnings about implicit conversion
	target_compile_options(VVVVVV PRIVATE /wd4244)
endif()

if(MSVC)
	# Disable exceptions
	string(REGEX REPLACE "/EH[a-z]+" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")

	# Disable RTTI
	string(REGEX REPLACE "/GR" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GR-")
else()
	# Disable exceptions
	string(REGEX REPLACE "-fexceptions" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")

	# Disable RTTI
	string(REGEX REPLACE "-frtti" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()

# Unfortunately, it doesn't seem like distros package LodePNG
add_library(lodepng-static STATIC ${PNG_SRC})

target_compile_definitions(lodepng-static PRIVATE
	-DLODEPNG_NO_COMPILE_ALLOCATORS
	-DLODEPNG_NO_COMPILE_DISK
	-DLODEPNG_NO_COMPILE_ENCODER
)

if(BUNDLE_DEPENDENCIES)
	add_library(tinyxml2-static STATIC ${XML2_SRC})
	add_library(physfs-static STATIC ${PFS_SRC})
	target_compile_definitions(physfs-static PRIVATE
		-DPHYSFS_SUPPORTS_DEFAULT=0 -DPHYSFS_SUPPORTS_ZIP=1
	)

	# PhysFS needs some extensions...
	if(${CMAKE_VERSION} VERSION_GREATER "3.1.3"
	OR ${CMAKE_VERSION} VERSION_EQUAL "3.1.3")
		set_property(TARGET physfs-static PROPERTY C_EXTENSIONS ON)
	endif()

	target_link_libraries(VVVVVV physfs-static tinyxml2-static lodepng-static)
else()
	find_package(utf8cpp CONFIG)

	target_link_libraries(VVVVVV physfs tinyxml2 utf8cpp lodepng-static)
endif()

# SDL2 Dependency (Detection pulled from FAudio)
if(DEFINED SDL2_INCLUDE_DIRS AND DEFINED SDL2_LIBRARIES)
	message(STATUS "Using pre-defined SDL2 variables SDL2_INCLUDE_DIRS and SDL2_LIBRARIES")
	target_include_directories(VVVVVV SYSTEM PRIVATE "$<BUILD_INTERFACE:${SDL2_INCLUDE_DIRS}>")
	target_link_libraries(VVVVVV ${SDL2_LIBRARIES})
else()
	# Only try to autodetect if both SDL2 variables aren't explicitly set
	find_package(SDL2 CONFIG)
	if(TARGET SDL2::SDL2)
		message(STATUS "Using TARGET SDL2::SDL2")
		target_link_libraries(VVVVVV SDL2::SDL2 SDL2_mixer)
	elseif(TARGET SDL2)
		message(STATUS "Using TARGET SDL2")
		target_link_libraries(VVVVVV SDL2 SDL2_mixer)
	else()
		message(STATUS "No TARGET SDL2::SDL2, or SDL2, using variables")
		find_path(SDL2_MIXER_INCLUDE_DIRS NAMES SDL_mixer.h PATH_SUFFIXES SDL2)
		target_include_directories(VVVVVV SYSTEM PRIVATE "$<BUILD_INTERFACE:${SDL2_INCLUDE_DIRS}>" ${SDL2_MIXER_INCLUDE_DIRS})
		target_link_libraries(VVVVVV ${SDL2_LIBRARIES} SDL2_mixer)
	endif()
endif()

# Yes, more Apple Crap
if(APPLE)
	find_library(FOUNDATION NAMES Foundation)
	find_library(IOKIT NAMES IOKit)
	target_link_libraries(VVVVVV objc ${IOKIT} ${FOUNDATION})
endif()
# But hey, also some Haiku crap
if(HAIKU)
	find_library(BE_LIBRARY be)
	find_library(ROOT_LIBRARY root)
	target_link_libraries(VVVVVV ${BE_LIBRARY} ${ROOT_LIBRARY})
endif()
