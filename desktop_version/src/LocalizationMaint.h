#ifndef LOCALIZATIONMAINT_H
#define LOCALIZATIONMAINT_H

namespace loc
{

bool sync_lang_files(void);

bool save_roomname_to_file(const std::string& langcode, bool custom_level, int roomx, int roomy, const char* tra, const char* explanation);
bool save_roomname_explanation_to_files(bool custom_level, int roomx, int roomy, const char* explanation);

void local_limits_check(void);
void global_limits_check(void);

void populate_testable_script_ids(void);
bool populate_cutscene_test(const char* script_id);

} /* namespace loc */

#endif /* LOCALIZATIONMAINT_H */
