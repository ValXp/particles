package com.valxp.particles;

import java.io.IOException;
import java.io.InputStream;

import android.app.Activity;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.util.Log;
import android.widget.Toast;


public class JavaCaller {
	
	public static ParticlesActivity context = null;
	public static String readAsset(String name)
	{
		if (context == null)
		{
			Log.e("JavaCaller", "Context not set, cannot read asset : " + name);
		}
		AssetManager ass = context.getAssets();
	
		InputStream is = null;
		try {
			is = ass.open(name);
		} catch (IOException e) {
			
			e.printStackTrace();
		}
		String str = new String();
		
		int size = 0;
		while (size != -1)
		{
			byte[] buffer = new byte[6];
			try {
				size = is.read(buffer);
			} catch (IOException e) {
				e.printStackTrace();
			}
			
			if (size > 0)
			{
				str += new String(buffer).substring(0, size);
			}
		}
		return str;
	}
	
	
	public static Bitmap getImage()
	{
		if (context == null)
		{
			new Exception("Context not set, cannot read image").printStackTrace();
			return null;
		}
		AssetManager ass = context.getAssets();
		
		InputStream is = null;
		try {
			is = ass.open("Moi.JPG");
		} catch (IOException e) {
			
			e.printStackTrace();
			return null;
		}
		bmp = BitmapFactory.decodeStream(is);
		return bmp;
	}
	
	public static Bitmap bmp;
	
	private static class messageRunnable implements Runnable {
		public messageRunnable(String message, int duration) {
			msg = message;
			dur = duration;
		}
		private String 	msg;
		private int		dur;
		@Override
		public void run() {
			Toast.makeText(context, msg, dur).show();
		}
		
	}
	
	public static void printMessage(String text, int duration)
	{
		messageRunnable r = new messageRunnable(text, duration);
		if (context == null) {
			new Exception("Context not set, cannot print message").printStackTrace();
			return ;
		}
		Activity a = (Activity)context;
		a.runOnUiThread(r);
		//Toast.makeText(MainActivity.context, text, duration).show();
		System.out.println("printMessage : " + text + " duration " + duration);
	}
	
	public static void onEngineLoaded(int status) {
	    context.onEngineLoaded(status);
	}
	
	public static void onFPSUpdate(float cpu, float gpu, long particlesNumber) {
	    context.onFPSUpdate(cpu, gpu, particlesNumber);
	}
}
