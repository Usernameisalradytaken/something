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
bool lines = 1;





// general things
void must_init(bool test, const char* des)
{
    if(test)
        return;

    printf ("coundln't initialize the %s\n", des);
}
#define multi_n 2
#define BUFFER_W 320*multi_n
#define BUFFER_H 240*multi_n

#define DISP_SCALE 2
#define DISP_W (BUFFER_W * DISP_SCALE)
#define DISP_H (BUFFER_H * DISP_SCALE)

ALLEGRO_DISPLAY* disp;
ALLEGRO_BITMAP* buffer;

void disp_init()
{
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
        	al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
    disp = al_create_display(DISP_W, DISP_H);
    must_init(disp, "display");

    const float scale_factor_x = ((float)al_get_display_width(disp)) / DISP_W;
	const float scale_factor_y = ((float)al_get_display_height(disp)) / DISP_H;

	printf("%1.f - %1.f \n",scale_factor_x,scale_factor_y);

    	ALLEGRO_TRANSFORM t;

    al_identity_transform(&t);
	al_scale_transform(&t, scale_factor_x, scale_factor_y);
	al_use_transform(&t);


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

// randomness

int between(int lo, int hi)
{
    return lo + (rand() % (hi - lo));
}

float between_f(float lo, float hi)
{
    return lo + ((float)rand() / (float)RAND_MAX) * (hi - lo);
}




// lines
#define line_thickness 2
ALLEGRO_COLOR line_colour = al_map_rgba(105,105,105,9);
void draw_lines()
{

    if(lines)
    {
        al_draw_line(BUFFER_W/2,BUFFER_H/2,0,BUFFER_H-(50*multi_n),line_colour,line_thickness); // fence left line
        al_draw_line(BUFFER_W/2,BUFFER_H/2,BUFFER_W,BUFFER_H-(50*multi_n),line_colour,line_thickness);  //fence right line
        al_draw_line(BUFFER_W/2,BUFFER_H/2-(30*multi_n),0,80,line_colour,line_thickness); // left wall line
        al_draw_line(BUFFER_W/2,BUFFER_H/2-(30*multi_n),BUFFER_W/2,BUFFER_H/2,line_colour,line_thickness); // vertical middle line
        al_draw_line(BUFFER_W/2,BUFFER_H/2-(30*multi_n),BUFFER_W,80,line_colour,line_thickness); // right wall line
    }


    al_draw_line(BUFFER_W/2,BUFFER_H/2,0,BUFFER_H,line_colour,line_thickness); //mid to bottom left conner
    al_draw_line(BUFFER_W/2,BUFFER_H/2,BUFFER_W,BUFFER_H,line_colour,line_thickness); // mid to bottom right conner
    al_draw_line(BUFFER_W/2,BUFFER_H/2,(BUFFER_W*1.0)/3.0,BUFFER_H,line_colour,line_thickness);  // first partition
    al_draw_line(BUFFER_W/2,BUFFER_H/2,BUFFER_W - BUFFER_W/3, BUFFER_H,line_colour,line_thickness);  // last partiton

}


//motion walls

enum motion_walls_types
{
    L_100,
    L_010,
    L_001,
    L_110,
    L_011,
    L_101,
    L_111,
    L_nl
};

typedef struct Motion_Walls
{
    ALLEGRO_COLOR color;
    float dx,dy,dy1,dy2;
    bool On_Off;
    int type;
} Motion_Walls;

Motion_Walls motion_walls[L_nl];

void init_walls()
{

    for(int i=0 ; i < L_nl ; i++)
    {
        Motion_Walls* mw_pointer = &motion_walls[i];
        mw_pointer->color = al_map_rgb(between(0,255),between(0,255),between(0,255));
        mw_pointer->dx =0;
        mw_pointer->dy =0;
        mw_pointer->dy1 =0;mw_pointer->dy2 =0;
        mw_pointer->On_Off = false;
        mw_pointer->type = i;
        if(i == 1)
        {
           // mw_pointer->On_Off = true;
        }

    }
}

void update_motion_walls()
{
    // this will chosse which wall will made on 600 frame
    int a = between(0,7);

    for(int i=0; i<L_nl ; i++)
    {
        Motion_Walls* mw_pointer = &motion_walls[i];

        // this will reset the wall
        if(mw_pointer->dx >= 200)
        {
            mw_pointer->On_Off = false;

            mw_pointer->dx=0;

            mw_pointer->color = al_map_rgb(between(0,255),between(0,255),between(0,255));

        }
        // this will make new wall at 600 frames
        if(frames%200 == 0 && i == between(1,6))
        {
            mw_pointer->On_Off = true;
        }

        // this will update the wall if "N" wall is true
        if(mw_pointer->On_Off)
        {
            mw_pointer->dx += speed*4.0;
            mw_pointer->dy = (3.0/4.0)*mw_pointer->dx; // for corner
            mw_pointer->dy1 = (7.0/16.0)*mw_pointer->dx; // for fence
            mw_pointer->dy2 = (((BUFFER_W/3.0)*1.0 - 320.0 )/240)*mw_pointer->dy; // one of the third partion of x axis
        }
    }

}


void draw_motion_walls()
{

    for(int i=0; i < L_nl ; i++)
    {
        Motion_Walls* mw_pointer = &motion_walls[i];
        if(mw_pointer->On_Off)
        {
            switch(mw_pointer->type)
            {
            case L_111:
               // al_draw_filled_rectangle( 320.0 - mw_pointer->dx, 240.0 + mw_pointer->dy, 320.0 + mw_pointer->dx, 240.0 + mw_pointer->dy1,mw_pointer->color);    //111
                break;
            case L_110:
                al_draw_filled_rectangle( 320 - mw_pointer->dx, 240 + mw_pointer->dy1, 320 - mw_pointer->dy2, 240 + mw_pointer->dy, mw_pointer->color);    //110
                break;
            case L_100:
                al_draw_filled_rectangle(320 - mw_pointer->dx, 240 + mw_pointer->dy1, 320 + mw_pointer->dy2, 240 + mw_pointer->dy, mw_pointer->color );     //100
                break;
            case L_011:
                al_draw_filled_rectangle(320 + mw_pointer->dx, 240 + mw_pointer->dy1, 320 + mw_pointer->dy2, 240 + mw_pointer->dy, mw_pointer->color);     //011
                break;
            case L_001:

                al_draw_filled_rectangle(320 + mw_pointer->dx, 240 + mw_pointer->dy1, 320 - mw_pointer->dy2, 240 + mw_pointer->dy, mw_pointer->color);     //001
                break;
            case L_101:
                al_draw_filled_rectangle(320 + mw_pointer->dx, 240 + mw_pointer->dy1, 320 - mw_pointer->dy2, 240 + mw_pointer->dy, mw_pointer->color );
                al_draw_filled_rectangle(320 - mw_pointer->dx, 240 + mw_pointer->dy1, 320 + mw_pointer->dy2, 240 + mw_pointer->dy, mw_pointer->color );
                break;
            case L_010:
                al_draw_filled_rectangle( 320 + mw_pointer->dy2, 240 + mw_pointer->dy1, 320 - mw_pointer->dy2, 240 + mw_pointer->dy, mw_pointer->color ); //010
                break;
            }
        }

    }

}






int main()
{
    must_init(al_init(),"allegro");
    must_init(al_install_keyboard(), "keyboard");
    must_init(al_init_primitives_addon(), "primitives");
    must_init(al_init_image_addon(),"Image addons");

    ALLEGRO_TIMER* timer = al_create_timer(1.0 /60.0);
    must_init(timer,"timer");
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    must_init(queue,"queue");
    ALLEGRO_FONT* font = al_create_builtin_font();
    must_init(font,"font");

    disp_init();

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    ALLEGRO_EVENT event;
    bool done = false;
    bool redraw = true;

    init_walls();

    al_start_timer(timer);
    while(1)
    {
        al_wait_for_event(queue, &event);

        switch(event.type)
        {
        case ALLEGRO_EVENT_TIMER:
            frames++;
            update_motion_walls();
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

        if(redraw && al_is_event_queue_empty(queue))
        {
            disp_pre_draw();
            al_clear_to_color(al_map_rgb(0, 0, 0));
            al_draw_textf(font,al_map_rgb(255,255,255),0,0,0,"frames : %ld",frames);
            draw_lines();
            draw_motion_walls();
            disp_post_draw();
            redraw = false;

        }


    }
    disp_deinit();
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_destroy_font(font);
}




