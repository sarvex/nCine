set(RUNTIME_INSTALL_DESTINATION bin)
set(LIBRARY_INSTALL_DESTINATION lib)
set(ARCHIVE_INSTALL_DESTINATION lib)
set(INCLUDE_INSTALL_DESTINATION include/ncine)

if(MSVC OR APPLE)
	set(README_INSTALL_DESTINATION .)
	set(DATA_INSTALL_DESTINATION data)
	set(MAIN_CPP_INSTALL_DESTINATION src)
	set(SHADERS_INSTALL_DESTINATION data/shaders)
	set(NCINE_CONFIG_INSTALL_DESTINATION cmake)
	set(DOCUMENTATION_INSTALL_DESTINATION docs)
	set(TEST_SOURCES_INSTALL_DESTINATION src)
	set(ANDROID_INSTALL_DESTINATION android)
else()
	set(README_INSTALL_DESTINATION share/ncine)
	set(DATA_INSTALL_DESTINATION share/ncine/data)
	set(MAIN_CPP_INSTALL_DESTINATION share/ncine)
	set(SHADERS_INSTALL_DESTINATION share/ncine/shaders)
	set(NCINE_CONFIG_INSTALL_DESTINATION lib/cmake/nCine)
	set(DOCUMENTATION_INSTALL_DESTINATION share/doc/ncine)
	set(TEST_SOURCES_INSTALL_DESTINATION share/ncine)
	set(ANDROID_INSTALL_DESTINATION share/ncine/android)
endif()

set(CPACK_PACKAGE_NAME "nCine")
set(CPACK_PACKAGE_VENDOR "")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A multi-platform 2d game engine")
set(CPACK_PACKAGE_DESCRIPTION "nCine is a 2d game engine for Linux, Windows, OS X and Android")
set(CPACK_PACKAGE_CONTACT "encelo@gmail.com")
set(CPACK_PACKAGE_VERSION ${NCINE_VERSION})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "nCine")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
if(MSVC)
	set(CPACK_NSIS_MUI_ICON "${NCINE_DATA_DIR}/icons/nCine.ico")
	set(CPACK_NSIS_COMPRESSOR "/SOLID lzma")
	set(CPACK_NSIS_MODIFY_PATH ON)
elseif(APPLE)
	set(CPACK_GENERATOR "Bundle")
	set(CPACK_BUNDLE_NAME nCine)
	set(FRAMEWORKS_INSTALL_DESTINATION ../Frameworks)

	configure_file(${CMAKE_SOURCE_DIR}/Info.plist.in ${CMAKE_BINARY_DIR}/Info.plist @ONLY)
	set(CPACK_BUNDLE_PLIST ${CMAKE_BINARY_DIR}/Info.plist)

	file(RELATIVE_PATH RELPATH_TO_BIN ${CMAKE_INSTALL_PREFIX}/MacOS ${CMAKE_INSTALL_PREFIX}/Resources/${RUNTIME_INSTALL_DESTINATION})
	file(WRITE ${CMAKE_BINARY_DIR}/bundle_executable "#!/usr/bin/env sh\ncd \"$(dirname \"$0\")\" \ncd ${RELPATH_TO_BIN} &&./apptest_scene")
	install(FILES ${CMAKE_BINARY_DIR}/bundle_executable DESTINATION ../MacOS/ RENAME ${CPACK_BUNDLE_NAME}
		PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE COMPONENT tests)

	add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/${CPACK_BUNDLE_NAME}.iconset
		COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/${CPACK_BUNDLE_NAME}.iconset
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${NCINE_DATA_DIR}/icons/icon1024.png ${CMAKE_BINARY_DIR}/${CPACK_BUNDLE_NAME}.iconset/icon_512x512@2x.png
		COMMAND sips -z 512 512 ${NCINE_DATA_DIR}/icons/icon1024.png --out ${CMAKE_BINARY_DIR}/${CPACK_BUNDLE_NAME}.iconset/icon_512x512.png
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/${CPACK_BUNDLE_NAME}.iconset/icon_512x512.png ${CMAKE_BINARY_DIR}/${CPACK_BUNDLE_NAME}.iconset/icon_256x256@2x.png
		COMMAND sips -z 256 256 ${NCINE_DATA_DIR}/icons/icon1024.png --out ${CMAKE_BINARY_DIR}/${CPACK_BUNDLE_NAME}.iconset/icon_256x256.png
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/${CPACK_BUNDLE_NAME}.iconset/icon_256x256.png ${CMAKE_BINARY_DIR}/${CPACK_BUNDLE_NAME}.iconset/icon_128x128@2x.png
		COMMAND sips -z 128 128 ${NCINE_DATA_DIR}/icons/icon1024.png --out ${CMAKE_BINARY_DIR}/${CPACK_BUNDLE_NAME}.iconset/icon_128x128.png
		COMMAND sips -z 64 64 ${NCINE_DATA_DIR}/icons/icon1024.png --out ${CMAKE_BINARY_DIR}/${CPACK_BUNDLE_NAME}.iconset/icon_32x32@2x.png
		COMMAND sips -z 32 32 ${NCINE_DATA_DIR}/icons/icon1024.png --out ${CMAKE_BINARY_DIR}/${CPACK_BUNDLE_NAME}.iconset/icon_32x32.png
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/${CPACK_BUNDLE_NAME}.iconset/icon_32x32.png ${CMAKE_BINARY_DIR}/${CPACK_BUNDLE_NAME}.iconset/icon_16x16@2x.png
		COMMAND sips -z 16 16 ${NCINE_DATA_DIR}/icons/icon1024.png --out ${CMAKE_BINARY_DIR}/${CPACK_BUNDLE_NAME}.iconset/icon_16x16.png
		COMMAND iconutil --convert icns --output ${CMAKE_BINARY_DIR}/${CPACK_BUNDLE_NAME}.icns ${CMAKE_BINARY_DIR}/${CPACK_BUNDLE_NAME}.iconset)
	add_custom_target(iconutil_convert ALL DEPENDS ${CMAKE_BINARY_DIR}/${CPACK_BUNDLE_NAME}.iconset)
	set(CPACK_BUNDLE_ICON ${CMAKE_BINARY_DIR}/${CPACK_BUNDLE_NAME}.icns)
endif()

set(CPACK_COMPONENTS_ALL libraries)
set(CPACK_COMPONENT_LIBRARIES_DISPLAY_NAME "Libraries")

if(NCINE_BUILD_TESTS)
	set(CPACK_COMPONENTS_ALL "${CPACK_COMPONENTS_ALL};data;tests")
	set(CPACK_COMPONENT_DATA_DEPENDS libraries)
	set(CPACK_COMPONENT_TESTS_DEPENDS data)

	set(CPACK_COMPONENT_DATA_DISPLAY_NAME "Data")
	set(CPACK_COMPONENT_DATA_DESCRIPTION "Data files for tests")
	set(CPACK_COMPONENT_TESTS_DISPLAY_NAME "Executables")
	set(CPACK_COMPONENT_TESTS_DESCRIPTION "Test executables")

	set(CPACK_COMPONENT_DATA_GROUP testgroup)
	set(CPACK_COMPONENT_TESTS_GROUP testgroup)
	set(CPACK_COMPONENT_GROUP_TESTGROUP_DISPLAY_NAME "Test projects")
	set(CPACK_COMPONENT_GROUP_TESTGROUP_DESCRIPTION "Test projects and their data")
endif()

if(NCINE_INSTALL_DEV_SUPPORT)
	set(CPACK_COMPONENTS_ALL "${CPACK_COMPONENTS_ALL};android;devsupport")
	set(CPACK_COMPONENT_DEVSUPPORT_DEPENDS libraries)

	set(CPACK_COMPONENT_ANDROID_DISPLAY_NAME "Android")
	set(CPACK_COMPONENT_ANDROID_DESCRIPTION "Android development support")
	set(CPACK_COMPONENT_DEVSUPPORT_DISPLAY_NAME "Headers")
	set(CPACK_COMPONENT_DEVSUPPORT_DESCRIPTION "Headers and additional files to support development")
	set(CPACK_COMPONENT_LIBRARIES_DESCRIPTION "Run-time and development libraries")

	set(CPACK_COMPONENT_DEVSUPPORT_GROUP devgroup)
	set(CPACK_COMPONENT_DOCUMENTATION_GROUP devgroup)
	set(CPACK_COMPONENT_GROUP_DEVGROUP_DISPLAY_NAME "Development")
	set(CPACK_COMPONENT_GROUP_DEVGROUP_DESCRIPTION "Development support")

	if(NCINE_BUILD_DOCUMENTATION)
		set(CPACK_COMPONENTS_ALL "${CPACK_COMPONENTS_ALL};documentation")
		set(CPACK_COMPONENT_DOCUMENTATION_DEPENDS devsupport)
		set(CPACK_COMPONENT_DOCUMENTATION_DISPLAY_NAME "Documentation")
		set(CPACK_COMPONENT_DOCUMENTATION_DESCRIPTION "Doxygen generated documentation")
	endif()
else()
	set(CPACK_COMPONENT_LIBRARIES_DESCRIPTION "Run-time libraries")
endif()

include(CPack)
