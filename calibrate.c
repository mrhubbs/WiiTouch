#include <stdio.h>
#include <math.h>

#include <sys/select.h>
#include <termios.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

#define TRUE 1
#define FALSE 0

struct termios orig_termios;

int kbhit() {
	struct timeval tv = { 0L, 0L };
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(0, &fds);

	return select(1, &fds, NULL, NULL, &tv);
}

int getch() {
	int r;
	unsigned char c;

	if ((r = read(0, &c, sizeof(c))) < 0) {
		return r;
	}
	else {
		return c;
	}
}

void reset_terminal_mode() {
	tcsetattr(0, TCSANOW, &orig_termios);
}

void set_conio_terminal_mode() {
	struct termios new_termios;

	/* take two copies - one for now, one for later */
	tcgetattr(0, &orig_termios);
	memcpy(&new_termios, &orig_termios, sizeof(new_termios));

	/* register cleanup handler, and set the new terminal mode */
	atexit(reset_terminal_mode);
	cfmakeraw(&new_termios);
	tcsetattr(0, TCSANOW, &new_termios);
}

int main(int argc, char **argv) {
	set_conio_terminal_mode();

	if (!al_install_system(ALLEGRO_VERSION_INT, NULL)) {
		printf("could not init Allegro\n");
		return 1;
	}

	al_set_new_display_flags(ALLEGRO_FULLSCREEN);
	ALLEGRO_DISPLAY *display = al_create_display(1920, 1080);

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

/*	al_set_window_position(display, 250, 150);*/

	int xOfs, yOfs;

	al_get_window_position(display, &xOfs, &yOfs);

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

	int i = 0;

	for (; i < 4; i++) {
		xCenters[i] += xOfs;
		yCenters[i] += yOfs;
	}

	while (d < 4) {
		if (al_get_next_event(queue, &event)) {
			if (event.type == ALLEGRO_EVENT_KEY_DOWN || event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
				break;
			}
		}

		if (kbhit()) {
			d = getch() - 48;
		}

		if (d > 0 && d < 5) {
			printf("%d,%d\n", xCenters[d - 1], yCenters[d - 1]);

			if (d >= 4) {
				return 0;
			}

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

			al_rest(0.4);
		}
	}

	return 0;
}
