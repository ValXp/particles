#ifndef PARTICLEENGINE_H_INCLUDED
#   define PARTICLEENGINE_H_INCLUDED

# include <cstring> // for size_t
# include <pthread.h>
# include "utils.h"

#   define RAND .3
#   define LIMIT_X 9.0
#   define LIMIT_Y 9.0
#   define LIMIT_Z 9.0
#   define GRAVITY .002
#   define SPEED_X .05
#   define SPEED_Y .5
#   define SPEED_Z .05
#   define CONE_TOP_Y   -4.0
#   define CONE_BOT_Y   -5.0
#   define CONE_BASE_RADIUS 2
#   define CONE_RAD_SQR     (CONE_BASE_RADIUS * CONE_BASE_RADIUS)
#   define CONE_DELTA   (CONE_TOP_Y - CONE_BOT_Y)
#   define BOUNCYNESS   .2f
#   define GROUND_Y -4
#   define GROUND_X 1.50
#   define GROUND_Z 1.50
#   define WAVE_WIDTH 0.2f
#   define RANDOM_SPEED .3f

#   define COLOR_R(idx) (*(m_colors + (idx * 3) + 0))
#   define COLOR_G(idx) (*(m_colors + (idx * 3) + 1))
#   define COLOR_B(idx) (*(m_colors + (idx * 3) + 2))

#   define POS_X(idx) (*(m_positions + (idx * 3) + 0))
#   define POS_Y(idx) (*(m_positions + (idx * 3) + 1))
#   define POS_Z(idx) (*(m_positions + (idx * 3) + 2))

#   define PSPEED_X(idx) (*(m_speed + (idx * 3) + 0))
#   define PSPEED_Y(idx) (*(m_speed + (idx * 3) + 1))
#   define PSPEED_Z(idx) (*(m_speed + (idx * 3) + 2))

#   define MAX_FRAME_TIME_MS 16 // corresponds to 60 FPS

#   define MEM_ALLOC_ERROR_MSG  "Too many particles : try less, or restart your device"
#   define MEM_ALLOC_THREAD_ERROR_MSG   "Fatal error. Cannot allocate thread memory"

typedef unsigned int uint;
typedef unsigned long ulong;



class ParticleEngine
{
    public:
    // Creates the engine and storage for the particles, you can specify the number of thread to run. If 0, will run as much threads as your CPU has cores.
    ParticleEngine(uint particleNumber, uint threadNb = 0);
    ~ParticleEngine();
   
    // Start/Stop particle emission, keep running the engine, but stop generating new particles
    inline void start_emit()
    {
        m_emit = true;
    }
    inline void stop_emit()
    {
        m_emit = false;
    }

	inline bool emitting()
	{
		return m_emit;
	}

    inline void pause()
    {
        if (m_pause)
            return;
        m_pause = true;
        for (uint cur = 0; cur < m_threadNb; ++cur)
        {
            pthread_mutex_lock(&(m_args[cur].mutex));
        }
    }
    inline void unpause()
    {
        if (!m_pause)
            return;
        m_pause = false;
        for (uint cur = 0; cur < m_threadNb; ++cur)
        {
            pthread_mutex_unlock(&(m_args[cur].mutex));
        }
    }
    inline bool paused()
    {
        return m_pause;
    }

    inline void setRandomSpeed()
    {
        m_randomCursor = LIMIT_Y / 2;
        m_randomSpeed = -RANDOM_SPEED;
    }

    inline void setRandomSpeed(uint part)
    {
        PSPEED_X(part) = Utils::myRand(-RAND, RAND) / 5;
        PSPEED_Y(part) = Utils::myRand(-RAND, RAND) / 5;
        PSPEED_Z(part) = Utils::myRand(-RAND, RAND) / 5;
    }

    //void    step(); // useless because of the threads

    inline float *positions()
    {
        return m_positions;
    }
    /*
    inline float *colors()
    {
        return m_colors;
    }*/

    inline uint particleNumber()
    {
        return m_part_nbr;
    }

    inline void setParticleNumber(uint pnumber)
    {
        if (pnumber > m_max_part_nbr)
            pnumber = m_max_part_nbr;
            
       /* 
        if (pnumber > m_part_nbr)
        {
            for (int i = m_part_nbr; i < pnumber; ++i)
                initParticle(i);
        }
        */
        m_part_nbr = pnumber;
    }

    inline uint maxParticleNumber()
    {
        return m_max_part_nbr;
    }

    inline void changeSpeed(float x, float y, float z)
    {
        m_user.x = x;
        m_user.y = y;
        m_user.z = z;
    }
    inline ulong getFrameNb()
    {
        return m_frameNb;
    }
    inline unsigned long getTimeSlept()
    {
        return m_timeSlept;
    }

    // Vector 3D
    struct  t_coord
    {
        float       x;
        float       y;
        float       z;
    };

    // thread infos
    struct ThreadArg
    {
        uint            id;
        bool            die;
        ulong           frame_nb;
        pthread_mutex_t mutex;
        bool            paused;
        ParticleEngine  *engine;
    };

    class EngineException
    {
        public:
        EngineException() throw()
        {
            memset(static_cast<void*>(msg), 0, 512);
        }
        EngineException(const char *what) throw()
        {
            strncpy(msg, what, 511);
        }
        const char *    what() const throw()
        {
            return msg;
        }
        private:
        char  msg[512];
    };

    void    startWave(float intensity, float speed);
    void    _step(ThreadArg &arg);
    bool	hasFailed()
    {
    	return m_hasFailed;
    }
    private:
    void    moveWave();
    bool    initParticles();
    void    initParticle(uint idx);
    void 	allocFailed(int size);
    
    // Particles storage, aligned arrays easier to upload to opengl 
    // don't need colors anymore
    //float           *m_colors;
    float           *m_positions;
    float           *m_speed;
    int             *m_active;

    t_coord         m_pos;
    t_coord         m_limit;
    float           m_gravity;

    uint            m_part_nbr;
    uint            m_max_part_nbr;
    pthread_t       *m_threads;
    ThreadArg       *m_args;
    uint            m_threadNb;
    t_coord         m_user;
    long            m_frameNb;
    bool            m_emit;
    ulong           m_timeSlept;
    bool            m_pause;
    float           m_waveDist;
    float           m_waveEnd;
    clock_t         m_lastMoveMs;
    float           m_waveSpeed;
    bool            m_hasWave;
    float           m_waveIntensity;
    bool			m_hasFailed;
    float           m_randomCursor;
    float           m_randomSpeed;
};



#endif // PARTICLEENGINE_H_INCLUDED
