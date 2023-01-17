#ifndef INTERIMVERSION_H
#define INTERIMVERSION_H

#ifdef INTERIM_VERSION_EXISTS

#ifdef __cplusplus
extern "C"
{
#endif

extern const char* INTERIM_COMMIT;

extern const char* COMMIT_DATE;

extern const char* BRANCH_NAME;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* INTERIM_VERSION_EXISTS */

#endif /* INTERIMVERSION_H */
