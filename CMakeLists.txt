cmake_minimum_required(VERSION 3.5.1)


#############
# Constants #
#############

#set(ASSETS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/assets")
set(CMAKE_GENERATOR_PLATFORM Win32)

###########
# Options #
###########

option(LTO "Enable link-time optimisation" OFF)
option(MSVC_LINK_STATIC_RUNTIME "Link the static MSVC runtime library" OFF)


#########
# Setup #
#########

project(DirectX_Game LANGUAGES C CXX)

add_executable(DirectX_Game WIN32
	"src/Resource/Pixgame.rc"
    "src/Main.cpp"
	"src/Main.h"
    "src/Carets.cpp"
    "src/Carets.h"
    "src/Doggy.cpp"
    "src/Doggy.h"
    "src/Draw.cpp"
    "src/Draw.h"
    "src/Game.cpp"
    "src/Game.h"
    "src/Generic.cpp"
    "src/Generic.h"
    "src/Objects.cpp"
    "src/Objects.h"
    "src/Sound.cpp"
    "src/Sound.h"
)


###################
# Option handling #
###################

set(BUILD_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/game")

if(LTO)
	include(CheckIPOSupported)

	check_ipo_supported(RESULT result)

	if(result)
		set_target_properties(DirectX_Game PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
	endif()
endif()

# This is messy as hell, and has been replaced by CMAKE_MSVC_RUNTIME_LIBRARY,
# but that's a very recent CMake addition, so we're still doing it this way for now
if(MSVC AND MSVC_LINK_STATIC_RUNTIME)
	# Statically-link the CRT (vcpkg static libs do this)
	foreach(flag_var CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
		if(${flag_var} MATCHES "/MD")
			string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
		endif()
	endforeach()
endif()


##########
# Tweaks #
##########

# Make some tweaks if we're using MSVC
if(MSVC)
	# Disable warnings that normally fire up on MSVC when using "unsafe" functions instead of using MSVC's "safe" _s functions
	target_compile_definitions(DirectX_Game PRIVATE _CRT_SECURE_NO_WARNINGS)

	# Make it so source files are recognized as UTF-8 by MSVC
	target_compile_options(DirectX_Game PRIVATE "/utf-8")
endif()


##################
# Misc. settings #
##################

# Force strict C90
set_target_properties(DirectX_Game PROPERTIES
	C_STANDARD 90
	C_STANDARD_REQUIRED ON
	C_EXTENSIONS OFF
)

# Force strict C++98
set_target_properties(DirectX_Game PROPERTIES
	CXX_STANDARD 98
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
)

# Name debug builds "Pixgame_debug", to distinguish them
set_target_properties(DirectX_Game PROPERTIES DEBUG_OUTPUT_NAME "DirectX_Game_debug")
set_target_properties(DirectX_Game PROPERTIES OUTPUT_NAME "DirectX_Game")

# Send executable to the build_en/build_jp directory
set_target_properties(DirectX_Game PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIRECTORY}
	RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BUILD_DIRECTORY}
	RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${BUILD_DIRECTORY}
	RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${BUILD_DIRECTORY}
	RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BUILD_DIRECTORY}
)


################
# Dependencies #
################

# Link libraries
#target_link_libraries(DirectX_Game PRIVATE ddraw dsound version shlwapi imm32 winmm dxguid gdi32)
#
## Newer MSVC is missing `dinput.lib`
#if(MSVC AND NOT (MSVC_VERSION LESS 1500))
#	target_link_libraries(DirectX_Game PRIVATE dinput8)
#else()
#	target_link_libraries(DirectX_Game PRIVATE dinput)
#endif()
target_link_libraries(DirectX_Game PRIVATE ddraw dsound version shlwapi imm32 winmm dxguid gdi32)