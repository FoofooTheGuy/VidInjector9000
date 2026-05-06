#!/usr/bin/env bash

CLI="../build/VidInjector9002-CLI"

valgrind --leak-check=full ${CLI} -build_cia out/a.vi9p out/a.cia
