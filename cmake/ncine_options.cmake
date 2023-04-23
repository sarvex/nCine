option(NCINE_BUILD_TESTS "Build the engine test programs" ON)
option(NCINE_BUILD_UNIT_TESTS "Build the engine unit tests" OFF)
option(NCINE_BUILD_BENCHMARKS "Build the engine micro benchmarks" OFF)
option(NCINE_INSTALL_DEV_SUPPORT "Install files to support development" ON)
option(NCINE_LINKTIME_OPTIMIZATION "Compile the engine with link time optimization when in release" OFF)
option(NCINE_AUTOVECTORIZATION_REPORT "Enable report generation from compiler auto-vectorization" OFF)
option(NCINE_DYNAMIC_LIBRARY "Compile the engine as a dynamic library" ON)
option(NCINE_BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" OFF)
option(NCINE_IMPLEMENTATION_DOCUMENTATION "Include implementation classes in the documentation" OFF)
option(NCINE_EMBED_SHADERS "Export shader files to C strings to be included in engine sources" ON)
option(NCINE_BUILD_ANDROID "Build the Android version of the engine" OFF)
option(NCINE_STRIP_BINARIES "Enable symbols stripping from libraries and executables when in release" OFF)

set(NCINE_PREFERRED_BACKEND "GLFW" CACHE STRING "Specify the preferred backend on desktop")
set_property(CACHE NCINE_PREFERRED_BACKEND PROPERTY STRINGS "GLFW;SDL2;QT5")

if(EMSCRIPTEN)
	option(NCINE_WITH_THREADS "Enable the Emscripten Pthreads support" OFF)
else()
	option(NCINE_WITH_THREADS "Enable support for threads" ON)
endif()
if(MSVC)
	option(NCINE_WITH_ANGLE "Enable Google ANGLE libraries support" OFF)
endif()
if(NOT WIN32 AND NOT NCINE_ARM_PROCESSOR)
	option(NCINE_WITH_GLEW "Enable GLEW support" ON)
endif()
option(NCINE_WITH_PNG "Enable PNG image file loading" ON)
option(NCINE_WITH_WEBP "Enable WebP image file loading" ON)
option(NCINE_WITH_AUDIO "Enable OpenAL support and thus sound" ON)
option(NCINE_WITH_VORBIS "Enable Ogg Vorbis audio file loading" ON)
option(NCINE_WITH_LUA "Enable Lua scripting integration" ON)
if(NCINE_WITH_LUA)
	option(NCINE_WITH_SCRIPTING_API "Enable Lua scripting API" ON)
endif()

option(NCINE_WITH_ALLOCATORS "Enable the custom memory allocators" OFF)
option(NCINE_WITH_IMGUI "Enable the integration with Dear ImGui" ON)
option(NCINE_WITH_NUKLEAR "Enable the integration with Nuklear" OFF)
option(NCINE_WITH_TRACY "Enable the integration with the Tracy frame profiler" OFF)
option(NCINE_WITH_RENDERDOC "Enable the integration with RenderDoc" OFF)

if(EMSCRIPTEN)
	set(NCINE_DYNAMIC_LIBRARY OFF)
endif()

set(NCINE_DATA_DIR "${PARENT_SOURCE_DIR}/nCine-data" CACHE PATH "Set the path to the engine data directory")
set(NCINE_ICONS_DIR "${PARENT_SOURCE_DIR}/nCine-data/icons" CACHE PATH "Set the path to the engine icons directory")
set(NCINE_TESTS_DATA_DIR "" CACHE STRING "Set the path to the data directory that will be embedded in test executables")
# The external Android dir is set regardless of the status of build Android flag, so that presets work even when the flag is off
set(EXTERNAL_ANDROID_DIR "${PARENT_SOURCE_DIR}/nCine-android-external" CACHE PATH "Set the path to the Android libraries directory")

if(NCINE_BUILD_ANDROID)
	set(NDK_DIR "" CACHE PATH "Set the path to the Android NDK")
	set(NCINE_NDK_ARCHITECTURES arm64-v8a CACHE STRING "Set the NDK target architectures")
	option(NCINE_ASSEMBLE_APK "Assemble the Android APK of the startup test with Gradle" OFF)
	option(NCINE_UNIVERSAL_APK "Configure the Gradle build script to assemble an universal APK for all ABIs" OFF)
endif()

if(NCINE_WITH_ALLOCATORS)
	option(NCINE_RECORD_ALLOCATIONS "Record a timestamp of every allocation and deallocation" OFF)
	option(NCINE_OVERRIDE_NEW "Override global new and delete operators to use custom allocators" OFF)
	option(NCINE_USE_FREELIST "Use the free list custom allocator instead of malloc()/free()" OFF)
	set(NCINE_FREELIST_BUFFER "33554432" CACHE STRING "Size in bytes of the free list allocator buffer")
endif()

if(NCINE_WITH_RENDERDOC)
	set(RENDERDOC_DIR "" CACHE PATH "Set the path to the RenderDoc directory")
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	option(NCINE_ADDRESS_SANITIZER "Enable the AddressSanitizer memory error detector of GCC and Clang" OFF)
	option(NCINE_UNDEFINED_SANITIZER "Enable the UndefinedBehaviorSanitizer detector of GCC and Clang" OFF)
	option(NCINE_CODE_COVERAGE "Enable gcov instrumentation for testing code coverage" OFF)
endif()
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	option(NCINE_GCC_HARDENING "Enable memory corruption mitigation methods of GCC" OFF)
endif()

set(NCINE_STARTUP_TEST "apptest_camera" CACHE STRING "Set the starting test project and default running executable")

# nCine options presets
set(NCINE_OPTIONS_PRESETS "Default" CACHE STRING "Presets for CMake options")
set(NCINE_OPTIONS_PRESETS_STRINGS "Default" "BinDist" "DevDist" "LuaDist" "UnitTests" "Benchmarks")
set_property(CACHE NCINE_OPTIONS_PRESETS PROPERTY STRINGS ${NCINE_OPTIONS_PRESETS_STRINGS})

if (NOT "${NCINE_OPTIONS_PRESETS}" STREQUAL "Default")
	foreach(NCINE_OPTIONS_PRESETS_STRING ${NCINE_OPTIONS_PRESETS_STRINGS})
		if("${NCINE_OPTIONS_PRESETS}" STREQUAL "${NCINE_OPTIONS_PRESETS_STRING}")
			set(NCINE_USING_OPTIONS_PRESETS TRUE)
			break()
		endif()
	endforeach()
endif()

if(NCINE_USING_OPTIONS_PRESETS)
	message(STATUS "Options presets: ${NCINE_OPTIONS_PRESETS}")

	set(NCINE_BUILD_TESTS ON)
	set(NCINE_BUILD_UNIT_TESTS OFF)
	set(NCINE_BUILD_BENCHMARKS OFF)
	set(NCINE_LINKTIME_OPTIMIZATION ON)
	set(NCINE_AUTOVECTORIZATION_REPORT OFF)
	set(NCINE_DYNAMIC_LIBRARY ON)
	set(NCINE_IMPLEMENTATION_DOCUMENTATION OFF)
	set(NCINE_EMBED_SHADERS ON)
	set(TESTS_DATA_DIR_DIST ON)
	set(NCINE_STARTUP_TEST apptest_camera)
	if(DEFINED NCINE_ADDRESS_SANITIZER)
		set(NCINE_ADDRESS_SANITIZER OFF)
	endif()
	if(DEFINED NCINE_CODE_COVERAGE)
		set(NCINE_CODE_COVERAGE OFF)
	endif()
	if(DEFINED NCINE_GCC_HARDENING)
		set(NCINE_GCC_HARDENING ON)
	endif()
	set(NCINE_PREFERRED_BACKEND "GLFW")
	set(NCINE_STRIP_BINARIES ON)
	set(NCINE_WITH_IMGUI ON)
	set(NCINE_WITH_TRACY OFF)
	set(NCINE_WITH_RENDERDOC OFF)

	if("${NCINE_OPTIONS_PRESETS}" STREQUAL "BinDist")
		set(NCINE_INSTALL_DEV_SUPPORT OFF)
		set(NCINE_BUILD_ANDROID OFF)
		set(NCINE_BUILD_DOCUMENTATION OFF)
	elseif("${NCINE_OPTIONS_PRESETS}" STREQUAL "DevDist")
		set(NCINE_INSTALL_DEV_SUPPORT ON)
		set(NCINE_BUILD_ANDROID ON)
		set(NCINE_ASSEMBLE_APK OFF)
		set(NCINE_NDK_ARCHITECTURES armeabi-v7a arm64-v8a x86_64)
		set(NCINE_BUILD_DOCUMENTATION OFF)
	elseif("${NCINE_OPTIONS_PRESETS}" STREQUAL "LuaDist")
		set(NCINE_INSTALL_DEV_SUPPORT OFF)
		set(NCINE_BUILD_ANDROID OFF)
		set(NCINE_BUILD_DOCUMENTATION OFF)
		set(NCINE_WITH_IMGUI OFF)
		set(NCINE_WITH_THREADS OFF)
		set(NCINE_WITH_SCRIPTING_API ON)
		set(NCINE_STARTUP_TEST ncinelua)
	elseif("${NCINE_OPTIONS_PRESETS}" STREQUAL "UnitTests" OR "${NCINE_OPTIONS_PRESETS}" STREQUAL "Benchmarks")
		set(NCINE_BUILD_ANDROID OFF)
		set(NCINE_BUILD_DOCUMENTATION OFF)
		set(NCINE_WITH_IMGUI OFF)
		set(NCINE_WITH_THREADS OFF)
		set(NCINE_WITH_PNG OFF)
		# Compile the nCine with no support for WebP or unit tests will not find libsharpyuv.so.0 on Linux
		set(NCINE_WITH_WEBP OFF)
		set(NCINE_WITH_AUDIO OFF)
		set(NCINE_WITH_VORBIS OFF)
		set(NCINE_WITH_LUA OFF)
		set(NCINE_BUILD_TESTS OFF)
		if("${NCINE_OPTIONS_PRESETS}" STREQUAL "UnitTests")
			set(NCINE_LINKTIME_OPTIMIZATION OFF)
			set(NCINE_STRIP_BINARIES OFF)
			set(NCINE_BUILD_UNIT_TESTS ON)
		elseif("${NCINE_OPTIONS_PRESETS}" STREQUAL "Benchmarks")
			set(NCINE_LINKTIME_OPTIMIZATION ON)
			set(NCINE_STRIP_BINARIES ON)
			set(NCINE_BUILD_BENCHMARKS ON)
		endif()
	endif()

	if((MINGW OR MSYS) AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		# Not stripping binaries on MinGW when using Clang to avoid the "unexpected associative section index" error
		# https://github.com/llvm/llvm-project/issues/53433
		set(NCINE_STRIP_BINARIES OFF)
	endif()
	if(EMSCRIPTEN)
		set(NCINE_DYNAMIC_LIBRARY OFF)
		set(NCINE_IMPLEMENTATION_DOCUMENTATION ON)
		set(NCINE_BUILD_ANDROID OFF)
		set(NCINE_STRIP_BINARIES OFF)
	endif()
endif()
