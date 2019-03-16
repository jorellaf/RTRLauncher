#ifndef DATAHEADER_H
#define DATAHEADER_H

#ifndef maxres
#define maxres "3840x2160"
#endif
#ifndef launcherDataFile
#define launcherDataFile "/launcher.dat"
#endif
#ifndef playerDataFile
#define playerDataFile "/player.dat"
#endif
#ifndef preferencesFile
#define preferencesFile "/../preferences/preferences.txt"
#endif

#ifndef defaultPlayerDat
#define defaultPlayerDat "// ------------------------------------------------------------------------------------------------------------\n"\
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

#ifndef defaultPreferencesData
#define defaultPreferencesData "SAFE_REFRESH:TRUE\n"\
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
"STRATEGY_MAX_RESOLUTION:3840x2160\n"\
"STRATEGY_32_BIT:TRUE\n"\
"BATTLE_RESOLUTION:1024x768\n"\
"BATTLE_MAX_RESOLUTION:3840x2160\n"\
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
"32BIT_UI:FALSE\n"\
"GS_LOG_RESULTS:FALSE\n"\
"CHAT_COLOURS:name=ffffa0,private=ff7707,game=94aadc,public=fff777,system=ffffff,lobby=fff777,setup=94aadc,playing=ff0707\n"\
"EDIT_SETTLEMENT_NAMES:FALSE"
#endif

#include <QString>
#include <QList>

struct OptionObject
{
    QString pathtype;
    QString abspath;
    QString optioncode;
    QString displayname;
    QString optiontype;
};

struct PlayerOption
{
    QString object;
    QString setting;
};

class ListOptionObjects
{
private:
    QList<OptionObject> listofobjs;
public:
    // filepath relative to RTW/RTR/data
    void addObj(OptionObject optionobj)
    {
        this->listofobjs.append(optionobj);
    }
    QList<OptionObject> getListOfObjs()
    {
        return this->listofobjs;
    }
    QList<OptionObject> getListByOptionType(QString objtype)
    {
        QList<OptionObject> newlist;
        foreach (OptionObject optionobj, this->listofobjs)
        {
            if (optionobj.optiontype==objtype)
                newlist.append(optionobj);
        }
        return newlist;
    }
    QList<OptionObject> getListOfDirs()
    {
        QList<OptionObject> newlist;
        foreach (OptionObject optionobj, this->listofobjs)
        {
            if (optionobj.pathtype=="dir")
                newlist.append(optionobj);
        }
        return newlist;
    }
    QList<OptionObject> getListOfFiles()
    {
        QList<OptionObject> newlist;
        foreach (OptionObject optionobj, this->listofobjs)
        {
            if (optionobj.pathtype=="file")
                newlist.append(optionobj);
        }
        return newlist;
    }
    QList<OptionObject> getListOfEdus()
    {
        QList<OptionObject> newlist;
        foreach (OptionObject optionobj, this->listofobjs)
        {
            if (optionobj.optiontype=="edu")
                newlist.append(optionobj);
        }
        return newlist;
    }
    QList<OptionObject> getListOfCamps()
    {
        QList<OptionObject> newlist;
        foreach (OptionObject optionobj, this->listofobjs)
        {
            if (optionobj.optiontype=="camp")
                newlist.append(optionobj);
        }
        return newlist;
    }
    QList<OptionObject> getListOfTrees()
    {
        QList<OptionObject> newlist;
        foreach (OptionObject optionobj, this->listofobjs)
        {
            if (optionobj.optiontype=="trees")
                newlist.append(optionobj);
        }
        return newlist;
    }
};

#endif // DATAHEADER_H
