#ifndef OPTIONDATA_H
#define OPTIONDATA_H

// Define the max resolution of the game as 8K UHD.
#ifndef maxres
#define maxres "7680x4320"
#endif

// Define the path of the launcher data file as being in the same folder as the launcher executable.
#ifndef launcherDataFile
#define launcherDataFile "/launcher.dat"
#endif

// Define the path of the player data file as being in the same folder as the launcher executable.
#ifndef playerDataFile
#define playerDataFile "/player.dat"
#endif

// Define the path of the mod's RTW preferences file assuming the user has correctly installed the launcher in
// [RTW/MODFOLDER/Launcher].
#ifndef preferencesFile
#define preferencesFile "/../preferences/preferences.txt"
#endif

// Define the default player data file that will be generated if none is found. The checkboxes in the main window are
// hardcoded options that would need changing here if altered.
#ifndef defaultPlayerDat
#define defaultPlayerDat ""\
    "// ------------------------------------------------------------------------------------------------------------\n"\
    "// Start options checkboxes\n"\
    "showerrBox:1\n"\
    "neBox:0\n"\
    "nmBox:0\n"\
    "multirunBox:0\n"\
    "editorBox:0\n"\
    "moviecamBox:0\n"\
    "naBox:0\n"\
    "aiBox:0\n"\
    "spritesBox:0\n"\
    "swBox:0\n"\
    "maprwmBox:0\n\n"\
    "// ------------------------------------------------------------------------------------------------------------\n"\
    "// EDU select\n"\
    "eduComboBox:default\n\n"\
    "// ------------------------------------------------------------------------------------------------------------\n"\
    "// Trees select\n"\
    "treesComboBox:default\n\n"\
    "// ------------------------------------------------------------------------------------------------------------\n"\
    "// Campaign select\n"\
    "campComboBox:default\n\n"\
    "// ------------------------------------------------------------------------------------------------------------\n\n\n"\
    "// ------------------------------------------------------------------------------------------------------------\n"\
    "// ------------------------------------------------------------------------------------------------------------\n"\
    "// This is an example of a comment\n"\
    "Lines without a colon will get ignored without displaying a warning too.\n"\
    "//So will empty lines.\n\n"\
    "// This file lists all the player-customised options chosen for launching the game, including start options and chosen campaign and game type.\n"\
    "// Syntax: [option code]:[setting code]\n"\
    "// File encoding must be in UTF-8!\n"\
    "// ------------------------------------------------------------------------------------------------------------\n"\
    "// ------------------------------------------------------------------------------------------------------------"
#endif

// Define the default RTW preferences file of the mod. Generated from my own vanilla RTW installation and slightly
// edited in some cases, such as the maximum screen resolution and enabling 32-bit depth UI rendering.
#ifndef defaultPreferencesData
#define defaultPreferencesData ""\
    "SAFE_REFRESH:TRUE\n"\
    "USE_TRIPLE_BUFFER:FALSE\n"\
    "USE_WIDESCREEN:FALSE\n"\
    "SUBTITLES:FALSE\n"\
    "MORALE:TRUE\n"\
    "FATIGUE:TRUE\n"\
    "LIMITED_AMMO:TRUE\n"\
    "SUPPLY:TRUE\n"\
    "FOG_OF_WAR:TRUE\n"\
    "RESTRICT_CAMERA:FALSE\n"\
    "DEFAULT_BATTLE_CAMERA:RTS\n"\
    "EVENT_CUTSCENES:TRUE\n"\
    "SMOKE:FALSE\n"\
    "DESYNC:FALSE\n"\
    "SPLASHES:FALSE\n"\
    "GLINTS:FALSE\n"\
    "REFLECTIONS:FALSE\n"\
    "SHADOWS:FALSE\n"\
    "VEGETATION:FALSE\n"\
    "DETAILED_UNIT_SHADOWS:FALSE\n"\
    "GLOSS_MAPPING:FALSE\n"\
    "STENCIL_SHADOWS:FALSE\n"\
    "MULTI_TEXTURE:FALSE\n"\
    "AUTO_SAVE:TRUE\n"\
    "SHOW_BANNERS:TRUE\n"\
    "UNIT_EXPERIENCE_UPGRADE_EFFECT:TRUE\n"\
    "UNIT_SIZE:20\n"\
    "MASTER_VOL:100\n"\
    "SPEECH_VOL:100\n"\
    "SFX_VOL:100\n"\
    "MUSIC_VOL:100\n"\
    "CAMERA_ROTATE:100\n"\
    "CAMERA_MOVE:100\n"\
    "CAMERA_FOV:75.000000\n"\
    "ENABLE_AUDIO:TRUE\n"\
    "ENABLE_MUSIC:TRUE\n"\
    "ENABLE_UNIT_SPEECH:TRUE\n"\
    "DISABLE_BACKGROUND_FMV:FALSE\n"\
    "DISABLE_ARROW_MARKERS:FALSE\n"\
    "AUDIO_3D_PROVIDER:Miles Fast 2D Positional Audio\n"\
    "CLOUD_TRANSITIONS:TRUE\n"\
    "GRASS_DISTANCE:75\n"\
    "AA_QUALITY:AA_OFF\n"\
    "STRATEGY_RESOLUTION:1024x768\n"\
    "STRATEGY_MAX_RESOLUTION:7680x4320\n"\
    "STRATEGY_32_BIT:TRUE\n"\
    "BATTLE_RESOLUTION:1024x768\n"\
    "BATTLE_MAX_RESOLUTION:7680x4320\n"\
    "BATTLE_32_BIT:TRUE\n"\
    "UNIT_DETAIL:LOW\n"\
    "BUILDING_DETAIL:LOW\n"\
    "TGA_CAPTURE_WIDTH:1280\n"\
    "TGA_CAPTURE_ASPECT:1.333333\n"\
    "TGA_CAPTURE_INPUT_SCALE:1\n"\
    "TGA_CAPTURE_RESERVE_SPACE:FALSE\n"\
    "LABEL_CHARACTERS:FALSE\n"\
    "LABEL_SETTLEMENTS:TRUE\n"\
    "MINIMAL_UI:TRUE\n"\
    "CAMPAIGN_MAP_MAX_SCROLL_SPEED_MIN_ZOOM:30\n"\
    "CAMPAIGN_MAP_MAX_SCROLL_SPEED_MAX_ZOOM:30\n"\
    "FIRST_TIME_PLAY:TRUE\n"\
    "ADVISOR_VERBOSITY:0\n"\
    "MUTE_ADVISOR:FALSE\n"\
    "BLIND_ADVISOR:FALSE\n"\
    "CAMPAIGN_MAP_CAMERA:SKIP_AI_FACTIONS\n"\
    "CAMPAIGN_MAP_SPEED_UP:FALSE\n"\
    "CAMPAIGN_MAP_GAME_SPEED:99\n"\
    "DISABLE_EVENTS:0\n"\
    "MICROMANAGE_ALL_SETTLEMENTS:TRUE\n"\
    "ADVANCED_STATS_ALWAYS:FALSE\n"\
    "UNIT_USE_SHADERS:FALSE\n"\
    "TERRAIN_QUALITY:LOW\n"\
    "EFFECT_QUALITY:LOW\n"\
    "VEGETATION_QUALITY:LOW\n"\
    "KEYSET:0\n"\
    "UNLIMITED_MEN_ON_BATTLEFIELD:FALSE\n"\
    "CAMPAIGN_MAP_CAMERA_SMOOTHING:TRUE\n"\
    "CHAT_MSG_DURATION:10\n"\
    "USE_QUICKCHAT:FALSE\n"\
    "GS_PIDS:FALSE\n"\
    "GS_PIDS_FULL:FALSE\n"\
    "GS_PIDS_CUR:TRUE\n"\
    "ALWAYS_LOGFILE:FALSE\n"\
    "TIMESTAMP_LOGFILE:FALSE\n"\
    "32BIT_UI:TRUE\n"\
    "GS_LOG_RESULTS:FALSE\n"\
    "CHAT_COLOURS:name=ffffa0,private=ff7707,game=94aadc,public=fff777,system=ffffff,lobby=fff777,setup=94aadc,playing=ff0707\n"\
    "EDIT_SETTLEMENT_NAMES:FALSE"
#endif

// Include some QWidget classes for handling data.
#include <QString>
#include <QList>

//
// Create a struct to handle the options for the campaign, unit roster (export_descr_unit.txt, EDU), or the tree types
// (specific for RTR) selectors in the main window that are, and must be set up in the launcher data file with syntax
// [dir/file]:[file path relative to RTR/data/]|[Option code]|[Display name for the option]|[Option type to be added].
struct OptionObject
{
    QString pathtype; // Type of option, if a directory or path.
    QString abspath; // Path of the option file or directory. Must be stored as an absolute path in the struct.
    QString optioncode; // Option code is the internal codename for the option in question.
    QString displayname; // Name that will be displayed in the UI.
    QString optiontype; // The option is for which selector (available so far are 'camp', 'edu', and 'trees').
};

//
// Create a struct to handle the options the player has selected in a previous run of the launcher (TODO: refactor).
struct PlayerOption
{
    QString object; // The code of the object in question.
    QString setting; // The setting of the selected object (e.g. true or false).
};

//
// Create a class to handle all the options for campaigns, EDUs, and trees as a QList of OptionObject structs.
class OptionData
{
private:
    // The QList of OptionObject structs.
    QList<OptionObject> listofobjs;

public:
    // 'Setter' method to add an OptionObject to the list at the tail end (append).
    void addObj(OptionObject optionobj);

    // Getter method to obtain the QList of OptionObject structs.
    QList<OptionObject> getListOfObjs();

    // 'Getter' method to obtain a QList where the OptionObject.optiontype equals some value passed as a QString objtype.
    QList<OptionObject> getListByOptionType(QString objtype);

    // 'Getter' method to obtain a QList where the pathtype of the OptionObject structs is a directory (i.e. == "dir").
    QList<OptionObject> getListOfDirs();

    // 'Getter' method to obtain a QList where the pathtype of the OptionObject structs is a file (i.e. == "file").
    QList<OptionObject> getListOfFiles();

    // 'Getter' method to obtain a QList where the optiontype of the OptionObject structs is EDU (i.e. == "edu").
    QList<OptionObject> getListOfEdus();

    // 'Getter' method to obtain a QList where the optiontype of the OptionObject structs is campaign (i.e. == "camp").
    QList<OptionObject> getListOfCamps();

    // 'Getter' method to obtain a QList where the optiontype of the OptionObject structs is trees (i.e. == "trees").
    QList<OptionObject> getListOfTrees();
};

#endif // OPTIONDATA_H
