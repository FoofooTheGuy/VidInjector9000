#!/usr/bin/env bash

CLI="../build/VidInjector9002-CLI"

# set Ptitles
${CLI} -set_parameter out/a.vi9p 12 'ONE' out/a.vi9p
${CLI} -set_parameter out/a.vi9p 13 'TWO' out/a.vi9p
${CLI} -set_parameter out/a.vi9p 14 'THREE' out/a.vi9p
${CLI} -set_parameter out/a.vi9p 15 'FOUR' out/a.vi9p
