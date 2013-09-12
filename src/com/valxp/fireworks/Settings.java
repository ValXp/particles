
package com.valxp.fireworks;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager.NameNotFoundException;
import android.os.Bundle;
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

import com.valxp.particles.ParticlesCPP;

public class Settings extends Activity {
    
    boolean alreadyLaunched = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_settings);
        Button b = (Button) findViewById(R.id.button1);
        final EditText pNumberField = (EditText) findViewById(R.id.editText1);
        final SeekBar seek = (SeekBar) findViewById(R.id.seekBar1);
        final TextView size = (TextView) findViewById(R.id.textView3);
        final CheckBox check = (CheckBox) findViewById(R.id.checkBox1);
        TextView ver = (TextView) findViewById(R.id.textView4);
        try {
            ver.setText("v" + getPackageManager().getPackageInfo(getPackageName(), 0).versionName + " Build " + ParticlesCPP.getBuild());
        } catch (NameNotFoundException e1) {
            // TODO Auto-generated catch block
            e1.printStackTrace();
            ver.setText("");
        }
        int pNumber = PreferencesHelper.getParticleNumber(this);
        pNumberField.setText("" + (pNumber > 0 ? pNumber : ""));
        float pSize = PreferencesHelper.getParticleSize(this);
        size.setText("" + pSize);
        seek.setProgress(PreferencesHelper.sizeToSeek(pSize));
        check.setChecked(PreferencesHelper.getMotionBlur(this));
        seek.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
            public void onStopTrackingTouch(SeekBar seekBar) {
            }

            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            public void onProgressChanged(SeekBar seekBar, int progress,
                    boolean fromUser) {
                size.setText("" + PreferencesHelper.seekToSize(progress));
            }
        });
        pNumberField.setOnKeyListener(new OnKeyListener() {
            public boolean onKey(View v, int keyCode, KeyEvent event) {
                if (keyCode == KeyEvent.KEYCODE_ENTER)
                {
                    InputMethodManager in = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
                    in.hideSoftInputFromWindow(pNumberField
                            .getApplicationWindowToken(),
                            InputMethodManager.HIDE_NOT_ALWAYS);
                    storeSettings(pNumberField, check.isChecked(), seek.getProgress(), false);
                    return true;
                }
                return false;
            }
        });
        b.setOnClickListener(new OnClickListener() {

            public void onClick(View v) {
                InputMethodManager in = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
                in.hideSoftInputFromWindow(pNumberField
                        .getApplicationWindowToken(),
                        InputMethodManager.HIDE_NOT_ALWAYS);
                storeSettings(pNumberField, check.isChecked(), seek.getProgress(), false);
            }
        });
    }

    private void storeSettings(EditText t, Boolean blur, int partSize, Boolean showFps)
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
            PreferencesHelper.setParticleNumber(this, pnumber);
            PreferencesHelper.setParticleSize(this, PreferencesHelper.seekToSize(partSize));
            PreferencesHelper.setMotionBlur(this, blur);
            PreferencesHelper.setShowFps(this, showFps);
            if (alreadyLaunched)
                return;
            alreadyLaunched = true;
            finish();
        }
    }
    
    @Override
    protected void onResume() {
        alreadyLaunched = false;
        super.onResume();
    }

    public void errorParticleNumber()
    {
        Toast.makeText(getApplicationContext(), R.string.Not_Enough_Particles, Toast.LENGTH_SHORT).show();
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (resultCode == ParticlesActivity.RESULT_FAILURE) {
                Toast.makeText(this, R.string.too_many_particles, Toast.LENGTH_LONG).show();
        }
        super.onActivityResult(requestCode, resultCode, data);
    }
}