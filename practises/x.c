
#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>

void must_init(bool test, const char *description)
{
    if(test) return;

    printf("couldn't initialize %s\n", description);
    exit(1);

}





int main()
{
    must_init(al_init(), "allegro");
    must_init(al_install_keyboard(), "keyboard");

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    must_init(timer, "timer");

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    must_init(queue, "queue");

    //al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);

    /* for mointer info
           ALLEGRO_MONITOR_INFO info;

           ALLEGRO_DISPLAY *display = al_create_display(321,231);

           int width = al_get_display_width(display);
           int height = al_get_display_height(display);

           printf("(%d, %d)\n", width, info.y1);

        int adapters ;
       adapters = al_get_num_video_adapters();


       printf("%d adapters found...\n", adapters);



        al_get_monitor_info(adapters, &info);

        printf("(%d, %d) - (%d, %d)\n", info.x1, info.y1, info.x2, info.y2);



        for (int j = 0; j < al_get_num_display_modes(); j++) {

             ALLEGRO_DISPLAY_MODE mode;

             al_get_display_mode(j, &mode);

             printf("   Mode %3d: %4d x %4d, %d Hz\n",
                j, mode.width, mode.height, mode.refresh_rate);
          }
          */

    ALLEGRO_DISPLAY* disp = al_create_display(640, 480);

    int width = al_get_display_width(disp);
    int height = al_get_display_height(disp);

    printf("(%d, %d)\n", width, height);

    must_init(disp, "display");

#define SCALED_WIDTH 320
#define SCALED_HEIGHT 240
    const float scale_factor_x = ((float)al_get_display_width(disp)) / SCALED_WIDTH;
    const float scale_factor_y = ((float)al_get_display_height(disp)) / SCALED_HEIGHT;

    printf("%1.f - %1.f \n",scale_factor_x,scale_factor_y);

    ALLEGRO_TRANSFORM t;

    al_identity_transform(&t);
    al_scale_transform(&t, scale_factor_x, scale_factor_y);
    al_use_transform(&t);

    ALLEGRO_FONT* font = al_create_builtin_font();
    must_init(font, "font");

    must_init(al_init_image_addon(), "image addon");






    character fight;


    ALLEGRO_BITMAP* sprites;
    ALLEGRO_BITMAP* spritess;

    //ALLEGRO_BITMAP* fighting[6];
    fight.maxFrames = 5;
    fight.curFrame = 0;
    fight.frameCount = 0;
    fight.frameDelay = 10;
    ALLEGRO_BITMAP* running[8];
    ALLEGRO_BITMAP* running2[8];
    ALLEGRO_BITMAP* idle[4];

    float speed = 0.5;
    float x1;
    ALLEGRO_BITMAP* background = al_load_bitmap("skill-foreground.png");
    must_init(background,"background");


    sprites = al_load_bitmap("HeavyBanditc.png");
    must_init(sprites,"sprite sheet");

    spritess = al_load_bitmap("HeavyBandit.png");
    must_init(spritess,"sprite sheet");

    idle[0] = al_create_sub_bitmap(sprites,336,0,48,48);
   idle[1] = al_create_sub_bitmap(sprites,288,0,48,48);
idle[2] = al_create_sub_bitmap(sprites,240,0,48,48);
    idle[3] = al_create_sub_bitmap(sprites,192,0,48,48);

    //rest.bitmap[0] = al_create_sub_bitmap(sprites,336,144,48,48);
    //rest.bitmap[1] = al_create_sub_bitmap(sprites,288,144,48,48);
    //rest.bitmap[2] = al_create_sub_bitmap(sprites,240,144,48,48);
    //rest.bitmap[3] = al_create_sub_bitmap(sprites,192,144,48,48);
    //resting[4] = al_create_sub_bitmap(sprites,144,144,48,48);
    //resting[5] = al_create_sub_bitmap(sprites,96,144,48,48);
    //resting[6] = al_create_sub_bitmap(sprites,48,144,48,48);
    //resting[7] = al_create_sub_bitmap(sprites,0,144,48,48);

    fight.bitmap[0] = al_create_sub_bitmap(sprites,192,96,48,48);
    fight.bitmap[1] = al_create_sub_bitmap(sprites,144,96,48,48);
    fight.bitmap[2] = al_create_sub_bitmap(sprites,96,96,48,48);
    fight.bitmap[3] = al_create_sub_bitmap(sprites,48,96,48,48);
    fight.bitmap[4] = al_create_sub_bitmap(sprites,0,96,48,48);
    //fight.bitmap[5] = al_create_sub_bitmap(sprites,192,0,48,48);

    running[0] = al_create_sub_bitmap(sprites,336,48,48,48);
    running[1] = al_create_sub_bitmap(sprites,288,48,48,48);
    running[2] = al_create_sub_bitmap(sprites,240,48,48,48);
    running[3] = al_create_sub_bitmap(sprites,192,48,48,48);
    running[4] = al_create_sub_bitmap(sprites,144,48,48,48);
    running[5] = al_create_sub_bitmap(sprites,96,48,48,48);
    running[6] = al_create_sub_bitmap(sprites,48,48,48,48);
    running[7] = al_create_sub_bitmap(sprites,0,48,48,48);

    running2[7] = al_create_sub_bitmap(spritess,336,48,48,48);
    running2[6] = al_create_sub_bitmap(spritess,288,48,48,48);
    running2[5] = al_create_sub_bitmap(spritess,240,48,48,48);
    running2[4] = al_create_sub_bitmap(spritess,192,48,48,48);
    running2[3] = al_create_sub_bitmap(spritess,144,48,48,48);
    running2[2] = al_create_sub_bitmap(spritess,96,48,48,48);
    running2[1] = al_create_sub_bitmap(spritess,48,48,48,48);
    running2[0] = al_create_sub_bitmap(spritess,0,48,48,48);



    int a;
    bool fightON=false;

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    bool done = false;
    bool redraw = true;
    ALLEGRO_EVENT event;

#define KEY_SEEN     1
#define KEY_RELEASED 2

    unsigned char key[ALLEGRO_KEY_MAX];
    memset(key, 0, sizeof(key));


    al_start_timer(timer);
    while(1)
    {
        a = x;
        al_wait_for_event(queue, &event);

        switch(event.type)
        {
        case ALLEGRO_EVENT_TIMER:
            if(key[ALLEGRO_KEY_LEFT] && !fightON)
            {
                x--;
                direction=-1;
            }
            else if(key[ALLEGRO_KEY_RIGHT] && !fightON)
            {
                x++;
                direction = 1;
            }
            else if(key[ALLEGRO_KEY_X]){
                fightON = true;
            }
            else if(key[ALLEGRO_KEY_ESCAPE])
                done = true;

            for(int i = 0; i < ALLEGRO_KEY_MAX; i++)
                key[i] &= KEY_SEEN;

            redraw = true;
            break;

        case ALLEGRO_EVENT_KEY_DOWN:
            key[event.keyboard.keycode] = KEY_SEEN | KEY_RELEASED;
            break;
        case ALLEGRO_EVENT_KEY_UP:
            key[event.keyboard.keycode] &= KEY_RELEASED;
            break;

        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            done = true;
            break;
        }

        if(done)
            break;

        if(redraw && al_is_event_queue_empty(queue))
        {
            al_clear_to_color(al_map_rgb(0, 0, 0));
            al_draw_text(font, al_map_rgb(255, 255, 255), 0, 0, 0, "Hello world!");



            al_flip_display();

            redraw = false;
        }
    }

    for(int i =0 ; i < 5 ; i++)
    {
        al_destroy_bitmap(fight.bitmap[i]);
    }

    al_destroy_bitmap(sprites);
    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}

