#include <unistd.h>
#include <iostream>
#include "utils.h"
#include "ParticleEngine.h"

int main(void)
{
    ParticleEngine *engine = new ParticleEngine(5000000, 8);
    engine->unpause();
    engine->start_emit();
    sleep(10);
    delete engine;
    return 0;
}

