#include <stdio.h>
#include <time.h>

#include <freetype2/ft2build.h>
#include <freetype2/freetype/freetype.h>

#define SDL_MAIN_USE_CALLBACKS
#include "app.h"
#include "logger.h"

FT_Library library;
FT_Face face;
SDL_Texture *glyphTexture = NULL;

int bearingX;
int bearingY;
float glyphWidth;
float glyphHeight;

void update_points(AppState *state, int w, int h)
{
    if (state->points.w == w && state->points.h == h)
    {
        return;
    }

    for (size_t i = 0; i < SDL_arraysize(state->points.dt); i++)
    {
        state->points.dt[i].x = (SDL_randf() * (w - 200)) + 100.0f;
        state->points.dt[i].y = (SDL_randf() * (h - 200)) + 100.0f;
    }

    state->points.w = w;
    state->points.h = h;

    log_info("Window Size Changed to { w: %d, h: %d }", w, h);
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
    AppState *state = SDL_calloc(1, sizeof(AppState));
    if (!state)
    {
        return SDL_APP_FAILURE;
    }

    if (argc > 1)
    {
        state->file_path = argv[1];
    }

    state->points.w = 0;
    state->points.h = 0;
    SDL_memset(state->points.dt, 0, sizeof(state->points.dt));

    *appstate = state;

    update_points(
        state,
        INITIAL_APP_WIDTH,
        INITIAL_APP_HEIGHT);

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer(
            APP_NAME,
            state->points.w,
            state->points.h,
            SDL_WINDOW_RESIZABLE,
            &state->window,
            &state->renderer))
    {
        return SDL_APP_FAILURE;
    }

    // initializes freetype2
    FT_Error error = FT_Init_FreeType(&library);
    if (error)
    {
        log_info("Error loading FreeType2: %d", error);
        return SDL_APP_FAILURE;
    }

    FT_Error font_error = FT_New_Face(
        library,
        "/home/shiogui/workspace/projects/eddy/assets/fonts/Righteous-Regular.ttf",
        0,
        &face);
    if (font_error == FT_Err_Unknown_File_Format)
    {
        log_info("Unknown Font Type: %d", error);
        return SDL_APP_FAILURE;
    }
    else if (font_error)
    {
        log_info("Error loading Font Face: %d", error);
        return SDL_APP_FAILURE;
    }

    FT_Set_Pixel_Sizes(face, 0, 24);

    char charToRender = 'A';
    if (FT_Load_Char(face, charToRender, FT_LOAD_RENDER))
    {
        SDL_Log("Could not load character glyph");
    }

    FT_GlyphSlot slot = face->glyph;
    glyphWidth = (float)slot->bitmap.width;
    glyphHeight = (float)slot->bitmap.rows;

    if (glyphWidth > 0 && glyphHeight > 0)
    {
        // Create an interim CPU surface. FreeType's default output is an 8-bit indexing array (Grayscale/Alpha)
        // We use SDL_PIXELFORMAT_INDEX8 to match FreeType's exact data architecture
        SDL_Surface *surface = SDL_CreateSurfaceFrom(
            (int)glyphWidth,
            (int)glyphHeight,
            SDL_PIXELFORMAT_INDEX8,
            slot->bitmap.buffer,
            slot->bitmap.pitch);

        if (surface)
        {
            // Setup a simple grayscale palette for the 8-bit index surface
            SDL_Palette *palette = SDL_CreatePalette(256);
            if (palette)
            {
                SDL_Color colors[256];
                for (int i = 0; i < 256; i++)
                {
                    colors[i].r = 255; // Keep font color white
                    colors[i].g = 255;
                    colors[i].b = 255;
                    colors[i].a = (Uint8)i; // Map FreeType shade intensity directly to transparency alpha
                }
                SDL_SetPaletteColors(palette, colors, 0, 256);
                SDL_SetSurfacePalette(surface, palette);
            }

            // Upload the CPU pixel surface memory directly into a GPU texture structure
            glyphTexture = SDL_CreateTextureFromSurface(state->renderer, surface);
            SDL_DestroySurface(surface); // CPU buffer memory is no longer needed
        }
    }

    // Extract spacing offsets for pixel-perfect structural layouts
    bearingX = (float)slot->bitmap_left;
    bearingY = (float)slot->bitmap_top;

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    SDL_Delay(16); // ~60 FPS

    AppState *state = (AppState *)appstate;

    int w, h;
    if (!SDL_GetWindowSize(state->window, &w, &h))
    {
        printf("[%s] Failed to obtain Window Size.\n", APP_NAME);
        return SDL_APP_FAILURE;
    }

    update_points(state, w, h);

    SDL_FRect rect;

    SDL_SetRenderDrawColor(state->renderer, 33, 33, 33, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(state->renderer);

    // draw a filled rectangle in the middle of the canvas
    SDL_SetRenderDrawColor(state->renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
    rect.x = rect.y = 100;
    rect.w = w - 200;
    rect.h = h - 200;
    SDL_RenderFillRect(state->renderer, &rect);

    // draw some points across the canvas
    SDL_SetRenderDrawColor(state->renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderPoints(state->renderer, state->points.dt, SDL_arraysize(state->points.dt));

    // draw a unfilled rectangle in-set a little bit
    SDL_SetRenderDrawColor(state->renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
    rect.x += 30;
    rect.y += 30;
    rect.w -= 60;
    rect.h -= 60;
    SDL_RenderRect(state->renderer, &rect);

    // draw two lines in an X across the whole canvas
    SDL_SetRenderDrawColor(state->renderer, 255, 255, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderLine(state->renderer, 0, 0, w, h);
    SDL_RenderLine(state->renderer, w, 0, 0, h);

    // Position the character relative to a baseline origin point (e.g., X=100, Y=200)
    SDL_FRect dstRect = {
        .x = 10.0f + bearingX,
        .y = 50.0f - bearingY, // Subtract bearingY because SDL screen Y coordinates grow downward
        .w = glyphWidth,
        .h = glyphHeight};

    // Set blend mode so transparency maps correctly
    SDL_SetTextureBlendMode(glyphTexture, SDL_BLENDMODE_BLEND);
    SDL_RenderTexture(state->renderer, glyphTexture, NULL, &dstRect);

    SDL_RenderPresent(state->renderer);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    (void)appstate;

    switch (event->type)
    {
    case SDL_EVENT_KEY_DOWN:

        if (event->key.key == SDLK_ESCAPE)
        {
            return SDL_APP_SUCCESS;
        }

        return SDL_APP_CONTINUE;

    case SDL_EVENT_QUIT:

        return SDL_APP_SUCCESS;
    }

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    AppState *state = (AppState *)appstate;

    if (glyphTexture)
        SDL_DestroyTexture(glyphTexture);

    FT_Done_Face(face);
    FT_Done_FreeType(library);

    SDL_DestroyRenderer(state->renderer);
    SDL_DestroyWindow(state->window);

    SDL_free(state);

    printf("[%s] Application exited with status %d\n", APP_NAME, result);
}
