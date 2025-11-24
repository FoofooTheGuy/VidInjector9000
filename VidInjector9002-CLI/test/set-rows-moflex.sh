#!/usr/bin/env bash

CLI="../build/VidInjector9002-CLI"

# set moflex (make sure to do this after adding rows)
${CLI} -set_parameter out/a.vi9p 16 'placeholder.moflex' out/a.vi9p
${CLI} -set_parameter out/a.vi9p 17 'placeholder.moflex' out/a.vi9p
${CLI} -set_parameter out/a.vi9p 18 'placeholder.moflex' out/a.vi9p
${CLI} -set_parameter out/a.vi9p 19 'placeholder.moflex' out/a.vi9p

