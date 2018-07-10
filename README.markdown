# ![a2x](https://github.com/alxm/a2x/raw/master/doc/a2x-logo.png "a2x") [![Build Status](https://travis-ci.org/alxm/a2x.svg?branch=master)](https://travis-ci.org/alxm/a2x)

a2x is a C framework I wrote for my video games. It runs natively on Linux and FreeBSD, and can be cross-compiled for Windows, GP2X, Wiz, Caanoo, Open Pandora, and HTML5.

## Install

### Download & Dependencies

```sh
$ cd ~
$ apt install git build-essential python3 libpng-dev libsdl2-dev libsdl2-mixer-dev
$ git clone git://github.com/alxm/a2x.git
```

Add `~/a2x/bin` to the `$PATH` variable, or prepend it to all calls to `a2x_*` scripts below.

### Build

```sh
$ a2x_install
```

### Hello World Project

`a2x_new` generates a small starter project:

```sh
$ a2x_new hello
$ cd hello/make/
$ make run
```

![Starter Project Screenshot](https://github.com/alxm/a2x/raw/master/doc/hello.gif "Starter Project Screenshot")

The starter project draws a square on the screen which you can move around with the arrow keys. The code is in `hello/src/main.c`:

```C
#include <a2x.h>

A_SETUP
{
    a_settings_set("app.title", "hello");
    a_settings_set("app.version", "1.0");
    a_settings_set("app.author", "<your username>");
    a_settings_set("app.output.on", "yes");
}

A_STATE(drawBox)
{
    static struct {
        int x, y;
        AInputButton* up;
        AInputButton* down;
        AInputButton* left;
        AInputButton* right;
    } context;

    A_STATE_INIT
    {
        context.x = a_screen_getWidth() / 2;
        context.y = a_screen_getHeight() / 2;

        context.up = a_button_new("key.up");
        context.down = a_button_new("key.down");
        context.left = a_button_new("key.left");
        context.right = a_button_new("key.right");
    }

    A_STATE_TICK
    {
        if(a_button_pressGet(context.up)) {
            context.y--;
        } else if(a_button_pressGet(context.down)) {
            context.y++;
        }

        if(a_button_pressGet(context.left)) {
            context.x--;
        } else if(a_button_pressGet(context.right)) {
            context.x++;
        }
    }

    A_STATE_DRAW
    {
        a_pixel_setHex(0xaaff88);
        a_draw_fill();

        a_pixel_setHex(0xffaa44);
        a_draw_rectangle(context.x - 40, context.y - 40, 80, 80);
    }
}

A_MAIN
{
    a_state_new("drawBox", drawBox, "", "");
    a_state_push("drawBox");
}
```

### Cross-Compile for Other Platforms

I originally wrote a2x for my GP2X games. The following targets are also supported:

* Emscripten (HTML5)
* GPH SDK (Caanoo)
* MinGW (Windows)
* Open2x SDK (GP2X, Wiz)
* Pandora SDK (Open Pandora)

Run `a2x_install`, then edit `~/.config/a2x/sdk.config` with your SDK paths and:

```sh
$ a2x_install
$ cd a2x/make/a2x
$ make -f Makefile.gp2x
```

## License

Copyright 2010-2018 Alex Margarit (alex@alxm.org)

* Code licensed under [GNU LGPL3](https://www.gnu.org/licenses/lgpl.html) (see `COPYING` and `COPYING.LESSER`)
* Logo licensed under [CC BY-NC-ND 4.0](https://creativecommons.org/licenses/by-nc-nd/4.0/) (see `doc/CC-BY-NC-ND`)

a2x is a small project that supports my other personal projects. In the interest of focus and simplicity, it is a solo endeavor.
