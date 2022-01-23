#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_color.h>


long frames;

void must_init(bool test, const char *description)
{

    if(test)
    {
        return;
    }

    printf("Value cant be initialize %s \n", description );
    exit(1);
}

//randomness
int between(int lo, int hi)
{
    return lo + (rand() % hi);
}
//randomness
float between_f(float lo, float hi)
{
    return lo + ((float)rand() / (float)RAND_MAX) * (hi);
}




#define BUFFER_W 480*4
#define BUFFER_H 270*4

#define DISP_SCALE 1
#define DISP_W (BUFFER_W * DISP_SCALE)
#define DISP_H (BUFFER_H * DISP_SCALE)

ALLEGRO_DISPLAY* disp;
ALLEGRO_BITMAP* buffer;

void disp_init()
{
    disp = al_create_display(DISP_W, DISP_H);
    must_init(disp, "display");

    buffer = al_create_bitmap(BUFFER_W, BUFFER_H);
    must_init(buffer, "bitmap buffer");
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
ALLEGRO_FONT* font1;
ALLEGRO_FONT* font2;
ALLEGRO_FONT* font3;
ALLEGRO_FONT* font4;
ALLEGRO_FONT* font5;
void font_draw()
{
    font1 = al_load_ttf_font("./Differentiator.ttf", 58, 0);
    font2 = al_load_ttf_font("./Differentiator.ttf", 44, 0);
    font3 = al_load_ttf_font("./Differentiator.ttf", 32, 0);
    font4 = al_load_ttf_font("./Differentiator.ttf", 52, 0);
    font5 = al_load_ttf_font("./Differentiator.ttf", 78, 0);
    must_init(font1,"font");
    must_init(font2,"font");
    must_init(font3,"font");
    must_init(font4,"font");
    must_init(font5,"font");

}

void font_dinit()
{
    al_destroy_font(font1);
    al_destroy_font(font2);
    al_destroy_font(font3);
    al_destroy_font(font4);
    al_destroy_font(font5);
}
////////////////////////////////

#define total 150

typedef struct ArrayOfNumber
{
    signed char line[100];
    float x,y;
    float dy;

} ArrayOfNumber;

ArrayOfNumber Line[total];


void init_ArrayOfNumber()
{


    for(int i =0; i< total; i++)
    {
        ArrayOfNumber* l = &Line[i];
        l->x=between_f(0,BUFFER_W);
        l->y=-(BUFFER_H);
        l->dy=between_f(0.1,1);
    }
}

void update_ArrayOfNumber()
{
    for(int i=0; i<total; i++)
    {
        ArrayOfNumber* l = &Line[i];
        l->y += l->dy;
        if(l->y >= BUFFER_H)
        {
            l->x=between_f(0,BUFFER_W);
            l->y = 0 - (BUFFER_H*2.5);
            l->dy = between_f(0.2,0.4);
        }
    }

}

void draw_Lines()
{
    //  al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ONE);

    for(int i=0; i<total; i += 5)
    {
        al_draw_multiline_text(font1,al_map_rgba(21,37,16,0.1),Line[i].x,Line[i].y,0,65,0,Line[i].line);
        al_draw_multiline_text(font2,al_map_rgba(0,30,0,0.1),Line[i+1].x,Line[i+1].y,0,55,0,Line[i+1].line);
        al_draw_multiline_text(font3,al_map_rgba(26,43,19,0.1),Line[i+2].x,Line[i+2].y,0,45,0,Line[i+2].line);
        al_draw_multiline_text(font4,al_map_rgba(75,147,51,0.1),Line[i+3].x,Line[i+3].y,0,65,0,Line[i+3].line);
        al_draw_multiline_text(font5,al_map_rgba(0,90,0,0.1),Line[i+4].x,Line[i+4].y,0,95,0,Line[i+4].line);
    }//al_map_rgba(41,81,28,0.5)
}




int main()
{

    must_init(al_init(),"Allegro");
    must_init(al_init_font_addon(),"Font Addons");
    must_init(al_init_ttf_addon(),"TTF fonts addons");

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0);
    must_init(timer,"timer");

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    must_init(queue,"Queue");

    font_draw();

    // al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
//al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
//al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);

    disp_init();


    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    bool redraw = true;
    bool done = false;
    frames = 0;
    ALLEGRO_EVENT event;

    for(int j=0; j<total; j++)
    {

        for(int i=0; i<between(30,50); i++)
        {

            if(i%2==0)
            {
                Line[j].line[i]='0';
                if(i%between(12,15) == 0)
                {
                    Line[j].line[i]='1';
                }
                if(i%between(6,8) == 0)
                {
                    Line[j].line[i]='1';
                }
                if(i%between(3,5) == 0)
                {
                    Line[j].line[i]=' ';
                }
                if(i%between(30,35) == 0)
                {
                    Line[j].line[i]=' ';
                }
            }

            else
            {
                Line[j].line[i]=' ';
            }
        }
    }


// 80 charater of line on x axis

    init_ArrayOfNumber();
    al_start_timer(timer);
    while(1)
    {

        al_wait_for_event(queue, &event);

        switch(event.type)
        {

        //logic here
        case ALLEGRO_EVENT_TIMER:
            update_ArrayOfNumber();
            //init_ArrayOfNumber();
            redraw = true;
            frames++;
            break;

        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            done = true;
            break;
        }

        if(done)
        {
            break;
        }


        if(redraw && al_is_event_queue_empty(queue) )
        {
            disp_pre_draw();
            al_clear_to_color(al_map_rgb(0, 0, 0));
            draw_Lines();
            disp_post_draw();
            redraw = false;
        }


    }

    disp_deinit();
    font_dinit();
    al_destroy_event_queue(queue);
    al_destroy_timer(timer);

    return 0;
}

