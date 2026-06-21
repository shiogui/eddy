# Changelog

All notable changes to this project will be documented in this file.

The format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/)
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Basic SDL3 window and renderer setup
- Main event loop using SDL3 callback API (`SDL_AppInit`, `SDL_AppIterate`, `SDL_AppEvent`, `SDL_AppQuit`)
- Escape key and window close to quit
- Cross-platform Makefile (Linux, macOS, Windows/MinGW) with `pkg-config` integration
