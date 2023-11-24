# Expects INPUT_FILE and OUTPUT_FILE to be defined

# Get interim commit and date of commit
EXECUTE_PROCESS(
    COMMAND "${GIT_EXECUTABLE}" log -1 --format=%h
    OUTPUT_VARIABLE INTERIM_COMMIT
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

EXECUTE_PROCESS(
    COMMAND "${GIT_EXECUTABLE}" log -1 --format=%cd --date=short
    OUTPUT_VARIABLE COMMIT_DATE
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

EXECUTE_PROCESS(
    COMMAND "${GIT_EXECUTABLE}" branch --show-current
    OUTPUT_VARIABLE BRANCH_NAME
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

MESSAGE(STATUS "This is interim commit ${INTERIM_COMMIT} (committed ${COMMIT_DATE}) on branch ${BRANCH_NAME}")

# Take the template file and replace the macros with what we have
CONFIGURE_FILE(${INPUT_FILE} ${OUTPUT_FILE})
