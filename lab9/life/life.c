#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <stdbool.h>
#include "grid.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <math.h>


typedef struct {
	int start;
	int end;

	char **foreground;
	char **background;
}pthread_arg;

void handler(int signum) {
	; //wake up
}

void *thread_fun(void *args) {
	pthread_arg *arg = (pthread_arg *)args;

	while (true) {
		pause();

		for (int i = arg->start; i < arg->end; i++) {
			int r = i / 30;
			int c = i % 30;

			(*arg->background)[i] = is_alive(r, c, *arg->foreground);
		}
	}
}


int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("too few arguments\n");
		return 1;
	}
	if (atoi(argv[1]) == 0) {
		printf("cannot convert %s to integer\n", argv[1]);
		return 1;
	}
	int threads_no = atoi(argv[1]);
	if (threads_no > 64) {
		printf("64 is maximum number of threads\n");
		return 1;
	}

	signal(SIGUSR1, handler);


	srand(time(NULL));
	setlocale(LC_CTYPE, "");
	initscr(); // Start curses mode

	char *foreground = create_grid();
	char *background = create_grid();
	char *tmp;

	pthread_t threads[threads_no];
	pthread_arg thread_args[threads_no];

	int chunk = (int)ceil(30 * 30 / threads_no);

	for (int i = 0; i < threads_no; i++) {
		thread_args[i].start = i * chunk;
		thread_args[i].end = (i + 1) * chunk;
		thread_args[i].background = &background;
		thread_args[i].foreground = &foreground;

		pthread_create(&threads[i], NULL, thread_fun, &thread_args[i]);
	}

	init_grid(foreground);

	while (true)
	{
		draw_grid(foreground);

		for (int i = 0; i < threads_no; i++) {
			pthread_kill(threads[i], SIGUSR1);
		}

		usleep(500 * 1000);

		// Step simulation
		update_grid(foreground, background);
		tmp = foreground;
		foreground = background;
		background = tmp;
	}

	endwin(); // End curses mode
	destroy_grid(foreground);
	destroy_grid(background);

	return 0;
}
