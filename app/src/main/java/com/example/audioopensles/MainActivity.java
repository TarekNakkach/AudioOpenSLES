package com.example.audioopensles;

import androidx.appcompat.app.AppCompatActivity;
import android.content.res.AssetManager;
import android.widget.ImageButton;
import android.os.Bundle;
import android.util.Log;
import android.view.View;



public class MainActivity extends AppCompatActivity {

    // Used to load the 'audioopensles' library on application startup.
    static {
        System.loadLibrary("audioopensles");
    }
    String TAG = "MainActivity";
    // static AssetManager assetManager;

    int SL_PLAYSTATE_STOPPED = 1;
    int SL_PLAYSTATE_PAUSED	 = 2;
    int SL_PLAYSTATE_PLAYING = 3;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Setup audio player to play asset file .mp3
        AssetManager assetManager = getAssets();
        String filename = "no_women_no_cry.mp3";
        NativeLibAudioSetup(assetManager, filename);

        // Play Button
        ImageButton playButton = findViewById(R.id.playButton);
        playButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                NativeLibAudioSetState(SL_PLAYSTATE_PLAYING);
                Log.d(TAG, "Track Position : " + NativeLibAudioGetPosition());
            }
        });

        // Pause Button
        ImageButton pauseButton = findViewById(R.id.pauseButton);
        pauseButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                NativeLibAudioSetState(SL_PLAYSTATE_PAUSED);
                Log.d(TAG, "Track Position : " + NativeLibAudioGetPosition());
            }
        });

        // Previous Button to perform a fast-rewind with 5 seconds
        ImageButton previousButton = findViewById(R.id.previousButton);
        previousButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                NativeLibAudioSeek(NativeLibAudioGetPosition() - 5);
                Log.d(TAG, "Track Position : " + NativeLibAudioGetPosition());
            }
        });

        // Next Button to perform a fast-forward with 5 seconds
        ImageButton nextButton = findViewById(R.id.nextButton);
        nextButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                NativeLibAudioSeek(NativeLibAudioGetPosition() + 5);
                Log.d(TAG, "Track Position : " + NativeLibAudioGetPosition());
            }
        });
    }

    // Start the audio player when calling Activity onStart()
    @Override
    protected void onStart() {
        super.onStart();
        NativeLibAudioSetState(SL_PLAYSTATE_PLAYING);
    }

    // Stop the audio player when calling Activity onStop()
    @Override
    protected void onStop() {
        super.onStop();
        NativeLibAudioSetState(SL_PLAYSTATE_STOPPED);
    }

    // Stop (if not stopped) and destroy the audio player when calling Activity onDestroy()
    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (NativeLibAudioGetState() != SL_PLAYSTATE_STOPPED) {
            NativeLibAudioSetState(SL_PLAYSTATE_STOPPED);
        }
        NativeLibAudioDestroy();
    }

    /**
     * A native method that is implemented by the 'audioopensles' native library,
     * which is packaged with this application.
     */
    public native void NativeLibAudioSetup(AssetManager assetManager, String filename);
    public native void NativeLibAudioDestroy();
    public native void NativeLibAudioSetState(int state);
    public native int NativeLibAudioGetState();
    public native int NativeLibAudioGetDuration();
    public native int NativeLibAudioGetPosition();
    public native void NativeLibAudioSeek(int position);
}