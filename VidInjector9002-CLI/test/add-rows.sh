#!/usr/bin/env bash

CLI="../build/VidInjector9002-CLI"

# set mode to multi
valgrind ${CLI} -set_parameter out/a.vi9p 0 1 out/a.vi9p

# add 3 more rows so we have 4
valgrind ${CLI} -add_row out/a.vi9p out/a.vi9p
valgrind ${CLI} -add_row out/a.vi9p out/a.vi9p
valgrind ${CLI} -add_row out/a.vi9p out/a.vi9p
