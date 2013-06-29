package com.valxp.particles;

import android.opengl.Visibility;
import android.os.Bundle;
import android.app.Activity;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;

public class JavaEngineTestActivity extends Activity implements OnClickListener{

	Button create, start, pause, unpause, stop;
	EditText partNbr;
	ParticlesEngine engine;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_java_engine_test);
		create = (Button)findViewById(R.id.create);
		start = (Button)findViewById(R.id.start);
		pause = (Button)findViewById(R.id.pause);
		unpause = (Button)findViewById(R.id.unpause);
		stop = (Button)findViewById(R.id.stop);
		partNbr = (EditText)findViewById(R.id.partNbr);
		create.setOnClickListener(this);
		start.setOnClickListener(this);
		pause.setOnClickListener(this);
		unpause.setOnClickListener(this);
		stop.setOnClickListener(this);
	}
	@Override
	public void onClick(View v) {
		Button b = (Button)v;
		if (b == null) {
			return;
		}
		if (b.equals(create)) {
			start.setVisibility(View.VISIBLE);
			pause.setVisibility(View.VISIBLE);
			unpause.setVisibility(View.VISIBLE);
			stop.setVisibility(View.VISIBLE);
			create.setVisibility(View.INVISIBLE);
			engine = new ParticlesEngine(Integer.parseInt(partNbr.getText().toString()), 4);
			
		}else if (b.equals(start)) {
			engine.start();
		} else if (b.equals(pause)) {
			engine.pause();
		} else if (b.equals(unpause)) {
			engine.unPause();
		} else if (b.equals(stop)) {
			engine.stop();
		}
	}
	


}
