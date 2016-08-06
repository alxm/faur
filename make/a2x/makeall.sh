#!/bin/sh

output=""
make_arg=$1

color_text ()
{
    output="$output\n\033[$1;1m$2\033[0m"
}

do_make ()
{
    make -f $2 $make_arg

    if [ $? -eq 0 ];
    then
        color_text 32 "[ Success ] $1"
    else
        color_text 31 "[ Failure ] $1"
    fi
}

do_make "Desktop Linux" Makefile
do_make "Caanoo" Makefile.caanoo
do_make "GP2X" Makefile.gp2x
do_make "Pandora" Makefile.pandora
do_make "Wiz" Makefile.wiz

echo $output
