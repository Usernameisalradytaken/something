#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>


long frames =10;
float speed = 0.1;
int X=200,Y=180,H=48,W=48,direction=1,lastPosition;
int seconds = 0;
bool WAIT;
bool PAUSE=true;
void must_init(bool test, const char* des)
{
    if(test)
        return;

    printf ("coundln't initialize the %s\n", des);
}

ALLEGRO_DISPLAY* disp;
ALLEGRO_BITMAP* buffer;


#define BUFFER_W 320
#define BUFFER_H 240

#define DISP_SCALE 3
#define DISP_W (BUFFER_W * DISP_SCALE)
#define DISP_H (BUFFER_H * DISP_SCALE)

void disp_init()
{
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    //al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);

    disp = al_create_display(DISP_W, DISP_H);
    must_init(disp, "display");

    buffer = al_create_bitmap(BUFFER_W, BUFFER_H);
    must_init(buffer, "bitmap buffer");

    const float scale_factor_x = ((float)al_get_display_width(disp)) / DISP_W;
    const float scale_factor_y = ((float)al_get_display_height(disp)) / DISP_H;

    printf("%1.f - %1.f \n",scale_factor_x,scale_factor_y);

    ALLEGRO_TRANSFORM t;

    al_identity_transform(&t);
    al_scale_transform(&t, scale_factor_x, scale_factor_y);
    al_use_transform(&t);


}

void disp_deinit()
{
    al_destroy_bitmap(buffer);
    al_destroy_display(disp);

}

void disp_pre_draw()
{
    al_set_target_bitmap(buffer);
}

void disp_post_draw()
{
    al_set_target_backbuffer(disp);
    al_draw_scaled_bitmap(buffer, 0, 0, BUFFER_W, BUFFER_H, 0, 0, DISP_W, DISP_H, 0);

    al_flip_display();
}

//Keyboard init

#define KEY_SEEN     1
#define KEY_RELEASED 2
unsigned char key[ALLEGRO_KEY_MAX];

void keyboard_init()
{
    memset(key, 0, sizeof(key));
}

void keyboard_update(ALLEGRO_EVENT* event)
{
    switch(event->type)
    {
    case ALLEGRO_EVENT_TIMER:
        for(int i = 0; i < ALLEGRO_KEY_MAX; i++)
            key[i] &= KEY_SEEN;
        break;

    case ALLEGRO_EVENT_KEY_DOWN:
        key[event->keyboard.keycode] = KEY_SEEN | KEY_RELEASED;
        break;
    case ALLEGRO_EVENT_KEY_UP:
        key[event->keyboard.keycode] &= KEY_RELEASED;
        break;
    }
}


// randomness

int between(int lo, int hi)
{
    return lo + (rand() % (hi - lo));
}

float between_f(float lo, float hi)
{
    return lo + ((float)rand() / (float)RAND_MAX) * (hi - lo);
}


bool collide(int ax1, int ay1, int ax2, int ay2, int bx1, int by1, int bx2, int by2)
{
    if(ax1 > bx2) return false;
    if(ax2 < bx1) return false;
    if(ay1 > by2) return false;
    if(ay2 < by1) return false;

    return true;
}


// Background


typedef struct Background
{
    float x;
    float speed;

    ALLEGRO_BITMAP* backg;
} Background;

Background background[5];

void init_background()
{

    background[0].speed = 0;
    background[1].speed = 0.2;
    background[2].speed = 0.5;
    background[3].speed = 1;
    background[4].speed = 0;

    background[0].backg = al_load_bitmap("bg.png");
    must_init(background[0].backg,"backg1\n");
    background[1].backg = al_load_bitmap("buildings.png");
    must_init(background[1].backg,"backg1\n");
    background[2].backg = al_load_bitmap("far-buildings.png");
    must_init(background[2].backg,"backg1\n");
    background[3].backg = al_load_bitmap("skill-foreground.png");
    must_init(background[3].backg,"backg1\n");
    background[4].backg = al_load_bitmap("dirty_roadc.png");
    must_init(background[4].backg,"backg1\n");

}

void update_background()
{


    for(int i=0 ; i < 5 ; i++)
    {

        if(WAIT)
        {
            background[i].x += 0;
        }
        else
        {
            background[i].x += background[i].speed*(float)direction;
        }

        if(background[i].x + 320 <= 0 )
        {
            background[i].x = 0;
        }
        if(background[i].x - 320 >= 0 )
        {

            background[i].x = -320;
        }
    }
}

void draw_background()
{

    al_draw_scaled_bitmap(background[0].backg,0,0,
                          al_get_bitmap_width(background[0].backg),
                          al_get_bitmap_height(background[0].backg),
                          0,0,
                          320,240,0);



    al_draw_scaled_bitmap(background[1].backg,0,0,
                          al_get_bitmap_width(background[1].backg),
                          al_get_bitmap_height(background[1].backg),
                          background[1].x,
                          BUFFER_H - al_get_bitmap_height(background[1].backg)-20,
                          320,al_get_bitmap_height(background[1].backg)-20,0);

    if(background[1].x + 320 < 320)
    {
        al_draw_scaled_bitmap(background[1].backg,0,0,
                              al_get_bitmap_width(background[1].backg),
                              al_get_bitmap_height(background[1].backg),
                              background[1].x + 320,
                              BUFFER_H - al_get_bitmap_height(background[1].backg)-20,
                              320,al_get_bitmap_height(background[1].backg)-20,0);

    }
    if(background[1].x - 320 < 0)
    {
        al_draw_scaled_bitmap(background[1].backg,0,0,
                              al_get_bitmap_width(background[1].backg),
                              al_get_bitmap_height(background[1].backg),
                              background[1].x - 320,
                              BUFFER_H - al_get_bitmap_height(background[1].backg)-20,
                              320,al_get_bitmap_height(background[1].backg)-20,0);

    }


    al_draw_scaled_bitmap(background[2].backg,0,0,
                          al_get_bitmap_width(background[2].backg),
                          al_get_bitmap_height(background[2].backg),
                          background[2].x,
                          BUFFER_H - al_get_bitmap_height(background[2].backg)-20,
                          320,al_get_bitmap_height(background[2].backg)-20,0);

    if(background[2].x +320 < 320)
    {
        al_draw_scaled_bitmap(background[2].backg,0,0,
                              al_get_bitmap_width(background[2].backg),
                              al_get_bitmap_height(background[2].backg),
                              background[2].x + 320,
                              BUFFER_H - al_get_bitmap_height(background[2].backg)-20,
                              320,al_get_bitmap_height(background[2].backg)-20,0);

    }
    if(background[2].x - 320 < 0)
    {
        al_draw_scaled_bitmap(background[2].backg,0,0,
                              al_get_bitmap_width(background[2].backg),
                              al_get_bitmap_height(background[2].backg),
                              background[2].x - 320,
                              BUFFER_H - al_get_bitmap_height(background[2].backg)-20,
                              320,al_get_bitmap_height(background[2].backg)-20,0);

    }

    al_draw_scaled_bitmap(background[3].backg,0,0,
                          al_get_bitmap_width(background[3].backg),
                          al_get_bitmap_height(background[3].backg),
                          background[3].x,
                          BUFFER_H - al_get_bitmap_height(background[3].backg)-20,
                          320,al_get_bitmap_height(background[3].backg)-20,0);
    if(background[3].x + 320  < 320)
    {
        al_draw_scaled_bitmap(background[3].backg,0,0,
                              al_get_bitmap_width(background[3].backg),
                              al_get_bitmap_height(background[3].backg),
                              background[3].x + 320,
                              BUFFER_H - al_get_bitmap_height(background[3].backg)-20,
                              320,al_get_bitmap_height(background[3].backg)-20,0);
    }

    if(background[3].x - 320 < 0)
    {
        al_draw_scaled_bitmap(background[3].backg,0,0,
                              al_get_bitmap_width(background[3].backg),
                              al_get_bitmap_height(background[3].backg),
                              background[3].x - 320,
                              BUFFER_H - al_get_bitmap_height(background[3].backg)-20,
                              320,al_get_bitmap_height(background[3].backg)-20,0);

    }


    al_draw_scaled_bitmap(background[4].backg,
                          0,0,
                          al_get_bitmap_width(background[4].backg),
                          al_get_bitmap_height(background[4].backg),
                          background[4].x,
                          BUFFER_H - 40,
                          320,40,0);

    if(background[4].x + 320 < 320)
    {
        al_draw_scaled_bitmap(background[4].backg,
                              0,0,
                              al_get_bitmap_width(background[4].backg),
                              al_get_bitmap_height(background[4].backg),
                              background[4].x + 320,
                              BUFFER_H - 40,
                              320,40,0);

    }

    if(background[4].x - 320 < 0)
    {
        al_draw_scaled_bitmap(background[4].backg,
                              0,0,
                              al_get_bitmap_width(background[4].backg),
                              al_get_bitmap_height(background[4].backg),
                              background[4].x - 320,
                              BUFFER_H - 40,
                              320,40,0);

    }

}

void deinit_background()
{
    for(int i = 0 ; i < 5 ; i++)
    {
        al_destroy_bitmap(background[i].backg);
    }
}




/*
    the spritesheet is 48x48px
    13px above the bottom
*/


// sprite sheets

typedef struct SPRITES
{
    ALLEGRO_BITMAP* sheetright;
    ALLEGRO_BITMAP* sheetleft;

    ALLEGRO_BITMAP* idleright[4];
    ALLEGRO_BITMAP* idleleft[4];

    ALLEGRO_BITMAP* runningright[8];
    ALLEGRO_BITMAP* runningleft[8];

    ALLEGRO_BITMAP* fightingright[5];
    ALLEGRO_BITMAP* fightingleft[5];


    ALLEGRO_BITMAP* resting[8];

    ALLEGRO_BITMAP* damage[4];

    ALLEGRO_BITMAP* death[9];

    ALLEGRO_BITMAP* skeleton_sprite;
    ALLEGRO_BITMAP* skeleton_filped_sprite;

    ALLEGRO_BITMAP* old_guardian;

} SPRITES;

SPRITES sprites;


void sprite_init()
{
    sprites.sheetright = al_load_bitmap("HeavyBanditright.png");
    must_init(sprites.sheetright,"sprite sheet");

    sprites.sheetleft = al_load_bitmap("HeavyBanditleft.png");
    must_init(sprites.sheetright,"sprite sheet");

    sprites.skeleton_sprite = al_load_bitmap("skeleton.png");
    must_init(sprites.skeleton_sprite,"skeleton");

    sprites.skeleton_filped_sprite = al_load_bitmap("skeleton_filped.png");
    must_init(sprites.skeleton_filped_sprite,"skeleton");

    sprites.old_guardian = al_load_bitmap("old_guardian.png");
    must_init(sprites.old_guardian,"old_guardian");

    sprites.idleright[0] = al_create_sub_bitmap(sprites.sheetright,336,0,48,48);
    sprites.idleright[1] = al_create_sub_bitmap(sprites.sheetright,288,0,48,48);
    sprites.idleright[2] = al_create_sub_bitmap(sprites.sheetright,240,0,48,48);
    sprites.idleright[3] = al_create_sub_bitmap(sprites.sheetright,192,0,48,48);

    sprites.idleleft[0] = al_create_sub_bitmap(sprites.sheetleft,0,0,48,48);
    sprites.idleleft[1] = al_create_sub_bitmap(sprites.sheetleft,48,0,48,48);
    sprites.idleleft[2] = al_create_sub_bitmap(sprites.sheetleft,96,0,48,48);
    sprites.idleleft[3] = al_create_sub_bitmap(sprites.sheetleft,144,0,48,48);

    sprites.runningright[0] = al_create_sub_bitmap(sprites.sheetright,336,48,48,48);
    sprites.runningright[1] = al_create_sub_bitmap(sprites.sheetright,288,48,48,48);
    sprites.runningright[2] = al_create_sub_bitmap(sprites.sheetright,240,48,48,48);
    sprites.runningright[3] = al_create_sub_bitmap(sprites.sheetright,192,48,48,48);
    sprites.runningright[4] = al_create_sub_bitmap(sprites.sheetright,144,48,48,48);
    sprites.runningright[5] = al_create_sub_bitmap(sprites.sheetright,96,48,48,48);
    sprites.runningright[6] = al_create_sub_bitmap(sprites.sheetright,48,48,48,48);
    sprites.runningright[7] = al_create_sub_bitmap(sprites.sheetright,0,48,48,48);

    sprites.runningleft[7] = al_create_sub_bitmap(sprites.sheetleft,336,48,48,48);
    sprites.runningleft[6] = al_create_sub_bitmap(sprites.sheetleft,288,48,48,48);
    sprites.runningleft[5] = al_create_sub_bitmap(sprites.sheetleft,240,48,48,48);
    sprites.runningleft[4] = al_create_sub_bitmap(sprites.sheetleft,192,48,48,48);
    sprites.runningleft[3] = al_create_sub_bitmap(sprites.sheetleft,144,48,48,48);
    sprites.runningleft[2] = al_create_sub_bitmap(sprites.sheetleft,96,48,48,48);
    sprites.runningleft[1] = al_create_sub_bitmap(sprites.sheetleft,48,48,48,48);
    sprites.runningleft[0] = al_create_sub_bitmap(sprites.sheetleft,0,48,48,48);

    sprites.fightingright[0] = al_create_sub_bitmap(sprites.sheetright,192,96,48,48);
    sprites.fightingright[1] = al_create_sub_bitmap(sprites.sheetright,144,96,48,48);
    sprites.fightingright[2] = al_create_sub_bitmap(sprites.sheetright,96,96,48,48);
    sprites.fightingright[3] = al_create_sub_bitmap(sprites.sheetright,48,96,48,48);
    sprites.fightingright[4] = al_create_sub_bitmap(sprites.sheetright,0,96,48,48);

    sprites.fightingleft[0] = al_create_sub_bitmap(sprites.sheetleft,144,96,48,48);
    sprites.fightingleft[1] = al_create_sub_bitmap(sprites.sheetleft,192,96,48,48);
    sprites.fightingleft[2] = al_create_sub_bitmap(sprites.sheetleft,240,96,48,48);
    sprites.fightingleft[3] = al_create_sub_bitmap(sprites.sheetleft,288,96,48,48);
    sprites.fightingleft[4] = al_create_sub_bitmap(sprites.sheetleft,336,96,48,48);


    sprites.resting[0] = al_create_sub_bitmap(sprites.sheetright,336,144,48,48);
    sprites.resting[1] = al_create_sub_bitmap(sprites.sheetright,288,144,48,48);
    sprites.resting[2] = al_create_sub_bitmap(sprites.sheetright,240,144,48,48);
    sprites.resting[3] = al_create_sub_bitmap(sprites.sheetright,192,144,48,48);
    sprites.resting[4] = al_create_sub_bitmap(sprites.sheetright,144,144,48,48);
    sprites.resting[5] = al_create_sub_bitmap(sprites.sheetright,96,144,48,48);
    sprites.resting[6] = al_create_sub_bitmap(sprites.sheetright,48,144,48,48);
    sprites.resting[7] = al_create_sub_bitmap(sprites.sheetright,0,144,48,48);

    sprites.damage[0] = al_create_sub_bitmap(sprites.sheetright,336,192,48,48);
    sprites.damage[1] = al_create_sub_bitmap(sprites.sheetright,288,192,48,48);
    sprites.damage[2] = al_create_sub_bitmap(sprites.sheetright,240,192,48,48);
    sprites.damage[3] = al_create_sub_bitmap(sprites.sheetright,192,192,48,48);


    sprites.death[7] = al_create_sub_bitmap(sprites.sheetright,336,144,48,48);
    sprites.death[6] = al_create_sub_bitmap(sprites.sheetright,288,144,48,48);
    sprites.death[5] = al_create_sub_bitmap(sprites.sheetright,240,144,48,48);
    sprites.death[4] = al_create_sub_bitmap(sprites.sheetright,192,144,48,48);
    sprites.death[3] = al_create_sub_bitmap(sprites.sheetright,144,144,48,48);
    sprites.death[2] = al_create_sub_bitmap(sprites.sheetright,96,144,48,48);
    sprites.death[1] = al_create_sub_bitmap(sprites.sheetright,48,144,48,48);
    sprites.death[0] = al_create_sub_bitmap(sprites.sheetright,0,144,48,48);
    sprites.death[8] = al_create_sub_bitmap(sprites.sheetright,144,192,48,48);





    //al_create_sub_bitmap(sprites,198,0,48,48);

}

bool fightON;

typedef struct player_animation_types
{

    int maxFrames;
    int curFrame;
    int frameCount;
    int frameDelay;
} player_animation_types;

player_animation_types runningR;
player_animation_types runningL;
player_animation_types idleR;
player_animation_types idleL;
player_animation_types fightingR;
player_animation_types fightingL;
player_animation_types resting;
player_animation_types death;

void init_player_animation()
{
    runningR.maxFrames = 8;
    runningR.frameCount =0;
    runningR.curFrame = 0;
    runningR.frameDelay = 10;

    runningL.maxFrames = 8;
    runningL.frameCount =0;
    runningL.curFrame = 0;
    runningL.frameDelay = 10;

    idleR.maxFrames = 4;
    idleR.frameCount = 0;
    idleR.frameDelay = 15;
    idleR.curFrame = 0;

    idleL.maxFrames = 4;
    idleL.frameCount = 0;
    idleL.frameDelay = 15;
    idleL.curFrame = 0;

    fightingR.maxFrames = 5;
    fightingR.frameCount =0;
    fightingR.curFrame = 0;
    fightingR.frameDelay = 8;

    fightingL.maxFrames = 5;
    fightingL.frameCount =0;
    fightingL.curFrame = 0;
    fightingL.frameDelay = 8;

    death.maxFrames = 9;
    death.frameCount =0;
    death.curFrame = 0;
    death.frameDelay = 15;
}

void update_player_animation()
{

    lastPosition=X;
    if(key[ALLEGRO_KEY_LEFT] && !fightON)
    {
        X--;
        direction=-1;
    }
    else if(key[ALLEGRO_KEY_RIGHT] && !fightON)
    {
        X++;
        direction = 1;
    }
    else if(key[ALLEGRO_KEY_X])
    {
        fightON = true;

    }
    if(key[ALLEGRO_KEY_Z] && !WAIT)
    {
        if(direction == 1)
            X+=3;
        background[3].speed = 2;
        if(direction == -1)
            X-=3;
        background[3].speed = 2;
    }


    if(X <0 -13)
    {
        X=0-13 ;
    }
    if(X>320-35)
    {
        X=320-35;
    }
    printf("%d\n",X);
    if(++runningR.frameCount >= runningR.frameDelay)
    {
        if(++runningR.curFrame >= runningR.maxFrames)
            runningR.curFrame = 0;


        runningR.frameCount = 0;
    }

    if(++runningL.frameCount >= runningL.frameDelay)
    {
        if(++runningL.curFrame >= runningL.maxFrames)
            runningL.curFrame = 0;


        runningL.frameCount = 0;
    }


    if(++idleR.frameCount >= idleR.frameDelay)
    {
        if(++idleR.curFrame >= idleR.maxFrames)
            idleR.curFrame = 0;


        idleR.frameCount = 0;
    }

    if(++idleL.frameCount >= idleL.frameDelay)
    {
        if(++idleL.curFrame >= idleL.maxFrames)
            idleL.curFrame = 0;


        idleL.frameCount = 0;
    }


    if(++fightingR.frameCount >= fightingR.frameDelay)
    {
        if(++fightingR.curFrame >= fightingR.maxFrames)
            fightingR.curFrame = 0;

        fightingR.frameCount = 0;
    }

    if(++fightingL.frameCount >= fightingL.frameDelay)
    {
        if(++fightingL.curFrame >= fightingL.maxFrames)
            fightingL.curFrame = 0;

        fightingL.frameCount = 0;
    }

    if(++death.frameCount >= death.frameDelay)
    {
        if(++death.curFrame >= death.maxFrames)
            death.curFrame = 8;


        death.frameCount = 0;
    }

}


void draw_player_animation()
{

    if(lastPosition != X)
    {
        if(direction == 1)
            al_draw_bitmap(sprites.runningright[runningR.curFrame],X,Y,0);
        if(direction == -1)
            al_draw_bitmap(sprites.runningleft[runningL.curFrame],X,Y,0);

        lastPosition=X;
        WAIT=false;
    }
    else if(fightON)
    {
        if(direction == 1)
            al_draw_bitmap(sprites.fightingright[fightingR.curFrame],X,Y,0);
        if(direction == -1)
            al_draw_bitmap(sprites.fightingleft[fightingL.curFrame],X,Y,0);

        play_blade_swipe();
        fightON = false;
        WAIT=true;
    }
    else
    {
        if(direction == 1 || direction == 0)
            al_draw_bitmap(sprites.idleright[idleR.curFrame],X,Y,0);
        if(direction == -1)
            al_draw_bitmap(sprites.idleleft[idleL.curFrame],X,Y,0);

        WAIT = true;
    }
    // al_draw_bitmap(sub_sprite2,0,BUFFER_H-96-13,0);
}

void player_death_animation(bool D)
{
    if(D)
    {
        al_draw_bitmap(sprites.death[death.curFrame],X,Y,0);
    }

}


void sprite_deinit()
{


    for(int i = 0; i < 4 ; i++)
    {
        al_destroy_bitmap(sprites.idleright[i]);
        al_destroy_bitmap(sprites.idleleft[i]);
    }
    for(int i = 0; i < 8 ; i++)
    {
        al_destroy_bitmap(sprites.runningright[i]);
        al_destroy_bitmap(sprites.runningleft[i]);
    }
    for(int i = 0; i < 5 ; i++)
    {
        al_destroy_bitmap(sprites.fightingright[i]);
        al_destroy_bitmap(sprites.fightingleft[i]);
    }

    for(int i = 0; i < 8 ; i++)
    {
        al_destroy_bitmap(sprites.resting[i]);
    }

    for(int i=0; i<4; i++)
    {
        al_destroy_bitmap(sprites.damage[i]);
    }
    for(int i=0; i<9; i++)
    {
        al_destroy_bitmap(sprites.death[i]);
    }

    al_destroy_bitmap(sprites.skeleton_filped_sprite);
    al_destroy_bitmap(sprites.skeleton_sprite);
    al_destroy_bitmap(sprites.sheetright);
    al_destroy_bitmap(sprites.sheetleft);
}

typedef struct Enemy_animation_type
{
    int x,y;
    int row;
    int column;
    int direction;

    int maxFrames;
    int curFrame;
    int frameCount;
    int frameDelay;

    int frameHeight;
    int frameWeight;

} Enemy_Animation_Type;


Enemy_Animation_Type E_idleR;
Enemy_Animation_Type E_attackR;
Enemy_Animation_Type E_damageR;
Enemy_Animation_Type E_deathR;
Enemy_Animation_Type E_walkR;

Enemy_Animation_Type E_idleL;
Enemy_Animation_Type E_attackL;
Enemy_Animation_Type E_damageL;
Enemy_Animation_Type E_deathL;
Enemy_Animation_Type E_walkL;

Enemy_Animation_Type G_idle;
Enemy_Animation_Type G_attack1;
Enemy_Animation_Type G_attack2;
Enemy_Animation_Type G_damage;
Enemy_Animation_Type G_death;
Enemy_Animation_Type G_walk;
Enemy_Animation_Type G_jump;

void init_enemy_player_sprites()
{

    E_idleR.curFrame = 0;
    E_idleR.frameCount = 0;
    E_idleR.maxFrames = 6;
    E_idleR.frameDelay = 15;
    E_idleR.column = 3 ;

    E_walkR.curFrame = 0;
    E_walkR.frameCount = 0;
    E_walkR.maxFrames = 6;
    E_walkR.frameDelay = 10;
    E_walkR.column = 2 ;

    E_attackR.curFrame = 0;
    E_attackR.frameCount = 0;
    E_attackR.maxFrames = 10;
    E_attackR.frameDelay = 10;
    E_attackR.column = 1 ;

    E_damageR.curFrame = 0;
    E_damageR.frameCount = 0;
    E_damageR.maxFrames = 4;
    E_damageR.frameDelay = 10;
    E_damageR.column = 5;

    E_deathR.curFrame = 0;
    E_deathR.frameCount = 0;
    E_deathR.maxFrames = 5;
    E_deathR.frameDelay = 15;
    E_deathR.column = 4 ;

    ///

    E_idleL.curFrame = 0;
    E_idleL.frameCount = 0;
    E_idleL.maxFrames = 6;
    E_idleL.frameDelay = 15;
    E_idleL.column = 2 ;

    E_walkL.curFrame = 0;
    E_walkL.frameCount = 0;
    E_walkL.maxFrames = 6;
    E_walkL.frameDelay = 10;
    E_walkL.column = 3 ;

    E_attackL.curFrame = 0;
    E_attackL.frameCount = 0;
    E_attackL.maxFrames = 10;
    E_attackL.frameDelay = 10;
    E_attackL.column = 4 ;

    E_damageL.curFrame = 0;
    E_damageL.frameCount = 0;
    E_damageL.maxFrames = 4;
    E_damageL.frameDelay = 10;
    E_damageL.column = 0 ;

    E_deathL.curFrame = 0;
    E_deathL.frameCount = 0;
    E_deathL.maxFrames = 5;
    E_deathL.frameDelay = 15;
    E_deathL.column = 1 ;

    ///

    G_idle.curFrame = 0;
    G_idle.frameCount = 0;
    G_idle.frameDelay = 15;
    G_idle.maxFrames = 6 ;
    G_idle.column = 4 ;

    G_attack1.curFrame = 0;
    G_attack1.frameCount = 0;
    G_attack1.frameDelay = 10;
    G_attack1.maxFrames = 10 ;
    G_attack1.column = 0 ;

    G_attack2.curFrame = 0;
    G_attack2.frameCount = 0;
    G_attack2.frameDelay = 10;
    G_attack2.maxFrames = 8 ;
    G_attack2.column = 1 ;

    G_death.curFrame = 0;
    G_death.frameCount = 0;
    G_death.frameDelay = 15;
    G_death.maxFrames = 10 ;
    G_death.column = 3 ;

    G_damage.curFrame = 0;
    G_damage.frameCount = 0;
    G_damage.frameDelay = 10;
    G_damage.maxFrames = 4 ;
    G_damage.column = 5 ;

    G_jump.curFrame = 0;
    G_jump.frameCount = 0;
    G_jump.frameDelay = 10;
    G_jump.maxFrames = 7 ;
    G_jump.column = 6 ;

    G_walk.curFrame = 0;
    G_walk.frameCount = 0;
    G_walk.frameDelay = 10;
    G_walk.maxFrames = 8 ;
    G_walk.column = 2 ;


}

typedef struct Enemy
{
    int x,y;
    int health;
    int direction;
    int finalpath;

    bool walkON;
    bool attackON;
    bool deathON;
} Enemy;

Enemy enemy;

void update_enemy_animation()
{


    if(enemy.x < 0-24)
    {
        enemy.x = 0;
    }
    if(enemy.x > 320 - 120+24)
    {
        enemy.x = 320 - 100;

    }




    if(++E_idleR.frameCount >= E_idleR.frameDelay)
    {
        if(++E_idleR.curFrame >= E_idleR.maxFrames)
            E_idleR.curFrame = 0;

        E_idleR.frameCount = 0;
    }

    if(++E_walkR.frameCount >= E_walkR.frameDelay)
    {
        if(++E_walkR.curFrame >= E_walkR.maxFrames)
            E_walkR.curFrame = 0;

        E_walkR.frameCount = 0;
    }

    if(enemy.attackON){
    if(++E_attackR.frameCount >= E_attackR.frameDelay)
    {
        if(++E_attackR.curFrame >= E_attackR.maxFrames)
            E_attackR.curFrame = 0;


        E_attackR.frameCount = 0;
    }
    }

    if(++E_deathR.frameCount >= E_deathR.frameDelay)
    {
        if(++E_deathR.curFrame >= E_deathR.maxFrames)
            E_deathR.curFrame = 0;


        E_deathR.frameCount = 0;
    }

    if(++E_damageR.frameCount >= E_damageR.frameDelay)
    {
        if(++E_damageR.curFrame >= E_damageR.maxFrames)
            E_damageR.curFrame = 0;


        E_damageR.frameCount = 0;
    }

    if(++E_walkR.frameCount >= E_walkR.frameDelay)
    {
        if(++E_walkR.curFrame >= E_walkR.maxFrames)
            E_walkR.curFrame = 0;


        E_walkR.frameCount = 0;
    }

    ///

    if(++E_idleL.frameCount >= E_idleL.frameDelay)
    {
        if(++E_idleL.curFrame >= E_idleL.maxFrames)
            E_idleL.curFrame = 0;

        E_idleL.frameCount = 0;
    }

    if(++E_walkL.frameCount >= E_walkL.frameDelay)
    {
        if(++E_walkL.curFrame >= E_walkL.maxFrames)
            E_walkL.curFrame = 0;

        E_walkL.frameCount = 0;
    }

    if(++E_attackL.frameCount >= E_attackL.frameDelay)
    {
        if(++E_attackL.curFrame >= E_attackL.maxFrames)
            E_attackL.curFrame = 0;


        E_attackL.frameCount = 0;
    }

    if(++E_deathL.frameCount >= E_deathL.frameDelay)
    {
        if(++E_deathL.curFrame >= E_deathL.maxFrames)
            E_deathL.curFrame = 0;


        E_deathL.frameCount = 0;
    }

    if(++E_damageL.frameCount >= E_damageL.frameDelay)
    {
        if(++E_damageL.curFrame >= E_damageL.maxFrames)
            E_damageL.curFrame = 0;


        E_damageL.frameCount = 0;
    }

    if(++E_walkL.frameCount >= E_walkL.frameDelay)
    {
        if(++E_walkL.curFrame >= E_walkL.maxFrames)
            E_walkL.curFrame = 0;


        E_walkL.frameCount = 0;
    }

    ///


    if(++G_idle.frameCount >= G_idle.frameDelay)
    {
        if(++G_idle.curFrame >= G_idle.maxFrames)
            G_idle.curFrame = 0;


        G_idle.frameCount = 0;
    }

    if(++G_attack1.frameCount >= G_attack1.frameDelay)
    {
        if(++G_attack1.curFrame >= G_attack1.maxFrames)
            G_attack1.curFrame = 0;


        G_attack1.frameCount = 0;
    }

    if(++G_attack2.frameCount >= G_attack2.frameDelay)
    {
        if(++G_attack2.curFrame >= G_attack2.maxFrames)
            G_attack2.curFrame = 0;


        G_attack2.frameCount = 0;
    }

    if(++G_jump.frameCount >= G_jump.frameDelay)
    {
        if(++G_jump.curFrame >= G_jump.maxFrames)
            G_jump.curFrame = 0;


        G_jump.frameCount = 0;
    }

    if(++G_death.frameCount >= G_death.frameDelay)
    {
        if(++G_death.curFrame >= G_death.maxFrames)
            G_death.curFrame = 0;


        G_death.frameCount = 0;
    }

    if(++G_damage.frameCount >= G_damage.frameDelay)
    {
        if(++G_damage.curFrame >= G_damage.maxFrames)
            G_damage.curFrame = 0;


        G_damage.frameCount = 0;
    }

    if(++G_walk.frameCount >= G_walk.frameDelay)
    {
        if(++G_walk.curFrame >= G_walk.maxFrames)
            G_walk.curFrame = 0;


        G_walk.frameCount = 0;
    }

}

void enemy_stats_init()
{
    enemy.x = 0;
    enemy.y = 120;
    enemy.health = 500;
    enemy.direction = 1;
    enemy.finalpath = 180;
}
// TODO (Yash#1#): Here BoYE

// ENEMY AI
//  What it will do ? 1. It will follow the person - change the direction,
//                    2. Attack after 5 sec
//                    3.
//

void enemyAI()
{
    printf("Enemy position- %d\t", enemy.x );


    int player_position = X-W/2;
    int enemy_position = enemy.x - 60;
    // 40 is region
    if(enemy_position +40 < player_position - 70 )
    {
        enemy.x += 1 ;
        enemy.walkON = true;
        enemy.direction = +1;
    }
    if(enemy_position - 40 > player_position - 70  )
    {
        enemy.x -= 1;

        enemy.direction = -1;
        enemy.attackON = false;
        enemy.walkON = true;
    }
    /*
    if( enemy_position > player_position || enemy_position < player_position)
    {
        enemy.walkON = false;
        if(seconds%3 == 0)
        {
            enemy.attackON = true;
        }
        if(E_attackL.curFrame == 10 || E_attackR.curFrame == 10)
        {

            enemy.attackON = false;
            enemy.deathON = false;
        }
    }



        if(enemy.x < player_position )
        {
            enemy.x += 1 ;
            enemy.direction = +1;
            enemy.walkON = true;
        }
        else if(enemy.x > player_position  )
        {
            enemy.x -= 1;
            enemy.direction = -1;
            enemy.walkON = true;
        }
        else if( enemy.x > X || enemy.x < X )
        {
            enemy.walkON = false;
        }
    */
}

enum animationType
{
    attackON,
    walkON,
    deathON,

};

void draw_enemy_animation()
{

    printf("enemy.CurFrame %d enemy.framecount %d\t",E_attackR.curFrame,E_attackR.frameCount);


    if(enemy.attackON  )
    {
        if(enemy.direction == 1)
            al_draw_bitmap_region(sprites.skeleton_sprite,E_attackR.column*120,E_attackR.curFrame*120,120,120,enemy.x,120,0);
        if(enemy.direction == -1)
            al_draw_bitmap_region(sprites.skeleton_filped_sprite,E_attackL.column*120,E_attackL.curFrame*120,120,120,enemy.x,120,0);
    }
    else if(enemy.walkON  && !enemy.attackON)
    {
        if(enemy.direction == 1)
            al_draw_bitmap_region(sprites.skeleton_sprite,E_walkR.column*120,E_walkR.curFrame*120,120,120,enemy.x,120,0);
        if(enemy.direction == -1)
            al_draw_bitmap_region(sprites.skeleton_filped_sprite,E_walkL.column*120,E_walkL.curFrame*120,120,120,enemy.x,120,0);
    }
    else if(enemy.deathON)
    {
        al_draw_bitmap_region(sprites.skeleton_sprite,E_damageR.column*120,E_damageR.curFrame*120,120,120,enemy.x,120,0);
    }
    else if(enemy.health <= 0)
    {
        al_draw_bitmap_region(sprites.skeleton_sprite,E_deathR.column*120,E_deathR.curFrame*120,120,120,enemy.x,120,0);
    }
    else
    {
        if(enemy.direction == 1)
            al_draw_bitmap_region(sprites.skeleton_sprite,E_idleR.column*120,E_idleR.curFrame*120,120,120,enemy.x,120,0);
        if(enemy.direction == -1)
            al_draw_bitmap_region(sprites.skeleton_filped_sprite,E_idleL.column*120,E_idleL.curFrame*120,120,120,enemy.x,120,0);
    }


//    al_draw_bitmap_region(sprites.old_guardian,G_idle.column*120,G_idle.curFrame*120,120,120,0,0,0);

//    al_draw_bitmap_region(sprites.old_guardian,G_attack1.column*120,G_attack1.curFrame*120,120,120,120,0,0);

//    al_draw_bitmap_region(sprites.old_guardian,G_attack2.column*120,G_attack2.curFrame*120,120,120,240,0,0);

//    al_draw_bitmap_region(sprites.old_guardian,G_death.column*120,G_death.curFrame*120,120,120,0,120,0);

//    al_draw_bitmap_region(sprites.old_guardian,G_damage.column*120,G_damage.curFrame*120,120,120,120,120,0);

//    al_draw_bitmap_region(sprites.old_guardian,G_jump.column*120,G_jump.curFrame*120,120,120,240,120,0);


}

// sound

ALLEGRO_SAMPLE* industrial_music;
ALLEGRO_SAMPLE* blade_swipe;
ALLEGRO_SAMPLE* death_music;
ALLEGRO_SAMPLE* hurt;

void audio_init()
{
    must_init(al_install_audio(), "audio");
    must_init(al_init_acodec_addon(), "audio codecs");
    must_init(al_reserve_samples(16), "reserve samples");

    industrial_music = al_load_sample("industrial.wav");
    must_init(industrial_music, "industrial musc");

    blade_swipe = al_load_sample("blade_swipe.wav");
    must_init(blade_swipe, "blade swipe");

    death_music = al_load_sample("death.wav");
    must_init(death_music, "death");
}

void play_death_music()
{
    al_play_sample(death_music,1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
}

void play_blade_swipe()
{
    // al_play_sample(blade_swipe,0.2, 0.0, 0.5, ALLEGRO_PLAYMODE_ONCE, NULL);
}

void stop_industrial_music()
{
//    al_stop_sample(industrial_music);
}

void audio_deinit()
{
    al_destroy_sample(industrial_music);
    al_destroy_sample(blade_swipe);
    al_destroy_sample(death_music);
    al_destroy_sample(hurt);
}


int main()
{

    must_init(al_init(),"allegro");
    must_init(al_install_keyboard(), "keyboard");
    must_init(al_init_primitives_addon(), "primitives");
    must_init(al_init_image_addon(),"Image addons");


    disp_init();
    audio_init();

    ALLEGRO_TIMER* timer = al_create_timer(1.0 /60.0);
    must_init(timer,"timer");
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    must_init(queue,"queue");
    ALLEGRO_FONT* font = al_create_builtin_font();
    must_init(font,"font");


    keyboard_init();


    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));



    init_background();
    sprite_init();
    init_player_animation();
    init_enemy_player_sprites();
    enemy_stats_init();

    ALLEGRO_EVENT event;
    bool done = false;
    bool redraw = true;



    //al_play_sample(industrial_music,1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, NULL);

    al_start_timer(timer);
    while(1)
    {


        al_wait_for_event(queue, &event);

        switch(event.type)
        {
        case ALLEGRO_EVENT_TIMER:
            frames++;
            if(frames % 60 == 0)
            {
                seconds++;
            }
            update_background();
            enemyAI();
            update_player_animation();
            update_enemy_animation();

            if(key[ALLEGRO_KEY_ESCAPE])
                done = true;

            redraw = true;
            break;

        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            done = true;
            break;
        }

        if(done)
        {
            break;
        }

        keyboard_update(&event);


        if(redraw && al_is_event_queue_empty(queue))
        {
            disp_pre_draw();
            al_clear_to_color(al_map_rgb(0, 0, 0));
            draw_background();
            draw_player_animation();
            draw_enemy_animation();
            al_draw_textf(font,al_map_rgb(255,255,255),0,0,0,"%d \t %d",frames,seconds);
            //al_draw_bitmap_region(sprites.old_guardian,G_idle.column*120,G_idle.curFrame*120,120,120,50,135,0);

            //     player_death_animation(true);

            //al_draw_bitmap_region(sprites.skeleton_sprite,3*120,E_idle.curFrame*120,120,120,100,130,0);





            disp_post_draw();
            redraw = false;



        }



    }


    sprite_deinit();
    deinit_background();
    al_destroy_font(font);
    audio_deinit();
    disp_deinit();
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);


    return 0;
}



