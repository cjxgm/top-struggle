// vim: noet sts=0 sw=4 ts=4
//
// top-struggle
//
// Licensed under the MIT License.
// Copyright (C) 2013 eXerigumo Clanjor (哆啦比猫/兰威举)
//
// CONTRIBUTORS:
// 		eXerigumo Clanjor (哆啦比猫/兰威举) <cjxgm@126.com>
//


#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <err.h>
#include <xdo.h>

typedef struct
{
	unsigned int l:1;	// is left     mouse button down?
	unsigned int r:1;	// is    right mouse button down?
	unsigned int m:1;	// is  middle  mouse button down?
	unsigned int  :5;	// I don't know what those value mean.
	char x;
	char y;	// note that this value is actually "-y", I mean, flipped.
}
__attribute__((__packed__))
Event;

int main(int argc, char * argv[])
{
	const char * device_file = "/dev/input/mice";
	int threshold = 400;
	int size = 14;

	// open mouse device and set unbuffered mode
	FILE * fp = fopen(device_file, "r");
	if (!fp) err(1, "unable to open device '%s'", device_file);
	if (setvbuf(fp, NULL, _IONBF, 0)) err(1, "unable to set unbuffered mode on '%s'", device_file);

	// init xdo
	xdo_t * xdo = xdo_new(NULL);

	// create and register callback for SIGINT
	void sigint_cb()
	{
		fclose(fp);
		xdo_free(xdo);

		exit(0);
	}
	signal(SIGINT, (void *)sigint_cb);

	// the magic part start!
	Event e;
	int struggled = 0;
	int tx, ty;	// temporary absolute position of cursor
	int screen;	// screen number (used in xdo_mouselocation and xdo_mousemove);
	while (1) {
		fread(&e, sizeof(e), 1, fp);
		e.y = -e.y;	// remember? it's flipped!
		xdo_get_mouse_location(xdo, &tx, &ty, &screen);

		if (struggled < threshold) {
			if (ty <= size) {
				struggled -= e.y;
				if (struggled > 0) xdo_move_mouse(xdo, tx, size, screen);
			}
			else struggled = 0;
		}
		else if (ty > size) struggled = 0;
	}

	return 0;
}

