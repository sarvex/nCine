set(NCINE_OPTIONS_PRESETS "Default" CACHE STRING "Presets for CMake options")
set_property(CACHE NCINE_OPTIONS_PRESETS PROPERTY STRINGS Default BinDist DevDist)

if(NCINE_OPTIONS_PRESETS STREQUAL BinDist OR NCINE_OPTIONS_PRESETS STREQUAL DevDist)
	message(STATUS "Options presets: ${NCINE_OPTIONS_PRESETS}")

	set(CMAKE_BUILD_TYPE Release)
	set(CMAKE_CONFIGURATION_TYPES Release)
	set(NCINE_BUILD_TESTS ON)
	set(NCINE_EXTRA_OPTIMIZATION ON)
	set(NCINE_AUTOVECTORIZATION_REPORT OFF)
	set(NCINE_DYNAMIC_LIBRARY ON)
	set(NCINE_IMPLEMENTATION_DOCUMENTATION OFF)
	set(NCINE_EMBED_SHADERS ON)
	set(DEFAULT_DATA_DIR_DIST ON)
	if(DEFINED NCINE_ADDRESS_SANITIZER)
		set(NCINE_ADDRESS_SANITIZER OFF)
	endif()
	if(DEFINED NCINE_GCC_HARDENING)
		set(NCINE_GCC_HARDENING ON)
	endif()
endif()

if(NCINE_OPTIONS_PRESETS STREQUAL BinDist)
	set(NCINE_INSTALL_DEV_SUPPORT OFF)
	set(NCINE_BUILD_ANDROID OFF)
	set(NCINE_BUILD_DOCUMENTATION OFF)
elseif(NCINE_OPTIONS_PRESETS STREQUAL DevDist)
	set(NCINE_INSTALL_DEV_SUPPORT ON)
	set(NCINE_BUILD_ANDROID ON)
	set(NCINE_NDK_ARCHITECTURES armeabi-v7a arm64-v8a x86_64)
	set(NCINE_BUILD_DOCUMENTATION ON)
endif()
