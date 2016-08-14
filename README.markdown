![a2x](https://github.com/alxm/a2x/raw/master/doc/a2x-logo.png "a2x") [![Build Status](https://travis-ci.org/alxm/a2x.svg?branch=master)](https://travis-ci.org/alxm/a2x)
=========================================================================================================================================================================

a2x is a C framework I wrote for my video games. It supports desktop Linux, GP2X F-100 and F-200, GP2X Wiz, Caanoo, and Open Pandora.

Install
-------

### Download & Dependencies

    $ git clone git://github.com/alxm/a2x.git a2x
    $ apt install build-essentials python3 libpng12-dev libsdl2-dev libsdl2-mixer-dev

### Build

1. Add `a2x/bin` to your `$PATH` variable.
2. Run `a2x_install` to build a2x for desktop Linux.

Run `a2x_install clean` to delete all compiled and generated files from the step above.

Simple Project
--------------

    $ a2x_new simple
    $ cd simple/make/ && make run

`a2x_new` generated some bare-bones code:

```C
#include <a2x.h>

A_SETUP
{
    a_settings_set("app.title", "simple");
    a_settings_set("app.version", "1.0");
    a_settings_set("app.author", "alex");
    a_settings_set("app.output.on", "yes");
}

A_MAIN
{
    printf("Code me!\n");
}
```

Cross-compile for Other Platforms
---------------------------------

I originally wrote a2x for my GP2X games. The following are now supported:

* GPH SDK (Caanoo)
* Open2x SDK (GP2X, Wiz)
* Pandora SDK (Open Pandora)

### Setup

* Edit `a2x/cfg/sdk.config` with your SDK paths.
* Run `a2x_install` again to pick up your `sdk.config` changes.
* `cd a2x/make/a2x && make -f Makefile.X` to build a2x for platform `X`.

License
-------

Copyright 2010 Alex Margarit (alex@alxm.org)

Licensed under GNU LGPL3.
