# CMake File for VVVVVV
# Written by Ethan "flibitijibibo" Lee

cmake_minimum_required(VERSION 2.8.12...3.5)

# CMake Options
option(ENABLE_WARNINGS "Enable compilation warnings" ON)
option(ENABLE_WERROR "Treat compilation warnings as errors" OFF)

option(BUNDLE_DEPENDENCIES "Use bundled TinyXML-2, PhysicsFS, and FAudio (if disabled, TinyXML-2, PhysicsFS, and FAudio will be dynamically linked; LodePNG, C-HashMap and SheenBidi will still be statically linked)" ON)

option(STEAM "Use the Steam API" OFF)
option(GOG "Use the GOG API" OFF)

option(OFFICIAL_BUILD "Compile an official build of the game" OFF)

option(MAKEANDPLAY "Compile a version of the game without the main campaign (provided for convenience; consider modifying MakeAndPlay.h instead" OFF)

if(OFFICIAL_BUILD AND NOT MAKEANDPLAY)
    set(STEAM ON)
    set(GOG ON)
endif()

option(REMOVE_ABSOLUTE_PATHS "If supported by the compiler, replace all absolute paths to source directories compiled into the binary (if any) with relative paths" ON)


# Architecture Flags
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
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
elseif(CMAKE_SYSTEM_NAME STREQUAL "iOS")
    set(CMAKE_OSX_DEPLOYMENT_TARGET 12.0) # SDL goes back to iOS 8.0, but modern Xcode doesn't
endif()

project(VVVVVV)

if(APPLE)
    message(STATUS "Using macOS SDK at ${CMAKE_OSX_SYSROOT}")
endif()

# RPATH
if(NOT WIN32)
    if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
        set(BIN_LIBROOT "Frameworks")
        set(BIN_RPATH "@executable_path/Frameworks")
    elseif(APPLE)
        set(BIN_LIBROOT "osx")
        set(BIN_RPATH "@executable_path/osx")
    elseif(CMAKE_SIZEOF_VOID_P MATCHES "8")
        set(BIN_LIBROOT "lib64")
        set(BIN_RPATH "\$ORIGIN/lib64")
        set(CMAKE_EXE_LINKER_FLAGS "-Wl,--disable-new-dtags")
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
set(VVV_CXX_SRC
    src/BinaryBlob.cpp
    src/BlockV.cpp
    src/ButtonGlyphs.cpp
    src/CustomLevels.cpp
    src/CWrappers.cpp
    src/Editor.cpp
    src/Ent.cpp
    src/Entity.cpp
    src/FileSystemUtils.cpp
    src/Finalclass.cpp
    src/Font.cpp
    src/FontBidi.cpp
    src/Game.cpp
    src/Graphics.cpp
    src/GraphicsResources.cpp
    src/GraphicsUtil.cpp
    src/IMERender.cpp
    src/Input.cpp
    src/KeyPoll.cpp
    src/Labclass.cpp
    src/LevelDebugger.cpp
    src/Localization.cpp
    src/LocalizationMaint.cpp
    src/LocalizationStorage.cpp
    src/Logic.cpp
    src/Map.cpp
    src/Music.cpp
    src/Otherlevel.cpp
    src/preloader.cpp
    src/Render.cpp
    src/RenderFixed.cpp
    src/RoomnameTranslator.cpp
    src/Screen.cpp
    src/Script.cpp
    src/Scripts.cpp
    src/Spacestation2.cpp
    src/TerminalScripts.cpp
    src/Textbox.cpp
    src/Tower.cpp
    src/UtilityClass.cpp
    src/WarpClass.cpp
    src/XMLUtils.cpp
    src/main.cpp
)
set(VVV_C_SRC
    src/DeferCallbacks.c
    src/GlitchrunnerMode.c
    src/Network.c
    src/Textbook.c
    src/ThirdPartyDeps.c
    src/UTF8.c
    src/VFormat.c
    src/Vlogging.c
    src/Xoshiro.c
    ../third_party/physfs/extras/physfsrwops.c
)
if(STEAM)
    list(APPEND VVV_C_SRC src/SteamNetwork.c)
endif()
if(GOG)
    list(APPEND VVV_C_SRC src/GOGNetwork.c)
endif()
if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
    list(APPEND VVV_C_SRC src/SDL_uikit_main.c)
endif()

set(VVV_SRC ${VVV_CXX_SRC} ${VVV_C_SRC})

# Executable information
if(WIN32)
    add_executable(VVVVVV WIN32 ${VVV_SRC} icon.rc)
elseif(ANDROID)
    add_library(VVVVVV SHARED ${VVV_SRC})
elseif(CMAKE_SYSTEM_NAME STREQUAL "iOS")
    file(GLOB_RECURSE REPO_RESOURCES "fonts/*" "lang/*")

    add_executable(VVVVVV MACOSX_BUNDLE ${VVV_SRC} ${DATA_ZIP} AppIcon.xcassets ${REPO_RESOURCES})
    set_target_properties(VVVVVV PROPERTIES
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.distractionware.vvvvvvmobile"
        XCODE_ATTRIBUTE_PRODUCT_NAME "VVVVVV"
        XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2" # iPhone, iPad
        XCODE_ATTRIBUTE_CURRENT_PROJECT_VERSION "2.5"
        XCODE_ATTRIBUTE_MARKETING_VERSION "2.5"
        XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME AppIcon
        XCODE_ATTRIBUTE_GENERATE_INFOPLIST_FILE YES
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist"
        XCODE_ATTRIBUTE_INFOPLIST_KEY_LSSupportsOpeningDocumentsInPlace YES
        XCODE_ATTRIBUTE_INFOPLIST_KEY_UIApplicationSupportsIndirectInputEvents YES
        RESOURCE "${DATA_ZIP};AppIcon.xcassets"
    )

    foreach(REPO_FILE ${REPO_RESOURCES})
        file(RELATIVE_PATH REPO_FILE_REL "${CMAKE_CURRENT_SOURCE_DIR}" ${REPO_FILE})
        get_filename_component(REPO_FILE_DIR ${REPO_FILE_REL} DIRECTORY)
        set_property(SOURCE ${REPO_FILE} PROPERTY MACOSX_PACKAGE_LOCATION "Resources/${REPO_FILE_DIR}")
        source_group("Resources/${REPO_FILE_DIR}" FILES "${REPO_FILE}")
    endforeach()
else()
    add_executable(VVVVVV ${VVV_SRC})
endif()

# Include Directories
if(BUNDLE_DEPENDENCIES)
    target_include_directories(
        VVVVVV PRIVATE
        src
        ../third_party
        ../third_party/tinyxml2
        ../third_party/physfs/src
        ../third_party/physfs/extras
        ../third_party/lodepng
        ../third_party/c-hashmap
        ../third_party/FAudio/include
        ../third_party/FAudio/src
        ../third_party/SheenBidi/Headers
    )
else()
    target_include_directories(
        VVVVVV PRIVATE
        src
        ../third_party
        ../third_party/lodepng
        ../third_party/physfs/extras
        ../third_party/c-hashmap
        ../third_party/FAudio/src
        ../third_party/SheenBidi/Headers
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
set(FAUDIO_SRC
    ../third_party/FAudio/src/FAudio.c
    ../third_party/FAudio/src/FAudio_internal.c
    ../third_party/FAudio/src/FAudio_internal_simd.c
    ../third_party/FAudio/src/FAudio_operationset.c
    ../third_party/FAudio/src/FAudio_platform_sdl2.c
)
set(PFS_SRC
    ../third_party/physfs/src/physfs.c
    ../third_party/physfs/src/physfs_archiver_dir.c
    ../third_party/physfs/src/physfs_archiver_unpacked.c
    ../third_party/physfs/src/physfs_archiver_zip.c
    ../third_party/physfs/src/physfs_byteorder.c
    ../third_party/physfs/src/physfs_unicode.c
    ../third_party/physfs/src/physfs_platform_posix.c
    ../third_party/physfs/src/physfs_platform_unix.c
    ../third_party/physfs/src/physfs_platform_windows.c
    ../third_party/physfs/src/physfs_platform_haiku.cpp
    ../third_party/physfs/src/physfs_platform_android.c
)
if(APPLE)
    # Are you noticing a pattern with this Apple crap yet?
    set(PFS_SRC ${PFS_SRC} ../third_party/physfs/src/physfs_platform_apple.m)
endif()
set(PNG_SRC src/lodepng_wrapper.c)
set(PNG_DEF -DLODEPNG_NO_COMPILE_ALLOCATORS -DLODEPNG_NO_COMPILE_DISK)
set(CHM_SRC ../third_party/c-hashmap/map.c)
set(SBIDI_SRC ../third_party/SheenBidi/Source/SheenBidi.c)

if(NOT OFFICIAL_BUILD)
    # Add interim commit hash and its date to the build

    # These filenames have to be qualified, because when we run
    # the CMake script, its work dir gets set to the build folder
    set(VERSION_INPUT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/InterimVersion.in.c)
    set(VERSION_OUTPUT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/InterimVersion.out.c)

    add_custom_command(
        # This OUTPUT line is required for this to be ran every time
        OUTPUT ${VERSION_OUTPUT_FILE} ${CMAKE_CURRENT_SOURCE_DIR}/src/_dummy.c
        COMMAND ${CMAKE_COMMAND}
        # These args have to be passed through, otherwise the script can't see them
        # Also, these args have to come BEFORE `-P`! (Otherwise it fails with an unclear error)
        -DINPUT_FILE=${VERSION_INPUT_FILE}
        -DOUTPUT_FILE=${VERSION_OUTPUT_FILE}
        -P ${CMAKE_CURRENT_SOURCE_DIR}/version.cmake
    )

    target_compile_definitions(VVVVVV PRIVATE -DINTERIM_VERSION_EXISTS)

    add_library(InterimVersion STATIC src/InterimVersion.out.c)
    list(APPEND STATIC_LIBRARIES InterimVersion)
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

# Disable warnings about `long long` in C++03 (from including PhysFS)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(VVVVVV PRIVATE -Wno-long-long)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(VVVVVV PRIVATE -Wno-c++11-long-long)
endif()

# Disable warnings about flexible array members in C++ (from including FAudio)
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(VVVVVV PRIVATE -Wno-c99-extensions)
endif()

# Set standards version, disable exceptions and RTTI
if(MSVC)
    # MSVC doesn't have /std:c99 or /std:c++98 switches!

    # MSVC does not officially support disabling exceptions,
    # so this is as far as we are willing to go to disable them.
    string(REGEX REPLACE "/EH[a-z]+" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REPLACE "/GR" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    set_source_files_properties(${VVV_CXX_SRC} PROPERTIES COMPILE_FLAGS "/EHsc /GR-")

    if(MSVC_VERSION GREATER 1900)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /utf-8")
    endif()
else()
    string(REGEX REPLACE "-std=[a-z0-9]+" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    set_source_files_properties(${VVV_C_SRC} PROPERTIES COMPILE_FLAGS -std=c99)

    string(REGEX REPLACE "-std=[a-z0-9+]+" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REPLACE "-fexceptions" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REPLACE "-frtti" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    set_source_files_properties(${VVV_CXX_SRC} PROPERTIES COMPILE_FLAGS "-std=c++98 -fno-exceptions -fno-rtti")

    # Dependencies (as needed)
    set_source_files_properties(${FAUDIO_SRC} PROPERTIES COMPILE_FLAGS -std=c99)
    set_source_files_properties(${CHM_SRC} PROPERTIES COMPILE_FLAGS -std=c99)
endif()

add_library(c-hashmap-static STATIC ${CHM_SRC})

add_library(sheenbidi-static STATIC ${SBIDI_SRC})

target_compile_definitions(sheenbidi-static PRIVATE
    -DSB_CONFIG_UNITY
)
target_include_directories(sheenbidi-static PRIVATE
    ../third_party/SheenBidi/Headers
)

if(BUNDLE_DEPENDENCIES)
    list(APPEND STATIC_LIBRARIES lodepng-static physfs-static tinyxml2-static c-hashmap-static faudio-static sheenbidi-static)
else()
    list(APPEND STATIC_LIBRARIES c-hashmap-static sheenbidi-static)
endif()

if(BUNDLE_DEPENDENCIES)
    add_library(lodepng-static STATIC ${PNG_SRC})
    target_compile_definitions(lodepng-static PRIVATE ${PNG_DEF})
    add_library(tinyxml2-static STATIC ${XML2_SRC})
    add_library(physfs-static STATIC ${PFS_SRC})
    target_compile_definitions(physfs-static PRIVATE
        -DPHYSFS_SUPPORTS_DEFAULT=0 -DPHYSFS_SUPPORTS_ZIP=1
    )
    add_library(faudio-static STATIC ${FAUDIO_SRC})
    target_include_directories(
        faudio-static PRIVATE
        ../third_party/FAudio/include
    )
    # Disable FAudio debug stuff in release mode. This needs a generator expression for CMake reasons(TM)
    target_compile_definitions(faudio-static PRIVATE $<$<CONFIG:Release>:FAUDIO_DISABLE_DEBUGCONFIGURATION>)

    target_link_libraries(VVVVVV ${STATIC_LIBRARIES})
else()
    target_compile_definitions(VVVVVV PRIVATE
        ${PNG_DEF}
        -DSYSTEM_LODEPNG
    )
    target_link_libraries(VVVVVV ${STATIC_LIBRARIES} physfs tinyxml2 FAudio lodepng)
endif()


if(MSVC)
    # Statically link Microsoft's runtime library so end users don't have to install it (/MT)
    # Also, build with multiple processors (/MP)
    list(APPEND GLOBAL_COMPILE_FLAGS /MT /MP)
endif()


if(REMOVE_ABSOLUTE_PATHS)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 3.8)
            set(SUPPORTS_DEBUG_PREFIX_MAP TRUE)
        else()
            set(SUPPORTS_DEBUG_PREFIX_MAP FALSE)
        endif()
        if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 10.0)
            set(SUPPORTS_FILE_PREFIX_MAP TRUE)
        else()
            set(SUPPORTS_FILE_PREFIX_MAP FALSE)
        endif()
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(SUPPORTS_DEBUG_PREFIX_MAP TRUE)
        if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 8.0)
            set(SUPPORTS_FILE_PREFIX_MAP TRUE)
        else()
            set(SUPPORTS_FILE_PREFIX_MAP FALSE)
        endif()
    else()
        set(SUPPORTS_DEBUG_PREFIX_MAP FALSE)
        set(SUPPORTS_FILE_PREFIX_MAP FALSE)
    endif()

    get_filename_component(REPO_DIR ../ ABSOLUTE)

    # Remove absolute source paths from compiled binary
    if(SUPPORTS_FILE_PREFIX_MAP)
        list(APPEND GLOBAL_COMPILE_FLAGS -ffile-prefix-map=${REPO_DIR}=.)
    elseif(SUPPORTS_DEBUG_PREFIX_MAP)
        list(APPEND GLOBAL_COMPILE_FLAGS -fdebug-prefix-map=${REPO_DIR}=.)
    endif()
endif()


target_compile_options(VVVVVV PRIVATE ${GLOBAL_COMPILE_FLAGS})

foreach(static_library IN LISTS STATIC_LIBRARIES)
    target_compile_options(${static_library} PRIVATE ${GLOBAL_COMPILE_FLAGS})
endforeach(static_library)


# SDL2 Dependency (Detection pulled from FAudio)
if(DEFINED SDL2_INCLUDE_DIRS AND DEFINED SDL2_LIBRARIES)
    message(STATUS "Using pre-defined SDL2 variables SDL2_INCLUDE_DIRS and SDL2_LIBRARIES")
    target_include_directories(VVVVVV SYSTEM PRIVATE "$<BUILD_INTERFACE:${SDL2_INCLUDE_DIRS}>")
    target_link_libraries(VVVVVV ${SDL2_LIBRARIES})
    if(BUNDLE_DEPENDENCIES)
        target_include_directories(faudio-static SYSTEM PRIVATE "$<BUILD_INTERFACE:${SDL2_INCLUDE_DIRS}>")
        target_link_libraries(faudio-static ${SDL2_LIBRARIES})
    endif()
elseif (EMSCRIPTEN)
    message(STATUS "Using Emscripten SDL2")
    target_compile_options(VVVVVV PUBLIC -sUSE_SDL=2)
    target_link_libraries(VVVVVV -sUSE_SDL=2)
    if(BUNDLE_DEPENDENCIES)
        target_compile_options(faudio-static PUBLIC -sUSE_SDL=2)
        target_link_libraries(faudio-static -sUSE_SDL=2)
    endif()
elseif(DEFINED SDL2_FRAMEWORK)
    message(STATUS "Using pre-defined SDL2 variable SDL2_FRAMEWORK")
    target_include_directories(VVVVVV SYSTEM PRIVATE "$<BUILD_INTERFACE:${SDL2_FRAMEWORK}/Headers>")
    target_link_libraries(VVVVVV ${SDL2_FRAMEWORK})
    if(BUNDLE_DEPENDENCIES)
        target_include_directories(faudio-static SYSTEM PRIVATE "$<BUILD_INTERFACE:${SDL2_FRAMEWORK}/Headers>")
        target_link_libraries(faudio-static ${SDL2_FRAMEWORK})
    endif()
    set_target_properties(VVVVVV PROPERTIES XCODE_EMBED_FRAMEWORKS ${SDL2_FRAMEWORK})
else()
    # Only try to autodetect if both SDL2 variables aren't explicitly set
    find_package(SDL2 CONFIG)
    if(TARGET SDL2::SDL2)
        message(STATUS "Using TARGET SDL2::SDL2")
        target_link_libraries(VVVVVV SDL2::SDL2)
        if(BUNDLE_DEPENDENCIES)
            target_link_libraries(faudio-static SDL2::SDL2)
        endif()
    elseif(TARGET SDL2)
        message(STATUS "Using TARGET SDL2")
        target_link_libraries(VVVVVV SDL2)
        if(BUNDLE_DEPENDENCIES)
            target_link_libraries(faudio-static SDL2)
        endif()
    else()
        message(STATUS "No TARGET SDL2::SDL2, or SDL2, using variables")
        target_include_directories(VVVVVV SYSTEM PRIVATE "$<BUILD_INTERFACE:${SDL2_INCLUDE_DIRS}>")
        target_link_libraries(VVVVVV ${SDL2_LIBRARIES})
        if(BUNDLE_DEPENDENCIES)
            target_include_directories(faudio-static SYSTEM PRIVATE "$<BUILD_INTERFACE:${SDL2_INCLUDE_DIRS}>")
            target_link_libraries(faudio-static ${SDL2_LIBRARIES})
        endif()
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
if(EMSCRIPTEN)
    # 256MB is enough for everybody
    target_link_libraries(VVVVVV -sFORCE_FILESYSTEM=1 -sTOTAL_MEMORY=256MB)
endif()
