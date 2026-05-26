#!/usr/bin/env bash

CLI="../build/VidInjector9002-CLI"

valgrind ${CLI} -build_cia out/a.vi9p out/a.cia out/a.tar
