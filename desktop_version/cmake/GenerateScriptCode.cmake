set(SCRIPTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/scripts")
set(OUT_DIR "${SCRIPTS_DIR}/generated")

# Generate code for script files so they can be #included
file(GLOB SCRIPTS "${SCRIPTS_DIR}/*.vsc")
foreach(SCRIPT_FILE ${SCRIPTS})
	get_filename_component(SCRIPT_NAME "${SCRIPT_FILE}" NAME_WE)

	get_filename_component(OUT_FILE "${SCRIPT_FILE}" NAME)
	set(OUT_FILE "${OUT_DIR}/${OUT_FILE}.cpp")

	# Only regenerate what changed
	if(NOT (EXISTS "${OUT_FILE}" AND "${OUT_FILE}" IS_NEWER_THAN "${SCRIPT_FILE}"))
		file(READ ${SCRIPT_FILE} SCRIPT_CONTENT)

		# Wrap script in initialization code
		string(REGEX REPLACE "\r?\n" ";" SCRIPT_LINES "${SCRIPT_CONTENT}")
		set(SCRIPT_CONTENT "if (SDL_strcmp(t, \"${SCRIPT_NAME}\") == 0) {\n\tstatic const char* lines[] = {\n")
		foreach(LINE ${SCRIPT_LINES})
			# Skip comment lines
			if(NOT (LINE MATCHES "^#"))
				# Remove inline comments
				string(REGEX REPLACE "#.+$" "" LINE "${LINE}")

				# Surround each line in quotes and end with a comma
				string(REGEX REPLACE "^(.+)$" "\t\t\"\\1\"," LINE "${LINE}")
				set(SCRIPT_CONTENT "${SCRIPT_CONTENT}\n${LINE}")
			endif()
		endforeach()
		set(SCRIPT_CONTENT "${SCRIPT_CONTENT}\n\t};\n\tfilllines(lines);\n\treturn;\n}")

		# Save generated file
		file(WRITE "${OUT_FILE}" "${SCRIPT_CONTENT}")
		message(STATUS "Wrote ${OUT_FILE}")
	endif()
endforeach()