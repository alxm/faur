# [![Faur](./media/faur.png "Faur")](https://www.alxm.org)

*Faur* is my personal C framework for my [hobby video games](https://www.alxm.org).

Features include software or accelerated 2D graphics, abstractions for inputs and sound, application state management, entity-component-system model, and utilities to help with data, files, math, memory, strings, time, and more.

Faur builds native on Linux and cross-compiles for Web, Windows, and some embedded devices I like, including the GP2X handheld and a few Arduino compatibles. The build system uses GNU Make 4.1 and Python 3.6 or later.

## Install & New Project

### Install Dependencies & Clone Repo

```sh
$ sudo apt install build-essential git python3 python3-pil
$ sudo apt install libsdl2-dev libsdl2-mixer-dev libpng-dev

$ git clone https://github.com/alxm/faur.git
```

### New Project

```sh
$ faur/bin/faur-new hello

$ ls
faur hello

$ tree hello
hello
├── build
│   └── make
│       └── Makefile
└── src
    └── main.c

$ cd hello/build/make
$ make run FAUR_PATH=../../../faur
```

Move the square with the arrow keys or with a game controller:

![Hello, World screenshot](./media/hello.gif "Hello, World screenshot")

### hello/src/main.c

```c
#include <faur.h>

void f_main(void)
{
    static struct {
        int x, y;
        FButton *up, *down, *left, *right;
    } context;

    F_STATE_INIT
    {
        context.x = f_screen_sizeGetWidth() / 2;
        context.y = f_screen_sizeGetHeight() / 2;

        context.up = f_button_new();
        f_button_bindKey(context.up, F_KEY_UP);
        f_button_bindButton(context.up, NULL, F_BUTTON_UP);

        context.down = f_button_new();
        f_button_bindKey(context.down, F_KEY_DOWN);
        f_button_bindButton(context.down, NULL, F_BUTTON_DOWN);

        context.left = f_button_new();
        f_button_bindKey(context.left, F_KEY_LEFT);
        f_button_bindButton(context.left, NULL, F_BUTTON_LEFT);

        context.right = f_button_new();
        f_button_bindKey(context.right, F_KEY_RIGHT);
        f_button_bindButton(context.right, NULL, F_BUTTON_RIGHT);
    }

    F_STATE_TICK
    {
        if(f_button_pressGet(context.up)) {
            context.y--;
        }

        if(f_button_pressGet(context.down)) {
            context.y++;
        }

        if(f_button_pressGet(context.left)) {
            context.x--;
        }

        if(f_button_pressGet(context.right)) {
            context.x++;
        }
    }

    F_STATE_DRAW
    {
        f_color_colorSetHex(0xaaff88);
        f_draw_fill();

        f_color_colorSetHex(0xffaa44);
        f_draw_rectangle(context.x - 40, context.y - 40, 80, 80);
    }

    F_STATE_FREE
    {
        f_button_free(context.up);
        f_button_free(context.down);
        f_button_free(context.left);
        f_button_free(context.right);
    }
}
```

### hello/build/make/Makefile

```make
F_CONFIG_APP_AUTHOR := <author>
F_CONFIG_APP_NAME := hello

include $(FAUR_PATH)/make/default.mk
```

## Cross-Compile for Other Platforms

I started Faur by collecting my [GP2X games'](https://www.alxm.org/games/gamepark.html) shared code into a library. Over time I added support for more platforms:

Platform | Toolchain | Dependencies | More Info
--- | --- | --- | ---
***Desktop***
Linux | OS build tools | `libsdl2-dev` `libsdl2-mixer-dev` `libpng-dev`
Linux (older) | OS build tools | `libsdl1.2-dev` `libsdl-mixer1.2-dev` `libpng-dev`
FreeBSD | OS build tools | `gmake` `devel/sdl20 audio/sdl2_mixer` | [Building on FreeBSD](https://www.alxm.org/notes/a2x-freebsd.html)
Windows | MinGW-w64 | `mingw-w64` `libz-mingw-w64-dev` | [Cross-compiling with MinGW](https://www.alxm.org/notes/a2x-mingw.html)
Web (Wasm) | Emscripten | | [Emscripten Notes](https://www.alxm.org/notes/emscripten.html)
***Embedded Linux***
GP2X, GP2X Wiz | Open2x SDK | `ia32-libs` | [Open2x Project](https://sourceforge.net/p/open2x/code/HEAD/tree/)
Caanoo | GPH SDK | `ia32-libs` | [Caanoo SDK for Linux](https://dl.openhandhelds.org/cgi-bin/caanoo.cgi?0,0,0,0,17,631)
Open Pandora | Pandora SDK | `ia32-libs` | [Open Pandora Forums](https://pyra-handheld.com/boards/)
***Arduino***
Gamebuino META | Arduino 1.8.13, Arduino SAMD Boards 1.8.11, Gamebuino META Boards 1.2.2 | `Gamebuino META 1.3` `ffmpeg` | [Gamebuino META Makefile](https://www.alxm.org/notes/gamebuino-meta-makefile.html)
ODROID-GO | Arduino 1.8.13, Arduino ESP32 1.0.6 | `ODROID-GO 1.0.0` `python3-serial` `python-is-python3` | [ODROID Wiki](https://wiki.odroid.com/odroid_go/arduino/01_arduino_setup)

The default toolchain paths are in `make/global/sdk.mk`, and they can be overridden in a project Makefile or globally in `~/.config/faur/sdk.mk`. To build for different targets, change `include $(FAUR_PATH)/make/default.mk` to use other files from `$(FAUR_PATH)/make`.

## License

Copyright 2010-2022 Alex Margarit (alex@alxm.org)

* Source code licensed under [GNU GPL 3.0](https://www.gnu.org/licenses/gpl.html) (file `LICENSE`)
* Other content licensed under [CC BY-NC-ND 4.0](https://creativecommons.org/licenses/by-nc-nd/4.0/) (file `media/CC-BY-NC-ND`)

Faur is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. The project is named after the old Romanian word *faur*, sometimes used in fables to mean *wizard blacksmith*. ⚒️✨

### Contributing

This is my personal framework and ideas playground, a solo project. You are welcome to check it out and use it under the terms of the license, but I do not take pull requests to this repo.
