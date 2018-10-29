#include "game.h"
#include "driver.h"
#include <stdio.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

//prototypes des fonctions du driver
static int init(const Game* game);
static void start(void(*callback)(void*));
static int get_move(void);
static void draw_bg(void);
static void draw_entity(int ent_id);
static void draw_rocher(int ent_iddd);
static void update(void);

Driver sdl_driver = {
    .game = NULL,
    .init = init,
    .start = start,
    .get_move = get_move,
    .draw_bg = draw_bg,
    .draw_entity = draw_entity,
    .draw_rocher = draw_rocher,
    .update = update
};

#define GAME (sdl_driver.game)

static SDL_Window *win;
static SDL_Renderer *ren;

static SDL_Texture* tiles[NCell];
static const char* tiles_files[] = { "files/ciel.png", "files/rocher1.png", "files/sol2.png","files/gem.png","files/sol4.png","files/creuse.png"};

static SDL_Texture* sprites[NSprite1];
static const char* sprites_files[] = { "files/bender.png","files/rondoudou.png","files/rondoudou2.png","files/rondoudou3.png","files/rondoudou4.png"};

static SDL_Texture* rocher[NSprite2];
static const char* sprite_rocher[]= {"files/rocher1.png","files/rocher2.png","files/rocher3.png","files/rocher4.png"};

enum { SZ = 32 };
enum { FPS = 15 };

static int load_sprites(void) {
    int i;
    SDL_Surface *png;
    for(i = 0; i < NSprite1; ++i) {
        png = IMG_Load(sprites_files[i]);
        if (png == NULL){
            SDL_DestroyRenderer(ren);
            SDL_DestroyWindow(win);
            printf("Error: %s\n", SDL_GetError());
            SDL_Quit();
            return 1;
        }
        sprites[i] = SDL_CreateTextureFromSurface(ren, png);
        SDL_FreeSurface(png);
    }
    return 0;
}


static int load_sprites_rocher(void) {
    int i;
    SDL_Surface *png ;
    for(i = 0; i < NSprite2; i++) {
        png = IMG_Load(sprite_rocher[i]);
        if (png == NULL){
            SDL_DestroyRenderer(ren);
            SDL_DestroyWindow(win);
            printf("Error: %s\n", SDL_GetError());
            SDL_Quit();
            return 1;
        }    
        rocher[i] = SDL_CreateTextureFromSurface(ren, png);
        SDL_FreeSurface(png);
        
    }
    return 0;
}

static int load_tiles(void) {
    int i;
    SDL_Surface *png;
    for(i = 0; i < NCell; ++i) {
        png = IMG_Load(tiles_files[i]);
        if (png == NULL){
            SDL_DestroyRenderer(ren);
            SDL_DestroyWindow(win);
            printf("Error: %s\n", SDL_GetError());
            SDL_Quit();
            return 1;
        }
        tiles[i] = SDL_CreateTextureFromSurface(ren, png);
        SDL_FreeSurface(png);
    }
    return 0;
}

//initialisation du jeu 
static int init(const Game* game) {
    int i;

    GAME = game;
    if (SDL_Init(SDL_INIT_VIDEO) != 0){
        printf("SDL_Init Error: %s\n",  SDL_GetError());
        return 1;
    }
    
    win = SDL_CreateWindow("Game", 0, 0, GAME->w * SZ, GAME->h * SZ, SDL_WINDOW_SHOWN);
    if (win == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    
    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (ren == NULL){
        SDL_DestroyWindow(win);
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    
    if(load_tiles())
        return 1;
    
    if(load_sprites())
        return 1;

    if(load_sprites_rocher())
        return 1;
    
    return 0;
}

//démarrage du jeu 
static void start(void(*callback)(void*)) {
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(callback, &sdl_driver, FPS, 0);
#else
    for(;;) {
        callback(&sdl_driver);
        clock_t debut = clock();
        usleep(100000);
        clock_t ecoule = clock() - debut;
    }
#endif
}

//Gestion des évenements(clavier)
static int get_move(void) {
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type) {
        case SDL_KEYDOWN:
            switch(event.key.keysym.sym) {
#ifdef __EMSCRIPTEN__
                case SDL_SCANCODE_UP:
                    return Up;
                case SDL_SCANCODE_LEFT:
                    return Left;
                case SDL_SCANCODE_DOWN:
                    return Down;
                case SDL_SCANCODE_RIGHT:
                    return Right;
#else
                case SDLK_UP:
                    return Up;
                case SDLK_LEFT:
                    return Left;
                case SDLK_DOWN:
                    return Down;
                case SDLK_RIGHT:
                    return Right;
		case SDLK_ESCAPE: 
                exit(0);
                SDL_Quit();
#endif
                default:
                    break;
            }
            break;
        default:
            break;
    }
    return Nothing;
}

//initialisation de la map 
static void draw_bg(void) {
    SDL_RenderClear(ren);
    int y, x;
    SDL_Rect dst = {.x = 0, .y = 0, .w = SZ, .h = SZ };
    for(y = 0; y < GAME->h; ++y) {
        for(x = 0; x < GAME->w; ++x) {
            dst.x = x * SZ;
            dst.y = y * SZ;
            int typecell = GAME->background[y * GAME->w + x];
            SDL_RenderCopy(ren, tiles[typecell], NULL, &dst);
        }
    }
}

//initialisation du héros 
static void draw_entity(int ent_id) {
    static int sp = 0;
    SDL_Rect src = {.x = 0, .y = 0, .w = SZ, .h = SZ };
    SDL_Rect dst = {.x = SZ * GAME->entity[ent_id].x, .y = SZ * GAME->entity[ent_id].y, .w = SZ, .h = SZ };
    src.x = sp * 24;
    SDL_RenderCopy(ren, sprites[ent_id], &src, &dst);
   //sp = (sp + 1) % 6;
    //SDL_Delay(100);
}

static void draw_rocher(int ent_iddd){
    static int sp = 0;
   SDL_Rect src ={.x = 0, .y = 0, .w = SZ, .h = SZ};
   SDL_Rect dst = {.x = SZ * GAME->rocher[ent_iddd].x, .y = SZ * GAME->rocher[ent_iddd].y, .w = SZ, .h = SZ };
   SDL_RenderCopy(ren, rocher[ent_iddd], &src, &dst);

}
static void update(void) {
    SDL_RenderPresent(ren);
}

