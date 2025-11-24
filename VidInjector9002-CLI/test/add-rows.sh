#!/usr/bin/env bash

CLI="../build/VidInjector9002-CLI"

# set mode to multi
${CLI} -set_parameter out/a.vi9p 0 1 out/a.vi9p

# add 3 more rows so we have 4
${CLI} -add_row out/a.vi9p out/a.vi9p
${CLI} -add_row out/a.vi9p out/a.vi9p
${CLI} -add_row out/a.vi9p out/a.vi9p
