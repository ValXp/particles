package com.valxp.particles;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager.NameNotFoundException;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnKeyListener;
import android.view.inputmethod.InputMethodManager;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.EditText;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.TextView;
import android.widget.Toast;

public class Settings extends Activity {
    public static final String HAS_PREVIOUSLY_FAILED = "HAS_PREVIOUSLY_FAILED";
    private TextView mWarning;

    private void checkArgs() {
        Intent intent = getIntent();
        if (intent.getBooleanExtra(HAS_PREVIOUSLY_FAILED, false)) {
            mWarning.setVisibility(View.VISIBLE);
            Toast.makeText(this, R.string.too_many_particles, Toast.LENGTH_LONG).show();
        } else
            mWarning.setVisibility(View.GONE);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_settings);
        View b = findViewById(R.id.button1);
        mWarning = (TextView) findViewById(R.id.warning);
        checkArgs();
        final EditText pNumberField = (EditText) findViewById(R.id.editText1);
        final SeekBar seek = (SeekBar) findViewById(R.id.seekBar1);
        final TextView size = (TextView) findViewById(R.id.textView3);
        final CheckBox check = (CheckBox) findViewById(R.id.checkBox1);
        final CheckBox showFps = (CheckBox) findViewById(R.id.checkBox2);
        final CheckBox autoCount = (CheckBox) findViewById(R.id.checkBoxAuto);

        autoCount.setOnCheckedChangeListener(new OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                pNumberField.setEnabled(!isChecked);
                pNumberField.setFocusableInTouchMode(!isChecked);
                pNumberField.setFocusable(!isChecked);
            }
        });
        autoCount.setChecked(PreferencesHelper.getAutoCount(this));

        TextView ver = (TextView) findViewById(R.id.textView4);
        try {
            ver.setText("v" + getPackageManager().getPackageInfo(getPackageName(), 0).versionName + " Arch : "
                    + ParticlesCPP.getArch() + " Build " + ParticlesCPP.getBuild());
        } catch (NameNotFoundException e1) {
            e1.printStackTrace();
            ver.setText("");
        }
        long pNumber = PreferencesHelper.getParticleNumber(this);
        pNumberField.setText("" + (pNumber > 0 ? pNumber : ""));
        float pSize = PreferencesHelper.getParticleSize(this);
        size.setText("" + pSize);
        seek.setProgress(PreferencesHelper.sizeToSeek(pSize));
        check.setChecked(PreferencesHelper.getMotionBlur(this));
        showFps.setChecked(PreferencesHelper.getShowFps(this));
        seek.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
            public void onStopTrackingTouch(SeekBar seekBar) {
            }

            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                size.setText("" + PreferencesHelper.seekToSize(progress));
            }
        });

        pNumberField.addTextChangedListener(new TextWatcher() {
            @Override
            public void onTextChanged(CharSequence arg0, int arg1, int arg2, int arg3) {
                mWarning.setVisibility(View.GONE);
            }

            @Override
            public void beforeTextChanged(CharSequence arg0, int arg1, int arg2, int arg3) {
            }

            @Override
            public void afterTextChanged(Editable arg0) {
            }
        });
        pNumberField.setOnKeyListener(new OnKeyListener() {

            public boolean onKey(View v, int keyCode, KeyEvent event) {
                mWarning.setVisibility(View.GONE);
                if (keyCode == KeyEvent.KEYCODE_ENTER) {
                    InputMethodManager in = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
                    in.hideSoftInputFromWindow(pNumberField.getApplicationWindowToken(), InputMethodManager.HIDE_NOT_ALWAYS);
                    //storeSettings(pNumberField, check.isChecked(), seek.getProgress(), showFps.isChecked(), autoCount.isChecked());
                    return true;
                }
                return false;
            }
        });
        b.setOnClickListener(new OnClickListener() {

            public void onClick(View v) {
                InputMethodManager in = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
                in.hideSoftInputFromWindow(pNumberField.getApplicationWindowToken(), InputMethodManager.HIDE_NOT_ALWAYS);
                if (storeSettings(pNumberField, check.isChecked(), seek.getProgress(), showFps.isChecked(), autoCount.isChecked()))
                    finish();
            }
        });

    }

    private boolean storeSettings(EditText t, Boolean blur, int partSize, boolean showFps, boolean autoCount) {
        if (t.getText() == null && !autoCount) {
            errorParticleNumber();
            return false;
        }
        Long pnumber = 0l;
        try {
            pnumber = Long.valueOf(t.getText().toString());
        } catch (Exception e) {
            if (!autoCount) {
                errorParticleNumber();
                return false;
            }
        }

        if (pnumber <= 0 && !autoCount) {
            errorParticleNumber();
            return false;
        } else {
            PreferencesHelper.setParticleNumber(this, pnumber);
            PreferencesHelper.setParticleSize(this, PreferencesHelper.seekToSize(partSize));
            PreferencesHelper.setMotionBlur(this, blur);
            PreferencesHelper.setShowFps(this, showFps);
            PreferencesHelper.setAutoCount(this, autoCount);
            // Intent intent = new Intent(this, ParticlesActivity.class);
            // startActivityForResult(intent, 4242);
            return true;
        }
    }

    public void errorParticleNumber() {
        Toast.makeText(getApplicationContext(), R.string.Not_Enough_Particles, Toast.LENGTH_SHORT).show();
    }

}
