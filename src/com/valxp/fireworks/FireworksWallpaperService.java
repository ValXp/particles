package com.valxp.fireworks;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.opengl.GLSurfaceView.Renderer;

import com.valxp.particles.JavaCaller;
import com.valxp.particles.ParticlesCPP;

public class FireworksWallpaperService extends OpenGLES2WallpaperService {
	
	@Override
	public void onCreate() {
		JavaCaller.context = FireworksWallpaperService.this;
		super.onCreate();
	}

	@Override
	Renderer getNewRenderer() {
		return new FireworksRenderer();
	}

	private class FireworksRenderer implements Renderer {

		public void onDrawFrame(GL10 gl) {
			ParticlesCPP.step();

		}

		public void onSurfaceChanged(GL10 gl, int width, int height) {

			System.out.println("[JAVA] On surface Changed");
			ParticlesCPP.init(width, height, PreferencesHelper
					.getParticleNumber(FireworksWallpaperService.this),
					PreferencesHelper
							.getParticleSize(FireworksWallpaperService.this),
					PreferencesHelper
							.getMotionBlur(FireworksWallpaperService.this));

		}

		public void onSurfaceCreated(GL10 gl, EGLConfig config) {
			System.out.println("[JAVA] On surface Created");
		}
	}

}
