#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#ifndef PC_VERSION
#include <cpu-features.h>
#endif
#include "ParticleEngine.h"
#include "utils.h"


void    ParticleEngine::initParticle(uint idx)
{
    POS_X(idx) = m_pos.x;
    POS_Y(idx) = m_pos.y;
    POS_Z(idx) = m_pos.z;



    PSPEED_Y(idx) = (SPEED_Y * Utils::myRand(RAND / 1.2, RAND)) + m_user.y;
    // sampling a point in a disk to have a round projection of particles
    float x = Utils::myRand(-RAND, RAND) , y = Utils::myRand(-RAND, RAND);
    //Utils::genRandomDiskPoint(RAND, x, y);
    PSPEED_X(idx) = (SPEED_X * x ) + m_user.x;
    PSPEED_Z(idx) = (SPEED_Z * y ) + m_user.z;

    /*
    COLOR_R(idx) = (float)Utils::myRand(0, 255) / 255.0f;
    COLOR_G(idx) = (float)Utils::myRand(0, 255) / 255.0f;
    COLOR_B(idx) = (float)Utils::myRand(0, 255) / 255.0f;
    */

    m_active[idx] = (int)Utils::myRand(-50, 0);
}

void    ParticleEngine::initParticles()
{
    srandom(time(0));
	unsigned int size = (m_max_part_nbr * sizeof(int)) + 		// m_active
						/*(m_max_part_nbr * sizeof(float) * 3) + */	// m_colors
						(m_max_part_nbr * sizeof(float) * 3) +	// m_positions
						(m_max_part_nbr * sizeof(float) * 3);	// m_speed
	LOGI("Allocating particles memory : %d kbytes\n", size / 1000);
    m_active = 0;
    //m_colors  = 0;
    m_positions = m_speed = 0;
    m_active = new int[m_max_part_nbr];
    //m_colors = new float[m_max_part_nbr * 3];
    m_positions = new float[m_max_part_nbr * 3];
    m_speed = new float[m_max_part_nbr * 3];
    if (m_active == 0 /*|| m_colors == 0*/ ||
        m_positions == 0 || m_speed == 0)
    {
        if (m_active)
            delete[] m_active;
        /*if (m_colors)
            delete[] m_colors;*/
        if (m_positions)
            delete[] m_positions;
        if (m_speed)
            delete[] m_speed;
        LOGE("Cannot allocate %d particles. Not enough memory (%d kbytes)\n", m_max_part_nbr, size/1000);
        return ; // exceptions disabled
        //throw EngineException(MEM_ALLOC_ERROR_MSG);
    }
    for (uint cur = 0; cur < m_max_part_nbr; ++cur)
        initParticle(cur);
}

void    *threadFunc(void *arg)
{
    ParticleEngine::ThreadArg *targ = static_cast<ParticleEngine::ThreadArg*>(arg);
    ParticleEngine  *engine = targ->engine;

    pthread_mutex_lock(&targ->mutex);
    pthread_mutex_unlock(&targ->mutex);
    LOGI("Thread %d birth\n", targ->id);
    while (!targ->die)
    {
        engine->_step(*targ);
        if (engine->paused()) // what is faster ? an if or a mutex lock and unlock ?
        {
            pthread_mutex_lock(&targ->mutex);
            pthread_mutex_unlock(&targ->mutex);
        }
    }
    LOGI("Thread %d death\n", targ->id);
    pthread_exit(0);
    return 0;
}

void    ParticleEngine::startWave(float intensity, float speed)
{
    if (m_hasWave)
    {
        if (m_waveIntensity < intensity)
            m_waveIntensity = intensity;
        return;
    }
    m_hasWave = true;
    m_waveEnd = CONE_RAD_SQR;
    m_waveDist = 0.0f;
    m_lastMoveMs = TIME_MS();
    m_waveSpeed = speed;
    m_waveIntensity = intensity;
    //LOGI("Start wave : %f\n", m_waveDist);
}

void    ParticleEngine::moveWave()
{
    if (!m_hasWave)
        return;
    clock_t current = TIME_MS();
    m_waveDist += m_waveSpeed * (float)(current - m_lastMoveMs) / 100.0f;
    m_lastMoveMs = current;
    if (m_waveDist > m_waveEnd)
        m_hasWave = false;
}

void    ParticleEngine::_step(ThreadArg &arg)
{
    clock_t last = TIME_MS();
    uint fid = (uint)arg.id;
    uint particlePerThread = m_part_nbr / m_threadNb;
    uint    start = fid * particlePerThread;
    uint    end = (fid + 1) * particlePerThread;
    // Begin loop
    for (uint i = start; i < end; ++i)
    {
        if (m_active[i] == 1)
        {
            POS_X(i) += PSPEED_X(i);
            POS_Y(i) += PSPEED_Y(i);
            POS_Z(i) += PSPEED_Z(i);
            PSPEED_Y(i) -= m_gravity;

            if (POS_Y(i) > LIMIT_Y || POS_Y(i) < -LIMIT_Y ||
                POS_X(i) > LIMIT_X || POS_X(i) < -LIMIT_X ||
                POS_Z(i) > LIMIT_Z || POS_Z(i) < -LIMIT_Z)
                initParticle(i);
           
            // consider the center of the cone at 0,CONE_TOP_Y,0
            // it was supposed to be a cone....
            
            if (POS_Y(i) <= CONE_TOP_Y && POS_Y(i) >= CONE_BOT_Y && PSPEED_Y(i) <= 0)
            {
               // compute distance to the center
               float dist = ( POS_X(i) * POS_X(i)) + (POS_Z(i) * POS_Z(i));
               float maxDist = ((CONE_TOP_Y - POS_Y(i)) / CONE_DELTA) * CONE_RAD_SQR;
               if (dist <= maxDist)
               {
                    if (m_hasWave && 
                        dist <= m_waveDist + WAVE_WIDTH &&
                        dist >= m_waveDist - WAVE_WIDTH)
                        PSPEED_Y(i) = m_waveIntensity;
                    else
                        PSPEED_Y(i) = -PSPEED_Y(i) * BOUNCYNESS;
               }
            }

        }
        else if (m_emit)
        {
            ++(m_active[i]);
        }
    }
    if (arg.id == 0)
    {
        ++m_frameNb;
        if (m_hasWave)
            moveWave();
    }
    ++(arg.frame_nb);
    clock_t current = TIME_MS();
    clock_t delta = current - last;
    

    if (delta < MAX_FRAME_TIME_MS && delta >= 0) // will slow if the frame is too fast
    {
        //LOGI("CPU %d ms too fast\n", MAX_FRAME_TIME_MS - delta);
        //Utils::atomicAdd(&m_timeSlept, (MAX_FRAME_TIME_MS - delta));
        int frame = MAX_FRAME_TIME_MS - delta;
        ATOMIC_ADD(&m_timeSlept, frame);
        usleep(1000 * frame);
    }
    
}

ParticleEngine::ParticleEngine(uint max_part_nbr, uint threadNb)
{
    LOGI("Engine initialization : %d particles\n", max_part_nbr);

#ifndef PC_VERSION
    LOGI("Cpu count : %d\n", android_getCpuCount());
#endif
  
    m_threadNb = threadNb;
#ifndef PC_VERSION
    if (threadNb == 0)
        m_threadNb = android_getCpuCount();
#endif
  

    if (m_threadNb < 1)
        m_threadNb = 1;
    m_threads = 0;
    m_args = 0;
    m_threads = new pthread_t[m_threadNb];
    m_args = new ThreadArg[m_threadNb];
    if (m_threads == 0 || m_args == 0)
    {
        LOGE("Cannot allocate %d threads.\n", m_threadNb);
        // exceptions disabled
        return;
        //throw EngineException(MEM_ALLOC_THREAD_ERROR_MSG);
    }
    m_part_nbr = max_part_nbr;
    m_max_part_nbr = max_part_nbr;
    m_gravity = GRAVITY;
    m_pos.x = 0;
    m_pos.y = GROUND_Y;
    m_pos.z = 0;
    m_user.x = 0;
    m_user.y = 0;
    m_user.z = 0;
    m_frameNb = 0;
    m_emit = true;
    m_timeSlept = 0;
    m_pause = true;

    m_hasWave = false;

    initParticles();

    for (int i = 0; i < m_threadNb; ++i)
    {
        m_args[i].die = false;
        m_args[i].id = i;
        pthread_mutex_init(&m_args[i].mutex, 0);
        pthread_mutex_lock(&m_args[i].mutex);
        m_args[i].paused = true;
        m_args[i].engine = this;
        pthread_create(m_threads + i, 0, threadFunc, m_args + i);
    }
    LOGI("Engine Loaded\n");
}


ParticleEngine::~ParticleEngine()
{
    m_pause = false;
    for (uint i = 0; i < m_threadNb; ++i)
    {
        m_args[i].die = true;
        pthread_mutex_unlock(&(m_args[i].mutex));
        pthread_join(m_threads[i], 0);
        pthread_mutex_destroy(&(m_args[i].mutex));
    }
    LOGI("Frames : %ld Time slept : %ldms\n", getFrameNb(), getTimeSlept());
    // at this point every thread should be dead

    delete[] m_threads;
    delete[] m_args;
    //delete[] m_colors;
    delete[] m_positions;
    delete[] m_speed;
    delete[] m_active;
    LOGI("Engine Killed. \n");
}






