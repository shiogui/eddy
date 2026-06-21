# Eddy

A very (very!!!) simple text editor (soon to be, for now it's just a window with barely any purpose or usefulness).

>  
> 🚨 **Disclaimer**  
>  
> AI usage on this project is permitted for documentation purposes only.  
> All development and debugging must be done the old fashioned way.  
> This is mostly a learning project — let's learn!  
>  

## Dependencies

| Library | Version | Notes |
|---------|---------|-------|
| SDL3    | 3.x     | Main graphics/event library |

### Installing SDL3

**Linux (Debian/Ubuntu)**
```bash
# SDL3 is not yet in most package repos — build from source or use a PPA
sudo apt install libsdl3-dev
# or build from source: https://github.com/libsdl-org/SDL
```

**macOS (Homebrew)**
```bash
brew install sdl3
```

**Windows (MSYS2)**
```bash
pacman -S mingw-w64-x86_64-SDL3
```

You also need `pkg-config` on Linux/macOS for the Makefile to pick up SDL3 flags automatically.

```bash
# Linux
sudo apt install pkg-config

# macOS
brew install pkg-config
```

## Architecture

This project does **not** use a conventional `main()` function. It uses SDL3's
[main function callbacks](https://wiki.libsdl.org/SDL3/README-main-functions)
(`SDL_AppInit`, `SDL_AppIterate`, `SDL_AppEvent`, `SDL_AppQuit`), enabled via
`SDL_MAIN_USE_CALLBACKS`. SDL3 owns the entry point and drives the app loop.

## Build

```bash
make
```

## Run

```bash
make run
```

Or directly after building:

```bash
./eddy
```

## Clean

```bash
make clean
```

## Controls

| Key    | Action |
|--------|--------|
| Escape | Quit   |

## Versioning

This project uses [Semantic Versioning](https://semver.org) (`MAJOR.MINOR.PATCH`).
Releases are marked with git tags:

```bash
git tag -a v1.0.0 -m "v1.0.0"
git push origin v1.0.0
```

See [CHANGELOG.md](CHANGELOG.md) for release history.

## License

[MIT](LICENSE)
