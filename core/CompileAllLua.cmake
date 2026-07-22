set(LUA_DIR "${LUA_DIR}")

file(GLOB_RECURSE LUA_FILES CONFIGURE_DEPENDS
	"${LUA_DIR}/*.lua"
)

foreach(LUA_FILE IN LISTS LUA_FILES)
	string(REGEX REPLACE "\\.lua$" ".ljbc" OUTPUT_FILE "${LUA_FILE}")

	message(STATUS "Compiling ${LUA_FILE} -> ${OUTPUT_FILE}")

	execute_process(
		COMMAND luajit -bsX "${LUA_FILE}" "${OUTPUT_FILE}"
		RESULT_VARIABLE RESULT
	)

	if(NOT RESULT EQUAL 0)
		message(FATAL_ERROR "Failed to compile ${LUA_FILE}")
	endif()

	file(REMOVE "${LUA_FILE}")
endforeach()