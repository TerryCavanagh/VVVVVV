/* For now, this isn't really a foundation for action sets yet; button glyphs
 * just need to be able to identify actions that are printed in text like
 * "Press ENTER to teleport". Thus, this currently ONLY contains identifiers
 * for the actions that button glyphs are needed for.
 *
 * Based on this comment:
 * https://github.com/TerryCavanagh/VVVVVV/issues/834#issuecomment-1015692161
 */


#ifndef ACTIONSETS_H
#define ACTIONSETS_H


#ifdef __cplusplus
extern "C"
{
#endif


/*----------------------------------------*
 * List of all action sets (all "states") *
 *----------------------------------------*/
typedef enum
{
    //ActionSet_Global,
    ActionSet_Menu,
    ActionSet_InGame
    //ActionSet_Editor
}
ActionSet;


/*----------------------------------------------------------*
 * An enum for each actionset, with the actions in that set *
 *----------------------------------------------------------*/
/*
typedef enum
{
    Action_Global_Mute,
    Action_Global_MuteMusic
}
Action_Global;
*/

typedef enum
{
    Action_Menu_Accept
}
Action_Menu;

typedef enum
{
    Action_InGame_ACTION,
    Action_InGame_Interact,
    Action_InGame_Map
}
Action_InGame;

/*
typedef enum
{
    //Action_Editor_PrevTool,
    //Action_Editor_NextTool
}
Action_Editor;
*/


/*-----------------------------------------*
 * A union to represent any actionset enum *
 *-----------------------------------------*/
typedef union
{
    int intval;
    //Action_Global Global;
    Action_Menu Menu;
    Action_InGame InGame;
    //Action_Editor Editor;
}
Action;


#ifdef __cplusplus
} // extern "C"
#endif

#endif // ACTIONSETS_H
