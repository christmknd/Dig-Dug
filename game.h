typedef struct Game Game;
typedef struct Driver Driver;
typedef struct Entity Entity;
typedef struct Rocher Rocher;


struct Entity {
    int x;
    int y;
    int id;
    int dir;
    int vie;
};

struct Rocher {
    int x;
    int y;
    int iddd;
    int dir;
};

struct Game {
    int h;
    int w;
    unsigned char* background;
    Entity* entity;
    Rocher* rocher;
};

enum {
    sol1,
    roche,
    sol3,
    gemme,
    ciel,
    creuse,
    NCell
};

enum {
    DigDug,
    Ennemi1,
    Ennemi2,
    Ennemi3,
    Ennemi4,
    NSprite1
};

enum {
    Rocher1,
    Rocher2,
    Rocher3,
    Rocher4,
    NSprite2
};

enum {
    Nothing, Left, Right, Up, Down
};

void init_game(const Driver*);

