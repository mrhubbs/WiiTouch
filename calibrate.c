#include <stdio.h>
#include <math.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

#define TRUE 1
#define FALSE 0

int main(int argc, char **argv) {
	if (!al_install_system(ALLEGRO_VERSION_INT, NULL)) {
		printf("could not init Allegro\n");
		return 1;
	}

/*	al_set_new_display_flags(ALLEGRO_FULLSCREEN);*/
	ALLEGRO_DISPLAY *display = al_create_display(800, 600);

	if (!display) {
		printf("could not create display\n");
		return 1;
	}

	if (!al_install_mouse()) {
		printf("could not install mouse\n");
		return 1;
	}

	if (!al_install_keyboard()) {
		printf("could not install keybaord\n");
		return 1;
	}

	if (!al_init_image_addon()) {
		printf("could not init image addon\n");
		return 1;
	}

	al_hide_mouse_cursor(display);

	ALLEGRO_BITMAP *target = al_load_bitmap("target.jpg");

	al_set_target_backbuffer(display);
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_bitmap(target, 0, 0, 0);
	al_flip_display();

	ALLEGRO_MOUSE_STATE state;

	ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
	ALLEGRO_EVENT event;

	al_register_event_source(queue, al_get_keyboard_event_source());
	al_register_event_source(queue, al_get_display_event_source(display));

	int w = al_get_bitmap_width(al_get_backbuffer(display));
	int h = al_get_bitmap_height(al_get_backbuffer(display));

	int tw = al_get_bitmap_width(target);
	int th = al_get_bitmap_height(target);

	int d = 0;

	int xCenters[] = {tw / 2, w - tw / 2, w - tw / 2, tw / 2};
	int yCenters[] = {th / 2, th  / 2, h - th / 2, h - th / 2};

	while (d < 4) {
		if (al_get_next_event(queue, &event)) {
			if (event.type == ALLEGRO_EVENT_KEY_DOWN || event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
				break;
			}
		}

		if (d >= 4) {
			break;
		}

		scanf("%d", &d);
		al_get_mouse_state(&state);

		printf("%d,%d\n", xCenters[d - 1], yCenters[d - 1]);

		if (d) {
			switch (d) {
				case 1:
					al_clear_to_color(al_map_rgb(0, 0, 0));
					al_draw_bitmap(target, w - tw, 0, 0);
					al_flip_display();
					break;

				case 2:
					al_clear_to_color(al_map_rgb(0, 0, 0));
					al_draw_bitmap(target, w - tw, h - th, 0);
					al_flip_display();
					break;

				case 3:
					al_clear_to_color(al_map_rgb(0, 0, 0));
					al_draw_bitmap(target, 0, h - th, 0);
					al_flip_display();
					break;

				default: break;
			}
		}
	}

	return 0;
}
