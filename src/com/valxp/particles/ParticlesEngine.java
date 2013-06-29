package com.valxp.particles;

import java.util.Date;
import java.util.Random;
import java.util.Vector;


public class ParticlesEngine {
	final static String tag = "java engine";
	final static Float	GROUND_Y = -4.0f;
	final static Float	GROUND_Z = 1.5f;
	final static Float	GROUND_X = 1.5f;
	final static Float	RAND = 0.3f;
	final static Float	LIMIT_X = 9.0f;
	final static Float	LIMIT_Y = 9.0f;
	final static Float	LIMIT_Z = 9.0f;
	final static Float	GRAVITY = 0.002f;
	final static Float	SPEED_X = 0.05f;
	final static Float	SPEED_Y = 0.5f;
	final static Float	SPEED_Z = 0.05f;
	final static Float	CONE_TOP_Y = -4f;
	final static Float	CONE_BOT_Y = -5f;
	final static Float	CONE_BASE_RADIUS = 2f;
	final static Float	CONE_RAD_SQR = (CONE_BASE_RADIUS * CONE_BASE_RADIUS);
	final static Float	CONE_DELTA = (CONE_TOP_Y - CONE_BOT_Y);
	final static Float	BOUNCYNESS = 0.2f;
	final static Float	WAVE_WIDTH = 0.2f;
	final static Float	MAX_FRAME_TIME_MS = 16f;
	
	
	private Vector<EngineThread>	m_threads;
	private Object	m_lockPause;
	private Random	m_rand;
	private int		m_particlesNumber;
	private Float[]	m_positions;
	private	Float[] m_speed;
	private Integer[]	m_active;
	private Float m_posX;
	private Float m_posY;
	private Float m_posZ;
	private Float m_limitX;
	private Float m_limitY;
	private Float m_limitZ;
	private Integer m_frameNb;
	private Float m_userX;
	private Float m_userY;
	private Float m_userZ;
	private Boolean m_emit;
	private Integer	m_timeSlept;
	private Boolean	m_pause;
	private Float	m_gravity;
	private Boolean m_hasWave;
	private Float	m_waveDist;
	private Float	m_waveEnd;
	private Float	m_waveSpeed;
	private Float	m_waveIntensity;
	
	public ParticlesEngine(int particlesNumber, int threadNumber) {
		System.out.println(tag + "Engine initialization : " + particlesNumber + " particles");
		threadNumber = 4; // for now, TODO: getcpu count
		m_threads = new Vector<EngineThread>();
		m_lockPause = new Object();
		for (int i = 0; i < threadNumber; ++i) {
			m_threads.add(new EngineThread(i));
			m_threads.elementAt(i).pause();
		}
		m_particlesNumber = particlesNumber;
		m_posX = 0.0f;
		m_posY = GROUND_Y;
		m_posZ = 0.0f;
		m_limitX = LIMIT_X;
		m_limitY = LIMIT_Y;
		m_limitZ = LIMIT_Z;
		m_userX = 0.0f;
		m_userY = 0.0f;
		m_userZ = 0.0f;
		m_frameNb = 0;
		m_emit = true;
		m_timeSlept = 0;
		m_pause = true;
		m_gravity = GRAVITY;
		m_hasWave = false;
		initParticles();
		
	}
	
	
	
	private void setXPos(int id, Float pos) {
		m_positions[id * 3] = pos;
	}
	private void setYPos(int id, Float pos) {
		m_positions[id * 3 + 1] = pos;
	}
	private void setZPos(int id, Float pos) {
		m_positions[id * 3 + 2] = pos;
	}
	
	private void setXSpeed(int id, Float pos) {
		m_speed[id * 3] = pos;
	}
	private void setYSpeed(int id, Float pos) {
		m_speed[id * 3 + 1] = pos;
	}
	private void setZSpeed(int id, Float pos) {
		m_speed[id * 3 + 2] = pos;
	}
	
	private Float getXPos(int id) {
		return m_positions[id * 3];
	}
	private Float getYPos(int id) {
		return m_positions[id * 3 + 1];
	}
	private Float getZPos(int id) {
		return m_positions[id * 3 + 2];
	}

	private Float getXSpeed(int id) {
		return m_speed[id * 3];
	}
	private Float getYSpeed(int id) {
		return m_speed[id * 3 + 1];
	}
	private Float getZSpeed(int id) {
		return m_speed[id * 3 + 2];
	}
	
	public void	start() {
		for(int i = 0; i < m_threads.size(); ++i) {
			m_threads.elementAt(i).start();
		}
	}
	
	public void pause() {
		for(int i = 0; i < m_threads.size(); ++i) {
			m_threads.elementAt(i).pause();
		}
	}
	
	public void unPause() {
		for(int i = 0; i < m_threads.size(); ++i) {
			m_threads.elementAt(i).unPause();
		}
		synchronized(m_lockPause) {
			m_lockPause.notifyAll();
		}
	}
	
	public void stop() {
		for(int i = 0; i < m_threads.size(); ++i) {
			m_threads.elementAt(i).terminate();
			m_threads.elementAt(i).unPause();
		}
		synchronized(m_lockPause) {
			m_lockPause.notifyAll();
		}
		for (int i = 0; i < m_threads.size(); ++i) {
			try {
				m_threads.elementAt(i).join();
			} catch (InterruptedException e) {
				System.out.println(tag + "Error joining thread " + i + " exception : " + e.getMessage());
			}
		}
		System.out.println("Frames : " + m_frameNb + " Time slept : " + m_timeSlept);
	}
	

	
	private void initParticles() {
		m_rand = new Random();
		System.out.println(tag + "Allocating a lot of memory");
		m_positions = new Float[m_particlesNumber * 3];
		m_speed = new Float[m_particlesNumber * 3];
		m_active = new Integer[m_particlesNumber];
		// TODO : if alloc fails
		for (int cur = 0; cur < m_particlesNumber; ++cur) {
			initParticle(cur);
		}
	}
	
	Float[] genRandomDiskPoint(Float disk_radius) // Z will be zero
	{
		Float mrand = m_rand.nextFloat();
	    Float phi = (float) (2 * Math.PI * m_rand.nextInt());
	    Float rad = (float) (Math.sqrt(mrand) * disk_radius);

	    Float[] ret = new Float[2];
	    ret[0] = (float) (Math.sin(phi) * rad);
	    ret[1] = (float) (Math.cos(phi) * rad);
	    return ret;
	}


	Float	myRand(Float min, Float max) {
		return (m_rand.nextFloat() * (max - min)) + min;
	}
	
	private void initParticle(int idx) {
		setXPos(idx, m_posX);
		setYPos(idx, m_posY);
		setZPos(idx, m_posZ);
		
		setYSpeed(idx, (SPEED_Y * myRand(RAND / 1.2f, RAND)) + m_userX);
		
		Float[] points = genRandomDiskPoint(RAND);
		setXSpeed(idx, (SPEED_X * points[0]) + m_userX);
		setZSpeed(idx, (SPEED_Z * points[0]) + m_userZ);
		
		m_active[idx] = myRand(-50.0f, 0.0f).intValue();
	}
	private class EngineThread extends Thread {
		private Integer 	m_id;
		private Boolean		m_run;
		private Boolean		m_pause;
		private Integer		m_frameNb;

		public EngineThread(int id) {
			m_id = id;
			m_run = true;
			m_pause = true;
			m_frameNb = 0;
			System.out.println(tag + "Thread " + id + " birth");
		}
		
		public void terminate() {
			m_run = false;
		}
		public void	pause() {
			m_pause = true;
		}
		
		public void unPause() {
			m_pause = false;
		}
		
		public void incFrame() {
			m_frameNb++;
		}
		
		public Integer	getTId() {
			return m_id;
		}
		
		
		@Override
		public void run() {
			System.out.println(tag + "Thread " + m_id + " start");
			if (m_pause) {
				System.out.println(tag + "Thread " + m_id + " paused");
				try {
					synchronized(m_lockPause) {
						m_lockPause.wait();
					}
				} catch (InterruptedException e) {
					System.out.println(tag + "Thread " + m_id + " interrupted");
					return ;
				}
			}
			while (m_run) {
				_step(this);
				if (m_pause) {
					System.out.println(tag + "Thread " + m_id + " paused");
					try {
						synchronized(m_lockPause) {
							m_lockPause.wait();
						}
					} catch (InterruptedException e) {
						System.out.println(tag + "Thread " + m_id + " interrupted");
						return ;
					}
				}
			}
			System.out.println(tag + "Thread " + m_id + " death");
		}
	}
	public void _step(EngineThread thread) {
		long ms = new Date().getTime();
		Integer particlePerThread = m_particlesNumber / m_threads.size();
		Integer	start = thread.getTId() * particlePerThread;
		Integer	end = (thread.getTId() + 1) * particlePerThread;
		for (Integer i = start; i < end; ++i) {
			if (m_active[i] == 1) {
				setXPos(i, getXPos(i) + getXSpeed(i));
				setYPos(i, getYPos(i) + getYSpeed(i));
				setZPos(i, getZPos(i) + getZSpeed(i));
				
				setYSpeed(i, getYSpeed(i) - m_gravity);
				
				if (	getYPos(i) > LIMIT_Y || getYPos(i) < -LIMIT_Y ||
						getXPos(i) > LIMIT_X || getXPos(i) < -LIMIT_X ||
						getZPos(i) > LIMIT_Z || getZPos(i) < -LIMIT_Z) {
					initParticle(i);
				}
				
				if (getYPos(i) <= CONE_TOP_Y && getYPos(i) >= CONE_BOT_Y && getYSpeed(i) <= 0) {
					Float dist = (getXPos(i) * getXPos(i)) + (getZPos(i) * getZPos(i));
					Float maxDist = ((CONE_TOP_Y - getYPos(i)) / CONE_DELTA) * CONE_RAD_SQR;
					if (dist <= maxDist) {
						if (m_hasWave &&
							dist <= m_waveDist + WAVE_WIDTH &&
							dist >= m_waveDist - WAVE_WIDTH) {
							setYSpeed(i, m_waveIntensity);
						} else
						{
							setYSpeed(i, -getYSpeed(i) * BOUNCYNESS);
						}
					}
				}
				
			} else if (m_emit){
				++(m_active[i]);
			}
		}
		if (thread.getTId() == 0) {
			++m_frameNb;
			if (m_hasWave) {
				moveWave();
			}
		}
		thread.incFrame();
		Long current = new Date().getTime();
		Long delta = current - ms;
		
		if (delta < MAX_FRAME_TIME_MS && delta >= 0) {
			Integer frame = (int) (MAX_FRAME_TIME_MS - delta);
			synchronized (m_timeSlept) {
				m_timeSlept++;
			}
			try {
				Thread.sleep(frame);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
	}



	private void moveWave() {
		// TODO : implement waves
	}
}
