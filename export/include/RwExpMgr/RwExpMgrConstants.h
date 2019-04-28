#ifndef RWEXPMGRCONSTANTS_H
#define RWEXPMGRCONSTANTS_H

// This is the number of asset templates defined,
// the rest will be project templates.
#define NUM_ASSET_TEMPLATES             4

#define ANIMATION_TEMPLATE              "Animation.rwt"
#define STATIC_WORLD_TEMPLATE           "Static World.rwt"
#define ANIMATED_HIERARCHY_TEMPLATE     "Animated Hierarchy.rwt"
#define SPLINE_TEMPLATE                 "Spline.rwt"

#define GAMECUBE_TEMPLATE               "GameCube.rwt"
#define GENERIC_TEMPLATE                "Generic.rwt"
#define PS2_TEMPLATE                    "PS2.rwt"
#define XBOX_TEMPLATE                   "Xbox.rwt"
#define RWSTUDIO_TEMPLATE               "RwStudio.rwt"

// list indices
#define ANIMATION_LIST_INDEX            0
#define STATIC_WORLD_LIST_INDEX         1
#define ANIMATION_HIERARCHY_LIST_INDEX  2
#define SPLINE_LIST_INDEX               3

#define GAMECUBE_LIST_INDEX             4
#define GENERIC_LIST_INDEX              5
#define PS2_LIST_INDEX                  6
#define XBOX_LIST_INDEX                 7

static string   rwExpMgrLocationList[] =
{
    "anm", "bsp", "dff", "spl",
    "gamecube", "generic", "ps2", "xbox", "rwstudio"
};

static string   rwExpMgrTemplateList[] =
{
    ANIMATION_TEMPLATE, STATIC_WORLD_TEMPLATE,
    ANIMATED_HIERARCHY_TEMPLATE, SPLINE_TEMPLATE,
    GAMECUBE_TEMPLATE, GENERIC_TEMPLATE, PS2_TEMPLATE, XBOX_TEMPLATE, RWSTUDIO_TEMPLATE
};

#endif // RWEXPMGRCONSTANTS_H