#!/usr/bin/env bash

CLI="../build/VidInjector9002-CLI"

# set moflex (make sure to do this after adding rows)
${CLI} -set_parameter out/a.vi9p 20 'big buck banner.jpg' out/a.vi9p
${CLI} -set_parameter out/a.vi9p 21 'big buck banner.jpg' out/a.vi9p
${CLI} -set_parameter out/a.vi9p 22 'big buck banner.jpg' out/a.vi9p
${CLI} -set_parameter out/a.vi9p 23 'big buck banner.jpg' out/a.vi9p

