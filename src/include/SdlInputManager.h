#ifndef CLASS_NCINE_SDLINPUTMANAGER
#define CLASS_NCINE_SDLINPUTMANAGER

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_mouse.h>
#include "IInputManager.h"

namespace ncine {

/// Utility functions to convert between engine key enumerations and SDL ones
class SdlKeys
{
  public:
	static KeySym keySymValueToEnum(int keysym);
	static KeyMod keyModValueToEnum(int keymod);
	static int enumToKeySymValue(KeySym keysym);
	static int enumToKeyModValue(KeyMod keymod);
	static int enumToScancode(KeySym keysym);
};

/// Information about SDL mouse state
class SdlMouseState : public MouseState
{
  public:
	SdlMouseState() : buttons_(0) { }

	inline bool isLeftButtonDown() const { return (buttons_ & SDL_BUTTON_LMASK) != 0; }
	inline bool isMiddleButtonDown() const { return (buttons_ & SDL_BUTTON_MMASK) != 0; }
	inline bool isRightButtonDown() const { return (buttons_ & SDL_BUTTON_RMASK) != 0; }
	inline bool isFourthButtonDown() const { return (buttons_ & SDL_BUTTON_X1MASK) != 0; }
	inline bool isFifthButtonDown() const { return (buttons_ & SDL_BUTTON_X2MASK) != 0; }

  private:
	unsigned int buttons_;

	friend class SdlInputManager;
};

/// Information about an SDL mouse event
class SdlMouseEvent : public MouseEvent
{
  public:
	SdlMouseEvent() : button_(0) { }

	inline bool isLeftButton() const { return button_ == SDL_BUTTON_LEFT; }
	inline bool isMiddleButton() const { return button_ == SDL_BUTTON_MIDDLE; }
	inline bool isRightButton() const { return button_ == SDL_BUTTON_RIGHT; }
	inline bool isFourthButton() const { return button_ == SDL_BUTTON_X1; }
	inline bool isFifthButton() const { return button_ == SDL_BUTTON_X2; }

  private:
	unsigned char button_;

	friend class SdlInputManager;
};

/// Information about an SDL scroll event
class SdlScrollEvent : public ScrollEvent
{
  public:
	SdlScrollEvent() { }

	friend class SdlInputManager;
};

/// Information about SDL keyboard state
class SdlKeyboardState : public KeyboardState
{
  public:
	SdlKeyboardState() { keyState_ = SDL_GetKeyboardState(NULL); }

	inline bool isKeyDown(KeySym key) const { return keyState_[SdlKeys::enumToScancode(key)] != 0; }

	friend class SdlInputManager;

  private:
	const unsigned char *keyState_;
};

/// The class for parsing and dispatching SDL input events
class SdlInputManager : public IInputManager
{
  public:
	/// The constructor takes care of opening available joysticks
	SdlInputManager();
	/// The destructor releases every opened joystick
	~SdlInputManager();

	static void parseEvent(const SDL_Event &event);

	inline const MouseState &mouseState()
	{
		mouseState_.buttons_ = SDL_GetMouseState(&mouseState_.x, &mouseState_.y);
		return mouseState_;
	}

	inline const KeyboardState &keyboardState() const	{ return keyboardState_; }

	bool isJoyPresent(int joyId) const;
	const char *joyName(int joyId) const;
	int joyNumButtons(int joyId) const;
	int joyNumAxes(int joyId) const;
	bool isJoyButtonPressed(int joyId, int buttonId) const;
	short int joyAxisValue(int joyId, int axisId) const;
	float joyAxisNormValue(int joyId, int axisId) const;

	void setMouseCursorMode(MouseCursorMode mode);

  private:
	static const unsigned int MaxNumJoysticks = 16;

	static SdlMouseState mouseState_;
	static SdlMouseEvent mouseEvent_;
	static SdlScrollEvent scrollEvent_;
	static SdlKeyboardState keyboardState_;
	static KeyboardEvent keyboardEvent_;

	static SDL_Joystick *sdlJoysticks_[MaxNumJoysticks];
	static JoyButtonEvent joyButtonEvent_;
	static JoyAxisEvent joyAxisEvent_;
	static JoyConnectionEvent joyConnectionEvent_;

	/// Private copy constructor
	SdlInputManager(const SdlInputManager &);
	/// Private assignment operator
	SdlInputManager &operator=(const SdlInputManager &);

	static short int hatEnumToAxisValue(unsigned char hatState, bool upDownAxis);
	static void handleJoyDeviceEvent(const SDL_Event &event);
};

}

#endif
