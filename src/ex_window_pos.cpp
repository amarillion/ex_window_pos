/* An example demonstrating different blending modes.
 */
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_color.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>

void abort_example(char const *format, ...)
{
   va_list args;
   va_start(args, format);
   vfprintf(stderr, format, args);
   va_end(args);
   exit(1);
}


struct Example
{
   ALLEGRO_FONT *font;
   ALLEGRO_EVENT_QUEUE *queue;
   ALLEGRO_COLOR background, text, white;
   ALLEGRO_DISPLAY *display;
   ALLEGRO_CONFIG *config;
   int dx, dy, dw, dh;
} ex;

static void draw(void)
{
   al_clear_to_color(ex.background);
   al_draw_text(ex.font, ex.text, 10, 10, 0, "Hello World");
}

static void tick(void)
{
   draw();
   al_flip_display();
}

static void run(void)
{
   ALLEGRO_EVENT event;
   bool need_draw = true;

   while (1) {
      if (need_draw && al_is_event_queue_empty(ex.queue)) {
         tick();
         need_draw = false;
      }

      al_wait_for_event(ex.queue, &event);

      switch (event.type) {
         case ALLEGRO_EVENT_DISPLAY_CLOSE:
            return;

         case ALLEGRO_EVENT_KEY_DOWN:
            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
               return;
            break;

         case ALLEGRO_EVENT_TIMER:
            need_draw = true;
            break;
      }
   }
}

static int get_config_int(ALLEGRO_CONFIG *config, const char *section, const char *key, int default_value) {
   const char *val = al_get_config_value(config, section, key);
   if (!val) return default_value;
   return atoi(val);
}

static void set_config_int(ALLEGRO_CONFIG *config, const char *section, const char *key, int value) {
   char buf[10];
   snprintf(buf, sizeof(buf), "%i", value);
   al_set_config_value(config, section, key, buf);
}

static void read_window_position() {
   ex.config = al_load_config_file("config.ini");
   if (!ex.config) {
      ex.config = al_create_config();
   }
   ex.dx = get_config_int(ex.config, "window", "x", INT_MAX);
   ex.dy = get_config_int(ex.config, "window", "y", INT_MAX);
   ex.dw = get_config_int(ex.config, "window", "w", 640);
   ex.dh = get_config_int(ex.config, "window", "h", 480);
   printf("Reading window position: %i,%i: %ix%i\n", ex.dx, ex.dy, ex.dw, ex.dh);
}

static void save_window_position() {
   ALLEGRO_DISPLAY *display = al_get_current_display();
   ex.dw = al_get_display_width(display);
   ex.dh = al_get_display_height(display);
   al_get_window_position(display, &ex.dx, &ex.dy);
   set_config_int(ex.config, "window", "x", ex.dx);
   set_config_int(ex.config, "window", "y", ex.dy);
   set_config_int(ex.config, "window", "w", ex.dw);
   set_config_int(ex.config, "window", "h", ex.dh);
   printf("Saving window position: %i,%i: %ix%i\n", ex.dx, ex.dy, ex.dw, ex.dh);
   al_save_config_file("config.ini", ex.config);
}

static void init(void)
{
   ex.font = al_create_builtin_font();
   if (!ex.font) {
      abort_example("data/fixed_font.tga not found\n");
   }
   ex.background = al_color_name("beige");
   ex.text = al_color_name("black");
   ex.white = al_color_name("white");
}

int main(int argc, char **argv)
{
   ALLEGRO_DISPLAY *display;
   ALLEGRO_TIMER *timer;

   (void)argc;
   (void)argv;

   if (!al_init()) {
      abort_example("Could not init Allegro.\n");
   }

   al_install_keyboard();
   al_install_mouse();
   al_init_font_addon();

   read_window_position();
   al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE);
   al_set_new_window_position(ex.dx, ex.dy);

   display = al_create_display(ex.dw, ex.dh);
   if (!display) {
      abort_example("Error creating display\n");
   }
   
   init();

   timer = al_create_timer(1.0 / 50);

   ex.queue = al_create_event_queue();
   al_register_event_source(ex.queue, al_get_keyboard_event_source());
   al_register_event_source(ex.queue, al_get_mouse_event_source());
   al_register_event_source(ex.queue, al_get_display_event_source(display));
   al_register_event_source(ex.queue, al_get_timer_event_source(timer));

   al_start_timer(timer);
   run();

   al_destroy_event_queue(ex.queue);  

   save_window_position();
   
   return 0;
}
