#!/usr/bin/env bash

CLI="../build/VidInjector9002-CLI"

valgrind ${CLI} -set_parameter out/a.vi9p 13 'placeholder.moflex' out/a.vi9p
