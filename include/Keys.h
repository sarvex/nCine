#ifndef NCINE_KEYS
#define NCINE_KEYS

namespace ncine {

typedef enum
{
	KEY_UNKNOWN = -1,

// Common keysyms
	KEY_BACKSPACE,
	KEY_TAB,
	KEY_RETURN,
	KEY_ESCAPE,
	KEY_SPACE,
	KEY_QUOTE,
	KEY_PLUS,
	KEY_COMMA,
	KEY_MINUS,
	KEY_PERIOD,
	KEY_SLASH,
	KEY_0,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_SEMICOLON,
	KEY_LEFTBRACKET,
	KEY_BACKSLASH,
	KEY_RIGHTBRACKET,
	KEY_BACKQUOTE,

	KEY_A,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,
	KEY_DELETE,

	KEY_KP0,
	KEY_KP1,
	KEY_KP2,
	KEY_KP3,
	KEY_KP4,
	KEY_KP5,
	KEY_KP6,
	KEY_KP7,
	KEY_KP8,
	KEY_KP9,
	KEY_KP_PERIOD,
	KEY_KP_DIVIDE,
	KEY_KP_MULTIPLY,
	KEY_KP_MINUS,
	KEY_KP_PLUS,
	KEY_KP_ENTER,
	KEY_KP_EQUALS,

	KEY_UP,
	KEY_DOWN,
	KEY_RIGHT,
	KEY_LEFT,
	KEY_INSERT,
	KEY_HOME,
	KEY_END,
	KEY_PAGEUP,
	KEY_PAGEDOWN,

	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12,
	KEY_F13,
	KEY_F14,
	KEY_F15,

	KEY_NUM_LOCK,
	KEY_CAPS_LOCK,
	KEY_SCROLL_LOCK,
	KEY_RSHIFT,
	KEY_LSHIFT,
	KEY_RCTRL,
	KEY_LCTRL,
	KEY_RALT,
	KEY_LALT,
	KEY_RSUPER,
	KEY_LSUPER,
	KEY_PRINTSCREEN,
	KEY_PAUSE,
	KEY_MENU,

// SDL only keysyms
	KEY_CLEAR, // Android too
	KEY_EXCLAIM,
	KEY_QUOTEDBL,
	KEY_HASH,
	KEY_DOLLAR,
	KEY_AMPERSAND,
	KEY_LEFTPAREN,
	KEY_RIGHTPAREN,
	KEY_ASTERISK,
	KEY_COLON,
	KEY_LESS,
	KEY_EQUALS, // Android too
	KEY_GREATER,
	KEY_QUESTION,
	KEY_AT, // Android too
	KEY_CARET,
	KEY_UNDERSCORE,
	KEY_MODE,
	KEY_APPLICATION,
	KEY_HELP,
	KEY_SYSREQ, // Android too
	KEY_POWER, // Android too
	KEY_UNDO,

// GLFW only keysyms
	KEY_WORLD1,
	KEY_WORLD2,

// Android only keysyms
	KEY_SOFT_LEFT,
	KEY_SOFT_RIGHT,
	KEY_BACK,
	KEY_CALL,
	KEY_ENDCALL,
	KEY_STAR,
	KEY_POUND,
	KEY_DPAD_CENTER,
	KEY_VOLUME_UP,
	KEY_VOLUME_DOWN,
	KEY_CAMERA,
	KEY_SYM,
	KEY_EXPLORER,
	KEY_ENVELOPE,
	KEY_NUM,
	KEY_HEADSETHOOK,
	KEY_FOCUS,
	KEY_NOTIFICATION,
	KEY_SEARCH,
	KEY_MEDIA_PLAY_PAUSE,
	KEY_MEDIA_STOP,
	KEY_MEDIA_NEXT,
	KEY_MEDIA_PREVIOUS,
	KEY_MEDIA_REWIND,
	KEY_MEDIA_FAST_FORWARD,
	KEY_MUTE,
	KEY_PICTSYMBOLS,
	KEY_SWITCH_CHARSET,
	KEY_BUTTON_A,
	KEY_BUTTON_B,
	KEY_BUTTON_C,
	KEY_BUTTON_X,
	KEY_BUTTON_Y,
	KEY_BUTTON_Z,
	KEY_BUTTON_L1,
	KEY_BUTTON_R1,
	KEY_BUTTON_L2,
	KEY_BUTTON_R2,
	KEY_BUTTON_THUMBL,
	KEY_BUTTON_THUMBR,
	KEY_BUTTON_START,
	KEY_BUTTON_SELECT,
	KEY_BUTTON_MODE,
	// From API level 13
	KEY_FORWARD_DEL,
	KEY_FUNCTION,
	KEY_MOVE_HOME,
	KEY_MOVE_END,
	KEY_FORWARD,
	KEY_MEDIA_PLAY,
	KEY_MEDIA_PAUSE,
	KEY_MEDIA_CLOSE,
	KEY_MEDIA_EJECT,
	KEY_MEDIA_RECORD,
	KEY_KP_COMMA,
	KEY_KP_LEFTPAREN,
	KEY_KP_RIGHTPAREN,
	KEY_VOLUME_MUTE,
	KEY_INFO,
	KEY_CHANNEL_UP,
	KEY_CHANNEL_DOWN,
	KEY_ZOOM_IN,
	KEY_ZOOM_OUT,
	KEY_TV,
	KEY_WINDOW,
	KEY_GUIDE,
	KEY_DVR,
	KEY_BOOKMARK,
	KEY_CAPTIONS,
	KEY_SETTINGS,
	KEY_TV_POWER,
	KEY_TV_INPUT,
	KEY_STB_POWER,
	KEY_STB_INPUT,
	KEY_AVR_POWER,
	KEY_AVR_INPUT,
	KEY_PROG_RED,
	KEY_PROG_GREEN,
	KEY_PROG_YELLOW,
	KEY_PROG_BLUE,
	KEY_APP_SWITCH,
	KEY_BUTTON_1,
	KEY_BUTTON_2,
	KEY_BUTTON_3,
	KEY_BUTTON_4,
	KEY_BUTTON_5,
	KEY_BUTTON_6,
	KEY_BUTTON_7,
	KEY_BUTTON_8,
	KEY_BUTTON_9,
	KEY_BUTTON_10,
	KEY_BUTTON_11,
	KEY_BUTTON_12,
	KEY_BUTTON_13,
	KEY_BUTTON_14,
	KEY_BUTTON_15,
	KEY_BUTTON_16,

	KEYSYM_COUNT
} KeySym;

typedef enum
{
	KEYMOD_NONE = 0,

// Common keymods
	KEYMOD_SHIFT,
	KEYMOD_CTRL,
	KEYMOD_ALT,
	KEYMOD_SUPER,

// Android and SDL only keymods
	KEYMOD_LSHIFT,
	KEYMOD_RSHIFT,
	KEYMOD_LCTRL,
	KEYMOD_RCTRL,
	KEYMOD_LALT,
	KEYMOD_RALT,
	KEYMOD_LSUPER,
	KEYMOD_RSUPER,
	KEYMOD_NUM,
	KEYMOD_CAPS,

// SDL only keymods
	KEYMOD_MODE,

// Android only keymods
	KEYMOD_SCROLL,
	KEYMOD_SYM,

	KEYMOD_COUNT
} KeyMod;

}

#endif
