#include "game.h"
#include "driver.h"

#include <stdio.h>
#include <stdlib.h>

#define H 18
#define W 32


Game g;

static const char* level1 = {
    "AAAAAAAAAAAABAAAAAAAAAAADAAAAAAA"
    "AAABAABBABAAAAAAAAAAAAAAAAAAAAAA"
    "AAABAAAAAAAAAAABAAAAAAAAAAAAAAAA"
    "AAAAAAAAAABAAAAAAAAAAAAAADAAAAAA"
    "AAAAAAAAAADAAAAAABAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AABAAAAAAAAAAAAAAAAAAABAAAAAAAAA"
    "AAAAAADAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAADAAA"
    "DCCCCCCCCCCCCCCDCCCCCCCCCCCCCCCC"
    "CCCCCCCCCCCCCCBCCCCCCCCCCCCCCCCC"
    "CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC"
    "EEEEEEEBEEEEEEEEEEEEEEEEEEEEDEEE"
    "EEEEEEEEEEEEEEBEEEEEEEEEEEEEEEEE"
    "EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE"
    "DEEEEEEBEEEEEEEEEEEEEEEEEEEEEEEE"
    "EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE"
    "EEEEEDEEEEEEEEEEEEEEEEEEEEEEEEDE"
};

static unsigned char bg[H * W];

static Entity entity[5];
static Rocher rocher[4];


static int dirx[] = {0, -1, 1, 0, 0 };
static int diry[] = {0, 0, 0, -1, 1 };


static void process_move(int move) {

    entity[0].dir = move;
    int i ;
    int nextx = entity[0].x + dirx[move];
    int nexty = entity[0].y + diry[move];
    entity[0].dir = move;
    if((nextx < 0 || nextx >= W || nexty < 0 || nexty >= H || bg[nexty * W + nextx] == H)||
      (nextx < 0 || nextx >= W || nexty < 0 || nexty >= H || bg[nexty * W + nextx] == roche)) 

        return;
    //collision dig dug / rocher -> évite le traversage
    for(i=0;i<4;i++){
      if(nextx == rocher[i].x && nexty == rocher[i].y)
	return;
    }
    /*collision dig dug/ennemi -> évite traversage mais n'enlève pas encore de vie*/
    for(i=1;i<5;i++){
      if(nextx == entity[i].x && nexty == entity[i].y)
        return ;
    }
    entity[0].x = nextx;
    entity[0].y = nexty;
}

int distance(int x1, int x2, int y1, int y2){

    return (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) ;
}

static int ennemy_dire(int entx,int ennx, int enty, int enny) {
int best=0, m, mindist = H*W;int nextx,minD,nexty;


    for(int m=0;m<5;m++){
        nextx = ennx + dirx[m];
        nexty = enny + diry[m];
        if((nextx < 0 || nextx >= W || nexty < 0 || nexty >= H || bg[nexty * W + nextx] == roche )||
        (nextx < 0 || nextx >= W || nexty < 0 || nexty >= H || bg[nexty * W + nextx] == roche)){
            continue;
        }


        minD= distance(entx,nextx,enty,nexty);
        if(minD<mindist){
            mindist=minD;
            best  = m;
        }
    }
       return best;
}

static int ennemy2_dire(int entx,int ennx, int enty, int enny) {
int best=0, m, mindist = H*W;int nextx,minD,nexty; char *rien="je ne sers à rien en fait";


    for(int m=0;m<5;m++){
        nextx = ennx + dirx[m];
        nexty = enny + diry[m];
        if((nextx < 0 || nextx >= W || nexty < 0 || nexty >= H || bg[nexty * W + nextx] == roche )||
        (nextx < 0 || nextx >= W || nexty < 0 || nexty >= H || bg[nexty * W + nextx] == roche)){
            continue;
        }

        if(nextx==entity[1].x && nexty==entity[1].y){
            best = 3;
            return 0;
        }
        minD= distance(entx,nextx,enty,nexty);
        if(minD<mindist){
            mindist=minD;
            best  = m;
        }
    }
       return best;
}

static void ennemy_move(int ent_id,int ennemy_dir){

    entity[ent_id].dir = ennemy_dir;
    entity[ent_id].x += dirx[ennemy_dir];
    entity[ent_id].y += diry[ennemy_dir];
    entity[ent_id].dir = ennemy_dir;
}

int score(){
    static int ppt = 0;
    int realPoint;

    if(bg[entity[0].y*W+entity[0].x]== gemme)
    {
        ppt+=10;

    }
    printf("Votre score est de %d\n", ppt);
    return ppt;
}

void win(){
    int result= score();
    for(int i =0; i < H*W;i++){
        
        if(result >=110){
	  sleep(2);
        printf(" Vous avez gagné damn \n");
	 printf("Votre score est de %d\n", result);
        exit(1);
        }   
    }
}

 void lose(){

  if((entity[0].x == entity[1].x && entity[0].y == entity[1].y)||
    (entity[0].x == entity[2].x && entity[0].y == entity[2].y))
    
    {
    entity[0].vie--;
    if(entity[0].vie>0){
    g.entity[0].x = W/2;
    g.entity[0].y = H/2;
    g.entity[0].id = 0;
    g.entity[0].dir = Right;

    g.entity[1].x =16 ;
    g.entity[1].y =2 ;
    g.entity[1].id = 0;
    g.entity[2].x =4 ;
    g.entity[2].y =9 ;
    g.entity[2].id = 1;

    }
  }
  
  if(entity[0].vie == 0){
    /*g.entity[0].x = 50;
    g.entity[0].y = 50;
    g.entity[1].x = 50;
    g.entity[1].y = 50;
    g.entity[2].x = 50;
    g.entity[2].y = 50;*/


    printf("Votre score est de %d\n", score());
    printf("Vous avez perdu");
    exit(1);
  } 

}


static void callback(void* d) {
    const Driver* dr = (Driver*)d;
    int move = dr->get_move();
    int dir = ennemy_dire(entity[0].x,entity[1].x,entity[0].y,entity[1].y);
    int dir1 = ennemy2_dire(entity[0].x,entity[2].x,entity[0].y,entity[2].y);
    process_move(move);
    ennemy_move(1,dir);
    ennemy_move(2,dir1);
    score();
    
    

   
    bg[entity[0].y*W+entity[0].x]=creuse; 

    dr->draw_bg();

    dr->draw_entity(0);
    dr->draw_entity(1);
    dr->draw_entity(2);
    win();
    lose();
    dr->update();
}






void init_game(const Driver* dr) {
    int i;
    g.h = H;
    g.w = W;
    g.background = bg;
    g.entity = entity;
    for(int i = 0; i < H * W; ++i)
        g.background[i] = level1[i] - 'A';


    g.entity[0].x = 0;
    g.entity[0].y = 0;
    g.entity[0].id = 0;
    g.entity[0].dir = Right;
    g.entity[0].vie = 4;

    g.entity[1].x =16 ;
    g.entity[1].y =2 ;
    g.entity[1].id = 0;
    g.entity[2].x =4 ;
    g.entity[2].y =9 ;
    g.entity[2].id = 1;

      
    dr->init(&g);
    dr->start(callback);
}

