#include <stdio.h>
#include <math.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#define TRUE 1
#define FALSE 0

typedef struct VECTOR {
	float x;
	float y;
}VECTOR;

typedef struct POINT {
	float x;
	float y;
}POINT;

int xOfs = 400;
int yOfs = 0;

POINT input_points[4];
int input_counter = 0;

POINT A;
POINT B;
POINT C;
POINT D;

VECTOR getCornerOffset(POINT point, POINT pointPrime) {
	VECTOR res;
	res.x = pointPrime.x - point.x;
	res.y = pointPrime.y - point.y;

	return res;
}

POINT standardizePoint(POINT input, VECTOR stan) {
	POINT res;
	res.x = input.x + stan.x;
	res.y = input.y + stan.y;

	return res;
}

VECTOR scaleVector(VECTOR v, float scale) {
	VECTOR res;
	res.x = v.x * scale;
	res.y = v.y * scale;

	return res;
}

float distance(POINT a, POINT b) {
	return sqrt( ((a.x - b.x) * (a.x - b.x)) + ((a.y - b.y) * (a.y - b.y)) );
}

int main(int argc, char **argv) {
	A.x = 0;
	A.y = 0;
	B.x = 400;
	B.y = 0;
	C.x = 400;
	C.y = 350;
	D.x = 0;
	D.y = 350;

	if (!al_install_system(ALLEGRO_VERSION_INT, NULL)) {
		printf("could not init Allegro\n");
		return 1;
	}

	ALLEGRO_DISPLAY *display = al_create_display(800, 600);

	if (!display) {
		printf("could not create display\n");
		return 1;
	}

	if (!al_init_primitives_addon()) {
		printf("could not init primitives\n");
		return 1;
	}

	if (!al_install_keyboard()) {
		printf("could not install keybaord\n");
		return 1;
	}

	if (!al_install_mouse()) {
		printf("could not install mouse\n");
		return 1;
	}

	ALLEGRO_BITMAP *bitmap = al_create_bitmap(800, 600);

	if (bitmap == NULL) {
		printf("could not create bitmap\n");
		return 1;
	}

	al_set_target_bitmap(bitmap);
	al_clear_to_color(al_map_rgb(255, 255, 255));
	al_draw_rectangle(A.x + xOfs, A.y + yOfs, C.x + xOfs, C.y + yOfs, al_map_rgb(0, 0, 0), 2.0);

	al_set_target_backbuffer(display);
	al_draw_bitmap(bitmap, 0, 0, 0);
	al_flip_display();

	int pair1 = 0;
	int pair2 = 0;

	ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
	ALLEGRO_EVENT event;
	al_register_event_source(queue, al_get_mouse_event_source());
	al_register_event_source(queue, al_get_keyboard_event_source());
	al_register_event_source(queue, al_get_display_event_source(display));

	while (input_counter < 4) {
		al_wait_for_event(queue, &event);
		if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && event.mouse.button == 1) {
			input_points[input_counter].x = event.mouse.x;
			input_points[input_counter].y = event.mouse.y;

			input_counter++;

			if (input_counter > 1) {
				pair1 = input_counter - 2;
				pair2 = input_counter - 1;

				al_set_target_bitmap(bitmap);
				al_draw_line(input_points[pair1].x, input_points[pair1].y, input_points[pair2].x, input_points[pair2].y, al_map_rgb(0, 150, 0), 3.0);
				al_set_target_backbuffer(display);
				al_draw_bitmap(bitmap, 0, 0, 0);
				al_flip_display();
			}

			if (input_counter == 4) {
				pair1 = 3;
				pair2 = 0;

				al_set_target_bitmap(bitmap);
				al_draw_line(input_points[pair1].x, input_points[pair1].y, input_points[pair2].x, input_points[pair2].y, al_map_rgb(0, 150, 0), 3.0);
				al_set_target_backbuffer(display);
				al_draw_bitmap(bitmap, 0, 0, 0);
				al_flip_display();
			}
		}
	}

	VECTOR AAPrime = getCornerOffset(input_points[0], A);
	VECTOR BBPrime = getCornerOffset(input_points[1], B);
	VECTOR CCPrime = getCornerOffset(input_points[2], C);
	VECTOR DDPrime = getCornerOffset(input_points[3], D);

	VECTOR AAPrimeS, BBPrimeS, CCPrimeS, DDPrimeS;

	ALLEGRO_MOUSE_STATE state;

	POINT p;
	POINT PPPrime;

	POINT center;
	center.x = 0;
	center.y = 0;

	for (int i = 0; i < 4; i++) {
		center.x += input_points[i].x;
		center.y += input_points[i].y;
	}

	center.x /= 4;
	center.y /= 4;

	float total;

	al_flush_event_queue(queue);

	while (1) {
		al_wait_for_event(queue, &event);

		if (event.type != ALLEGRO_EVENT_MOUSE_AXES) {
			if (event.type == ALLEGRO_EVENT_KEY_DOWN || event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
				break;
			}

			continue;
		}

		al_draw_bitmap(bitmap, 0, 0, 0);

		// get the input point

		al_get_mouse_state(&state);

		p.x = state.x;
		p.y = state.y;

		// calculate the distances
		float ADistance = distance(p, input_points[0]);
		float BDistance = distance(p, input_points[1]);
		float CDistance = distance(p, input_points[2]);
		float DDistance = distance(p, input_points[3]);

		float AB = ADistance / BDistance;
		float BA = BDistance / ADistance;
		total = AB + BA;
		AB /= total;
		BA /= total;

		float topVec = AAPrime.y * BA + BBPrime.y * AB;

		float ACurs = fabs(p.y - input_points[0].y);
		float BCurs = fabs(p.y - input_points[1].y);
		float topCurs = ACurs * BA + BCurs * AB;

		al_draw_line(p.x, p.y, p.x, p.y + topVec, al_map_rgb(0, 255, 0), 4.0);
		al_draw_circle(p.x, p.y + topVec, 5, al_map_rgb(0, 0, 0), 2.0);

		float CD = CDistance / DDistance;
		float DC = DDistance / CDistance;
		total = DC + CD;
		CD /= total;
		DC /= total;

		float bottomVec = CCPrime.y * DC + DDPrime.y * CD;

		float DCurs = fabs(p.y - input_points[3].y);
		float CCurs = fabs(p.y - input_points[2].y);
		float bottomCurs = DCurs * CD + CCurs * DC;

		al_draw_line(p.x, p.y, p.x, p.y + bottomVec, al_map_rgb(0, 255, 0), 4.0);
		al_draw_circle(p.x, p.y + bottomVec, 5, al_map_rgb(0, 0, 0), 2.0);

		float topBottom = topCurs / bottomCurs;
		float bottomTop = bottomCurs / topCurs;
		total = topBottom + bottomTop;
		topBottom /= total;
		bottomTop /= total;

		float BC = BDistance / CDistance;
		float CB = CDistance / BDistance;
		total = BC + CB;
		BC /= total;
		CB /= total;

		float rightVec = BBPrime.x * CB + CCPrime.x * BC;

		BCurs = fabs(p.x - input_points[1].x);
		CCurs = fabs(p.x - input_points[2].x);
		float rightCurs= BCurs * CB + CCurs * BC;

		al_draw_line(p.x, p.y, p.x + rightVec, p.y, al_map_rgb(0, 255, 0), 4.0);
		al_draw_circle(p.x + rightVec, p.y, 5, al_map_rgb(0, 0, 0), 2.0);

		float DA = DDistance / ADistance;
		float AD = ADistance / DDistance;
		total = DA + AD;
		DA /= total;
		AD /= total;

		float leftVec = DDPrime.x * AD + AAPrime.x * DA;

		ACurs = fabs(p.x - input_points[0].x);
		DCurs = fabs(p.x - input_points[3].x);
		float leftCurs = ACurs * DA + DCurs * AD;

		al_draw_line(p.x, p.y, p.x + leftVec, p.y, al_map_rgb(0, 255, 0), 4.0);
		al_draw_circle(p.x + leftVec, p.y, 5, al_map_rgb(0, 0, 0), 2.0);

		float leftRight = leftCurs / rightCurs;
		float rightLeft = rightCurs / leftCurs;
		total = leftRight + rightLeft;
		leftRight /= total;
		rightLeft /= total;

		// draw the main corner vectors
/*		al_draw_line(input_points[0].x, input_points[0].y, input_points[0].x + AAPrime.x, input_points[0].y + AAPrime.y, al_map_rgb(0, 0, 0), 2.0);*/
/*		al_draw_line(input_points[1].x, input_points[1].y, input_points[1].x + BBPrime.x, input_points[1].y + BBPrime.y, al_map_rgb(0, 0, 0), 2.0);*/
/*		al_draw_line(input_points[2].x, input_points[2].y, input_points[2].x + CCPrime.x, input_points[2].y + CCPrime.y, al_map_rgb(0, 0, 0), 2.0);*/
/*		al_draw_line(input_points[3].x, input_points[3].y, input_points[3].x + DDPrime.x, input_points[3].y + DDPrime.y, al_map_rgb(0, 0, 0), 2.0);*/

		// draw the corner vector components
/*		al_draw_line(input_points[0].x, input_points[0].y, input_points[0].x + AAPrime.x, input_points[0].y, al_map_rgb(255, 0, 0), 1.0);*/
/*		al_draw_line(input_points[0].x, input_points[0].y, input_points[0].x, input_points[0].y + AAPrime.y, al_map_rgb(255, 0, 0), 1.0);*/

/*		al_draw_line(input_points[1].x, input_points[1].y, input_points[1].x + BBPrime.x, input_points[1].y, al_map_rgb(255, 0, 0), 1.0);*/
/*		al_draw_line(input_points[1].x, input_points[1].y, input_points[1].x, input_points[1].y + BBPrime.y, al_map_rgb(255, 0, 0), 1.0);*/

/*		al_draw_line(input_points[2].x, input_points[2].y, input_points[2].x + CCPrime.x, input_points[2].y, al_map_rgb(255, 0, 0), 1.0);*/
/*		al_draw_line(input_points[2].x, input_points[2].y, input_points[2].x, input_points[2].y + CCPrime.y, al_map_rgb(255, 0, 0), 1.0);*/

/*		al_draw_line(input_points[3].x, input_points[3].y, input_points[3].x + DDPrime.x, input_points[3].y, al_map_rgb(255, 0, 0), 1.0);*/
/*		al_draw_line(input_points[3].x, input_points[3].y, input_points[3].x, input_points[3].y + DDPrime.y, al_map_rgb(255, 0, 0), 1.0);*/

		// draw the processed point

		PPPrime.y = topVec * bottomTop + bottomVec * topBottom;
		PPPrime.x = rightVec * leftRight + leftVec * rightLeft;

		al_draw_line(p.x, p.y, p.x + PPPrime.x, p.y, al_map_rgb(0, 0, 255), 2.0);
		al_draw_line(p.x, p.y, p.x, p.y + PPPrime.y, al_map_rgb(0, 0, 255), 2.0);

		al_draw_circle(p.x + xOfs + PPPrime.x, p.y + yOfs + PPPrime.y, 5, al_map_rgb(0, 255, 0), 2.0);
		al_draw_circle(p.x + xOfs + PPPrime.x, p.y + yOfs + PPPrime.y, 1, al_map_rgb(0, 0, 0), 1.0);

		al_flip_display();
	}

	return 0;
}

/*
	-find prime vector for each point
		-prime vector = point' - closest_point
	-for each pair of left, top, right, bottom
		-average pair based on ratios of distances between input point and verts
			xy = x / y
			yx = y / x
			total = xy + yx
			x /= total
			y /= total
		-result = x * yx + y * xy
	-for each pair of top-bottom, left-right
		-average pair based on ratios of distances between input point and perpendicular liness
	-if A does not correspond to A', rotate accordingly
*/
