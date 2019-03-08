# ![a2x](https://github.com/alxm/a2x/raw/master/media/a2x-logo.png "a2x") [![Build Status](https://travis-ci.org/alxm/a2x.svg?branch=master)](https://travis-ci.org/alxm/a2x)

a2x is a C framework I wrote for my video games. It runs natively on Linux and FreeBSD, and can be cross-compiled for Windows, GP2X, Wiz, Caanoo, Open Pandora, and HTML5.

## Dependencies & Path Setup

```sh
# Basics
sudo apt install build-essential python3 git

# Core libraries
sudo apt install libpng-dev libsdl2-dev libsdl2-mixer-dev

# Optional extras
sudo apt install libsdl1.2-dev libsdl-mixer1.2-dev mingw-w64

# Clone repo to ~/a2x
cd ~
git clone git://github.com/alxm/a2x.git

# Set environment vars
export A2X_PATH="$HOME/a2x"
export PATH="$PATH:$A2X_PATH/bin"
```

## *Hello, World* Project

```sh
a2x_new hello
cd hello/make/
make -j run
```

![Starter Project Screenshot](https://github.com/alxm/a2x/raw/master/media/hello.gif "Starter Project Screenshot")

Move the square with the arrow keys or gamepad. Here are the generated `main.c` and `Makefile`:

#### main.c

```C
#include <a2x.h>

enum {
    STATE_DRAWBOX,
    STATE_NUM
};

A_STATE(drawBox);

A_MAIN
{
    a_state_init(STATE_NUM);
    a_state_new(STATE_DRAWBOX, drawBox, "Draw Box");

    a_state_push(STATE_DRAWBOX);
}

A_STATE(drawBox)
{
    static struct {
        int x, y;
        AButton *up, *down, *left, *right;
    } context;

    A_STATE_INIT
    {
        context.x = a_screen_sizeGetWidth() / 2;
        context.y = a_screen_sizeGetHeight() / 2;

        context.up = a_button_new();
        a_button_bind(context.up, A_KEY_UP);
        a_button_bind(context.up, A_BUTTON_UP);

        context.down = a_button_new();
        a_button_bind(context.down, A_KEY_DOWN);
        a_button_bind(context.down, A_BUTTON_DOWN);

        context.left = a_button_new();
        a_button_bind(context.left, A_KEY_LEFT);
        a_button_bind(context.left, A_BUTTON_LEFT);

        context.right = a_button_new();
        a_button_bind(context.right, A_KEY_RIGHT);
        a_button_bind(context.right, A_BUTTON_RIGHT);
    }

    A_STATE_TICK
    {
        if(a_button_pressGet(context.up)) {
            context.y--;
        }

        if(a_button_pressGet(context.down)) {
            context.y++;
        }

        if(a_button_pressGet(context.left)) {
            context.x--;
        }

        if(a_button_pressGet(context.right)) {
            context.x++;
        }
    }

    A_STATE_DRAW
    {
        a_pixel_colorSetHex(0xaaff88);
        a_draw_fill();

        a_pixel_colorSetHex(0xffaa44);
        a_draw_rectangle(context.x - 40, context.y - 40, 80, 80);
    }

    A_STATE_FREE
    {
        a_button_free(context.up);
        a_button_free(context.down);
        a_button_free(context.left);
        a_button_free(context.right);
    }
}
```

#### Makefile

```make
A_CONFIG_APP_AUTHOR := you
A_CONFIG_APP_BIN := hello.bin
A_CONFIG_APP_TITLE := hello

A_CONFIG_APP_VERSION_MAJOR := 1
A_CONFIG_APP_VERSION_MINOR := 0
A_CONFIG_APP_VERSION_MICRO := 0

include $(A2X_PATH)/make/dev
```

## Cross-Compile for Other Platforms

I originally wrote a2x to make games for the Linux-based [GP2X handheld](https://www.alxm.org/games/gamepark.html). The following targets are also supported:

* Emscripten (HTML5)
* GPH SDK (Caanoo)
* MinGW (Windows)
* Open2x SDK (GP2X, Wiz)
* Pandora SDK (Open Pandora)

Edit `~/.config/a2x/sdk.config` with your SDK paths, then build a default project to sync your changes.

## License

Copyright 2010-2019 Alex Margarit (alex@alxm.org)

* Code licensed under [GNU GPLv3](https://www.gnu.org/licenses/gpl.html) (see `COPYING`)
* Graphics licensed under [CC BY-NC-ND 4.0](https://creativecommons.org/licenses/by-nc-nd/4.0/) (see `media/CC-BY-NC-ND`)

I'm writing a2x to support my other software. To keep it focused and manageable, it is a solo endeavor.
