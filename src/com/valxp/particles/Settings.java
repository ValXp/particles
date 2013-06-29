package com.valxp.particles;

import android.os.Bundle;
import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.content.pm.PackageManager.NameNotFoundException;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnKeyListener;
import android.view.inputmethod.InputMethodManager;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.SeekBar.OnSeekBarChangeListener;

public class Settings extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		Button b = (Button)findViewById(R.id.button1);
        final EditText t = (EditText)findViewById(R.id.editText1);
        final SeekBar seek = (SeekBar)findViewById(R.id.seekBar1);
        final TextView size = (TextView)findViewById(R.id.textView3);
        final CheckBox check = (CheckBox)findViewById(R.id.checkBox1);
        TextView ver = (TextView)findViewById(R.id.textView4);
        try {
			ver.setText("v" + getPackageManager().getPackageInfo(getPackageName(), 0).versionName);
		} catch (NameNotFoundException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
			ver.setText("");
		}
        
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

					storeSettings(t, check.isChecked(), seek.getProgress());
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
					storeSettings(t, check.isChecked(), seek.getProgress());
			}
		});
	}
	
	private void storeSettings(EditText t, Boolean blur, int partSize)
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
			// save settings
			// getApplicationContext(), pnumber, (1.0f + partSize / 10.0f), blur
			SharedPreferences prefs = getSharedPreferences(getString(R.string.settings_name), MODE_MULTI_PROCESS);
			Editor edit = prefs.edit();
			edit.putInt(getString(R.string.settings_particle_number), pnumber);
			edit.putFloat(getString(R.string.settings_particle_size), (1.0f + partSize / 10.0f));
			edit.putBoolean(getString(R.string.settings_particle_blur), blur);
			edit.commit();
			System.exit(0);
		}
	}

	public void errorParticleNumber()
	{
		Toast.makeText(getApplicationContext(), R.string.Not_Enough_Particles, Toast.LENGTH_SHORT).show();
	}

}
