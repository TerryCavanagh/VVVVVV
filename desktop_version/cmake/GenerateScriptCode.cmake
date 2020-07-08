SET(SCRIPTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/scripts")
SET(OUT_DIR "${SCRIPTS_DIR}/generated")

# Generate code for script files so they can be #included
FILE(GLOB SCRIPTS "${SCRIPTS_DIR}/*.vsc")
FOREACH(SCRIPT_FILE ${SCRIPTS})
	GET_FILENAME_COMPONENT(SCRIPT_NAME "${SCRIPT_FILE}" NAME_WE)

	GET_FILENAME_COMPONENT(OUT_FILE "${SCRIPT_FILE}" NAME)
	SET(OUT_FILE "${OUT_DIR}/${OUT_FILE}.cpp")

	# Only regenerate what changed
	IF(EXISTS "${OUT_FILE}" AND "${OUT_FILE}" IS_NEWER_THAN "${SCRIPT_FILE}")
		CONTINUE()
	ENDIF()

	FILE(READ ${SCRIPT_FILE} SCRIPT_CONTENT)

	# Wrap script in initialization code
	STRING(REGEX REPLACE "\r?\n" ";" SCRIPT_LINES "${SCRIPT_CONTENT}")
	SET(SCRIPT_CONTENT "if (SDL_strcmp(t, \"${SCRIPT_NAME}\") == 0) {\n\tstatic const char* lines[] = {\n")
	FOREACH(LINE ${SCRIPT_LINES})
		# Skip comment lines
		IF (LINE MATCHES "^#")
			CONTINUE()
		ENDIF()

		# Remove inline comments
		STRING(REGEX REPLACE "#.+$" "" LINE "${LINE}")

		# Surround each line in quotes and end with a comma
		STRING(REGEX REPLACE "^(.+)$" "\t\t\"\\1\"," LINE "${LINE}")
		SET(SCRIPT_CONTENT "${SCRIPT_CONTENT}\n${LINE}")
	ENDFOREACH()
	SET(SCRIPT_CONTENT "${SCRIPT_CONTENT}\n\t};\n\tfilllines(lines);\n\treturn;\n}")

	# Save generated file
	FILE(WRITE "${OUT_FILE}" "${SCRIPT_CONTENT}")
	MESSAGE(STATUS "Wrote ${OUT_FILE}")
ENDFOREACH()