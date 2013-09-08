package com.valxp.particles;

public class ParticlesCPP{
	static {
		System.loadLibrary("Particles");
	}

	public static native void init(int width, int height, int pNumber, float ptSize, boolean motionBlur);
	public static native void step();
	public static native void changeOrientation(float x, float y, float z);
	public static native void die();
	public static native void pause();
	public static native void unpause();
	public static native void beat(float intensity, float speed); // 0.05, 0.5 good values
	public static native void randomize();
	public static native int  getBuild();
	public static native void setZoom(float zoom);
	public static native void setBlur(float blur); // 0.0 to 1.0
	public static native float getBlur();
	public static native float getSize();
	public static native void setSize(float size);
}