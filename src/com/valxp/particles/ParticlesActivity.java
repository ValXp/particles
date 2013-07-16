
package com.valxp.particles;

import android.app.Activity;
import android.media.audiofx.Visualizer;
import android.media.audiofx.Visualizer.OnDataCaptureListener;
import android.os.Bundle;
import android.os.SystemClock;
import android.util.Log;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.TextView;

public class ParticlesActivity extends Activity {
    public static final int RESULT_FAILURE = 424242;
    private Visualizer visualizer;
    private GLView mview;
    private FrameLayout mContent;
    private boolean showFps;

    public void launch()
    {
        int pnumber = PreferencesHelper.getParticleNumber(this);
        float partSize = PreferencesHelper.getParticleSize(this);
        boolean motionBlur = PreferencesHelper.getMotionBlur(this);
        showFps = PreferencesHelper.getShowFps(this);
        mview = new GLView(this, pnumber, partSize, motionBlur);
        mview.setId(4242);
        mContent = (FrameLayout) findViewById(R.id.contentView);
        mContent.addView(mview, 0);
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.d("", "On create main activity");
        super.onCreate(savedInstanceState);
        JavaCaller.context = this;
        setContentView(R.layout.activity_main);
        launch();
    }

    protected void onResume() {
        mview.onResume();
        super.onResume();
        running = true;
        visualizer = new Visualizer(0);
        visualizer.getCaptureSize();

        OnDataCaptureListener listener = new OnDataCaptureListener() {

            public void onWaveFormDataCapture(Visualizer visualizer, byte[] waveform,
                    int samplingRate) {
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
                    max /= 2;
                    last = cur;
                }
                sum /= (float) length;
                if (sum > max || sum > max - (max / 5))
                {
                    max = sum;
                    ParticlesCPP.beat(max / 80.f, 0.5f);

                    last = cur;
                }
            }

            float max = 0;
            long last = 0;
        };

        visualizer.setDataCaptureListener(listener, Visualizer.getMaxCaptureRate() / 2, false, true);
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
                    if (showFps) {
                        mCpu.setVisibility(View.VISIBLE);
                        mGpu.setVisibility(View.VISIBLE);
                        mZoom.setVisibility(View.VISIBLE);
                    }
                }
            });
        } else {
            setResult(RESULT_FAILURE);
            finish();
        }
    }

    TextView mCpu, mGpu, mZoom;
    boolean running = true;

    public void onFPSUpdate(final float cpu, final float gpu) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (!running || !showFps)
                    return;
                mCpu.setText("Cpu Fps : " + cpu);
                mGpu.setText("Gpu Fps : " + gpu);
            }
        });
    }

    public void onZoomUpdate(float zoom) {
        if (!running || !showFps)
            return;
        mZoom.setText("Zoom : " + String.format("%.1f", zoom) + "x");
    }
    
    @Override
    public void onBackPressed() {
        super.onBackPressed();
    }

    protected void onPause() {
        mview.onPause();
        super.onPause();

        running = false;

        if (visualizer != null)
        {
            visualizer.setEnabled(false);
            visualizer.release();
            visualizer = null;
        }
        ParticlesCPP.pause();
    }

    protected void onStop() {
        super.onStop();
        ParticlesCPP.pause();
    }

    protected void onDestroy() {
        super.onDestroy();
        ParticlesCPP.die();
    }
}
