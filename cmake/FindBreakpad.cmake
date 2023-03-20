#.rst:
# FindBreakpad
# ------------
# Locate Breakpad tools
#
# The ``BREAKPAD_BUILD_DIR``, variable can be used as additional hint.
#
# If you built breakpad with the default setup in "out/Default" within the
# source directory, you can set the ``Breakpad_ROOT`` variable to the source
# directory, or add it to ``CMAKE_PREFIX_PATH``.

find_program(BREAKPAD_MINIDUMP_STACKWALK
	NAMES minidump_stackwalk minidump_stackwalk.exe
	PATH_SUFFIXES src/processor
	HINTS
		"${BREAKPAD_BUILD_DIR}"
		"${CMAKE_PREFIX_PATH}"
)
if(EXISTS ${BREAKPAD_MINIDUMP_STACKWALK})
	message(STATUS "Breakpad minidump_stackwalk: ${BREAKPAD_MINIDUMP_STACKWALK}")
endif()

find_program(BREAKPAD_MINIDUMP_DUMP
	NAMES minidump_dump minidump_dump.exe
	PATH_SUFFIXES src/processor
	HINTS
		"${BREAKPAD_BUILD_DIR}"
		"${CMAKE_PREFIX_PATH}"
)
if(EXISTS ${BREAKPAD_MINIDUMP_DUMP})
	message(STATUS "Breakpad minidump_dump: ${BREAKPAD_MINIDUMP_DUMP}")
endif()

if(WIN32)
	set(BREAKPAD_TOOLS_DIR src/tools/windows)
elseif(UNIX AND NOT APPLE)
	set(BREAKPAD_TOOLS_DIR src/tools/linux)
endif()

find_program(BREAKPAD_DUMP_SYMS
	NAMES dump_syms dump_syms.exe
	PATH_SUFFIXES
		${BREAKPAD_TOOLS_DIR}/dump_syms
		${BREAKPAD_TOOLS_DIR}/Release
		${BREAKPAD_TOOLS_DIR}/binaries
	HINTS
		"${BREAKPAD_BUILD_DIR}"
		"${CMAKE_PREFIX_PATH}"
)
if(EXISTS ${BREAKPAD_DUMP_SYMS})
	message(STATUS "Breakpad dump_syms: ${BREAKPAD_DUMP_SYMS}")
endif()

find_program(BREAKPAD_SYM_UPLOAD
	NAMES sym_upload symupload.exe
	PATH_SUFFIXES
		${BREAKPAD_TOOLS_DIR}/symupload
		${BREAKPAD_TOOLS_DIR}/Release
		${BREAKPAD_TOOLS_DIR}/binaries
	HINTS
		"${BREAKPAD_BUILD_DIR}"
		"${CMAKE_PREFIX_PATH}"
)
if(EXISTS ${BREAKPAD_SYM_UPLOAD})
	message(STATUS "Breakpad sym_upload: ${BREAKPAD_SYM_UPLOAD}")
endif()

if(UNIX AND NOT APPLE)
	find_program(BREAKPAD_MD2CORE
		NAMES minidump-2-core
		PATH_SUFFIXES src/tools/linux/md2core
		HINTS
			"${BREAKPAD_BUILD_DIR}"
			"${CMAKE_PREFIX_PATH}"
	)
	if(EXISTS ${BREAKPAD_MD2CORE})
		message(STATUS "Breakpad md2core: ${BREAKPAD_MD2CORE}")
	endif()
endif()

if(APPLE)
	find_program(BREAKPAD_DUMP_SYMS_MAC
		NAMES dump_syms_mac
		PATH_SUFFIXES src/tools/mac/dump_syms
		HINTS
			"${BREAKPAD_BUILD_DIR}"
			"${CMAKE_PREFIX_PATH}"
	)
	if(EXISTS ${BREAKPAD_DUMP_SYMS_MAC})
		message(STATUS "Breakpad dump_syms_mac: ${BREAKPAD_DUMP_SYMS_MAC}")
	endif()
endif()
