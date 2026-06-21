#include <stdio.h>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define APP_WIDTH 800
#define APP_HEIGHT 600

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_FPoint points[500];
    bool is_running;
} State;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
    (void)argc;
    (void)argv;
    
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        return SDL_APP_FAILURE;
    }

    State *state = SDL_calloc(1, sizeof(State));
    if (!state)
    {
        return SDL_APP_FAILURE;
    }

    *appstate = state;

    if (!SDL_CreateWindowAndRenderer("SDL3 Window Example", APP_WIDTH, APP_HEIGHT, 0, &state->window, &state->renderer))
    {
        return SDL_APP_FAILURE;
    }

    SDL_SetRenderLogicalPresentation(state->renderer, APP_WIDTH, APP_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    for (size_t i = 0; i < SDL_arraysize(state->points); i++)
    {
        state->points[i].x = (SDL_randf() * 440.0f) + 100.0f;
        state->points[i].y = (SDL_randf() * 280.0f) + 100.0f;
    }
    
    state->is_running = true;
    
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    SDL_Delay(16); // ~60 FPS
    State *state = (State *)appstate;

    SDL_FRect rect;

    SDL_SetRenderDrawColor(state->renderer, 33, 33, 33, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(state->renderer);

    // draw a filled rectangle in the middle of the canvas
    SDL_SetRenderDrawColor(state->renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
    rect.x = rect.y = 100;
    rect.w = 440;
    rect.h = 280;
    SDL_RenderFillRect(state->renderer, &rect);

    // draw some points across the canvas
    SDL_SetRenderDrawColor(state->renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderPoints(state->renderer, state->points, SDL_arraysize(state->points));
    
    // draw a unfilled rectangle in-set a little bit
    SDL_SetRenderDrawColor(state->renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
    rect.x = 30;
    rect.y = 30;
    rect.w = 60;
    rect.h = 60;
    SDL_RenderFillRect(state->renderer, &rect);

    // draw two lines in an X across the whole canvas
    SDL_SetRenderDrawColor(state->renderer, 255, 255, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderLine(state->renderer, 0, 0, 640, 480);
    SDL_RenderLine(state->renderer, 0, 480, 640, 0);
    
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
    State *state = (State *)appstate;

    SDL_DestroyRenderer(state->renderer);
    SDL_DestroyWindow(state->window);

    SDL_free(state);

    printf("[Eddy] Application exited with status %d\n", result);
}
