/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>

/* appearance */
static unsigned int borderpx  = 1;        /* border pixel of windows */
static unsigned int snap      = 32;       /* snap pixel */
static unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static unsigned int systrayspacing = 2;   /* systray spacing */
static int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static int showsystray        = 1;     /* 0 means no systray */
static int showbar            = 1;        /* 0 means no bar */
static int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "monospace:size=10" };
static const char dmenufont[]       = "monospace:size=10";
/*
 * Base16 dwm template by Daniel Mulford
 * Solarized Dark scheme by Ethan Schoonover (modified by aramisgithub)
 */
static const char col_base00[]      = "#002b36";
static const char col_base01[]      = "#073642";
static const char col_base02[]      = "#586e75";
static const char col_base04[]      = "#839496";
static const char col_base0C[]      = "#2aa198";
static const char col_base0D[]      = "#268bd2";
static char normbgcolor[]           = "#073642"; //col_base01;
static char normbordercolor[]       = "#586e75"; //col_base02;
static char normfgcolor[]           = "#839496"; //col_base04;
static char selfgcolor[]            = "#002b36"; //col_base00;
static char selbordercolor[]        = "#2aa198"; //col_base0C;
static char selbgcolor[]            = "#268bd2"; //col_base0D;
static char *colors[][3] = {
       /*               fg           bg           border   */
       [SchemeNorm] = { normfgcolor, normbgcolor, normbordercolor },
       [SchemeSel]  = { selfgcolor,  selbgcolor,  selbordercolor  },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	{ "Xpra",     NULL,       NULL,       0,            1,           -1 },
	{ "QtPass",     NULL,       NULL,       0,            1,           -1 },
	{ "Pinentry",     NULL,       NULL,       0,            1,           -1 },
	{ "Firefox",  NULL,       NULL,       1 << 8,       0,           -1 },
};

/* layout(s) */
static float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static int nmaster     = 1;    /* number of clients in master area */
static int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbordercolor, "-sf", selfgcolor, NULL };
static const char *termcmd[]  = { "st", "-e", "tmux", NULL };

/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
		{ "normbgcolor",        STRING,  &normbgcolor },
		{ "normbordercolor",    STRING,  &normbordercolor },
		{ "normfgcolor",        STRING,  &normfgcolor },
		{ "selbgcolor",         STRING,  &selbgcolor },
		{ "selbordercolor",     STRING,  &selbordercolor },
		{ "selfgcolor",         STRING,  &selfgcolor },
		{ "borderpx",          	INTEGER, &borderpx },
		{ "snap",          		INTEGER, &snap },
		{ "showbar",          	INTEGER, &showbar },
		{ "topbar",          	INTEGER, &topbar },
		{ "nmaster",          	INTEGER, &nmaster },
		{ "resizehints",       	INTEGER, &resizehints },
		{ "mfact",      	 	FLOAT,   &mfact },
};

static Key keys[] = {
	/* modifier                     key           function        argument */
	{ MODKEY,                       XK_p,         spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_Return,    spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_b,         togglebar,      {0} },
	{ MODKEY,                       XK_j,         focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,         focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,         incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,         incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,         setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,         setmfact,       {.f = +0.05} },
        // support for virtualbox inside Windows
	{ MODKEY,                       XK_Up,        focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_Down,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_KP_Add,      incnmaster,   {.i = +1 } },
	{ MODKEY,                       XK_KP_Subtract, incnmaster,   {.i = -1 } },
	{ MODKEY,                       XK_Left,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_Right,     setmfact,       {.f = +0.05} },

	{ MODKEY,                       XK_Return,    zoom,           {0} },
	{ MODKEY,                       XK_Tab,       view,           {0} },
	{ MODKEY|ShiftMask,             XK_c,         killclient,     {0} },
	{ MODKEY,                       XK_t,         setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,         setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,         setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_space,     setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,     togglefloating, {0} },
	{ MODKEY,                       XK_agrave,    view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_agrave,    tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,     focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_semicolon, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,     tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_semicolon, tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_ampersand,                 0)
	TAGKEYS(                        XK_eacute,                    1)
	TAGKEYS(                        XK_quotedbl,                  2)
	TAGKEYS(                        XK_apostrophe,                3)
	TAGKEYS(                        XK_parenleft,                 4)
	TAGKEYS(                        XK_minus,                     5)
	TAGKEYS(                        XK_egrave,                    6)
	TAGKEYS(                        XK_underscore,                7)
	TAGKEYS(                        XK_ccedilla,                  8)
	{ MODKEY|ShiftMask,             XK_q,         quit,           {0} },
	// for pulse compatible //
        { 0,                            XF86XK_AudioRaiseVolume, spawn, SHCMD("pamixer --allow-boost -i 3") },
        { 0,                            XF86XK_AudioLowerVolume, spawn, SHCMD("pamixer --allow-boost -d 3") },
        { 0,                            XF86XK_AudioMute,        spawn, SHCMD("pamixer -t") },
        { 0,                            XF86XK_AudioMicMute,     spawn, SHCMD("pactl set-source-mute @DEFAULT_SOURCE@ toggle") },
	// brightness control
        { 0,                            XF86MonBrightnessUp,     spawn, SHCMD("light -A 5") },
        { 0,                            XF86MonBrightnessDown,   spawn, SHCMD("light -U 5") },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

