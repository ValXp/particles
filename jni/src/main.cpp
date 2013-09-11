#include <unistd.h>
#include <iostream>
#include <stdlib.h>
#include "Egl.h"
#include "ParticleEngine.h"
#include "utils.h"
#include "GlApp.hpp"


int process_av(int ac, char **av, int &part_number, int &cpu_number)
{
    if (ac == 3)
    {
	cpu_number = atoi(av[1]);
	part_number = atoi(av[2]);
	printf("CPU : %d\nParticles : %d\n", cpu_number, part_number);
    }
    if (ac != 3 || cpu_number < 1 || part_number < 1)
    {
	fprintf(stderr, "Usage : \n%s cpu_number particles_number\ncpu_number : integer > 0\nparticlues_number : integer > 0\n", av[0]);
	return -1;
    }
    return 0;
}

#define WIDTH 1920
#define HEIGHT 1080

int main(int ac, char **av)
{
    int cpu_number, part_number;

    if (process_av(ac, av, part_number, cpu_number) == -1)
    {
	return EXIT_FAILURE;
    }
    eglInit(WIDTH, HEIGHT);


    ParticleEngine *engine = new ParticleEngine(part_number, cpu_number);
    GlApp *app = new GlApp(3, true, WIDTH, HEIGHT);
    app->setEngine(engine);
    engine->start_emit();
    engine->unpause();
    app->setZoom(-25);
    while (42)
    {

	app->draw();
	eglSwap();
	app->countFPS();
    }
    return 0;
}




