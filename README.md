# [![Faur](./media/faur.png "Faur")](https://www.alxm.org)

*Faur* is my personal C framework for my [hobby video games](https://www.alxm.org).

Features include software 2D graphics, abstractions for inputs and sound, application state management, entity-component-system model, and utilities to help with data, files, math, memory, strings, time, and more.

Faur builds native on Linux and [cross-compiles](#cross-compile-for-other-platforms) for Web, Windows, and some embedded devices I like. The build system uses GNU Make 4.1 and Python 3.6 or later.

## Dependencies & Path Setup

```sh
# Required
sudo apt install build-essential git python3 python3-pil
sudo apt install libsdl2-dev libsdl2-mixer-dev libpng-dev

# Optional
sudo apt install ffmpeg libsdl1.2-dev libsdl-mixer1.2-dev

# Clone repo to ~/faur
cd ~
git clone git://github.com/alxm/faur.git

# Set FAUR_PATH environment var and add tools to PATH
export FAUR_PATH="$HOME/faur"
export PATH="$PATH:$FAUR_PATH/bin"
```

## Hello, World

The sample project has two files, `main.c` and `Makefile`:

```sh
$ faur-new hello

$ tree hello/
hello/
├── build/
│   └── make/
│       └── Makefile
└── src/
    └── main.c

$ cd hello/build/make/
$ make run
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

I started Faur by collecting my [GP2X games](https://www.alxm.org/games/gamepark.html)' shared code into a library. Over time I added support for more platforms:

Platform | Toolchain | Support Libraries
--- | --- | ---
***Desktop***
Linux, FreeBSD | OS build tools | SDL 2.0, SDL_mixer 2.0, libpng 1.6
Windows | MinGW-w64 | SDL 2.0, SDL_mixer 2.0, libpng 1.6
Web (Wasm) | Emscripten | SDL 2.0, SDL_mixer 2.0, libpng 1.6
***Embedded Linux***
GP2X, GP2X Wiz | Open2x SDK | SDL 1.2, SDL_mixer 1.2, libpng 1.2
Caanoo | GPH SDK | SDL 1.2, SDL_mixer 1.2, libpng 1.2
Open Pandora | Pandora SDK | SDL 1.2, SDL_mixer 1.2, libpng 1.2
***Arduino***
Gamebuino META | Arduino SAMD | Gamebuino META 1.3
Odroid-GO | Arduino ESP32 | Odroid-GO 1.0

The default toolchain paths are in `make/global/sdk.mk`, and they can be overridden in a project Makefile or globally in `~/.config/faur/sdk.mk`. To build for different targets, change `include $(FAUR_PATH)/make/default.mk` to use other files from `$(FAUR_PATH)/make`.

## License

Copyright 2010-2022 Alex Margarit (alex@alxm.org)

* Source code licensed under [GNU GPL 3.0](https://www.gnu.org/licenses/gpl.html) (file `LICENSE`)
* Other content licensed under [CC BY-NC-ND 4.0](https://creativecommons.org/licenses/by-nc-nd/4.0/) (file `media/CC-BY-NC-ND`)

Faur is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. The project is named after the old Romanian word *faur*, sometimes used in fables to mean *wizard blacksmith*. ⚒️✨

### Contributing

This is my personal framework and test bed for ideas, and to that end it is a solo project. You are welcome to check it out and use it under the terms of the license, but I do not take pull requests to this repo.
