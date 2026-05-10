#!/usr/bin/env bash

CLI="../build/VidInjector9002-CLI"

# set Ptitles
${CLI} -set_parameter out/a.vi9p 12 'ONE あ' out/a.vi9p
${CLI} -set_parameter out/a.vi9p 13 'TWO あ \hi' out/a.vi9p
${CLI} -set_parameter out/a.vi9p 14 'THREE あ \ncool' out/a.vi9p
${CLI} -set_parameter out/a.vi9p 15 'FOUR あ' out/a.vi9p
