/*
    Copyright 2010 Alex Margarit

    This file is part of a2x-framework.

    a2x-framework is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    a2x-framework is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with a2x-framework.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "a2x_pack_sdl.v.h"

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#define A_MAX_BUTTON_CODES 4

typedef struct SdlInputInstance {
    char* name;
    InputInstance* input;
    int device_index;
    char* device_name;
    union {
        struct {
            int numCodes;
            int codes[A_MAX_BUTTON_CODES]; // SDL button/key code
        } button;
        struct {
            int xaxis_index;
            int yaxis_index;
        } analog;
        struct {
            //
        } touch;
    } u;
} SdlInputInstance;

typedef struct InputCollection {
    List* list; // inputs registered during init
    StrHash* names; // hash table of above inputs' names
} InputCollection;

#define a_inputs_new() ((InputCollection){a_list_new(), a_strhash_new()})

#define a_inputs_free(i)              \
({                                    \
    A_LIST_ITERATE(i.list, void, v) { \
        free(v);                      \
    }                                 \
    a_list_free(i.list);              \
    a_strhash_free(i.names);          \
})

#define a_inputs_add(i, ptr, name)     \
({                                     \
    a_list_addLast(i.list, ptr);       \
    a_strhash_add(i.names, name, ptr); \
})

typedef enum InputAction {
    A_ACTION_NONE, A_ACTION_PRESSED, A_ACTION_UNPRESSED
} InputAction;

static SDL_Surface* screen = NULL;
static bool screen_locked = false;

#define A_MAX_JOYSTICKS 8
static int joysticks_num;
static SDL_Joystick* joysticks[A_MAX_JOYSTICKS];

static InputCollection buttons;
static InputCollection analogs;
static InputCollection touches;

static void addButton(const char* name, int code)
{
    SdlInputInstance* b = a_strhash_get(buttons.names, name);

    if(!b) {
        b = a_mem_malloc(sizeof(SdlInputInstance));

        b->name = a_str_dup(name);
        b->u.button.numCodes = 1;
        b->u.button.codes[0] = code;

        a_inputs_add(buttons, b, name);
    } else {
        if(b->u.button.numCodes < A_MAX_BUTTON_CODES) {
            b->u.button.codes[b->u.button.numCodes++] = code;
        } else {
            a_out__error("Button '%s' has too many codes", name);
        }
    }
}

static void addAnalog(const char* name, int device_index, char* device_name, int xaxis_index, int yaxis_index)
{
    if(device_index == -1 && device_name == NULL) {
        a_out__error("Inputs must specify device index or name");
        return;
    }

    SdlInputInstance* a = a_strhash_get(analogs.names, name);

    if(a) {
        a_out__error("Analog '%s' is already defined", name);
        return;
    }

    a = a_mem_malloc(sizeof(SdlInputInstance));

    a->name = a_str_dup(name);
    a->device_index = device_index;
    a->device_name = device_name;
    a->u.analog.xaxis_index = xaxis_index;
    a->u.analog.yaxis_index = yaxis_index;

    // check if we requested a specific device by name
    if(device_name) {
        for(int j = joysticks_num; j--; ) {
            if(a_str_same(device_name, SDL_JoystickName(j))) {
                a->device_index = j;
                break;
            }
        }
    }

    a_inputs_add(analogs, a, name);
}

static void addTouch(const char* name)
{
    SdlInputInstance* t = a_strhash_get(touches.names, name);

    if(t) {
        a_out__error("Touch '%s' is already defined", name);
        return;
    }

    t = a_mem_malloc(sizeof(SdlInputInstance));

    t->name = a_str_dup(name);

    a_inputs_add(touches, t, name);
}

void a_sdl__init(void)
{
    int ret = 0;
    uint32_t sdlFlags = 0;

    if(a2x_bool("video.window")) {
        sdlFlags |= SDL_INIT_VIDEO;
    }

    if(a2x_bool("sound.on")) {
        sdlFlags |= SDL_INIT_AUDIO;
    }

    sdlFlags |= SDL_INIT_JOYSTICK;

    #if !(A_PLATFORM_WIZ || A_PLATFORM_CAANOO)
        sdlFlags |= SDL_INIT_TIMER;
    #endif

    ret = SDL_Init(sdlFlags);

    if(ret != 0) {
        a_out__fatal("SDL: %s", SDL_GetError());
    }

    if(a2x_bool("sound.on")) {
        #if A_PLATFORM_LINUXPC || A_PLATFORM_WINDOWS
            if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) != 0) {
                a2x_set("sound.on", "0");
            }
        #elif A_PLATFORM_GP2X
            if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 256) != 0) {
                a2x_set("sound.on", "0");
            }
        #else
            if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 512) != 0) {
                a2x_set("sound.on", "0");
            }
        #endif
    }

    joysticks_num = a_math_min(A_MAX_JOYSTICKS, SDL_NumJoysticks());

    if(joysticks_num > 0) {
        a_out__message("Found %d joysticks", joysticks_num);
        for(int j = joysticks_num; j--; ) {
            joysticks[j] = SDL_JoystickOpen(j);
        }
    }

    buttons = a_inputs_new();
    analogs = a_inputs_new();
    touches = a_inputs_new();

    #if A_PLATFORM_GP2X
        addButton("gp2x.Up", 0);
        addButton("gp2x.Down", 4);
        addButton("gp2x.Left", 2);
        addButton("gp2x.Right", 6);
        addButton("gp2x.UpLeft", 1);
        addButton("gp2x.UpRight", 7);
        addButton("gp2x.DownLeft", 3);
        addButton("gp2x.DownRight", 5);
        addButton("gp2x.L", 10);
        addButton("gp2x.R", 11);
        addButton("gp2x.A", 12);
        addButton("gp2x.B", 13);
        addButton("gp2x.X", 14);
        addButton("gp2x.Y", 15);
        addButton("gp2x.Start", 8);
        addButton("gp2x.Select", 9);
        addButton("gp2x.VolUp", 16);
        addButton("gp2x.VolDown", 17);
        addButton("gp2x.StickClick", 18);
        addTouch("gp2x.Touch");
    #elif A_PLATFORM_WIZ
        addButton("wiz.Up", 0);
        addButton("wiz.Down", 4);
        addButton("wiz.Left", 2);
        addButton("wiz.Right", 6);
        addButton("wiz.UpLeft", 1);
        addButton("wiz.UpRight", 7);
        addButton("wiz.DownLeft", 3);
        addButton("wiz.DownRight", 5);
        addButton("wiz.L", 10);
        addButton("wiz.R", 11);
        addButton("wiz.A", 12);
        addButton("wiz.B", 13);
        addButton("wiz.X", 14);
        addButton("wiz.Y", 15);
        addButton("wiz.Menu", 8);
        addButton("wiz.Select", 9);
        addButton("wiz.VolUp", 16);
        addButton("wiz.VolDown", 17);
        addTouch("wiz.Touch");
    #elif A_PLATFORM_CAANOO
        addButton("caanoo.Up", -1);
        addButton("caanoo.Down", -1);
        addButton("caanoo.Left", -1);
        addButton("caanoo.Right", -1);
        addButton("caanoo.A", 0);
        addButton("caanoo.X", 1);
        addButton("caanoo.B", 2);
        addButton("caanoo.Y", 3);
        addButton("caanoo.L", 4);
        addButton("caanoo.R", 5);
        addButton("caanoo.Home", 6);
        addButton("caanoo.Hold", 7);
        addButton("caanoo.Help1", 8);
        addButton("caanoo.Help2", 9);
        addAnalog("caanoo.Stick", 0, NULL, 0, 1);
        addTouch("caanoo.Touch");
    #elif A_PLATFORM_PANDORA
        addButton("pandora.Up", SDLK_UP);
        addButton("pandora.Down", SDLK_DOWN);
        addButton("pandora.Left", SDLK_LEFT);
        addButton("pandora.Right", SDLK_RIGHT);
        addButton("pandora.L", SDLK_RSHIFT);
        addButton("pandora.R", SDLK_RCTRL);
        addButton("pandora.A", SDLK_HOME);
        addButton("pandora.B", SDLK_END);
        addButton("pandora.X", SDLK_PAGEDOWN);
        addButton("pandora.Y", SDLK_PAGEUP);
        addButton("pandora.Start", SDLK_LALT);
        addButton("pandora.Select", SDLK_LCTRL);
        addTouch("pandora.Touch");
        addAnalog("pandora.Nub1", -1, "nub0", 0, 1);
        addAnalog("pandora.Nub2", -1, "nub1", 0, 1);
        addButton("pandora.m", SDLK_m);
        addButton("pandora.s", SDLK_s);
    #elif A_PLATFORM_LINUXPC
        addButton("pc.Up", SDLK_i);
        addButton("pc.Up", SDLK_UP);
        addButton("pc.Down", SDLK_k);
        addButton("pc.Down", SDLK_DOWN);
        addButton("pc.Left", SDLK_j);
        addButton("pc.Left", SDLK_LEFT);
        addButton("pc.Right", SDLK_l);
        addButton("pc.Right", SDLK_RIGHT);
        addButton("pc.z", SDLK_z);
        addButton("pc.x", SDLK_x);
        addButton("pc.c", SDLK_c);
        addButton("pc.v", SDLK_v);
        addButton("pc.m", SDLK_m);
        addButton("pc.Enter", SDLK_RETURN);
        addButton("pc.Space", SDLK_SPACE);
        addButton("pc.F1", SDLK_F1);
        addButton("pc.F2", SDLK_F2);
        addButton("pc.F3", SDLK_F3);
        addButton("pc.F4", SDLK_F4);
        addButton("pc.F5", SDLK_F5);
        addButton("pc.F6", SDLK_F6);
        addButton("pc.F7", SDLK_F7);
        addButton("pc.F8", SDLK_F8);
        addButton("pc.F9", SDLK_F9);
        addButton("pc.F10", SDLK_F10);
        addButton("pc.F11", SDLK_F11);
        addButton("pc.F12", SDLK_F12);
        addButton("pc.1", SDLK_1);
        addButton("pc.0", SDLK_0);
        addTouch("pc.Mouse");
        addAnalog("joypad.Analog1", 0, NULL, 0, 1);
        addAnalog("joypad.Analog2", 0, NULL, 3, 4);
    #endif
}

void a_sdl__uninit(void)
{
    a_inputs_free(buttons);
    a_inputs_free(analogs);
    a_inputs_free(touches);

    for(int j = joysticks_num; j--; ) {
        SDL_JoystickClose(joysticks[j]);
    }

    if(a2x_bool("sound.on")) {
        Mix_CloseAudio();
    }

    SDL_Quit();
}

bool a_sdl__screen_set(void)
{
    static bool first_time = true;

    int bpp = 0;
    int scale = a2x_int("video.scale");
    uint32_t videoFlags = SDL_SWSURFACE;

    if(a2x_bool("video.fullscreen")) {
        videoFlags |= SDL_FULLSCREEN;
    }

    bpp = SDL_VideoModeOK(a_width * scale, a_height * scale, A_BPP, videoFlags);

    if(bpp == 0) {
        if(first_time) {
            a_out__fatal("SDL: %dx%d video not available", a_width * scale, a_height * scale);
        } else {
            a_out__warning("SDL: %dx%d video not available", a_width * scale, a_height * scale);
            return false;
        }
    }

    first_time = false;
    screen = SDL_SetVideoMode(a_width * scale, a_height * scale, A_BPP, videoFlags);

    if(screen == NULL) {
        a_out__fatal("SDL: %s", SDL_GetError());
    }

    SDL_SetClipRect(screen, NULL);

    #if A_PLATFORM_LINUXPC
        char caption[64];
        snprintf(caption, 64, "%s %s", a2x_str("app.title"), a2x_str("app.version"));
        SDL_WM_SetCaption(caption, NULL);
    #else
        SDL_ShowCursor(SDL_DISABLE);
    #endif

    return true;
}

Pixel* a_sdl__screen_pixels(void)
{
    return screen->pixels;
}

void a_sdl__screen_lock(void)
{
    if(SDL_MUSTLOCK(screen) && !screen_locked) {
        SDL_LockSurface(screen);
        screen_locked = true;
    }
}

void a_sdl__screen_unlock(void)
{
    if(SDL_MUSTLOCK(screen) && screen_locked) {
        SDL_UnlockSurface(screen);
        screen_locked = false;
    }
}

void a_sdl__screen_flip(void)
{
    SDL_Flip(screen);
}

int a_sdl__sound_volumeMax(void)
{
    return MIX_MAX_VOLUME;
}

void* a_sdl__music_load(const char* path)
{
    Mix_Music* m = Mix_LoadMUS(path);

    if(!m) {
        a_out__error("%s", Mix_GetError());
    }

    return m;
}

void a_sdl__music_free(void* m)
{
    Mix_FreeMusic(m);
}

void a_sdl__music_setVolume(void)
{
    Mix_VolumeMusic((float)a2x_int("sound.music.scale") / 100 * a__volume);
}

void a_sdl__music_play(void* m)
{
    Mix_PlayMusic(m, -1);
}

void a_sdl__music_stop(void)
{
    Mix_HaltMusic();
}

void a_sdl__music_toggle(void)
{
    if(Mix_PausedMusic()) {
        Mix_ResumeMusic();
    } else {
        Mix_PauseMusic();
    }
}

void* a_sdl__sfx_loadFromFile(const char* path)
{
    return Mix_LoadWAV(path);
}

void* a_sdl__sfx_loadFromData(const uint16_t* data, int size)
{
    SDL_RWops* rw = SDL_RWFromMem((void*)data, size);
    Mix_Chunk* sfx = Mix_LoadWAV_RW(rw, 0);

    SDL_FreeRW(rw);

    return sfx;
}

void a_sdl__sfx_free(void* s)
{
    Mix_FreeChunk(s);
}

void a_sdl__sfx_setVolume(void* s, uint8_t volume)
{
    ((Mix_Chunk*)s)->volume = volume;
}

void a_sdl__sfx_play(void* s)
{
    Mix_PlayChannel(-1, s, 0);
}

uint32_t a_sdl__getTicks(void)
{
    return SDL_GetTicks();
}

void a_sdl__delay(uint32_t ms)
{
    SDL_Delay(ms);
}

void a_sdl__input_matchButton(const char* name, InputInstance* button)
{
    SdlInputInstance* i = a_strhash_get(buttons.names, name);

    if(!i) {
        a_out__error("No SDL binding for button %s", name);
    }

    i->input = button;
}

void a_sdl__input_matchAnalog(const char* name, InputInstance* analog)
{
    SdlInputInstance* i = a_strhash_get(analogs.names, name);

    if(!i) {
        a_out__error("No SDL binding for analog %s", name);
    }

    i->input = analog;
}

void a_sdl__input_matchTouch(const char* name, InputInstance* touch)
{
    SdlInputInstance* i = a_strhash_get(touches.names, name);

    if(!i) {
        a_out__error("No SDL binding for touchscreen %s", name);
    }

    i->input = touch;
}

void a_sdl__input_get(void)
{
    for(SDL_Event event; SDL_PollEvent(&event); ) {
        InputAction action = A_ACTION_NONE;
        int code = -1;

        switch(event.type) {
            case SDL_QUIT: {
                a_state_exit();
            } break;

            case SDL_KEYDOWN: {
                action = A_ACTION_PRESSED;
                code = event.key.keysym.sym;

                if(code == SDLK_ESCAPE) {
                    a_state_exit();
                }
            } break;

            case SDL_KEYUP: {
                action = A_ACTION_UNPRESSED;
                code = event.key.keysym.sym;
            } break;

            case SDL_JOYBUTTONDOWN: {
                action = A_ACTION_PRESSED;
                code = event.jbutton.button;
            } break;

            case SDL_JOYBUTTONUP: {
                action = A_ACTION_UNPRESSED;
                code = event.jbutton.button;
            } break;

            case SDL_JOYAXISMOTION: {
                A_LIST_ITERATE(analogs.list, SdlInputInstance, a) {
                    if(a->device_index == event.jaxis.which) {
                        if(event.jaxis.axis == a->u.analog.xaxis_index) {
                            a_input__analog_setXAxis(a->input, event.jaxis.value);
                        } else if(event.jaxis.axis == a->u.analog.yaxis_index) {
                            a_input__analog_setYAxis(a->input, event.jaxis.value);
                        }
                    }
                }
            } break;

            case SDL_MOUSEMOTION: {
                A_LIST_ITERATE(touches.list, SdlInputInstance, t) {
                    a_input__touch_addMotion(t->input, event.button.x, event.button.y);
                }
            } break;

            case SDL_MOUSEBUTTONDOWN: {
                switch(event.button.button) {
                    case SDL_BUTTON_LEFT:
                        A_LIST_ITERATE(touches.list, SdlInputInstance, t) {
                            a_input__touch_setCoords(t->input, event.button.x, event.button.y, true);
                        }
                    break;
                }
            } break;

            case SDL_MOUSEBUTTONUP: {
                switch(event.button.button) {
                    case SDL_BUTTON_LEFT:
                        A_LIST_ITERATE(touches.list, SdlInputInstance, t) {
                            a_input__touch_setCoords(t->input, event.button.x, event.button.y, false);
                        }
                    break;
                }
            } break;

            default:break;
        }

        if(action != A_ACTION_NONE) {
            A_LIST_ITERATE(buttons.list, SdlInputInstance, b) {
                for(int c = b->u.button.numCodes; c--; ) {
                    if(b->u.button.codes[c] == code) {
                        a_input__button_setState(b->input, action == A_ACTION_PRESSED);
                        break;
                    }
                }
            }
        }
    }
}
