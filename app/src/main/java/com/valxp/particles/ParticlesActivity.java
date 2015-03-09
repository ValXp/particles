package com.valxp.particles;

import java.lang.reflect.Method;

import android.annotation.TargetApi;
import android.app.Activity;
import android.media.audiofx.Visualizer;
import android.media.audiofx.Visualizer.OnDataCaptureListener;
import android.os.Build;
import android.os.Bundle;
import android.os.SystemClock;
import android.util.Log;
import android.view.View;
import android.view.View.OnSystemUiVisibilityChangeListener;
import android.widget.FrameLayout;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.TextView;

public class ParticlesActivity extends Activity {
    public static final int RESULT_FAILURE = 424242;
    private Visualizer visualizer;
    private GLView mview;
    private FrameLayout mContent;
    private boolean showFps;
    private long mLastPartNumber = 0;
    private long mSinceNotChange = 0;
    
    private void launch() {
        boolean autoGenerate = PreferencesHelper.getAutoCount(this);
        long pnumber = PreferencesHelper.getParticleNumber(this);
        float partSize = PreferencesHelper.getParticleSize(this);
        boolean motionBlur = PreferencesHelper.getMotionBlur(this);
        showFps = PreferencesHelper.getShowFps(this);
        if (autoGenerate)
            pnumber = 1000000;
        mLastPartNumber = pnumber;
        mview = new GLView(this, pnumber, partSize, motionBlur, autoGenerate);
        mview.setId(4242);
        mContent = (FrameLayout) findViewById(R.id.contentView);
        mContent.addView(mview, 0);
    }
    


    @TargetApi(Build.VERSION_CODES.KITKAT)
    private void hideNavigationBar() {
        View decorView = getWindow().getDecorView();
        int uiOptions = View.SYSTEM_UI_FLAG_FULLSCREEN;

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
            uiOptions |= View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION;
        }
        try {
        decorView.setSystemUiVisibility(uiOptions);
        } catch (NoSuchMethodError err) {
            
        }
    }
    

    @TargetApi(Build.VERSION_CODES.HONEYCOMB)
    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.d("", "On create main activity");
        super.onCreate(savedInstanceState);
        JavaCaller.context = this;
        View decorView = getWindow().getDecorView();
        try {
            Method setListener = View.class.getMethod("setOnSystemUiVisibilityChangeListener", OnSystemUiVisibilityChangeListener.class);
            setListener.invoke(decorView, new OnSystemUiVisibilityChangeListener() {
                
                @Override
                public void onSystemUiVisibilityChange(int arg0) {
                    hideNavigationBar();
                }
            });
        } catch (Exception e) {
            e.printStackTrace();
        } catch (NoClassDefFoundError e) {
            e.printStackTrace();
        }
        hideNavigationBar();
        setContentView(R.layout.activity_main);

        launch();
    }

    protected void onResume() {
        hideNavigationBar();
        mview.onResume();
        super.onResume();
        running = true;
        visualizer = new Visualizer(0);
        visualizer.getCaptureSize();

        OnDataCaptureListener listener = new OnDataCaptureListener() {

            public void onWaveFormDataCapture(Visualizer visualizer, byte[] waveform, int samplingRate) {
            }

            public void onFftDataCapture(Visualizer visualizer, byte[] fft, int samplingRate) {
                float sum = 0;
                int length = fft.length;// < 200 ? fft.length : 200;
                long cur = SystemClock.elapsedRealtime();

                for (int i = 0; i < length / 2; ++i) {
                    // float amp = (float) (((fft[i] * 2.0) + (fft[i + 1] *
                    // 2.0)) / 2.0);
                    sum += Math.abs(fft[i]);
                }
                if (sum == 0) {
                    if (mBlur != null && showFps)
                        mBlur.setVisibility(View.VISIBLE);
                } else {
                    if (mBlur != null)
                        mBlur.setVisibility(View.INVISIBLE);
                }
                // Log.d("sound", "sum " + sum);
                if (cur - last >= 1000) {
                    if (sum == 0 && max != 0) {
                        // ParticlesCPP.randomize();
                        max = 0;
                        
                    }
                    max /= 2;
                    last = cur;
                }

                sum /= (float) length / 2;
                // ParticlesCPP.setSize(sum / 6);
                // ParticlesCPP.setBlur(sum / 6);
                if (sum > max || sum > max - (max / 5)) {
                    max = sum;
                    ParticlesCPP.beat(max / 80.f, 0.5f);

                    last = cur;
                }
                if (max != 0 && sum != 0) {
                    ParticlesCPP.setBlur(((float) sum / max) - .1f);
                }
            }

            float max = 0;
            long last = 0;
        };

        visualizer.setDataCaptureListener(listener, Visualizer.getMaxCaptureRate(), false, true);
        visualizer.setEnabled(true);
    }

    public void onEngineLoaded(int status) {
        Log.d("", "Engine loaded status : " + status);
        if (status == 0) {
            runOnUiThread(new Runnable() {

                @Override
                public void run() {
                    View v = findViewById(R.id.loadingOverlay);
                    v.setVisibility(View.GONE);
                    mCpu = (TextView) findViewById(R.id.cpu);
                    mGpu = (TextView) findViewById(R.id.gpu);
                    mZoom = (TextView) findViewById(R.id.zoom);
                    mBlur = (SeekBar) findViewById(R.id.blur_seek);
                    mSize = (SeekBar) findViewById(R.id.size_seek);

                    mBlur.setMax(50);
                    mBlur.setProgress((int) ((ParticlesCPP.getBlur() - 0.5) * 100.0));
                    mBlur.setOnSeekBarChangeListener(new SeekChange());

                    mSize.setMax(100);
                    mSize.setProgress(PreferencesHelper.sizeToSeek(ParticlesCPP.getSize()));
                    mSize.setOnSeekBarChangeListener(new SeekChange());

                    if (showFps) {
                        mCpu.setVisibility(View.VISIBLE);
                        mGpu.setVisibility(View.VISIBLE);
                        mZoom.setVisibility(View.VISIBLE);
                        if (PreferencesHelper.getMotionBlur(getApplicationContext()))
                            mBlur.setVisibility(View.VISIBLE);
                        mSize.setVisibility(View.VISIBLE);
                    }
                }
            });
        } else {
            setResult(RESULT_FAILURE);
            finish();
        }
    }

    TextView mCpu, mGpu, mZoom;
    SeekBar mBlur, mSize;
    boolean running = true;

    private class SeekChange implements OnSeekBarChangeListener {

        @Override
        public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
            if (seekBar.equals(mBlur))
                ParticlesCPP.setBlur(((float) progress / 100.0f) + 0.5f);
            if (seekBar.equals(mSize))
                ParticlesCPP.setSize(PreferencesHelper.seekToSize(progress));
        }

        @Override
        public void onStartTrackingTouch(SeekBar seekBar) {
            // TODO Auto-generated method stub

        }

        @Override
        public void onStopTrackingTouch(SeekBar seekBar) {
            // TODO Auto-generated method stub

        }

    }
   
    public void onFPSUpdate(final float cpu, final float gpu, final long particlesNumber) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (!running)
                    return;
                if (showFps) {
                    mCpu.setText("Cpu Fps : " + cpu);
                    mGpu.setText("Gpu Fps : " + gpu);
                }
                String text = "";
                int visibility = View.VISIBLE;
                if (mLastPartNumber != particlesNumber) {
                    text = getString(R.string.calibrating);
                    mSinceNotChange = 0;
                } else {
                    if (++mSinceNotChange > 5)
                        visibility = View.GONE;
                }
                mZoom.setVisibility(visibility);
                mZoom.setText(text + particlesNumber +" " + getString(R.string.particles));
                mLastPartNumber = particlesNumber;
            }
        });
    }

    public void onZoomUpdate(float zoom) {
        if (!running || !showFps)
            return;
        mZoom.setVisibility(View.VISIBLE);
        mZoom.setText("Zoom : " + String.format("%.1f", zoom));
    }

    @Override
    public void onBackPressed() {
        super.onBackPressed();
    }

    protected void onPause() {
        Log.d("ParticlesActivity", "Activity Paused");
        mview.onPause();
        running = false;

        if (visualizer != null) {
            visualizer.setEnabled(false);
            visualizer.release();
            visualizer = null;
        }
        ParticlesCPP.pause();
        super.onPause(); 
    }

    protected void onStop() {
        Log.d("ParticlesActivity", "Activity Stopped");
        ParticlesCPP.pause();
        super.onStop();   
    }

    protected void onDestroy() {
        Log.d("ParticlesActivity", "Activity Destroyed");
        ParticlesCPP.die();
        super.onDestroy();
    }
}
