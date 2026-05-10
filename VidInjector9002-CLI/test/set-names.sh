#!/usr/bin/env bash

CLI="../build/VidInjector9002-CLI"

# set Ptitle
${CLI} -set_parameter out/a.vi9p 4 'short name' out/a.vi9p
${CLI} -set_parameter out/a.vi9p 5 'long name' out/a.vi9p
${CLI} -set_parameter out/a.vi9p 6 'publisher' out/a.vi9p
