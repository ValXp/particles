package com.valxp.particles;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;

public class StartActivity extends Activity implements OnClickListener{
    private View mStartButton;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_start);
        mStartButton = findViewById(R.id.buttonStart);
        View optionsButton = findViewById(R.id.buttonOptions);
        mStartButton.setOnClickListener(this);
        optionsButton.setOnClickListener(this);
    }
    @Override
    public void onClick(View v) {
        if (v == mStartButton) {
            Intent intent = new Intent(this, ParticlesActivity.class);
            startActivityForResult(intent, 4242);        
        } else {
            startSettings(false);
        }
    }
    
    private void startSettings(boolean previouslyFailed) {
        Intent intent = new Intent(this, Settings.class);
        intent.putExtra(Settings.HAS_PREVIOUSLY_FAILED, previouslyFailed);
        startActivity(intent);
    }
    
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (resultCode == ParticlesActivity.RESULT_FAILURE) {
            startSettings(true);
        }
        super.onActivityResult(requestCode, resultCode, data);
    }
}
