a2x
===

a2x is a light C framework I made to help me write my software faster and easier. It supports desktop Linux, GP2X F-100/200, GP2X Wiz, and Caanoo.

Installation and Use
--------------------

    $ cd ~
    $ git clone git://github.com/alxm/a2x.git a2x
    $ bash a2x/bin/a2x_install

    $ cd ~
    $ bash a2x/bin/a2x_new Sample
    Found a2x: ~/a2x/bin
    Making dir: Sample
    Making dir: Sample/make
    Making dir: Sample/src
    Writing file: Sample/make/Makefile
    Writing file: Sample/src/main.c
    Sample is ready!
    $ cd Sample/make
    $ make run
    Compiling main.c
    Building Sample
    Running Sample
    Code me!

**Sample Program**

    #include <a2x.h>

    void a2x(void)
    {
        a2x_set("title", "Sample a2x App");
        a2x_set("version", "1.0");
        a2x_set("author", "You");
        a2x_set("window", "no");
        a2x_set("quiet", "yes");
    }

    void Main(void)
    {
        printf("Code me!\n");
    }

License
-------

Copyright 2010 Alex Margarit (artraid@gmail.com)

Licensed under GNU LGPL3.
