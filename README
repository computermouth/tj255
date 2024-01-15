
### building

Follow the instructions below per-platform. The output will be in the `out` directory.

#### linux

```console
$ sudo apt-get install -y --no-install-recommends libglfw3 libglfw3-dev libx11-dev libxcursor-dev libxrandr-dev libxinerama-dev libxi-dev libxext-dev libxfixes-dev
$ make
```

#### wasm

I use emsdk, and source the `emsdk_env.sh` before building.

```console
$ make PLATFORM=PLATFORM_WEB
```

#### windows

The Windows build also expects a unixy shell. I would recommend MSYS2. Git bash is also probably ok.

```console
## msys2 -- mingw64
$ pacman -Sy mingw-w64-x86_64-gcc sed vim make
$ make
```

#### osx

You'll need xcode, maybe brew.

```console
$ make
```
