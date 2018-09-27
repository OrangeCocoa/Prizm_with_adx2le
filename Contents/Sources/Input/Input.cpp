
#include<unordered_map>

#include"Input.h"

namespace Prizm
{
	namespace Input
	{
		constexpr int keyscount = 256;	//keyboard and mouse keys num

		static const std::unordered_map<const char*, KeyCode> key_map_ = []()
		{
			std::unordered_map<const char*, KeyCode> m;

			// mouse button
			m["LButton"] = m["lbutton"] = 1;
			m["RButton"] = m["rbutton"] = 2;
			m["MButton"] = m["mbutton"] = 4;	// center button

												// keyboard key
			m["Backspace"] = m["backspace"] = 8;
			m["Tab"] = m["tab"] = 9;
			m["Enter"] = m["enter"] = 13;
			m["Shift"] = m["shift"] = 16;
			m["Controll"] = m["controll"] = m["Ctrl"] = m["ctrl"] = 17;
			m["Alt"] = m["alt"] = 18;
			m["Escape"] = m["escape"] = m["ESC"] = m["esc"] = 27;

			m["Space"] = m["space"] = 32;

			m["PageUp"] = m["pageup"] = 33;
			m["PageDown"] = m["pagedown"] = 34;

			m["End"] = m["end"] = 35;
			m["Home"] = m["home"] = 36;

			m["Left"] = m["left"] = 37;
			m["Up"] = m["up"] = 38;
			m["Right"] = m["right"] = 39;
			m["Down"] = m["down"] = 40;

			m["Select"] = m["select"] = 41;
			m["Print"] = m["print"] = 42;
			m["Execute"] = m["execute"] = 43;
			m["PrintScreen"] = m["printscreen"] = 44;
			m["Insert"] = m["insert"] = 45;
			m["Delete"] = m["delete"] = 46;
			m["Help"] = m["help"] = 47;

			m["0"] = 48;
			m["1"] = 49;		m["2"] = 50;		m["3"] = 51;
			m["4"] = 52;		m["5"] = 53;		m["6"] = 54;
			m["7"] = 55;		m["8"] = 56;		m["9"] = 57;

			m["A"] = m["a"] = 65;
			m["B"] = m["b"] = 66;
			m["C"] = m["c"] = 67;
			m["D"] = m["d"] = 68;
			m["E"] = m["e"] = 69;
			m["F"] = m["f"] = 70;
			m["G"] = m["g"] = 71;
			m["H"] = m["h"] = 72;
			m["I"] = m["i"] = 73;
			m["J"] = m["j"] = 74;
			m["K"] = m["k"] = 75;
			m["L"] = m["l"] = 76;
			m["M"] = m["m"] = 77;
			m["S"] = m["s"] = 83;
			m["N"] = m["n"] = 78;
			m["O"] = m["o"] = 79;
			m["P"] = m["p"] = 80;
			m["Q"] = m["q"] = 81;
			m["R"] = m["r"] = 82;
			m["T"] = m["t"] = 84;
			m["U"] = m["u"] = 85;
			m["V"] = m["v"] = 86;
			m["W"] = m["w"] = 87;
			m["X"] = m["x"] = 88;
			m["Y"] = m["y"] = 89;
			m["Z"] = m["z"] = 90;

			m["LWindows"] = m["lwindows"] = 91;	// Left windows key

			m["Numpad0"] = 96;
			m["Numpad1"] = 97;		m["Numpad2"] = 98;		m["Numpad3"] = 99;
			m["Numpad4"] = 100;		m["Numpad5"] = 101;		m["Numpad6"] = 102;
			m["Numpad7"] = 103;		m["Numpad8"] = 104;		m["Numpad9"] = 105;
			m["Numpad*"] = m["*"] = 106;
			m["Numpad+"] = m["+"] = 107;
			m["Numpad-"] = m["-"] = 109;
			m["Numpad."] = m["."] = 110;
			m["Numpad/"] = m["/"] = 111;

			m[","] = 108;

			m["F1"] = 112;			m["F2"] = 113;			m["F3"] = 114;
			m["F4"] = 115;			m["F5"] = 116;			m["F6"] = 117;
			m["F7"] = 118;			m["F8"] = 119;			m["F9"] = 120;
			m["F10"] = 121;			m["F11"] = 122;			m["F12"] = 123;

			m["LShift"] = m["lshift"] = 160;
			m["RShift"] = m["rshift"] = 161;
			m["LControll"] = m["lcontroll"] = 162;
			m["RControll"] = m["rcontroll"] = 163;
			m["LAlt"] = m["lalt"] = 164;
			m["RAlt"] = m["ralt"] = 165;

			m[";"] = 186;
			m["\\"] = 220;
			m["'"] = 222;

			return m;
		}();

		// mouse_state
		bool  mouse_captured = false;
		POINT capture_position;
	
		// input state
		bool ignore_input = false;

		// keyboard
		bool keys[keyscount];			// Included mouse L, R, Center button.
		bool prev_keys[keyscount];

		// mouse
		bool buttons[17];
		long mouse_pos[2];
		long mouse_delta[2];
		short mouse_scroll = 0;

		// touch
		POINT touch_position[max_touchcount];
		POINT prev_touch_position[max_touchcount];
		long touch_delta[max_touchcount][2];
		DWORD touch_state[max_touchcount];
		DWORD prev_touch_state[max_touchcount];

		void Initialize(void)
		{
			memset(keys, false, sizeof(bool) * keyscount);
			memset(prev_keys, false, sizeof(bool) * keyscount);

			memset(mouse_delta, 0, sizeof(long) * 2);
			memset(mouse_pos, 0, sizeof(long) * 2);
		}

		void CaptureMouse(HWND window_handle, bool do_capture)
		{
			mouse_captured = do_capture;

			if(do_capture)
			{
				RECT rect;
				GetWindowRect(window_handle, &rect);

				constexpr int border_pixel_offset = 15;
				constexpr int title_pixel_offset = 30;
				rect.left += border_pixel_offset;
				rect.right -= border_pixel_offset;
				rect.top += border_pixel_offset + title_pixel_offset;
				rect.bottom -= border_pixel_offset;

				while (ShowCursor(FALSE) >= 0);
				ClipCursor(&rect);
				GetCursorPos(&capture_position);
				SetForegroundWindow(window_handle);
				SetFocus(window_handle);

				ignore_input = false;
			}
			else
			{
				ClipCursor(nullptr);
				SetCursorPos(capture_position.x, capture_position.y);

				while (ShowCursor(TRUE) <= 0);
				SetForegroundWindow(NULL);

				ignore_input = false;
			}
		}
		bool IsMouseCaptured(void) { return mouse_captured; }
		POINT MouseCapturePosition(void) { return capture_position; }

		void KeyDown(KeyCode key) { keys[key] = true; }
		void KeyUp(KeyCode key) { keys[key] = false; }

		void ButtonDown(KeyCode button) { buttons[button] = true; }
		void ButtonUp(KeyCode button) { buttons[button] = false; }
		void UpdateMousePos(long x, long y, short scroll)
		{
			mouse_delta[0] = x;
			mouse_delta[1] = y;
			mouse_pos[0] = 0;
			mouse_pos[1] = 0;
			mouse_scroll = scroll;
		}

		void UpdateTouchPos(long x, long y, int count, DWORD flags)
		{
			touch_position[count].x = x;
			touch_position[count].y = y;
			touch_state[count] = flags;
		}

		// key state
		bool IsKeyPress(KeyCode key) { return keys[key] && !ignore_input; }
		bool IsKeyPress(const char* key){ const KeyCode code = key_map_.at(key); return keys[code] && !ignore_input; }
		bool IsKeyPress(const std::string& key) { const KeyCode code = key_map_.at(key.c_str()); return keys[code] && !ignore_input; }

		bool IsKeyReleased(const char* key) { const KeyCode code = key_map_.at(key); return (!keys[code] && prev_keys[code]) && !ignore_input; }
		bool IsKeyReleased(const std::string& key) { const KeyCode code = key_map_.at(key.c_str()); return (!keys[code] && prev_keys[code]) && !ignore_input; }

		bool IsKeyTriggered(KeyCode key) { return !prev_keys[key] && keys[key] && !ignore_input; }
		bool IsKeyTriggered(const char* key) { const KeyCode code = key_map_.at(key); return !prev_keys[code] && keys[code] && !ignore_input; }
		bool IsKeyTriggered(const std::string& key) { const KeyCode code = key_map_.at(key.c_str()); return !prev_keys[code] && keys[code] && !ignore_input; }

		// mouse state
		bool IsMousePress(KeyCode button) { return buttons[button] && !ignore_input; }
		bool IsMousePress(const char* button) { const KeyCode code = key_map_.at(button); return buttons[code] && !ignore_input; }
		bool IsMousePress(const std::string& button) { const KeyCode code = key_map_.at(button.c_str()); return buttons[code] && !ignore_input; }

		bool IsScrollUp(void) { return mouse_scroll > 0 && !ignore_input; }
		bool IsScrollDown(void) { return mouse_scroll < 0 && !ignore_input; }

		int  MouseDeltaX(void) { return !ignore_input ? mouse_delta[0] : 0; }
		int  MouseDeltaY(void) { return !ignore_input ? mouse_delta[1] : 0; }

		// touch state
		bool IsTouchPress(int count) { return touch_state[count] & (TOUCHEVENTF_DOWN | TOUCHEVENTF_MOVE); }
		bool IsTouchMove(int count) { return touch_state[count] & TOUCHEVENTF_MOVE; }
		bool IsTouchReleased(int count) { return touch_state[count] & TOUCHEVENTF_UP; }
		bool IsTouchTriggered(int count) { return touch_state[count] & TOUCHEVENTF_DOWN; }

		int  TouchDeltaX(int count)
		{
			touch_delta[count][0] = touch_position[count].x - prev_touch_position[count].x;
			return !ignore_input ? touch_position[count].x : 0;
		}
		int  TouchDeltaY(int count)
		{
			touch_delta[count][1] = touch_position[count].y - prev_touch_position[count].y;
			return !ignore_input ? touch_position[count].y : 0;
		}

		const long* GetMouseDelta(void)
		{
			return mouse_delta;
		}

		const long* GetTouchDelta(int count)
		{
			touch_delta[count][0] = touch_position[count].x - prev_touch_position[count].x;
			touch_delta[count][1] = touch_position[count].y - prev_touch_position[count].y;
			return touch_delta[count];
		}

		// update end of frame
		void PostStateUpdate(void)
		{
			memcpy(prev_keys, keys, sizeof(bool) * keyscount);
			mouse_delta[0] = mouse_delta[1] = 0;
			mouse_scroll = 0;

			for (int i = 0; i < max_touchcount; ++i)
			{
				touch_delta[i][0] = touch_delta[i][1] = 0;
				prev_touch_state[i] = touch_state[i];
				touch_state[i] = 0;
				prev_touch_position[i] = touch_position[i];
			}
		}
	}
}