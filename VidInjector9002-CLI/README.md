```
          @@@@@@@@@
        @@@@@@@@@@@@@
     @@@@@@@   @@@@@@@
  @@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@   @@@@@@@@@@@
@@@@@@@@@@@@   @@@@@@@@@@@@
@@@@@@@@@@@@   @@@@@@@@@@@@
 @@@@@@@@@@@   @@@@@@@@@@@@
  @@@@@@@@@@   @@@@@@@@@@@@
   @@@@@@@@@   @@@@@@@@@@
     @@@@@@@   @@@@@@@@
      @@@@@@@@@@@@@@
        @@@@@@@@@
```
# VidInjector9002-CLI
A real deal barebones CLI version of VidInjector9002

# Compiling
**Windows (MSYS2)**
1. Download and install [MSYS2](https://www.msys2.org/)
2. Run `pacman -S unzip`
4. Run build_deps.py
5. Run build.py

**Linux**
1. Install `make`, `python` or `python3`, and `perl` with your package manager if you don't already have them
2. Run build_deps.py
3. Run build.py

If `pip` doesn't work, run `python3 -m ensurepip`

If compiling MbedTLS gives errors, do these and try again:
```
python3 -m pip install -U 'jsonschema<4.0'
python3 -m pip install jinja2
```

*This README is under construction.*
