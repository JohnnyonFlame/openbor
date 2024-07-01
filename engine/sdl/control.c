/*
 * OpenBOR - http://www.chronocrash.com
 * -----------------------------------------------------------------------
 * All rights reserved, see LICENSE in OpenBOR root for details.
 *
 * Copyright (c)  OpenBOR Team
 */

// Generic control stuff (keyboard+joystick)

#include "video.h"
#include "globals.h"
#include "control.h"
#include "stristr.h"
#include "sblaster.h"
#include "joysticks.h"
#include "openbor.h"

#define T_AXIS 7000

#ifdef ANDROID
#include "jniutils.h"
#endif

SDL_GameController *controllers[JOY_LIST_TOTAL] = {NULL, NULL, NULL, NULL};  // SDL struct for joysticks
SDL_Haptic *joystick_haptic[JOY_LIST_TOTAL];   // SDL haptic for joysticks
static int usejoy;						        // To be or Not to be used?
static int numjoy;						        // Number of Joy(s) found
static int lastkey;						        // Last keyboard key Pressed
static int lastjoy;                             // Last joystick button/axis/hat input

int sdl_game_started  = 0;

extern int default_keys[MAX_BTN_NUM];
extern s_playercontrols default_control;

#ifdef ANDROID
extern int nativeWidth;
extern int nativeHeight;
static TouchStatus touch_info;
#endif

int get_free_ctrl_slot()
{
	for (int i = 0; i < JOY_LIST_TOTAL; i++)
	{
		if (controllers[i] == NULL)
			return i;
	}

	return -1;
}

int get_instance_id_slot(int which)
{
	SDL_GameController *ctrl = SDL_GameControllerFromInstanceID(which);
	for (int i = 0; i < JOY_LIST_TOTAL; i++)
	{
		if (controllers[i] == NULL)
			continue;

		if (controllers[i] == ctrl)
			return i;
	}

	return -1;
}

int get_gamecontroller_slot(SDL_GameController *ctrl)
{
	for (int i = 0; i < JOY_LIST_TOTAL; i++)
	{
		if (controllers[i] == NULL)
			continue;

		if (controllers[i] == ctrl)
			return i;
	}

	return -1;
}

/*
Here is where we aquiring all joystick events
and map them to BOR's layout.  Currently support
up to 4 controllers.
*/
void getPads(Uint8* keystate, Uint8* keystate_def)
{
	SDL_GameController *ctrl;
	int i, j, x, axis, btns, slot;
	SDL_Event ev;
	while(SDL_PollEvent(&ev))
	{
		switch(ev.type)
		{
			case SDL_KEYDOWN:
				lastkey = ev.key.keysym.scancode;
				if((keystate[SDL_SCANCODE_LALT] || keystate[SDL_SCANCODE_RALT]) && (lastkey == SDL_SCANCODE_RETURN))
				{
					video_fullscreen_flip();
					keystate[SDL_SCANCODE_RETURN] = 0;
				}
				if(lastkey != SDL_SCANCODE_F10) break;
#ifdef ANDROID
			case SDL_FINGERDOWN:
			{
				for(i=0; i<MAX_POINTERS; i++)
				{
					if(touch_info.pstatus[i] == TOUCH_STATUS_UP)
					{
						touch_info.pid[i] = ev.tfinger.fingerId;
						touch_info.px[i] = ev.tfinger.x*nativeWidth;
						touch_info.py[i] = ev.tfinger.y*nativeHeight;
						touch_info.pstatus[i] = TOUCH_STATUS_DOWN;

            // migration for White Dragon's vibration logic from SDLActivity.java
            if (is_touchpad_vibration_enabled() &&
                is_touch_area(touch_info.px[i], touch_info.py[i]))
            {
              jniutils_vibrate_device();
            }

						break;
					}
				}
				control_update_android_touch(&touch_info, MAX_POINTERS, keystate, keystate_def);
			}
				break;
			case SDL_FINGERUP:
			{
				for(i=0; i<MAX_POINTERS; i++)
				{
					if(touch_info.pid[i] == ev.tfinger.fingerId)
					{
						touch_info.pstatus[i] = TOUCH_STATUS_UP;
						break;
					}
				}
				control_update_android_touch(&touch_info, MAX_POINTERS, keystate, keystate_def);
			}
				break;
			case SDL_FINGERMOTION:
			{
				for(i=0; i<MAX_POINTERS; i++)
				{
					if(touch_info.pid[i] == ev.tfinger.fingerId)
					{
						touch_info.px[i] = ev.tfinger.x*nativeWidth;
						touch_info.py[i] = ev.tfinger.y*nativeHeight;
						touch_info.pstatus[i] = TOUCH_STATUS_DOWN;
						break;
					}
				}
				control_update_android_touch(&touch_info, MAX_POINTERS, keystate, keystate_def);
			}
				break;
#endif
			case SDL_QUIT:
				borShutdown(0, DEFAULT_SHUTDOWN_MESSAGE);
				break;

			case SDL_CONTROLLERDEVICEADDED:
				ctrl = SDL_GameControllerOpen(ev.cdevice.which); // different than SDL_CONTROLLERDEVICEREMOVED (which is instance id)

				// Already scanned, so ignore me.
				if (get_gamecontroller_slot(ctrl) != -1)
					break;

				// A hotplug more likely, handle it.
				slot = get_free_ctrl_slot();
				if (slot == -1)
					SDL_GameControllerClose(ctrl);
				
				open_joystick(slot);
				controllers[slot] = ctrl;
				break;

			case SDL_CONTROLLERDEVICEREMOVED:
				slot = get_instance_id_slot(ev.cdevice.which); // different than SDL_CONTROLLERDEVICEADDED (which is joystick id)
				if (slot == -1)
					break;

				SDL_GameControllerClose(controllers[slot]);
				controllers[slot] = NULL;
				break;

			default:
				break;
		}

	}

	SDL_GameControllerUpdate();
	for (i = 0; i < JOY_LIST_TOTAL; i++)
	{
		u64 prev_state = joysticks[i].Data;
		joysticks[i].Axes = joysticks[i].Hats = joysticks[i].Buttons = 0;
		ctrl = controllers[i];
		if (ctrl == NULL)
			continue;

		j = 0;
		joysticks[i].Buttons |= (SDL_GameControllerGetButton(ctrl, SDL_CONTROLLER_BUTTON_A)) << j++;
		joysticks[i].Buttons |= (SDL_GameControllerGetButton(ctrl, SDL_CONTROLLER_BUTTON_B)) << j++;
		joysticks[i].Buttons |= (SDL_GameControllerGetButton(ctrl, SDL_CONTROLLER_BUTTON_X)) << j++;
		joysticks[i].Buttons |= (SDL_GameControllerGetButton(ctrl, SDL_CONTROLLER_BUTTON_Y)) << j++;
		joysticks[i].Buttons |= (SDL_GameControllerGetButton(ctrl, SDL_CONTROLLER_BUTTON_LEFTSHOULDER)) << j++;
		joysticks[i].Buttons |= (SDL_GameControllerGetButton(ctrl, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER)) << j++;
		joysticks[i].Buttons |= (SDL_GameControllerGetAxis  (ctrl, SDL_CONTROLLER_AXIS_TRIGGERLEFT) > 8000) << j++;
		joysticks[i].Buttons |= (SDL_GameControllerGetAxis  (ctrl, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) > 8000) << j++;
		joysticks[i].Buttons |= (SDL_GameControllerGetButton(ctrl, SDL_CONTROLLER_BUTTON_LEFTSTICK)) << j++;
		joysticks[i].Buttons |= (SDL_GameControllerGetButton(ctrl, SDL_CONTROLLER_BUTTON_RIGHTSTICK)) << j++;
		joysticks[i].Buttons |= (SDL_GameControllerGetButton(ctrl, SDL_CONTROLLER_BUTTON_BACK)) << j++;
		joysticks[i].Buttons |= (SDL_GameControllerGetButton(ctrl, SDL_CONTROLLER_BUTTON_START)) << j++;
		joysticks[i].Buttons |= (SDL_GameControllerGetButton(ctrl, SDL_CONTROLLER_BUTTON_GUIDE)) << j++;
		/* Newer SDL2 builds have paddle support and all, but let's omit for now. */
		btns = j;

		j = 0;
		joysticks[i].Hats |= SDL_GameControllerGetButton(ctrl, SDL_CONTROLLER_BUTTON_DPAD_UP) << j++;
		joysticks[i].Hats |= SDL_GameControllerGetButton(ctrl, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) << j++;
		joysticks[i].Hats |= SDL_GameControllerGetButton(ctrl, SDL_CONTROLLER_BUTTON_DPAD_DOWN) << j++;
		joysticks[i].Hats |= SDL_GameControllerGetButton(ctrl, SDL_CONTROLLER_BUTTON_DPAD_LEFT) << j++;

		for(j = 0; j < joysticks[i].NumAxes; j++)
		{
			axis = SDL_GameControllerGetAxis(ctrl, SDL_CONTROLLER_AXIS_LEFTX + j);
			if(axis < -1*T_AXIS)  { joysticks[i].Axes |= 0x01 << (j*2); }
			if(axis >    T_AXIS)  { joysticks[i].Axes |= 0x02 << (j*2); }
		}

		joysticks[i].Data  = joysticks[i].Buttons;
		joysticks[i].Data |= joysticks[i].Axes << btns;
		joysticks[i].Data |= joysticks[i].Hats << (btns + (2 * 4)); /* axis = 4 is hardcoded... triggers are treated as buttons */

		//   We check the last controller state by checking if any key/axis/hat that
		// wasn't pressed (~prev_state) is now been pressed.
		//   Then, we find the index of said press with __builtin_clzll, which counts the leading
		// zeroes until the last change.
		// 0000000001100000 - last state
		// 0000000000100000 - new state
		// ----------x----- -> change = (16 - 10) = 6
		u64 lastbtn = 0;
		u64 lastbtnmask = ~prev_state & joysticks[i].Data;
		if (lastbtnmask != 0) {
			lastbtn = 63 - __builtin_clzll(lastbtnmask);
			lastjoy = 1 + i * JOY_MAX_INPUTS + lastbtn; 
		}
	}
}

/*
Convert binary masked data to indexes
*/
static int flag_to_index(u64 flag)
{
	int index = 0;
	u64 bit = 1;
	while(!((bit<<index)&flag) && index<JOY_MAX_INPUTS-1) ++index;
	return index;
}

char* get_joystick_name(const char* name)
{
    char lname[strlen(name) + 1];

    if (strlen(name) <= 0) return JOY_UNKNOWN_NAME;
    strcpy(lname,name);
    for(int i = 0; lname[i]; i++)
    {
        lname[i] = tolower(lname[i]);
    }
    if ( strstr(lname, "null") == NULL ) return JOY_UNKNOWN_NAME;
    return ( (char*)name );
}

/*
Search for usb joysticks. Set
types, defaults and keynames.
*/
void joystick_scan(int scan)
{
	int i, n_joy, n_ctrl = 0;

	n_joy = SDL_NumJoysticks();
	for (int i = 0; i < n_joy; i++)
	{
		if (n_ctrl == 4)
			break;

		if (!SDL_IsGameController(i))
			continue;

		controllers[n_ctrl] = SDL_GameControllerOpen(i);
		open_joystick(n_ctrl);
		n_ctrl++;
	}
}

/*
Open a single joystick
*/
void open_joystick(int i)
{
	if (controllers[i] == NULL)
		return;

	joysticks[i].NumHats = 4;
	joysticks[i].NumAxes = 4;
	joysticks[i].NumButtons = 12;
	strncpy(joysticks[i].Name, SDL_GameControllerName(controllers[i]), MAX_BUFFER_LEN);
	printf("Joystick: \"%s\" connected at port: %d\n", joysticks[i].Name, i);
}

void reset_joystick_map(int i)
{
	memset(joysticks[i].Name,0,sizeof(joysticks[i].Name));
	memset(joysticks[i].KeyName,0,sizeof(joysticks[i].KeyName));
	joysticks[i].Type = 0;
	joysticks[i].NumHats = 0;
	joysticks[i].NumAxes = 0;
	joysticks[i].NumButtons = 0;
	joysticks[i].Hats = 0;
	joysticks[i].Axes = 0;
	joysticks[i].Buttons = 0;
	joysticks[i].Data = 0;
}

/*
Reset All data back to Zero and
destroy all SDL Joystick data.
*/
void control_exit()
{
	for (int i = 0; i < JOY_LIST_TOTAL; i++)
	{
		if (controllers[i] == NULL)
			continue;

		SDL_GameControllerClose(controllers[i]);
		controllers[i] = NULL;
	}
}

/*
Create default values for joysticks if enabled.
Then scan for joysticks and update their data.
*/
void control_init(int joy_enable)
{
	int i;

	usejoy = joy_enable;
	if (usejoy)
	{
		SDL_Init(SDL_INIT_GAMECONTROLLER);
	}

	//memset(joysticks, 0, sizeof(s_joysticks) * JOY_LIST_TOTAL);
	for(i = 0; i < JOY_LIST_TOTAL; i++)
	{
		if (controllers[i] != NULL)
		{
			SDL_GameControllerClose(controllers[i]);
			controllers[i] = NULL;
		}
		reset_joystick_map(i);
	}
	joystick_scan(usejoy);

#ifdef ANDROID
	for(i = 0; i < MAX_POINTERS; i++)
    {
        touch_info.pstatus[i] = TOUCH_STATUS_UP;
    }
#endif
}

#define GAMECONTROLLERNAMES(x) \
	x" Button A", \
	x" Button B", \
	x" Button X", \
	x" Button Y", \
	x" Left Shoulder", \
	x" Right Shoulder", \
	x" Left Trigger", \
	x" Right Trigger", \
	x" Left Stick", \
	x" Right Stick", \
	x" Select", \
	x" Start", \
	x" Menu", \
	x" Left Stick Left", \
	x" Left Stick Right", \
	x" Left Stick Up", \
	x" Left Stick Down", \
	x" Right Stick Left", \
	x" Right Stick Right", \
	x" Right Stick Up", \
	x" Right Stick Down", \
	x" Up", \
	x" Right", \
	x" Down", \
	x" Left",

char *keynames[4][25] = {
	{ GAMECONTROLLERNAMES("P1") },
	{ GAMECONTROLLERNAMES("P2") },
	{ GAMECONTROLLERNAMES("P3") },
	{ GAMECONTROLLERNAMES("P4") }
};

char *control_getkeyname(unsigned int keycode)
{
	if(keycode > SDLK_FIRST && keycode < SDLK_LAST)
		return SDL_GetScancodeName(keycode);
	else if (keycode >= JOY_LIST_FIRST && keycode < JOY_LIST_LAST) {
		int code = (keycode - JOY_LIST_FIRST - 1) % JOY_MAX_INPUTS;
		int index = (keycode - JOY_LIST_FIRST - 1) / JOY_MAX_INPUTS;

		// Within bounds of the known joystick codes?
		if (index < (sizeof(keynames) / sizeof(keynames[0])))
			return keynames[index][code];
	}
	
	return "...";
}

/*
Set global variable, which is used for
enabling and disabling all joysticks.
*/
int control_usejoy(int enable)
{
	usejoy = enable;
	return 0;
}

#if ANDROID
/*
Get if touchscreen vibration is active
*/
int is_touchpad_vibration_enabled()
{
	return savedata.is_touchpad_vibration_enabled;
}
#endif

/*
Only used in openbor.c to get current status
of joystick usage.
*/
int control_getjoyenabled()
{
	return usejoy;
}


void control_setkey(s_playercontrols * pcontrols, unsigned int flag, int key)
{
	if(!pcontrols) return;
	pcontrols->settings[flag_to_index(flag)] = key;
	pcontrols->keyflags = pcontrols->newkeyflags = 0;
}

#if ANDROID
/*
Android touch logic, the rest of the code is in android/jni/video.c,
maybe they'll be merged someday.
*/
extern float bx[MAXTOUCHB];
extern float by[MAXTOUCHB];
extern float br[MAXTOUCHB];
extern unsigned touchstates[MAXTOUCHB];
int hide_t = 5000;
void control_update_android_touch(TouchStatus *touch_info, int maxp, Uint8* keystate, Uint8* keystate_def)
{
	int i, j;
	float tx, ty, tr;
	float r[MAXTOUCHB];
	float dirx, diry, circlea, circleb, tan;

	memset(touchstates, 0, sizeof(touchstates));

	for(j=0; j<MAXTOUCHB; j++)
	{
		r[j] = br[j]*br[j]*(1.5*1.5);
	}
	dirx = (bx[SDID_MOVERIGHT]+bx[SDID_MOVELEFT])/2.0;
	diry = (by[SDID_MOVEUP]+by[SDID_MOVEDOWN])/2.0;
	circlea = bx[SDID_MOVERIGHT]-dirx-br[SDID_MOVEUP];
	circleb = bx[SDID_MOVERIGHT]-dirx+br[SDID_MOVEUP]*1.5;
	circlea *= circlea;
	circleb *= circleb;
	#define tana 0.577350f
	#define tanb 1.732051f
	for (i=0; i<maxp; i++)
	{
		if(touch_info->pstatus[i] == TOUCH_STATUS_UP) continue;
		tx = touch_info->px[i]-dirx;
		ty = touch_info->py[i]-diry;
		tr = tx*tx + ty*ty;
		//direction button logic is different, check a ring instead of individual buttons
		if(tr>circlea && tr<=circleb)
		{
			if(tx<0)
			{
				tan = ty/tx;
				if(tan>=-tana && tan<=tana)
                {
					touchstates[SDID_MOVELEFT] = 1;
				}
				else if(tan<-tanb)
                {
					touchstates[SDID_MOVEDOWN] = 1;
				}
				else if(tan>tanb)
				{
					touchstates[SDID_MOVEUP] = 1;
				}
				else if(ty<0)
				{
					touchstates[SDID_MOVEUP] = touchstates[SDID_MOVELEFT] = 1;
				}
				else
				{
					touchstates[SDID_MOVELEFT] = touchstates[SDID_MOVEDOWN] = 1;
                }
			}
			else if(tx>0)
			{
				tan = ty/tx;
				if(tan>=-tana && tan<=tana)
                {
					touchstates[SDID_MOVERIGHT] = 1;
				}
				else if(tan<-tanb)
				{
					touchstates[SDID_MOVEUP] = 1;
				}
				else if(tan>tanb)
                {
					touchstates[SDID_MOVEDOWN] = 1;
				}
				else if(ty<0)
                {
					touchstates[SDID_MOVEUP] = touchstates[SDID_MOVERIGHT] = 1;
				}
				else
                {
					touchstates[SDID_MOVERIGHT] = touchstates[SDID_MOVEDOWN] = 1;
                }
			}
			else
			{
				if(ty>0)
				{
                    touchstates[SDID_MOVEDOWN] = 1;
				}
				else
				{
				    touchstates[SDID_MOVEUP] = 1;
                }
			}
		}
		//rest buttons
		for(j=0; j<MAXTOUCHB; j++)
		{
			if(j==SDID_MOVERIGHT || j==SDID_MOVEUP ||
				j==SDID_MOVELEFT || j==SDID_MOVEDOWN)
				continue;
			tx = touch_info->px[i]-bx[j];
			ty = touch_info->py[i]-by[j];
			tr = tx*tx + ty*ty;
			if(tr<=r[j])
            {
                touchstates[j] = 1;
            }
		}
	}
	#undef tana
	#undef tanb

	hide_t = timer_gettick() + 5000;

	//map to current user settings
	extern s_savedata savedata;
	#define pc(x) savedata.keys[0][x]
	keystate[pc(SDID_MOVEUP)] = touchstates[SDID_MOVEUP];
	keystate[pc(SDID_MOVEDOWN)] = touchstates[SDID_MOVEDOWN];
	keystate[pc(SDID_MOVELEFT)] = touchstates[SDID_MOVELEFT];
	keystate[pc(SDID_MOVERIGHT)] = touchstates[SDID_MOVERIGHT];
	keystate[pc(SDID_ATTACK)] = touchstates[SDID_ATTACK];
	keystate[pc(SDID_ATTACK2)] = touchstates[SDID_ATTACK2];
	keystate[pc(SDID_ATTACK3)] = touchstates[SDID_ATTACK3];
	keystate[pc(SDID_ATTACK4)] = touchstates[SDID_ATTACK4];
	keystate[pc(SDID_JUMP)] = touchstates[SDID_JUMP];
	keystate[pc(SDID_SPECIAL)] = touchstates[SDID_SPECIAL];
	keystate[pc(SDID_START)] = touchstates[SDID_START];
	keystate[pc(SDID_SCREENSHOT)] = touchstates[SDID_SCREENSHOT];
	#undef pc

	//use default value for touch key mapping
    keystate_def[default_keys[SDID_MOVEUP]]    = touchstates[SDID_MOVEUP];
    keystate_def[default_keys[SDID_MOVEDOWN]]  = touchstates[SDID_MOVEDOWN];
    keystate_def[default_keys[SDID_MOVELEFT]]  = touchstates[SDID_MOVELEFT];
    keystate_def[default_keys[SDID_MOVERIGHT]] = touchstates[SDID_MOVERIGHT];
    keystate_def[default_keys[SDID_ATTACK]]    = touchstates[SDID_ATTACK];
    keystate_def[default_keys[SDID_ATTACK2]]   = touchstates[SDID_ATTACK2];
    keystate_def[default_keys[SDID_ATTACK3]]   = touchstates[SDID_ATTACK3];
    keystate_def[default_keys[SDID_ATTACK4]]   = touchstates[SDID_ATTACK4];
    keystate_def[default_keys[SDID_JUMP]]      = touchstates[SDID_JUMP];
    keystate_def[default_keys[SDID_SPECIAL]]   = touchstates[SDID_SPECIAL];
    keystate_def[default_keys[SDID_START]]     = touchstates[SDID_START];
    keystate_def[default_keys[SDID_SCREENSHOT]] = touchstates[SDID_SCREENSHOT];

    keystate[CONTROL_ESC] = keystate_def[CONTROL_ESC] = touchstates[SDID_ESC];

    return;
}

int is_touch_area(float x, float y)
{
	int j;
	float tx, ty, tr;
	float r[MAXTOUCHB];
	float dirx, diry, circlea, circleb, tan;

	for(j=0; j<MAXTOUCHB; j++)
	{
		r[j] = br[j]*br[j]*(1.5*1.5);
	}
	dirx = (bx[SDID_MOVERIGHT]+bx[SDID_MOVELEFT])/2.0;
	diry = (by[SDID_MOVEUP]+by[SDID_MOVEDOWN])/2.0;
	circlea = bx[SDID_MOVERIGHT]-dirx-br[SDID_MOVEUP];
	circleb = bx[SDID_MOVERIGHT]-dirx+br[SDID_MOVEUP]*1.5;
	circlea *= circlea;
	circleb *= circleb;
	#define tana 0.577350f
	#define tanb 1.732051f
    tx = x-dirx;
    ty = y-diry;
    tr = tx*tx + ty*ty;
    //direction button logic is different, check a ring instead of individual buttons
    if(tr>circlea && tr<=circleb)
    {
        if(tx<0)
        {
            tan = ty/tx;
            if(tan>=-tana && tan<=tana)
            {
                return 1;
            }
            else if(tan<-tanb)
            {
                return 1;
            }
            else if(tan>tanb)
            {
                return 1;
            }
            else if(ty<0)
            {
                return 1;
            }
            else
            {
                return 1;
            }
        }
        else if(tx>0)
        {
            tan = ty/tx;
            if(tan>=-tana && tan<=tana)
            {
                return 1;
            }
            else if(tan<-tanb)
            {
                return 1;
            }
            else if(tan>tanb)
            {
                return 1;
            }
            else if(ty<0)
            {
                return 1;
            }
            else
            {
                return 1;
            }
        }
        else
        {
            if(ty>0)
            {
                return 1;
            }
            else
            {
                return 1;
            }
        }
    }
    //rest buttons
    for(j=0; j<MAXTOUCHB; j++)
    {
        if(j==SDID_MOVERIGHT || j==SDID_MOVEUP ||
            j==SDID_MOVELEFT || j==SDID_MOVEDOWN)
            continue;
        tx = x-bx[j];
        ty = y-by[j];
        tr = tx*tx + ty*ty;
        if(tr<=r[j])
        {
            return 1;
        }
    }
	#undef tana
	#undef tanb

	return 0;
}
#endif

int keyboard_getlastkey()
{
		int i, ret = lastkey;
		lastkey = 0;
		for(i = 0; i < JOY_LIST_TOTAL; i++) joysticks[i].Buttons = 0;
		return ret;
}


// Scan input for newly-pressed keys.
// Return value:
// 0  = no key was pressed
// >0 = key code for pressed key
// <0 = error
int control_scankey()
{
	static unsigned ready = 0;
	unsigned k = 0, j = 0;

	k = keyboard_getlastkey();
	j = lastjoy;
	lastjoy = 0;

	if(ready && (k || j))
	{
		ready = 0;
		if(k) return k;
		if(j) return JOY_LIST_FIRST+j;
		else return -1;
	}
	ready = (!k || !j);
	return 0;
}

void control_update(s_playercontrols ** playercontrols, int numplayers)
{
	u64 k;
	unsigned i;
	int player;
	int t;
	s_playercontrols * pcontrols;
	Uint8* keystate = (Uint8*)SDL_GetKeyState(NULL); // Here retrieve keyboard state
	Uint8* keystate_def = (Uint8*)SDL_GetKeyState(NULL); // Here retrieve keyboard state for default

	getPads(keystate,keystate_def);

	for(player = 0; player < numplayers; player++){

		pcontrols = playercontrols[player];

		k = 0;

		for(i = 0; i < JOY_MAX_INPUTS; i++)
		{
			t = pcontrols->settings[i];
			if(t >= SDLK_FIRST && t < SDLK_LAST){
                if(keystate[t]) k |= (1<<i);
			}
		}

        //White Dragon: Set input from default keys overriding previous keys
        //Default keys are available just if no configured keys are pressed!
        if (player <= 0 && !k)
        {
            for(i = 0; i < JOY_MAX_INPUTS; i++)
            {
                t = default_control.settings[i];
                if(t >= SDLK_FIRST && t < SDLK_LAST){
                    if(keystate_def[t]) k |= (1<<i);
                }
            }
        }

		if(usejoy)
		{
			for(i = 0; i < JOY_MAX_INPUTS; i++)
			{
				t = pcontrols->settings[i];
				if(t >= JOY_LIST_FIRST && t <= JOY_LIST_LAST)
				{
					int portnum = (t-JOY_LIST_FIRST-1) / JOY_MAX_INPUTS;
					int shiftby = (t-JOY_LIST_FIRST-1) % JOY_MAX_INPUTS;
					if(portnum >= 0 && portnum <= JOY_LIST_TOTAL-1)
					{
						if((joysticks[portnum].Data >> shiftby) & 1) k |= (1<<i);
					}
				}
			}
		}
		pcontrols->kb_break = 0;
		pcontrols->newkeyflags = k & (~pcontrols->keyflags);
		pcontrols->keyflags = k;

		//if (player <= 0) debug_printf("hats: %d, axes: %d, data: %d",joysticks[0].Hats,joysticks[0].Axes,joysticks[0].Data);
	}
}

void control_rumble(int port, int ratio, int msec)
{
}

