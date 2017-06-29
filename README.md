# project3
cse30341 project 3: processes and threads (02/24/2017)

Project Description:
In order to study parallelism, we must have a problem that will take a significant amount of computation. For fun, we will generate images in the Mandelbrot set, which is a well known fractal structure. The set is interesting both mathematically and aesthetically because it has an infinitely recursive structure. You can zoom into any part and find swirls, spirals, snowflakes, and other fun structures, as long as you are willing to do enough computation. 
Now, what does this all have to do with operating systems? It's very simple: it can take a long time to compute a Mandelbrot image. The larger the image, the closer it is to the origin, and the higher the max value, the longer it will take. Suppose that you want to create a movie of high resolution Mandelbrot images, and it is going to take a long time. Your job is to speed up the process by using multiple CPUs. You will do this in two different ways: using multiple processes and using multiple threads.
