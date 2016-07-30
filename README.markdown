![a2x](https://github.com/alxm/a2x/raw/master/doc/a2x-logo.png "a2x")
=====================================================================

a2x is a light C framework I made to help me write my software faster and easier. It supports desktop Linux, GP2X F-100/200, GP2X Wiz, and Caanoo.

**Requirements:** GCC, make, Python, SDL 1.2, SDL_mixer 1.2, libpng; Open2x toolchain for GP2X/Wiz builds, GPH's SDK for Caanoo builds.

Installation and Use
--------------------

### Download

    $ cd ~
    $ git clone git://github.com/alxm/a2x.git a2x

### Configure and Build

1. Add `~/a2x/bin` to your `$PATH` variable.
2. Edit `a2x/cfg/a2x.config` with the correct paths of GP2X SDKs.
3. Call `$ a2x_install` to build a2x. `$ a2x_install clean` deletes all compiled and generated files.

### Test with a Simple Program

    $ cd ~
    $ a2x_new Simple
    Found a2x: ~/a2x/bin
    Making dir: Simple
    Making dir: Simple/make
    Making dir: Simple/src
    Writing file: Simple/make/Makefile
    Writing file: Simple/src/main.c
    Simple is ready!

    $ cd Simple/make/
    $ make run
    Compiling main.c
    Building Simple
    Running Simple
    [ a2x Msg ] Opening Simple 1.0 by You, compiled 2015-12-30
    [ a2x Msg ] You can edit a2x.cfg
    [ a2x Msg ] Opening A_MAIN
    Code me!
    [ a2x Msg ] A_MAIN closed
    [ a2x Msg ] Closing Simple - see you next time.

### A Simple Program

    #include <a2x.h>

    A_SETUP
    {
        a2x_set("app.title", "A Simple Program");
        a2x_set("app.version", "1.0");
        a2x_set("app.author", "You");
        a2x_set("app.quiet", "no");
    }

    A_MAIN
    {
        printf("Code me!\n");
    }

License
-------

Copyright 2010 Alex Margarit (alex@alxm.org)

Licensed under GNU LGPL3.
