#!/bin/sh

output=""
make_arg=$1

color_text ()
{
    output="$output\n\033[$1;1m$2\033[0m"
}

record_result ()
{
    if [ $2 -eq 0 ];
    then
        color_text 32 "[ Success ] $1"
    else
        color_text 31 "[ Failure ] $1"
    fi
}

do_make ()
{
    make -j -f $2 $make_arg
    record_result "$1" $?
}

do_emmake ()
{
    emmake make -j -f $2 $make_arg
    record_result "$1" $?
}

do_make "Linux native" Makefile
do_make "Linux 32bit" Makefile.linux32
do_make "Linux 64bit" Makefile.linux64
do_make "Mingw 32bit" Makefile.mingw32
do_make "Mingw 64bit" Makefile.mingw64
do_make "Caanoo" Makefile.caanoo
do_make "GP2X" Makefile.gp2x
do_make "Pandora" Makefile.pandora
do_make "Wiz" Makefile.wiz
do_emmake "Emscripten" Makefile.emscripten

echo $output
