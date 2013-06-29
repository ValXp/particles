package com.valxp.particles;

import android.app.Activity;
import android.app.ActivityManager;
import android.app.ActivityManager.MemoryInfo;
import android.content.Context;
import android.content.pm.PackageManager.NameNotFoundException;
import android.media.audiofx.Visualizer;
import android.media.audiofx.Visualizer.OnDataCaptureListener;
import android.os.Bundle;
import android.os.SystemClock;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnKeyListener;
import android.view.inputmethod.InputMethodManager;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends Activity {

	GLView mview;
	static Context context;
	static Activity self;

	public void launch(TextView t, boolean motionBlur, int partSize)
	{
		if (t.getText() == null)
		{
			errorParticleNumber();
			return;
		}
		int pnumber = 0;
		try
		{
			pnumber = Integer.valueOf(t.getText().toString());
		} catch (Exception e)
		{
			errorParticleNumber();
			return;
		}
		
		if (pnumber <= 0)
		{
			errorParticleNumber();
		} else
		{
			mview = new GLView(getApplicationContext(), pnumber, (1.0f + partSize / 10.0f), motionBlur);
			setContentView(mview);
		}
	}
	
	public void errorParticleNumber()
	{
		Toast.makeText(getApplicationContext(), R.string.Not_Enough_Particles, Toast.LENGTH_SHORT).show();
	}
	
	public  void printMem()
	{
		MemoryInfo mi = new MemoryInfo();
        ActivityManager activityManager = (ActivityManager) getSystemService(ACTIVITY_SERVICE);
        activityManager.getMemoryInfo(mi);
        long availableMegs = mi.availMem / 1048576L;
        long threshold = mi.threshold / 1048576L;
        availableMegs -= threshold;
        Toast.makeText(getApplicationContext(), "Free mem : " + availableMegs + " MB", Toast.LENGTH_SHORT).show();
	}
	
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        JavaCaller.context = this;
        setContentView(R.layout.activity_main);
        Button b = (Button)findViewById(R.id.button1);
        final EditText t = (EditText)findViewById(R.id.editText1);
        final SeekBar seek = (SeekBar)findViewById(R.id.seekBar1);
        final TextView size = (TextView)findViewById(R.id.textView3);
        final CheckBox check = (CheckBox)findViewById(R.id.checkBox1);
        TextView ver = (TextView)findViewById(R.id.textView4);
        printMem();
        try {
			ver.setText("v" + getPackageManager().getPackageInfo(getPackageName(), 0).versionName);
		} catch (NameNotFoundException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
			ver.setText("");
		}
        //visualizer = null;
        
        
        seek.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
			public void onStopTrackingTouch(SeekBar seekBar) {
				// TODO Auto-generated method stub
				
			}
			
			public void onStartTrackingTouch(SeekBar seekBar) {
				// TODO Auto-generated method stub
				
			}
			
			public void onProgressChanged(SeekBar seekBar, int progress,
					boolean fromUser) {
				size.setText("" + (1.0 + progress / 10.0));
				
			}
		});
        t.setOnKeyListener(new OnKeyListener() {
			
			public boolean onKey(View v, int keyCode, KeyEvent event) {
				if (keyCode == KeyEvent.KEYCODE_ENTER)
				{
					InputMethodManager in = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
					in.hideSoftInputFromWindow(t
	                        .getApplicationWindowToken(),
	                        InputMethodManager.HIDE_NOT_ALWAYS);

					launch(t, check.isChecked(), seek.getProgress());
					return true;
				}
				// TODO Auto-generated method stub
				return false;
			}
		});
        b.setOnClickListener(new OnClickListener() {
			
			public void onClick(View v) {
				InputMethodManager in = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
					in.hideSoftInputFromWindow(t
	                        .getApplicationWindowToken(),
	                        InputMethodManager.HIDE_NOT_ALWAYS);
					launch(t, check.isChecked(), seek.getProgress());
			}
		});
        
    }


	protected void onResume() {
         super.onResume();

         visualizer = new Visualizer(0);
         int captureSize = visualizer.getCaptureSize();
         System.out.println("Sampling rate : " + captureSize);
         
         OnDataCaptureListener listener = new OnDataCaptureListener() {
			
        	 
			public void onWaveFormDataCapture(Visualizer visualizer, byte[] waveform,
					int samplingRate) {
				// TODO Auto-generated method stub
				
			}
			
			public void onFftDataCapture(Visualizer visualizer, byte[] fft,
					int samplingRate) {
				float sum = 0;
				int length = fft.length < 200 ? fft.length : 200;
				long cur = SystemClock.elapsedRealtime();
				
				for (int i = 2; i < length; i += 2)
		         {
		        	 sum += Math.abs(fft[i]) / (i * 0.1);
		         }
				if (cur - last >= 1000)
				{
					if (sum == 0 && max != 0)
					{
						ParticlesCPP.randomize();
						max = 0;
					}
					//System.out.println("-------------------> lower max");
					max /= 2;
					last = cur;
				}
				sum /= (float)length;
				if (sum > max || sum > max - (max / 5))
				{
					max = sum;
					//System.out.println("Beat : " + max);
					ParticlesCPP.beat(max / 80.f, 0.5f);
					
					last = cur;
				}
			}
			float max = 0;
			long last = 0;
		};

		visualizer.setDataCaptureListener(listener,Visualizer.getMaxCaptureRate() / 2, false, true);
		visualizer.setEnabled(true);
         System.out.println("[JAVA] Activity Resume");
	}
    
	protected void onPause() {
    	 super.onPause();
    	 
    	 if (visualizer != null)
    	 {
	    	 visualizer.setEnabled(false);
	    	 visualizer.release();
	    	 visualizer = null;
    	 }
    	 System.out.println("[JAVA] Activity Pause");
         ParticlesCPP.pause();
     }
     
     protected void onStop() {
    	 super.onStop();
    	 System.out.println("[JAVA] Activity Stop");
    	 ParticlesCPP.pause();
     }
     
     protected void onDestroy() {
    	 super.onDestroy();
    	 System.out.println("[JAVA] Activity Destroy");
    	 ParticlesCPP.die();
     }

    private Visualizer visualizer;
    
}
