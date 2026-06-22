#include <time.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <freetype2/ft2build.h>
#include <freetype2/freetype/freetype.h>

#define APP_NAME "Eddy"
#define INITIAL_APP_WIDTH 800
#define INITIAL_APP_HEIGHT 600

typedef struct PrettyPoints
{
    int w;
    int h;
    SDL_FPoint dt[2000];
} PrettyPoints;

typedef struct AppState
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    PrettyPoints points;
    char *file_path;
} AppState;
