![a2x](https://github.com/alxm/a2x/raw/master/doc/a2x-logo.png "a2x") [![Build Status](https://travis-ci.org/alxm/a2x.svg?branch=master)](https://travis-ci.org/alxm/a2x)
=========================================================================================================================================================================

a2x is a C framework I wrote for my video games. It runs natively on Linux and FreeBSD, and can be cross-compiled for Windows, GP2X, GP2X Wiz, Caanoo, and Open Pandora.

Install
-------

### Download & Dependencies

```sh
$ git clone git://github.com/alxm/a2x.git
$ apt install build-essential python3 libpng-dev libsdl2-dev libsdl2-mixer-dev
```

Add `a2x/bin/` to your `$PATH` variable, or prepend it to all calls to `a2x_*` scripts below.

### Build

```sh
$ a2x_install
```

### Hello World Project

```sh
$ a2x_new hello
$ cd hello/make/
$ make run
```

`a2x_new` generated some bare-bones code in `hello/src/main.c`:

```C
#include <a2x.h>

A_SETUP
{
    a_settings_set("app.title", "hello");
    a_settings_set("app.version", "1.0");
    a_settings_set("app.author", "<your username>");
    a_settings_set("app.output.on", "yes");
}

A_MAIN
{
    printf("Code me!\n");
}
```

### Cross-Compile for Other Platforms

I originally wrote a2x for my GP2X games. The following targets are also supported now:

* GPH SDK (Caanoo)
* MinGW (Windows)
* Open2x SDK (GP2X, Wiz)
* Pandora SDK (Open Pandora)

Edit `a2x/cfg/sdk.config` with your SDK paths, then:

```sh
$ a2x_install
$ cd a2x/make/a2x
$ make -f Makefile.gp2x
```

License
-------

Copyright 2010-2017 Alex Margarit (alex@alxm.org)

* Code licensed under [GNU LGPL3](https://www.gnu.org/licenses/lgpl.html) (see `COPYING` and `COPYING.LESSER`)
* Logo licensed under [CC BY-NC-ND 4.0](https://creativecommons.org/licenses/by-nc-nd/4.0/) (see `doc/CC-BY-NC-ND`)
