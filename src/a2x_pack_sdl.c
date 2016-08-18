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

#include <SDL.h>
#include <SDL_mixer.h>

#define A_MAX_BUTTON_CODES 4

typedef struct ASdlPhysicalInput {
    char* name;
    APhysicalInput* input;
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
} ASdlPhysicalInput;

static uint32_t g_sdlFlags;

#if A_USE_LIB_SDL
    static SDL_Surface* g_sdlScreen = NULL;
    static bool g_sdlScreenLocked = false;
#elif A_USE_LIB_SDL2
    static SDL_Window* g_sdlWindow = NULL;
    static SDL_Renderer* g_sdlRenderer = NULL;
    static SDL_Texture* g_sdlTexture = NULL;
#endif

#define A_MAX_JOYSTICKS 8
static int g_joysticksNum;
static SDL_Joystick* g_joysticks[A_MAX_JOYSTICKS];

static AStrHash* g_buttons;
static AStrHash* g_analogs;
static AStrHash* g_touchScreens;

static void addButton(const char* Name, int Code)
{
    ASdlPhysicalInput* b = a_strhash_get(g_buttons, Name);

    if(!b) {
        b = a_mem_malloc(sizeof(ASdlPhysicalInput));

        b->name = a_str_dup(Name);
        b->u.button.numCodes = 1;
        b->u.button.codes[0] = Code;

        a_strhash_add(g_buttons, Name, b);
    } else {
        if(b->u.button.numCodes < A_MAX_BUTTON_CODES) {
            b->u.button.codes[b->u.button.numCodes++] = Code;
        } else {
            a_out__error("Button '%s' has too many codes", Name);
        }
    }
}

#if !A_PLATFORM_GP2X && !A_PLATFORM_WIZ
static void addAnalog(const char* Name, int DeviceIndex, char* DeviceName, int XAxisIndex, int YAxisIndex)
{
    if(DeviceIndex == -1 && DeviceName == NULL) {
        a_out__error("Inputs must specify device index or name");
        return;
    }

    ASdlPhysicalInput* a = a_strhash_get(g_analogs, Name);

    if(a) {
        a_out__error("Analog '%s' is already defined", Name);
        return;
    }

    a = a_mem_malloc(sizeof(ASdlPhysicalInput));

    a->name = a_str_dup(Name);
    a->device_index = DeviceIndex;
    a->device_name = DeviceName;
    a->u.analog.xaxis_index = XAxisIndex;
    a->u.analog.yaxis_index = YAxisIndex;

    // check if we requested a specific device by Name
    if(DeviceName) {
        for(int j = g_joysticksNum; j--; ) {
            #if A_USE_LIB_SDL
                if(a_str_same(DeviceName, SDL_JoystickName(j))) {
                    a->device_index = j;
                    break;
                }
            #elif A_USE_LIB_SDL2
                if(a_str_same(DeviceName, SDL_JoystickName(g_joysticks[j]))) {
                    a->device_index = j;
                    break;
                }
            #endif
        }
    }

    a_strhash_add(g_analogs, Name, a);
}
#endif // !A_PLATFORM_GP2X && !A_PLATFORM_WIZ

static void addTouch(const char* Name)
{
    ASdlPhysicalInput* t = a_strhash_get(g_touchScreens, Name);

    if(t) {
        a_out__error("Touchscreen '%s' is already defined", Name);
        return;
    }

    t = a_mem_malloc(sizeof(ASdlPhysicalInput));

    t->name = a_str_dup(Name);

    a_strhash_add(g_touchScreens, Name, t);
}

void a_sdl__init(void)
{
    int ret = 0;
    g_sdlFlags = 0;

    if(a_settings_getBool("video.window")) {
        g_sdlFlags |= SDL_INIT_VIDEO;
    }

    if(a_settings_getBool("sound.on")) {
        g_sdlFlags |= SDL_INIT_AUDIO;
    }

    g_sdlFlags |= SDL_INIT_JOYSTICK;

    #if !(A_PLATFORM_WIZ || A_PLATFORM_CAANOO)
        g_sdlFlags |= SDL_INIT_TIMER;
    #endif

    ret = SDL_Init(g_sdlFlags);

    if(ret != 0) {
        a_out__fatal("SDL: %s", SDL_GetError());
    }

    if(a_settings_getBool("sound.on")) {
        #if A_PLATFORM_LINUXPC || A_PLATFORM_WINDOWS
            if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) != 0) {
                a_settings__set("sound.on", "0");
            }
        #elif A_PLATFORM_GP2X
            if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 256) != 0) {
                a_settings__set("sound.on", "0");
            }
        #else
            if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 512) != 0) {
                a_settings__set("sound.on", "0");
            }
        #endif
    }

    g_joysticksNum = a_math_min(A_MAX_JOYSTICKS, SDL_NumJoysticks());

    if(g_joysticksNum > 0) {
        a_out__message("Found %d joysticks", g_joysticksNum);
        for(int j = g_joysticksNum; j--; ) {
            g_joysticks[j] = SDL_JoystickOpen(j);
        }
    }

    g_buttons = a_strhash_new();
    g_analogs = a_strhash_new();
    g_touchScreens = a_strhash_new();

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
        addButton("wiz.AMenu", 8);
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
        addButton("caanoo.I", 8);
        addButton("caanoo.II", 9);
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
    A_STRHASH_ITERATE(g_buttons, ASdlPhysicalInput*, b) {
        free(b->name);
    }

    A_STRHASH_ITERATE(g_analogs, ASdlPhysicalInput*, a) {
        free(a->name);
    }

    A_STRHASH_ITERATE(g_touchScreens, ASdlPhysicalInput*, t) {
        free(t->name);
    }

    a_strhash_free(g_buttons);
    a_strhash_free(g_analogs);
    a_strhash_free(g_touchScreens);

    for(int j = g_joysticksNum; j--; ) {
        SDL_JoystickClose(g_joysticks[j]);
    }

    if(a_settings_getBool("sound.on")) {
        Mix_CloseAudio();
    }

    #if A_USE_LIB_SDL
        if(SDL_MUSTLOCK(g_sdlScreen) && g_sdlScreenLocked) {
            SDL_UnlockSurface(g_sdlScreen);
            g_sdlScreenLocked = false;
        }
    #elif A_USE_LIB_SDL2
        SDL_DestroyTexture(g_sdlTexture);
        SDL_DestroyRenderer(g_sdlRenderer);
        SDL_DestroyWindow(g_sdlWindow);
    #endif

    SDL_QuitSubSystem(g_sdlFlags);
    SDL_Quit();
}

void a_sdl__screen_set(void)
{
    #if A_USE_LIB_SDL
        int bpp = 0;
        uint32_t videoFlags = SDL_SWSURFACE;

        if(a_settings_getBool("video.fullscreen")) {
            videoFlags |= SDL_FULLSCREEN;
        }

        bpp = SDL_VideoModeOK(a_screen__width,
                              a_screen__height,
                              A_PIXEL_BPP,
                              videoFlags);
        if(bpp == 0) {
            a_out__fatal("SDL: %dx%d:%d video not available",
                         a_screen__width,
                         a_screen__height,
                         A_PIXEL_BPP);
        }

        g_sdlScreen = SDL_SetVideoMode(a_screen__width,
                                       a_screen__height,
                                       A_PIXEL_BPP,
                                       videoFlags);
        if(g_sdlScreen == NULL) {
            a_out__fatal("SDL: %s", SDL_GetError());
        }

        SDL_SetClipRect(g_sdlScreen, NULL);

        if(SDL_MUSTLOCK(g_sdlScreen) && !g_sdlScreenLocked) {
            SDL_LockSurface(g_sdlScreen);
            g_sdlScreenLocked = true;
        }

        a_screen__pixels = g_sdlScreen->pixels;
    #elif A_USE_LIB_SDL2
        int ret;
        a_settings__set("video.doubleBuffer", "1");

        g_sdlWindow = SDL_CreateWindow("",
                                       SDL_WINDOWPOS_UNDEFINED,
                                       SDL_WINDOWPOS_UNDEFINED,
                                       a_screen__width,
                                       a_screen__height,
                                       SDL_WINDOW_RESIZABLE);
        if(g_sdlWindow == NULL) {
            a_out__fatal("SDL_CreateWindow failed: %s", SDL_GetError());
        }

        g_sdlRenderer = SDL_CreateRenderer(g_sdlWindow,
                                           -1,
                                           0);
        if(g_sdlRenderer == NULL) {
            a_out__fatal("SDL_CreateRenderer failed: %s", SDL_GetError());
        }

        ret = SDL_RenderSetLogicalSize(g_sdlRenderer,
                                       a_screen__width,
                                       a_screen__height);
        if(ret < 0) {
            a_out__fatal("SDL_RenderSetLogicalSize failed: %s", SDL_GetError());
        }

        g_sdlTexture = SDL_CreateTexture(g_sdlRenderer,
                                         #if A_PIXEL_BPP == 16
                                             SDL_PIXELFORMAT_RGB565,
                                         #elif A_PIXEL_BPP == 32
                                             SDL_PIXELFORMAT_RGBX8888,
                                         #else
                                             #error Invalid A_PIXEL_BPP value
                                         #endif
                                         SDL_TEXTUREACCESS_STREAMING,
                                         a_screen__width,
                                         a_screen__height);
        if(g_sdlTexture == NULL) {
            a_out__fatal("SDL_CreateTexture failed: %s", SDL_GetError());
        }

        SDL_SetHintWithPriority(SDL_HINT_RENDER_SCALE_QUALITY,
                                "nearest",
                                SDL_HINT_OVERRIDE);
    #endif

    #if A_PLATFORM_LINUXPC
        char caption[64];
        snprintf(caption, 64, "%s %s",
            a_settings_getString("app.title"),
            a_settings_getString("app.version"));

        #if A_USE_LIB_SDL
            SDL_WM_SetCaption(caption, NULL);
        #elif A_USE_LIB_SDL2
            SDL_SetWindowTitle(g_sdlWindow, caption);
        #endif
    #else
        SDL_ShowCursor(SDL_DISABLE);
    #endif
}

void a_sdl__screen_show(void)
{
    #if A_USE_LIB_SDL
        if(a_settings_getBool("video.wizTear")) {
            // video.doubleBuffer is also set when video.wizTear is set
            #define A_WIDTH 320
            #define A_HEIGHT 240

            if(SDL_MUSTLOCK(g_sdlScreen) && !g_sdlScreenLocked) {
                SDL_LockSurface(g_sdlScreen);
                g_sdlScreenLocked = true;
            }

            APixel* dst = g_sdlScreen->pixels + A_WIDTH * A_HEIGHT;
            const APixel* src = a_screen__pixels;

            for(int i = A_HEIGHT; i--; dst += A_WIDTH * A_HEIGHT + 1) {
                for(int j = A_WIDTH; j--; ) {
                    dst -= A_HEIGHT;
                    *dst = *src++;
                }
            }

            if(SDL_MUSTLOCK(g_sdlScreen) && g_sdlScreenLocked) {
                SDL_UnlockSurface(g_sdlScreen);
                g_sdlScreenLocked = false;
            }

            SDL_Flip(g_sdlScreen);
        } else if(a_settings_getBool("video.doubleBuffer")) {
            if(SDL_MUSTLOCK(g_sdlScreen) && !g_sdlScreenLocked) {
                SDL_LockSurface(g_sdlScreen);
                g_sdlScreenLocked = true;
            }

            const APixel* src = a_screen__pixels;
            APixel* dst = g_sdlScreen->pixels;

            memcpy(dst, src, A_SCREEN_SIZE);

            if(SDL_MUSTLOCK(g_sdlScreen) && g_sdlScreenLocked) {
                SDL_UnlockSurface(g_sdlScreen);
                g_sdlScreenLocked = false;
            }

            SDL_Flip(g_sdlScreen);
        } else {
            if(SDL_MUSTLOCK(g_sdlScreen) && g_sdlScreenLocked) {
                SDL_UnlockSurface(g_sdlScreen);
                g_sdlScreenLocked = false;
            }

            SDL_Flip(g_sdlScreen);

            if(SDL_MUSTLOCK(g_sdlScreen) && !g_sdlScreenLocked) {
                SDL_LockSurface(g_sdlScreen);
                g_sdlScreenLocked = true;
            }

            a_screen__pixels = g_sdlScreen->pixels;
            a_screen__savedPixels = a_screen__pixels;
        }
    #elif A_USE_LIB_SDL2
        int ret;

        ret = SDL_UpdateTexture(g_sdlTexture,
                                NULL,
                                a_screen__pixels,
                                a_screen__width * sizeof(APixel));
        if(ret < 0) {
            a_out__fatal("SDL_UpdateTexture failed: %s", SDL_GetError());
        }

        ret = SDL_RenderCopy(g_sdlRenderer, g_sdlTexture, NULL, NULL);
        if(ret < 0) {
            a_out__fatal("SDL_RenderCopy failed: %s", SDL_GetError());
        }

        SDL_RenderPresent(g_sdlRenderer);
    #endif
}

int a_sdl__sound_volumeMax(void)
{
    return MIX_MAX_VOLUME;
}

void* a_sdl__music_load(const char* Path)
{
    Mix_Music* m = Mix_LoadMUS(Path);

    if(!m) {
        a_out__error("Mix_LoadMUS failed: %s", Mix_GetError());
    }

    return m;
}

void a_sdl__music_free(void* Music)
{
    Mix_FreeMusic(Music);
}

void a_sdl__music_setVolume(void)
{
    Mix_VolumeMusic(
        (float)a_settings_getInt("sound.music.scale") / 100 * a_sound__volume);
}

void a_sdl__music_play(void* Music)
{
    Mix_PlayMusic(Music, -1);
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

void* a_sdl__sfx_loadFromFile(const char* Path)
{
    Mix_Chunk* sfx = Mix_LoadWAV(Path);

    if(sfx == NULL) {
        a_out__error("Mix_LoadWAV(%s) failed: %s", Path, SDL_GetError());
    }

    return sfx;
}

void* a_sdl__sfx_loadFromData(const uint8_t* Data, int Size)
{
    SDL_RWops* rw;
    Mix_Chunk* sfx = NULL;

    rw = SDL_RWFromMem((void*)Data, Size);
    if(rw == NULL) {
        a_out__error("SDL_RWFromMem failed: %s", SDL_GetError());
        goto Done;
    }

    sfx = Mix_LoadWAV_RW(rw, 0);
    if(sfx == NULL) {
        a_out__error("Mix_LoadWAV_RW failed: %s", SDL_GetError());
        goto Done;
    }

Done:
    if(rw) {
        SDL_FreeRW(rw);
    }

    return sfx;
}

void a_sdl__sfx_free(void* Sfx)
{
    Mix_FreeChunk(Sfx);
}

void a_sdl__sfx_setVolume(void* Sfx, uint8_t Volume)
{
    ((Mix_Chunk*)Sfx)->volume = Volume;
}

void a_sdl__sfx_play(void* Sfx)
{
    Mix_PlayChannel(-1, Sfx, 0);
}

uint32_t a_sdl__getTicks(void)
{
    return SDL_GetTicks();
}

void a_sdl__delay(uint32_t Milis)
{
    SDL_Delay(Milis);
}

void a_sdl__input_matchButton(const char* Name, APhysicalInput* Button)
{
    ASdlPhysicalInput* b = a_strhash_get(g_buttons, Name);

    if(b) {
        b->input = Button;
    } else {
        a_out__error("No SDL binding for button %s", Name);
    }
}

void a_sdl__input_matchAnalog(const char* Name, APhysicalInput* Analog)
{
    ASdlPhysicalInput* a = a_strhash_get(g_analogs, Name);

    if(a) {
        a->input = Analog;
    } else {
        a_out__error("No SDL binding for analog %s", Name);
    }
}

void a_sdl__input_matchTouch(const char* Name, APhysicalInput* Touch)
{
    ASdlPhysicalInput* t = a_strhash_get(g_touchScreens, Name);

    if(t) {
        t->input = Touch;
    } else {
        a_out__error("No SDL binding for touchscreen %s", Name);
    }
}

void a_sdl__input_get(void)
{
    for(SDL_Event event; SDL_PollEvent(&event); ) {
        enum {
            A_ACTION_NONE,
            A_ACTION_PRESSED,
            A_ACTION_UNPRESSED,
        } action = A_ACTION_NONE;

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
                A_STRHASH_ITERATE(g_analogs, ASdlPhysicalInput*, a) {
                    if(a->device_index == event.jaxis.which) {
                        if(event.jaxis.axis == a->u.analog.xaxis_index) {
                            a_input__analog_setXAxis(a->input,
                                                     event.jaxis.value);
                        } else if(event.jaxis.axis == a->u.analog.yaxis_index) {
                            a_input__analog_setYAxis(a->input,
                                                     event.jaxis.value);
                        }
                    }
                }
            } break;

            case SDL_MOUSEMOTION: {
                A_STRHASH_ITERATE(g_touchScreens, ASdlPhysicalInput*, t) {
                    a_input__touch_addMotion(t->input,
                                             event.button.x,
                                             event.button.y);
                }
            } break;

            case SDL_MOUSEBUTTONDOWN: {
                switch(event.button.button) {
                    case SDL_BUTTON_LEFT: {
                        A_STRHASH_ITERATE(g_touchScreens, ASdlPhysicalInput*, t) {
                            a_input__touch_setCoords(t->input,
                                                     event.button.x,
                                                     event.button.y,
                                                     true);
                        }
                    } break;
                }
            } break;

            case SDL_MOUSEBUTTONUP: {
                switch(event.button.button) {
                    case SDL_BUTTON_LEFT: {
                        A_STRHASH_ITERATE(g_touchScreens, ASdlPhysicalInput*, t) {
                            a_input__touch_setCoords(t->input,
                                                     event.button.x,
                                                     event.button.y,
                                                     false);
                        }
                    } break;
                }
            } break;

            default:break;
        }

        if(action != A_ACTION_NONE) {
            A_STRHASH_ITERATE(g_buttons, ASdlPhysicalInput*, b) {
                for(int c = b->u.button.numCodes; c--; ) {
                    if(b->u.button.codes[c] == code) {
                        a_input__button_setState(b->input,
                                                 action == A_ACTION_PRESSED);
                        break;
                    }
                }
            }
        }
    }
}
