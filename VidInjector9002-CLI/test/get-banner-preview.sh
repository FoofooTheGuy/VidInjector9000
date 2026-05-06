#!/usr/bin/env bash

CLI="../build/VidInjector9002-CLI"

# set banner
valgrind ${CLI} -set_parameter out/a.vi9p 1 'big buck banner.jpg' out/a.vi9p

# generate preview
valgrind ${CLI} -generate_preview out/a.vi9p 1 out/banner.png
