
#include "bitmap.h"
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>

#define MAX_THREAD 1000

typedef struct thread_args {
	struct bitmap *bm;
	double xmin;
	double xmax; 
	double ymin; 
	double ymax; 
	double max;
	int start_pixel;
	int end_pixel;
	int increment;
} thread_args_struct;

int iteration_to_color( int i, int max );
int iterations_at_point( double x, double y, int max );
void * compute_image(void *args);

void show_help()
{
	printf("Use: mandel [options]\n");
	printf("Where options are:\n");
	printf("-m <max>    The maximum number of iterations per point. (default=1000)\n");
	printf("-x <coord>  X coordinate of image center point. (default=0)\n");
	printf("-y <coord>  Y coordinate of image center point. (default=0)\n");
	printf("-s <scale>  Scale of the image in Mandlebrot coordinates. (default=4)\n");
	printf("-W <pixels> Width of the image in pixels. (default=500)\n");
	printf("-H <pixels> Height of the image in pixels. (default=500)\n");
	printf("-o <file>   Set output file. (default=mandel.bmp)\n");
	printf("-n <num>	The number of threads to process the images\n");
	printf("-h          Show this help text.\n");
	printf("\nSome examples are:\n");
	printf("mandel -x -0.5 -y -0.5 -s 0.2\n");
	printf("mandel -x -.38 -y -.665 -s .05 -m 100\n");
	printf("mandel -x 0.286932 -y 0.014287 -s .0005 -m 1000\n\n");
}

int main( int argc, char *argv[] )
{
	char c;
	pthread_t *threads;
	thread_args_struct *t;

	// These are the default configuration values used
	// if no command line arguments are given.
	const char *outfile = "gbushong_mandel.bmp";
	double xcenter = 0;
	double ycenter = 0;
	double scale = 4;
	int    image_width = 500;
	int    image_height = 500;
	int    max = 1000;
	int    num_thread = 1;

	// For each command line argument given,
	// override the appropriate configuration value.
	while((c = getopt(argc,argv,"x:y:s:W:H:m:o:n:h"))!=-1) {
		switch(c) {
			case 'x':
				xcenter = atof(optarg);
				break;
			case 'y':
				ycenter = atof(optarg);
				break;
			case 's':
				scale = atof(optarg);
				break;
			case 'W':
				image_width = atoi(optarg);
				break;
			case 'H':
				image_height = atoi(optarg);
				break;
			case 'm':
				max = atoi(optarg);
				break;
			case 'o':
				outfile = optarg;
				break;
			case 'n':
				num_thread = atoi(optarg);
				break;
			case 'h':
				show_help();
				exit(1);
				break;
		}
	}

	if ((num_thread < 1) || (num_thread > MAX_THREAD)) {
		printf("The number of threads should be between 1 and %d.\n", MAX_THREAD);
		exit(1);
	}

	threads = (pthread_t *)malloc(num_thread*sizeof(*threads));

	t = (thread_args_struct *)malloc(sizeof(thread_args_struct) *num_thread);

	// Display the configuration of the image.
	printf("mandel: x=%lf y=%lf scale=%lf max=%d outfile=%s\n",xcenter,ycenter,scale,max,outfile);


	// Create a bitmap of the appropriate size.
	struct bitmap *bm = bitmap_create(image_width,image_height);

	// Fill it with a dark blue, for debugging
	bitmap_reset(bm,MAKE_RGBA(0,0,255,0));

	// Compute the Mandelbrot image -- change this - create N threads with pthread_create
	for (int i = 0; i < num_thread; i++) {
		// calculate start and end pixels
		int increment = image_height / num_thread;
		int this_start = i * increment;
		int this_end = this_start + increment;

		// make sure all the pixels get taken care of
		if (i == num_thread - 1) {
			this_end = image_height;
		}
		
		// create a Thread object to pass args to compute_image for this thread
		t[i].bm = bm;
		t[i].xmin = xcenter-scale;
		t[i].xmax = xcenter+scale;
		t[i].ymin = ycenter-scale;
		t[i].ymax = ycenter+scale;
		t[i].max = max;

		t[i].start_pixel = this_start;
		t[i].end_pixel = this_end;
		t[i].increment = increment;

		// Start up thread
		pthread_create(&threads[i], NULL, compute_image, (void *)(t + i));		
	}

	// Complete each thread
	for (int i = 0; i < num_thread; i++) {
		pthread_join(threads[i], NULL);
	}

	// Save the image in the stated file.
	if(!bitmap_save(bm,outfile)) {
		fprintf(stderr,"mandel: couldn't write to %s: %s\n",outfile,strerror(errno));
		return 1;
	}

	free(t);
	free(threads);

	return 0;
}

/*
Compute an entire Mandelbrot image, writing each point to the given bitmap.
Scale the image to the range (xmin-xmax,ymin-ymax), limiting iterations to "max"
*/
void * compute_image(void *args)
{
	int i,j;
	thread_args_struct *Thread = (thread_args_struct *)args;

	int width = bitmap_width(Thread->bm);
	int height = bitmap_height(Thread->bm);

	// For every pixel in the image...
	for(j=Thread->start_pixel;j<Thread->end_pixel;j++) {

		for(i=0;i<width;i++) {

			// Determine the point in x,y space for that pixel.
			double x = Thread->xmin + i*(Thread->xmax-Thread->xmin)/width;
			double y = Thread->ymin + j*(Thread->ymax-Thread->ymin)/height;

			// Compute the iterations at that point.
			int iters = iterations_at_point(x,y,Thread->max);

			// Set the pixel in the bitmap.
			bitmap_set(Thread->bm,i,j,iters);
		}
	}
	pthread_exit(NULL);
}

/*
Return the number of iterations at point x, y
in the Mandelbrot space, up to a maximum of max.
*/

int iterations_at_point( double x, double y, int max )
{
	double x0 = x;
	double y0 = y;

	int iter = 0;

	while( (x*x + y*y <= 4) && iter < max ) {

		double xt = x*x - y*y + x0;
		double yt = 2*x*y + y0;

		x = xt;
		y = yt;

		iter++;
	}

	return iteration_to_color(iter,max);
}

/*
Convert a iteration number to an RGBA color.
Here, we just scale to gray with a maximum of imax.
Modify this function to make more interesting colors.
*/

int iteration_to_color( int i, int max )
{
	int gray = 255*i/max;
	return MAKE_RGBA(gray,gray,gray,0);
}




