#include "EventDesc.h"
#include <linux/input.h>

// EV_??? (Event Types) (EV_MAX 0x1F)
static const char* event_desc[EV_CNT] = { "EV_SYN", "EV_KEY", "EV_REL", "EV_ABS", "EV_MSC", "EV_SW", "0x06", "0x07",
                                          "0x08", "0x09", "0x0A", "0x0B", "0x0C", "0x0D", "0x0E", "0x0F",
                                          "0x10", "EV_LED", "EV_SND", "0x13", "EV_REP", "EV_FF", "EV_PWR", "EV_FF_STATUS",
                                          "0x18", "0x19", "0x1A", "0x1B", "0x1C", "0x1D", "0x1E", "0x1F" };

// (EV_SYNC 0x00) (Synchronization Events) (SYN_MAX 0x0F)
static const char* syn_desc[SYN_CNT] = { "SYN_REPORT", "SYN_CONFIG", "SYN_MT_REPORT", "SYN_DROPPED", "0x04", "0x05", "0x06", "0x07",
                                         "0x08", "0x09", "0x0A", "0x0B", "0x0C", "0x0D", "0x0E", "0x0F" };

// (EV_KEY 0x01) (Keys and Buttons) (KEY_MAX 0x2FF)
// (keys 0 to 255)
static const char* key_desc[] = {
    "KEY_RESERVED",
    "KEY_ESC",
    "KEY_1",
    "KEY_2",
    "KEY_3",
    "KEY_4",
    "KEY_5",
    "KEY_6",
    "KEY_7",
    "KEY_8",
    "KEY_9",
    "KEY_0",
    "KEY_MINUS",
    "KEY_EQUAL",
    "KEY_BACKSPACE",
    "KEY_TAB",
    "KEY_Q",
    "KEY_W",
    "KEY_E",
    "KEY_R",
    "KEY_T",
    "KEY_Y",
    "KEY_U",
    "KEY_I",
    "KEY_O",
    "KEY_P",
    "KEY_LEFTBRACE",
    "KEY_RIGHTBRACE",
    "KEY_ENTER",
    "KEY_LEFTCTRL",
    "KEY_A",
    "KEY_S",
    "KEY_D",
    "KEY_F",
    "KEY_G",
    "KEY_H",
    "KEY_J",
    "KEY_K",
    "KEY_L",
    "KEY_SEMICOLON",
    "KEY_APOSTROPHE",
    "KEY_GRAVE",
    "KEY_LEFTSHIFT",
    "KEY_BACKSLASH",
    "KEY_Z",
    "KEY_X",
    "KEY_C",
    "KEY_V",
    "KEY_B",
    "KEY_N",
    "KEY_M",
    "KEY_COMMA",
    "KEY_DOT",
    "KEY_SLASH",
    "KEY_RIGHTSHIFT",
    "KEY_KPASTERISK",
    "KEY_LEFTALT",
    "KEY_SPACE",
    "KEY_CAPSLOCK",
    "KEY_F1",
    "KEY_F2",
    "KEY_F3",
    "KEY_F4",
    "KEY_F5",
    "KEY_F6",
    "KEY_F7",
    "KEY_F8",
    "KEY_F9",
    "KEY_F10",
    "KEY_NUMLOCK",
    "KEY_SCROLLLOCK",
    "KEY_KP7",
    "KEY_KP8",
    "KEY_KP9",
    "KEY_KPMINUS",
    "KEY_KP4",
    "KEY_KP5",
    "KEY_KP6",
    "KEY_KPPLUS",
    "KEY_KP1",
    "KEY_KP2",
    "KEY_KP3",
    "KEY_KP0",
    "KEY_KPDOT",
    "84",					// not defined
    "KEY_ZENKAKUHANKAKU",
    "KEY_102ND",
    "KEY_F11",
    "KEY_F12",
    "KEY_RO",
    "KEY_KATAKANA",
    "KEY_HIRAGANA",
    "KEY_HENKAN",
    "KEY_KATAKANAHIRAGANA",
    "KEY_MUHENKAN",
    "KEY_KPJPCOMMA",
    "KEY_KPENTER",
    "KEY_RIGHTCTRL",
    "KEY_KPSLASH",
    "KEY_SYSRQ",
    "KEY_RIGHTALT",
    "KEY_LINEFEED",
    "KEY_HOME",
    "KEY_UP",
    "KEY_PAGEUP",
    "KEY_LEFT",
    "KEY_RIGHT",
    "KEY_END",
    "KEY_DOWN",
    "KEY_PAGEDOWN",
    "KEY_INSERT",
    "KEY_DELETE",
    "KEY_MACRO",
    "KEY_MUTE",
    "KEY_VOLUMEDOWN",
    "KEY_VOLUMEUP",
    "KEY_POWER",
    "KEY_KPEQUAL",
    "KEY_KPPLUSMINUS",
    "KEY_PAUSE",
    "KEY_SCALE",
    "KEY_KPCOMMA",
    "KEY_HANGEUL",			// KEY_HANGUEL
    "KEY_HANJA",
    "KEY_YEN",
    "KEY_LEFTMETA",
    "KEY_RIGHTMETA",
    "KEY_COMPOSE",
    "KEY_STOP",
    "KEY_AGAIN",
    "KEY_PROPS",
    "KEY_UNDO",
    "KEY_FRONT",
    "KEY_COPY",
    "KEY_OPEN",
    "KEY_PASTE",
    "KEY_FIND",
    "KEY_CUT",
    "KEY_HELP",
    "KEY_MENU",
    "KEY_CALC",
    "KEY_SETUP",
    "KEY_SLEEP",
    "KEY_WAKEUP",
    "KEY_FILE",
    "KEY_SENDFILE",
    "KEY_DELETEFILE",
    "KEY_XFER",
    "KEY_PROG1",
    "KEY_PROG2",
    "KEY_WWW",
    "KEY_MSDOS",        
    "KEY_SCREENLOCK",		// KEY_COFFEE
    "KEY_ROTATE_DISPLAY",	// KEY_DIRECTION
    "KEY_CYCLEWINDOWS",
    "KEY_MAIL",
    "KEY_BOOKMARKS",
    "KEY_COMPUTER",
    "KEY_BACK",
    "KEY_FORWARD",
    "KEY_CLOSECD",
    "KEY_EJECTCD",
    "KEY_EJECTCLOSECD",
    "KEY_NEXTSONG",
    "KEY_PLAYPAUSE",
    "KEY_PREVIOUSSONG",
    "KEY_STOPCD",
    "KEY_RECORD",
    "KEY_REWIND",
    "KEY_PHONE",
    "KEY_ISO",
    "KEY_CONFIG",
    "KEY_HOMEPAGE",
    "KEY_REFRESH",
    "KEY_EXIT",
    "KEY_MOVE",
    "KEY_EDIT",
    "KEY_SCROLLUP",
    "KEY_SCROLLDOWN",
    "KEY_KPLEFTPAREN",
    "KEY_KPRIGHTPAREN",
    "KEY_NEW",
    "KEY_REDO",
    "KEY_F13",
    "KEY_F14",
    "KEY_F15",
    "KEY_F16",
    "KEY_F17",
    "KEY_F18",
    "KEY_F19",
    "KEY_F20",
    "KEY_F21",
    "KEY_F22",
    "KEY_F23",
    "KEY_F24",
    "195",
    "196",
    "197",
    "198",
    "199",
    "KEY_PLAYCD",
    "KEY_PAUSECD",
    "KEY_PROG3",
    "KEY_PROG4",
    "KEY_ALL_APPLICATIONS", // KEY_DASHBOARD
    "KEY_SUSPEND",
    "KEY_CLOSE",
    "KEY_PLAY",
    "KEY_FASTFORWARD",
    "KEY_BASSBOOST",
    "KEY_PRINT",
    "KEY_HP",
    "KEY_CAMERA",
    "KEY_SOUND",
    "KEY_QUESTION",
    "KEY_EMAIL",
    "KEY_CHAT",
    "KEY_SEARCH",
    "KEY_CONNECT",
    "KEY_FINANCE",
    "KEY_SPORT",
    "KEY_SHOP",
    "KEY_ALTERASE",
    "KEY_CANCEL",
    "KEY_BRIGHTNESSDOWN",
    "KEY_BRIGHTNESSUP",
    "KEY_MEDIA",
    "KEY_SWITCHVIDEOMODE",
    "KEY_KBDILLUMTOGGLE",
    "KEY_KBDILLUMDOWN",
    "KEY_KBDILLUMUP",
    "KEY_SEND",
    "KEY_REPLY",
    "KEY_FORWARDMAIL",
    "KEY_SAVE",
    "KEY_DOCUMENTS",
    "KEY_BATTERY",
    "KEY_BLUETOOTH",
    "KEY_WLAN",
    "KEY_UWB",
    "KEY_UNKNOWN",
    "KEY_VIDEO_NEXT",
    "KEY_VIDEO_PREV",
    "KEY_BRIGHTNESS_CYCLE",
    "KEY_BRIGHTNESS_AUTO",  // KEY_BRIGHTNESS_ZERO
    "KEY_DISPLAY_OFF",
    "KEY_WWAN",             // KEY_WIMAX
    "KEY_RFKILL",
    "KEY_MICMUTE",
    "249",
    "250",
    "251",
    "252",
    "253",
    "254",
    "255",
};

// (keys 0x100 to 0x14F) need to subtract 0x100 from code before using this array
static const char* key_desc_100h[] = { "BTN_0", "BTN_1", "BTN_2", "BTN_3", "BTN_4", "BTN_5", "BTN_6", "BTN_7",
    "BTN_8", "BTN_9", "0x0A", "0x0B", "0x0C", "0x0D", "0x0E", "0x0F",
    "BTN_LEFT", "BTN_RIGHT", "BTN_MIDDLE", "BTN_SIDE", "BTN_EXTRA", "BTN_FORWARD", "BTN_BACK", "BTN_TASK",
    "0x18", "0x19", "0x1A", "0x1B", "0x1C", "0x1D", "0x1E", "0x1F",
    "BTN_JOYSTICK", "BTN_THUMB", "BTN_THUMB2", "BTN_TOP", "BTN_TOP2", "BTN_PINKIE", "BTN_BASE", "BTN_BASE2",
    "BTN_BASE3", "BTN_BASE4", "BTN_BASE5", "BTN_BASE6", "0x2C", "0x2D", "0x2E", "BTN_DEAD",
    "BTN_A", "BTN_B", "BTN_C", "BTN_X", "BTN_Y", "BTN_Z", "BTN_TL", "BTN_TR",
    "BTN_TL2", "BTN_TR2", "BTN_SELECT", "BTN_START", "BTN_MODE", "BTN_THUMBL", "BTN_THUMBR", "0x3F",
    "BTN_TOOL_PEN", "BTN_TOOL_RUBBER", "BTN_TOOL_BRUSH", "BTN_TOOL_PENCIL", "BTN_TOOL_AIRBRUSH", "BTN_TOOL_FINGER", "BTN_TOOL_MOUSE", "BTN_TOOL_LENS",
    "BTN_TOOL_QUINTTAP", "BTN_STYLUS3", "BTN_TOUCH", "BTN_STYLUS", "BTN_STYLUS2", "BTN_TOOL_DOUBLETAP", "BTN_TOOL_TRIPLETAP", "BTN_TOOL_QUADTAP" };

// (keys 0x160 to 0x17f) need to subtract 0x160 from code before using this array
// TODO: there are additional keys to add
static const char* key_desc_160h[] = { "KEY_OK", "KEY_SELECT", "KEY_GOTO", "KEY_CLEAR", "KEY_POWER2", "KEY_OPTION", "KEY_INFO", "KEY_TIME",
    "KEY_VENDOR", "KEY_ARCHIVE", "KEY_PROGRAM", "KEY_CHANNEL", "KEY_FAVORITES", "KEY_EPG", "KEY_PVR", "KEY_MHP",
    "KEY_LANGUAGE", "KEY_TITLE", "KEY_SUBTITLE", "KEY_ANGLE", "KEY_FULL_SCREEN", "KEY_MODE", "KEY_KEYBOARD", "KEY_ASPECT_RATIO",
    "KEY_PC", "KEY_TV", "KEY_TV2", "KEY_VCR", "KEY_VCR2", "KEY_SAT", "KEY_SAT2", "KEY_CD",
    "", "", "", "", "", "", "", "" };

// (EV_REL 0x02) (Relative Axes) (REL_MAX 0x0f)
static const char* rel_desc[REL_CNT] = { "REL_X", "REL_Y", "REL_Z", "REL_RX", "REL_RY", "REL_RZ", "REL_HWHEEL", "REL_DIAL",
                                           "REL_WHEEL", "REL_MISC", "REL_RESERVED", "REL_WHEEL_HI_RES", "REL_HWHEEL_HI_RES", "0x0D", "0x0E", "0x0F" };

// (EV_ABS 0x03) (Absolute Axes) (ABS_MAX 0x3F)
static const char* abs_desc[ABS_CNT] = { "ABS_X", "ABS_Y", "ABS_Z", "ABS_RX", "ABS_RY", "ABS_RZ", "ABS_THROTTLE", "ABS_RUDDER",
                                           "ABS_WHEEL", "ABS_GAS", "ABS_BRAKE", "0x0B", "0x0C", "0x0D", "0x0E", "0x0F",
                                           "ABS_HAT0X", "ABS_HAT0Y", "ABS_HAT1X", "ABS_HAT1Y", "ABS_HAT2X", "ABS_HAT2Y", "ABS_HAT3X", "ABS_HAT3Y",
                                           "ABS_PRESSURE", "ABS_DISTANCE", "ABS_TILT_X", "ABS_TILT_Y", "ABS_TOOL_WIDTH", "0x1D", "0x1E", "0x1F",
                                           "ABS_VOLUME", "ABS_PROFILE", "0x22", "0x23", "0x24", "0x25", "0x26", "0x27",
                                           "ABS_MISC", "0x29", "0x2A", "0x2B", "0x2C", "0x2D", "ABS_RESERVED", "ABS_MT_SLOT",
                                           "ABS_MT_TOUCH_MAJOR", "ABS_MT_TOUCH_MINOR", "ABS_MT_WIDTH_MAJOR", "ABS_MT_WIDTH_MINOR", "ABS_MT_ORIENTATION", "ABS_MT_POSITION_X", "ABS_MT_POSITION_Y", "ABS_MT_TOOL_TYPE",
                                           "ABS_MT_BLOB_ID", "ABS_MT_TRACKING_ID", "ABS_MT_PRESSURE", "ABS_MT_DISTANCE", "ABS_MT_TOOL_X", "ABS_MT_TOOL_Y", "0x3E", "0x3F" };

// (EV_MSC 0x04) (Misc Events) (MSC_MAX 0x07)
static const char* msc_desc[MSC_CNT] = { "MSC_SERIAL", "MSC_PULSELED", "MSC_GESTURE", "MSC_RAW", "MSC_SCAN", "MSC_TIMESTAMP", "0x06", "0x07" };

// (EV_SW 0x05) (Switch Events) (SW_MAX 0x10)
static const char* sw_desc[SW_CNT] = { "SW_LID", "SW_TABLET_MODE", "SW_HEADPHONE_INSERT", "SW_RFKILL_ALL", "SW_MICROPHONE_INSERT", "SW_DOCK", "SW_LINEOUT_INSERT", "SW_JACK_PHYSICAL_INSERT", 
                                       "SW_VIDEOOUT_INSERT", "SW_CAMERA_LENS_COVER", "SW_KEYPAD_SLIDE", "SW_FRONT_PROXIMITY", "SW_ROTATE_LOCK", "SW_LINEIN_INSERT", "SW_MUTE_DEVICE", "SW_PEN_INSERTED",
                                       "SW_MACHINE_COVER" };

// (EV_LED 0x11) (LED's) (LED_MAX 0x0F)
static const char* led_desc[LED_CNT] = { "LED_NUML", "LED_CAPSL", "LED_SCROLLL", "LED_COMPOSE", "LED_KANA", "LED_SLEEP", "LED_SUSPEND", "LED_MUTE",
                                         "LED_MISC", "LED_MAIL", "LED_CHARGING", "0x0B", "0x0C", "0x0D", "0x0E", "0x0F" };

// (EV_SND 0x12) (Sounds) (SND_MAX 0x07)
static const char* snd_desc[SND_CNT] = { "SND_CLICK", "SND_BELL", "SND_TONE", "0x03", "0x04", "0x05", "0x06", "0x07" };

// (EV_REP 0x14) (Autorepeat Values) (REP_MAX 0x01)
static const char* rep_desc[REP_CNT] = { "REP_DELAY", "REP_PERIOD" };

// (EV_FF  0x15)
// (EV_PWR 0x16)
// (EV_FF_STATUS 0x17)

const char* getEventTypeDescription(uint16_t type) {
    if (type <= EV_MAX) {
        return event_desc[type];
    }
    return "?";
}

const char* getEventCodeDescription(uint16_t type, uint16_t code) {
    if (type == EV_SYN && code <= SYN_MAX) { return syn_desc[code]; }
    else if (type == EV_KEY && code <= 255) { return key_desc[code]; }
    else if (type == EV_KEY && (code >= 0x100 && code <= 0x14F)) { return key_desc_100h[code-0x100]; }
    else if (type == EV_KEY && (code >= 0x160 && code <= 0x17F)) { return key_desc_160h[code-0x160]; }
    else if (type == EV_REL && code <= REL_MAX) { return rel_desc[code]; }
    else if (type == EV_ABS && code <= ABS_MAX) { return abs_desc[code]; }
    else if (type == EV_MSC && code <= MSC_MAX) { return msc_desc[code]; }
    else if (type == EV_SW && code <= SW_MAX) { return sw_desc[code]; }
    else if (type == EV_LED && code <= LED_MAX) { return led_desc[code]; }
    else if (type == EV_SND && code <= SND_MAX) { return snd_desc[code]; }
    else if (type == EV_REP && code <= REP_MAX) { return rep_desc[code]; }
    return "?";
}