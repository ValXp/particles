#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <new>
#ifdef ANDROID
#include <cpu-features.h>
#endif
#include "ParticleEngine.h"
#include "utils.h"

void ParticleEngine::initParticlePlatform(uint idx) {
	POS_X(idx) = m_pos.x;
	POS_Y(idx) = m_pos.y;
	POS_Z(idx) = m_pos.z;

	PSPEED_Y(idx) = (SPEED_Y * Utils::myRand(RAND / 1.2, RAND)) + m_user.y;
	// sampling a point in a disk to have a round projection of particles
	float x, y;
	//x = Utils::myRand(-RAND, RAND);
	//y = Utils::myRand(-RAND, RAND);

	Utils::genRandomDiskPoint(RAND, x, y);
	PSPEED_X(idx) = (SPEED_X * x) + m_user.x;
	PSPEED_Z(idx) = (SPEED_Z * y) + m_user.z;

	/*
	 COLOR_R(idx) = (float)Utils::myRand(0, 255) / 255.0f;
	 COLOR_G(idx) = (float)Utils::myRand(0, 255) / 255.0f;
	 COLOR_B(idx) = (float)Utils::myRand(0, 255) / 255.0f;
	 */

	m_active[idx] = (int) Utils::myRand(-50, 0);
}

void ParticleEngine::initParticleRandom(uint idx) {
	POS_X(idx) = Utils::myRand(-LIMIT_X, LIMIT_X) / 1;
	POS_Y(idx) = Utils::myRand(-LIMIT_X, LIMIT_X) / 1;
	POS_Z(idx) = Utils::myRand(-LIMIT_Z, LIMIT_Z) / 1;

	PSPEED_X(idx) = 0;//Utils::myRand(-SPEED_X, SPEED_X) / 10;
	PSPEED_Y(idx) = 0;//Utils::myRand(-SPEED_Y, SPEED_Y) / 10;
	PSPEED_Z(idx) = 0;//Utils::myRand(-SPEED_Z, SPEED_Z) / 10;
}

// Returns true in case of allocation failure
bool ParticleEngine::initParticles() {
	srandom(time(0));
	unsigned int size = (m_max_part_nbr * sizeof(int)) + // m_active
			(m_max_part_nbr * sizeof(float) * 3) + // m_positions
			(m_max_part_nbr * sizeof(float) * 3) + // m_speed
			(EIGHT_SIZE * sizeof(float) * 2); // m_eightPos
	LOGI("Allocating particles memory : %d kbytes\n", size / 1000);
	m_active = 0;
	m_positions = m_speed = m_eightPos = 0;

	m_active = new (std::nothrow) int[m_max_part_nbr];
	if (!m_active) {
		allocFailed(size);
		return true;
	}
	m_positions = new (std::nothrow) float[m_max_part_nbr * 3];
	if (!m_positions) {
		allocFailed(size);
		return true;
	}
	m_speed = new (std::nothrow) float[m_max_part_nbr * 3];
	if (!m_speed) {
		allocFailed(size);
		return true;
	}
	m_eightPos = new (std::nothrow) float[EIGHT_SIZE * 2];
	if (!m_eightPos) {
		allocFailed(size);
		return true;
	}

	for (uint i = 0; i < m_max_part_nbr; ++i) {
		initParticle(i);
	}
	float R = 3;
	float slices = EIGHT_SIZE / 2.0f;

	for (int i = 0; i < EIGHT_SIZE; ++i) {
		float theta = fmod(i, slices);
		float phi = (((i - phi) / slices) / 1) * (M_PI);
		theta = (theta / EIGHT_SIZE) * (2 * M_PI);
		float u = R * cos(theta);
		m_eightPos[i * 2] = sqrt(R * R - u * u) * cos(phi);
		m_eightPos[i * 2 + 1] = u;
	}
	return false;
}

void ParticleEngine::allocFailed(int size) {
	if (m_active == 0 || m_positions == 0 || m_speed == 0) {
		LOGE("Cannot allocate %ld particles. Not enough memory (%d kbytes)\n",
				m_max_part_nbr, size / 1000);
		if (m_active)
			delete[] m_active;
		m_active = 0;
		if (m_positions)
			delete[] m_positions;
		m_positions = 0;
		if (m_speed)
			delete[] m_speed;
		m_speed = 0;
		if (m_eightPos)
			delete[] m_eightPos;
		m_eightPos = 0;
		return; // exceptions disabled
		//throw EngineException(MEM_ALLOC_ERROR_MSG);
	}
}

void *threadFunc(void *arg) {
	ParticleEngine::ThreadArg *targ =
			static_cast<ParticleEngine::ThreadArg*>(arg);
	ParticleEngine *engine = targ->engine;

	pthread_mutex_lock(&targ->mutex);
	pthread_mutex_unlock(&targ->mutex);
	LOGI("Thread %d birth\n", targ->id);
	while (!targ->die) {
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

void ParticleEngine::startWave(float intensity, float speed) {
	if (m_hasWave) {
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

void ParticleEngine::moveWave() {
	if (!m_hasWave)
		return;
	clock_t current = TIME_MS();
	m_waveDist += m_waveSpeed * (float) (current - m_lastMoveMs) / 100.0f;
	m_lastMoveMs = current;
	if (m_waveDist > m_waveEnd)
		m_hasWave = false;
}

void ParticleEngine::_step(ThreadArg &arg) {
	clock_t last = TIME_MS();
	uint fid = (uint) arg.id;
	uint particlePerThread = m_part_nbr / m_threadNb;
	uint start = fid * particlePerThread;
	uint end = (fid + 1) * particlePerThread;

	(*this.*mParticleStep)(start, end);

	if (arg.id == 0) {
		++m_frameNb;
		if (m_hasWave)
			moveWave();
		if (m_randomCursor > -LIMIT_Y + m_randomSpeed)
			m_randomCursor += m_randomSpeed;
//        if (m_part_nbr < m_max_part_nbr - 100 && m_isAuto)
//        	m_part_nbr += 100;
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

ParticleEngine::ParticleEngine(uint max_part_nbr, bool isAuto, uint threadNb) :
		m_hasFailed(false), m_isAuto(isAuto) {
	LOGI("Engine initialization : %d particles\n", max_part_nbr);

#ifdef ANDROID
	LOGI("Cpu count : %d\n", android_getCpuCount());
#endif
	m_threadNb = threadNb;
#ifdef ANDROID
	if (threadNb == 0)
	m_threadNb = android_getCpuCount() - 1;
#endif

	if (m_threadNb < 1)
		m_threadNb = 1;

	if (max_part_nbr < m_threadNb) {
		max_part_nbr = m_threadNb;
	}
	max_part_nbr -= max_part_nbr % m_threadNb;

	m_threads = new (std::nothrow) pthread_t[m_threadNb];
	m_args = new (std::nothrow) ThreadArg[m_threadNb];
	if (m_threads == 0 || m_args == 0) {
		LOGE("Cannot allocate %d threads.\n", m_threadNb);
		// exceptions disabled
		m_hasFailed = true;
		m_threadNb = 0;
		return;
		//throw EngineException(MEM_ALLOC_THREAD_ERROR_MSG);
	}
	m_part_nbr = m_isAuto ? 1000 : max_part_nbr;
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
	m_randomCursor = -LIMIT_Y;
	m_randomSpeed = 0;

	m_hasWave = false;

	initFunctions();

	if (m_hasFailed = initParticles()) {
		m_threadNb = 0;
		return;
	}

	for (unsigned int i = 0; i < m_threadNb; ++i) {
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

void ParticleEngine::initFunctions() {
	mFunctionsArraySize = 4;
	mFunctionsArray = new ParticleStepFunction[mFunctionsArraySize];

	mFunctionsArray[0] = &ParticleEngine::stepEnginePlatform;
	mFunctionsArray[1] = &ParticleEngine::stepEngineGravity;
	mFunctionsArray[2] = &ParticleEngine::stepEngineLorrenz;
	mFunctionsArray[3] = &ParticleEngine::stepEngineEightGravity;
	mFunctionCursor = 0;
	mParticleStep = mFunctionsArray[0];
	if (mFunctionCursor == 0)
		mParticleInit = &ParticleEngine::initParticlePlatform;
	else
		mParticleInit = &ParticleEngine::initParticleRandom;
}

ParticleEngine::~ParticleEngine() {
	LOGI("Cleaning up Particles Engine");
	m_pause = false;
	for (uint i = 0; i < m_threadNb; ++i) {
		m_args[i].die = true;
		pthread_mutex_unlock(&(m_args[i].mutex));
		pthread_join(m_threads[i], 0);
		pthread_mutex_destroy(&(m_args[i].mutex));
	}
	LOGI("Frames : %ld Time slept : %ldms\n", getFrameNb(), getTimeSlept());
	// at this point every thread should be dead

	if (m_threads)
		delete[] m_threads;
	m_threads = 0;
	if (m_args)
		delete[] m_args;
	m_args = 0;
	if (m_positions)
		delete[] m_positions;
	m_positions = 0;
	if (m_speed)
		delete[] m_speed;
	m_speed = 0;
	if (m_active)
		delete[] m_active;
	m_active = 0;
	if (m_eightPos)
		delete[] m_eightPos;
	m_eightPos = 0;
	LOGI("Engine Killed. \n");
}

void ParticleEngine::stepEnginePlatform(int start, int end) {
	// Begin loop
	for (uint i = start; i < end; ++i) {
		if (m_active[i] == 1) {
			POS_X(i) += PSPEED_X(i);
			POS_Y(i) += PSPEED_Y(i);
			POS_Z(i) += PSPEED_Z(i);
			PSPEED_Y(i) -= m_gravity;

			if (POS_Y(i) > LIMIT_Y || POS_Y(i) < -LIMIT_Y || POS_X(i) > LIMIT_X
					|| POS_X(i) < -LIMIT_X || POS_Z(i) > LIMIT_Z
					|| POS_Z(i) < -LIMIT_Z)
				initParticle(i);

			// consider the center of the cone at 0,CONE_TOP_Y,0
			// it was supposed to be a cone....
			if (POS_Y(i) <= CONE_TOP_Y && POS_Y(i) >= CONE_BOT_Y
					&& PSPEED_Y(i) <= 0) {
				// compute distance to the center
				float dist = (POS_X(i) * POS_X(i)) + (POS_Z(i) * POS_Z(i));
				float maxDist = ((CONE_TOP_Y - POS_Y(i)) / CONE_DELTA)
						* CONE_RAD_SQR;
				if (dist <= maxDist) {
					if (m_hasWave && dist <= m_waveDist + WAVE_WIDTH
							&& dist >= m_waveDist - WAVE_WIDTH)
						PSPEED_Y(i) = m_waveIntensity;
					else
						PSPEED_Y(i) = -PSPEED_Y(i) * BOUNCYNESS;
				}
			}
			if (POS_Y(i) <= m_randomCursor - m_randomSpeed
					&& POS_Y(i) >= m_randomCursor + m_randomSpeed)
				setRandomSpeed(i);
		} else if (m_emit) {
			++(m_active[i]);
		}
	}
}

void ParticleEngine::stepEngineGravity(int start, int end) {
	for (uint i = start; i < end; ++i) {

		if (POS_Y(i) > LIMIT_X || POS_Y(i) < -LIMIT_X || POS_X(i) > LIMIT_X
				|| POS_X(i) < -LIMIT_X || POS_Z(i) > LIMIT_Z
				|| POS_Z(i) < -LIMIT_Z)
			initParticle(i);

		POS_X(i) += PSPEED_X(i);
		POS_Y(i) += PSPEED_Y(i);
		POS_Z(i) += PSPEED_Z(i);

		const float x = POS_X(i);
		const float y = POS_Y(i) + 3;
		const float z = POS_Z(i);

		float distance = x * x + y * y + z * z;

		PSPEED_X(i) -= (.001f / distance) * x;
		PSPEED_Y(i) -= (.001f / distance) * y;
		PSPEED_Z(i) -= (.001f / distance) * z;
	}
}

void ParticleEngine::stepEngineLorrenz(int start, int end) {
	const float teta = 10 / 10.f;
	const float phi = 28 / 10.f;
	const float beta = 8.0f / 3.0f / 10.f;
	const float speed = .01f;

	for (uint i = start; i < end; ++i) {

		if (POS_Y(i) > LIMIT_X || POS_Y(i) < -LIMIT_X || POS_X(i) > LIMIT_X
				|| POS_X(i) < -LIMIT_X || POS_Z(i) > LIMIT_Z
				|| POS_Z(i) < -LIMIT_Z)
			initParticle(i);
		POS_X(i) += PSPEED_X(i);
		POS_Y(i) += PSPEED_Y(i);
		POS_Z(i) += PSPEED_Z(i);

		const float x = POS_X(i);
		const float y = POS_Y(i) + 3;
		const float z = POS_Z(i) + 3;

		PSPEED_X(i) = (teta * (y - x)) * speed;
		PSPEED_Y(i) = (x * (phi - z) - y) * speed;
		PSPEED_Z(i) = (x * y - beta * z) * speed;

	}
}

void ParticleEngine::stepEngineEightGravity(int start, int end) {
	float ratio = (float) EIGHT_SIZE / (float) m_part_nbr;
	for (int j = start; j < end; ++j) {
		if (POS_Y(j) > LIMIT_X || POS_Y(j) < -LIMIT_X || POS_X(j) > LIMIT_X
				|| POS_X(j) < -LIMIT_X || POS_Z(j) > LIMIT_Z
				|| POS_Z(j) < -LIMIT_Z)
			initParticle(j);
		int i = j * ratio;
		float u = m_eightPos[i * 2];
		float X = m_eightPos[i * 2 + 1];
		float Z = X;
		float Y = u - 3;

		POS_X(j) += PSPEED_X(j);
		POS_Y(j) += PSPEED_Y(j);
		POS_Z(j) += PSPEED_Z(j);

		const float x = POS_X(j) - X;
		const float y = POS_Y(j) - Y;
		const float z = POS_Z(j) - Z;

		PSPEED_X(j) = -x / 40;
		PSPEED_Y(j) = -y / 40;
		PSPEED_Z(j) = -z / 40;
	}
}

