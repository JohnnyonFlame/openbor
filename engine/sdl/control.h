/*
 * OpenBOR - http://www.chronocrash.com
 * -----------------------------------------------------------------------
 * All rights reserved, see LICENSE in OpenBOR root for details.
 *
 * Copyright (c)  OpenBOR Team
 */

#ifndef	CONTROL_H
#define	CONTROL_H

// Generic control stuff (keyboard+joystick).
#include <SDL_keycode.h>
#include "joysticks.h"

/*
0	x" Button A", \
1	x" Button B", \
2	x" Button X", \
3	x" Button Y", \
4	x" Left Shoulder", \
5	x" Right Shoulder", \
6	x" Left Trigger", \
7	x" Right Trigger", \
8	x" Left Stick", \
9	x" Right Stick", \
10	x" Select", \
11	x" Start", \
12	x" Menu", \
13	x" Left Stick Left", \
14	x" Left Stick Right", \
15	x" Left Stick Up", \
16	x" Left Stick Down", \
17	x" Right Stick Left", \
18	x" Right Stick Right", \
19	x" Right Stick Up", \
20	x" Right Stick Down", \
21	x" Up", \
22	x" Right", \
23	x" Down", \
24	x" Left",
*/

#define	CONTROL_ESC                 SDL_SCANCODE_ESCAPE
#define	CONTROL_DEFAULT1_START		((JOY_LIST_FIRST + 12))
#define	CONTROL_DEFAULT1_UP         ((JOY_LIST_FIRST + 22))
#define	CONTROL_DEFAULT1_DOWN		((JOY_LIST_FIRST + 24))
#define	CONTROL_DEFAULT1_LEFT		((JOY_LIST_FIRST + 25))
#define	CONTROL_DEFAULT1_RIGHT		((JOY_LIST_FIRST + 23))
#define	CONTROL_DEFAULT1_FIRE1		((JOY_LIST_FIRST + 1 ))
#define	CONTROL_DEFAULT1_FIRE2		((JOY_LIST_FIRST + 3 ))
#define	CONTROL_DEFAULT1_FIRE3		((JOY_LIST_FIRST + 5 ))
#define	CONTROL_DEFAULT1_FIRE4		((JOY_LIST_FIRST + 6 ))
#define	CONTROL_DEFAULT1_FIRE5		((JOY_LIST_FIRST + 2 ))
#define	CONTROL_DEFAULT1_FIRE6		((JOY_LIST_FIRST + 4 ))
#define	CONTROL_DEFAULT1_SCREENSHOT	((JOY_LIST_FIRST + 13))
#define	CONTROL_DEFAULT1_ESC        ((JOY_LIST_FIRST + 10))

#define	CONTROL_DEFAULT2_UP			((JOY_LIST_FIRST + 22) + JOY_MAX_INPUTS)
#define	CONTROL_DEFAULT2_RIGHT		((JOY_LIST_FIRST + 24) + JOY_MAX_INPUTS)
#define	CONTROL_DEFAULT2_DOWN		((JOY_LIST_FIRST + 25) + JOY_MAX_INPUTS)
#define	CONTROL_DEFAULT2_LEFT		((JOY_LIST_FIRST + 23) + JOY_MAX_INPUTS)
#define CONTROL_DEFAULT2_FIRE1		((JOY_LIST_FIRST + 1 ) + JOY_MAX_INPUTS)
#define CONTROL_DEFAULT2_FIRE2		((JOY_LIST_FIRST + 3 ) + JOY_MAX_INPUTS)
#define	CONTROL_DEFAULT2_FIRE3		((JOY_LIST_FIRST + 5 ) + JOY_MAX_INPUTS)
#define	CONTROL_DEFAULT2_FIRE4		((JOY_LIST_FIRST + 6 ) + JOY_MAX_INPUTS)
#define	CONTROL_DEFAULT2_FIRE5		((JOY_LIST_FIRST + 2 ) + JOY_MAX_INPUTS)
#define	CONTROL_DEFAULT2_FIRE6		((JOY_LIST_FIRST + 4 ) + JOY_MAX_INPUTS)
#define CONTROL_DEFAULT2_START		((JOY_LIST_FIRST + 12) + JOY_MAX_INPUTS)
#define CONTROL_DEFAULT2_SCREENSHOT ((JOY_LIST_FIRST + 13) + JOY_MAX_INPUTS)
#define	CONTROL_DEFAULT2_ESC        ((JOY_LIST_FIRST + 10) + JOY_MAX_INPUTS)

#define	CONTROL_DEFAULT3_UP			((JOY_LIST_FIRST + 22) + (JOY_MAX_INPUTS * 2))
#define	CONTROL_DEFAULT3_RIGHT		((JOY_LIST_FIRST + 24) + (JOY_MAX_INPUTS * 2))
#define	CONTROL_DEFAULT3_DOWN		((JOY_LIST_FIRST + 25) + (JOY_MAX_INPUTS * 2))
#define	CONTROL_DEFAULT3_LEFT		((JOY_LIST_FIRST + 23) + (JOY_MAX_INPUTS * 2))
#define CONTROL_DEFAULT3_FIRE1		((JOY_LIST_FIRST + 1 ) + (JOY_MAX_INPUTS * 2))
#define CONTROL_DEFAULT3_FIRE2		((JOY_LIST_FIRST + 3 ) + (JOY_MAX_INPUTS * 2))
#define	CONTROL_DEFAULT3_FIRE3		((JOY_LIST_FIRST + 5 ) + (JOY_MAX_INPUTS * 2))
#define	CONTROL_DEFAULT3_FIRE4		((JOY_LIST_FIRST + 6 ) + (JOY_MAX_INPUTS * 2))
#define	CONTROL_DEFAULT3_FIRE5		((JOY_LIST_FIRST + 2 ) + (JOY_MAX_INPUTS * 2))
#define	CONTROL_DEFAULT3_FIRE6		((JOY_LIST_FIRST + 4 ) + (JOY_MAX_INPUTS * 2))
#define CONTROL_DEFAULT3_START		((JOY_LIST_FIRST + 12) + (JOY_MAX_INPUTS * 2))
#define CONTROL_DEFAULT3_SCREENSHOT ((JOY_LIST_FIRST + 13) + (JOY_MAX_INPUTS * 2))
#define	CONTROL_DEFAULT3_ESC        ((JOY_LIST_FIRST + 10) + (JOY_MAX_INPUTS * 2))

#define	CONTROL_DEFAULT4_UP			((JOY_LIST_FIRST + 22) + (JOY_MAX_INPUTS * 3))
#define	CONTROL_DEFAULT4_RIGHT		((JOY_LIST_FIRST + 24) + (JOY_MAX_INPUTS * 3))
#define	CONTROL_DEFAULT4_DOWN		((JOY_LIST_FIRST + 25) + (JOY_MAX_INPUTS * 3))
#define	CONTROL_DEFAULT4_LEFT		((JOY_LIST_FIRST + 23) + (JOY_MAX_INPUTS * 3))
#define CONTROL_DEFAULT4_FIRE1		((JOY_LIST_FIRST + 1 ) + (JOY_MAX_INPUTS * 3))
#define CONTROL_DEFAULT4_FIRE2		((JOY_LIST_FIRST + 3 ) + (JOY_MAX_INPUTS * 3))
#define	CONTROL_DEFAULT4_FIRE3		((JOY_LIST_FIRST + 5 ) + (JOY_MAX_INPUTS * 3))
#define	CONTROL_DEFAULT4_FIRE4		((JOY_LIST_FIRST + 6 ) + (JOY_MAX_INPUTS * 3))
#define	CONTROL_DEFAULT4_FIRE5		((JOY_LIST_FIRST + 2 ) + (JOY_MAX_INPUTS * 3))
#define	CONTROL_DEFAULT4_FIRE6		((JOY_LIST_FIRST + 4 ) + (JOY_MAX_INPUTS * 3))
#define CONTROL_DEFAULT4_START		((JOY_LIST_FIRST + 12) + (JOY_MAX_INPUTS * 3))
#define CONTROL_DEFAULT4_SCREENSHOT ((JOY_LIST_FIRST + 13) + (JOY_MAX_INPUTS * 3))
#define	CONTROL_DEFAULT4_ESC        ((JOY_LIST_FIRST + 10) + (JOY_MAX_INPUTS * 3))

#define	CONTROL_NONE				((JOY_LIST_FIRST + 1) + (JOY_MAX_INPUTS * 99)) //Kratus (20-04-21) value used to clear all keys

#define JOYBUTTON(index, btn) (1 + i * JOY_MAX_INPUTS + btn)
#define JOYAXIS(index, axis, dir) (JOYBUTTON(index, joysticks[index].NumButtons) + 2 * axis + dir)

#define SDLK_FIRST SDL_SCANCODE_UNKNOWN
#define SDLK_LAST  SDL_NUM_SCANCODES
#define SDL_GetKeyState SDL_GetKeyboardState
#define SDL_JoystickName(x) SDL_JoystickName(joystick[x])

typedef struct{
	int		settings[JOY_MAX_INPUTS];
	u64		keyflags, newkeyflags;
	int		kb_break;
}s_playercontrols;

void open_joystick(int i);
void close_joystick(int i);
void control_exit();
void control_init(int joy_enable);
int control_usejoy(int enable);
int control_getjoyenabled();

void control_setkey(s_playercontrols * pcontrols, unsigned int flag, int key);
int control_scankey();

void reset_joystick_map(int i);
char* get_joystick_name(const char* name);
char *control_getkeyname(unsigned int keycode);
void control_update(s_playercontrols ** playercontrols, int numplayers);
void control_rumble(int port, int ratio, int msec);
int keyboard_getlastkey();

#ifdef ANDROID
#define MAX_POINTERS 30
typedef enum
{
    TOUCH_STATUS_UP,
    TOUCH_STATUS_DOWN
} touch_status;

typedef struct TouchStatus {
    float px[MAX_POINTERS];
    float py[MAX_POINTERS];
    SDL_FingerID pid[MAX_POINTERS];
    touch_status pstatus[MAX_POINTERS];
} TouchStatus;

int is_touchpad_vibration_enabled();
void control_update_android_touch(TouchStatus *touch_info, int maxp, Uint8* keystate, Uint8* keystate_def);
int is_touch_area(float x, float y);
#endif



#endif

