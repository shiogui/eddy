#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SDL_MAIN_USE_CALLBACKS
#include "app.h"
#include "logger.h"

FT_Library library;
FT_Face face;
SDL_Texture *glyphTexture = NULL;
SDL_Palette *palette;

int bearingX;
int bearingY;
float glyphWidth;
float glyphHeight;

int printed = 0;
char *temp_content = "[Eddy] Total charmaps found: 2  Index 0: Platform ID = 0, Encoding ID = 3 Index 1: Platform ID = 3, Encoding ID = 1 [CURRENTLY ACTIVE]";

typedef struct RopeNode
{
    struct RopeNode *left;
    struct RopeNode *right;
    int weight; // lenght of the subtree (or string length if it's a leaf)
    char *str;  // points to string if leaf, NULL if internal node
} RopeNode;

// helper function to calculate total string length of a subtree
int total_length(RopeNode *node)
{
    if (node == NULL)
        return 0;

    if (node->str != NULL)
        return node->weight;

    return node->weight + total_length(node->right);
}

// function to create a new leaf node
RopeNode *create_leaf(const char *s)
{
    RopeNode *node = (RopeNode *)malloc(sizeof(RopeNode));
    if (!node)
        return NULL;

    node->left = NULL;
    node->right = NULL;
    node->weight = strlen(s);
    node->str = strdup(s);
    return node;
}

// function to concatenate two ropes
RopeNode *concatenate(RopeNode *r1, RopeNode *r2)
{
    if (r1 == NULL)
        return r2;
    if (r2 == NULL)
        return r1;

    RopeNode *root = (RopeNode *)malloc(sizeof(RopeNode));
    if (!root)
        return NULL;

    root->left = r1;
    root->right = r2;
    root->str = NULL;

    root->weight = total_length(r1);
    return root;
}

// function to print the rope (in-order traversal)
void print_rope(RopeNode *root)
{
    if (root == NULL)
        return;

    // if it's a leaf node, print it's string content
    if (root->str != NULL)
    {
        printf("%s", root->str);
        return;
    }

    print_rope(root->left);
    print_rope(root->right);
}

// function to free allocated memory
void free_rope(RopeNode *root)
{
    if (root == NULL)
        return;

    free_rope(root->left);
    free_rope(root->right);

    if (root->str)
        free(root->str);

    free(root);
}

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

void draw_text(SDL_Renderer *renderer, char *text)
{
    FT_Error error;

    char c;
    int pen_x = 50, pen_y = 50;
    int num_chars = strlen(text);

    for (int n = 0; n < num_chars; n++)
    {
        c = text[n];

        error = FT_Load_Char(face, c, FT_LOAD_RENDER);
        if (error)
        {
            log_info("Error: Failed to load character (%c)", c);
            continue;
        }

        FT_GlyphSlot slot = face->glyph;
        glyphWidth = (float)slot->bitmap.width;
        glyphHeight = (float)slot->bitmap.rows;

        // if (!printed)
        // {
        //     log_info("Glyph Info: { w: %f, h: %f }", glyphWidth, glyphHeight);
        // }

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

            if (!surface)
            {
                continue;
            }

            SDL_SetSurfacePalette(surface, palette);

            // Upload the CPU pixel surface memory directly into a GPU texture structure
            glyphTexture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_DestroySurface(surface); // CPU buffer memory is no longer needed
        }

        // Extract spacing offsets for pixel-perfect structural layouts
        bearingX = (float)slot->bitmap_left;
        bearingY = (float)slot->bitmap_top;

        SDL_FRect dstRect = {
            .x = pen_x + bearingX,
            .y = pen_y - bearingY, // Subtract bearingY because SDL screen Y coordinates grow downward
            .w = glyphWidth,
            .h = glyphHeight};

        SDL_SetTextureBlendMode(glyphTexture, SDL_BLENDMODE_BLEND);
        SDL_RenderTexture(renderer, glyphTexture, NULL, &dstRect);

        // if (!printed)
        // {
        //     log_info("X Movement: { pen_x: %d, slot_advance_x: %d }", pen_x, slot->advance.x >> 6);
        // }

        /* increment pen position */
        pen_x += slot->advance.x >> 6;
    }

    printed = 1;
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

    // Setup a simple grayscale palette for the 8-bit index surface
    palette = SDL_CreatePalette(256);

    SDL_Color colors[256];
    for (int i = 0; i < 256; i++)
    {
        colors[i].r = 255; // Keep font color white
        colors[i].g = 255;
        colors[i].b = 255;
        colors[i].a = (Uint8)i; // Map FreeType shade intensity directly to transparency alpha
    }
    SDL_SetPaletteColors(palette, colors, 0, 256);

    // 1. create leaf nodes with string segments
    RopeNode *r1 = create_leaf("Hello_");
    RopeNode *r2 = create_leaf("my_");
    RopeNode *r3 = create_leaf("friend!");

    // 2. concatenate them together to form a tree
    RopeNode *root1 = concatenate(r1, r2);
    RopeNode *root = concatenate(root1, r3);

    // 3. display results
    printf("Full String: ");
    print_rope(root);
    printf("\n");

    printf("Root left subtree weight: %d chars\n", root->weight);

    // 4. cleanup memory
    free_rope(root);

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

    SDL_SetRenderDrawColor(state->renderer, 33, 33, 33, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(state->renderer);

    draw_text(state->renderer, temp_content);

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
