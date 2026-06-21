#include <stdio.h>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
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

    if (!SDL_CreateWindowAndRenderer("SDL3 Window Example", 800, 600, 0, &state->window, &state->renderer))
    {
        return SDL_APP_FAILURE;
    }

    state->is_running = true;
    
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    SDL_Delay(16); // ~60 FPS
    State *state = (State *)appstate;

    SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, 255);
    SDL_RenderClear(state->renderer);
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
