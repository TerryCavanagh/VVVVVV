# Expects INPUT_FILE and OUTPUT_FILE to be defined

# find_package sets GIT_FOUND and GIT_EXECUTABLE
find_package(Git)

if(GIT_FOUND)
    # Get interim commit and date of commit
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" log -1 --format=%h
        OUTPUT_VARIABLE INTERIM_COMMIT
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    execute_process(
        COMMAND "${GIT_EXECUTABLE}" log -1 --format=%cd --date=short
        OUTPUT_VARIABLE COMMIT_DATE
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    execute_process(
        COMMAND "${GIT_EXECUTABLE}" branch --show-current
        OUTPUT_VARIABLE BRANCH_NAME
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif()

# Defaults if we don't have git or its commands fail for any reason or give blanks
# For annoying CMake reasons, must use "${VAR}" syntax rather than VAR
if("${INTERIM_COMMIT}" STREQUAL "")
    set(INTERIM_COMMIT "(commit?)")
endif()
if("${COMMIT_DATE}" STREQUAL "")
    set(COMMIT_DATE "(date?)")
endif()
if("${BRANCH_NAME}" STREQUAL "")
    set(BRANCH_NAME "(branch?)")
endif()

message(STATUS "This is interim commit ${INTERIM_COMMIT} (committed ${COMMIT_DATE}) on branch ${BRANCH_NAME}")

# Take the template file and replace the macros with what we have
configure_file(${INPUT_FILE} ${OUTPUT_FILE})
