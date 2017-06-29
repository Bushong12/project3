#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char *argv[]) {

	// Error catching for wrong arguments
	if (argc != 2) {
		printf("mandelmovie: Incorrect arguments!\n");
		printf("usage: %s <num_processes>\n", argv[0]);
		exit(1);
	}

	// assign variables
	int num_proc = atoi(argv[1]);
	int iterations = 0;
	float s = 2;
	while (iterations < 50) {

		for (int i = 0; i < num_proc; i++) {
			if (iterations < 50) {
				iterations++;
				s = s / 1.2;
				int rc = fork();
				if (rc < 0) {						// fork failed; exit
					fprintf(stderr, "mandelmovie: fork failed\n");
					exit(1);
				}
				else if(rc == 0) {					// child (new process)
					char command[256];
					char *myargs[100];
					sprintf(command, "./mandel -x .286932 -y .014287 -m 1000 -s %lf -o gbushong_mandel%d.bmp", s, iterations);
					char *token;
					int nwords = 0;
					token = strtok(command, " ");
					while (token != NULL) {
						myargs[nwords] = token;
						token = strtok(NULL, " ");
						nwords++;
					}
					myargs[nwords] = NULL;
					execvp(myargs[0], myargs);
				}
			}
		}

		for (int j = 0; j < num_proc; j++) {
			wait(NULL);
		}
		
	}
	return 0;
}
