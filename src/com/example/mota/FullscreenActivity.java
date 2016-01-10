package com.example.mota;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStreamWriter;

import com.example.mota.util.SystemUiHider;

import android.annotation.TargetApi;
import android.app.Activity;
import android.content.Context;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;

/**
 * An example full-screen activity that shows and hides the system UI (i.e.
 * status bar and navigation/system bar) with user interaction.
 * 
 * @see SystemUiHider
 */
public class FullscreenActivity extends Activity
{
	/* Members by Aananth */
	private Thread mSotaThread;
	private boolean mSotaThreadActive = false;
	private boolean mAppActive = false;
	private boolean mWifiConn = false;
	private boolean mMobileConn = false;
	
	/* Functions added by Aananth */
	private native void nativeSotaMain();
	
	/**
	 * Whether or not the system UI should be auto-hidden after
	 * {@link #AUTO_HIDE_DELAY_MILLIS} milliseconds.
	 */
	private static final boolean AUTO_HIDE = true;

	/**
	 * If {@link #AUTO_HIDE} is set, the number of milliseconds to wait after
	 * user interaction before hiding the system UI.
	 */
	private static final int AUTO_HIDE_DELAY_MILLIS = 3000;

	/**
	 * If set, will toggle the system UI visibility upon interaction. Otherwise,
	 * will show the system UI visibility upon interaction.
	 */
	private static final boolean TOGGLE_ON_CLICK = true;

	/**
	 * The flags to pass to {@link SystemUiHider#getInstance}.
	 */
	private static final int HIDER_FLAGS = SystemUiHider.FLAG_HIDE_NAVIGATION;

	/**
	 * The instance of the {@link SystemUiHider} for this activity.
	 */
	private SystemUiHider mSystemUiHider;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		setContentView(R.layout.activity_fullscreen);

		final View controlsView = findViewById(R.id.fullscreen_content_controls);
		final View contentView = findViewById(R.id.fullscreen_content);

		// Set up an instance of SystemUiHider to control the system UI for
		// this activity.
		mSystemUiHider = SystemUiHider.getInstance(this, contentView,
				HIDER_FLAGS);
		mSystemUiHider.setup();
		mSystemUiHider
				.setOnVisibilityChangeListener(new SystemUiHider.OnVisibilityChangeListener() {
					// Cached values.
					int mControlsHeight;
					int mShortAnimTime;

					@Override
					@TargetApi(Build.VERSION_CODES.HONEYCOMB_MR2)
					public void onVisibilityChange(boolean visible) {
						if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB_MR2) {
							// If the ViewPropertyAnimator API is available
							// (Honeycomb MR2 and later), use it to animate the
							// in-layout UI controls at the bottom of the
							// screen.
							if (mControlsHeight == 0) {
								mControlsHeight = controlsView.getHeight();
							}
							if (mShortAnimTime == 0) {
								mShortAnimTime = getResources().getInteger(
										android.R.integer.config_shortAnimTime);
							}
							controlsView
									.animate()
									.translationY(visible ? 0 : mControlsHeight)
									.setDuration(mShortAnimTime);
						} else {
							// If the ViewPropertyAnimator APIs aren't
							// available, simply show or hide the in-layout UI
							// controls.
							controlsView.setVisibility(visible ? View.VISIBLE
									: View.GONE);
						}

						if (visible && AUTO_HIDE) {
							// Schedule a hide().
							delayedHide(AUTO_HIDE_DELAY_MILLIS);
						}
					}
				});

		// Set up the user interaction to manually show or hide the system UI.
		contentView.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View view) {
				if (TOGGLE_ON_CLICK) {
					mSystemUiHider.toggle();
				} else {
					mSystemUiHider.show();
				}
			}
		});

		// Upon interacting with UI controls, delay any scheduled hide()
		// operations to prevent the jarring behavior of controls going away
		// while interacting with the UI.
		findViewById(R.id.dummy_button).setOnTouchListener(
				mDelayHideTouchListener);

		/* Aananth added the following lines */
		createTempDir();
		readClientInfo();
        startSotaThread();
		mAppActive = true;
	}

	@Override
	protected void onPostCreate(Bundle savedInstanceState) {
		super.onPostCreate(savedInstanceState);

		// Trigger the initial hide() shortly after the activity has been
		// created, to briefly hint to the user that UI controls
		// are available.
		delayedHide(100);
	}

	/**
	 * Touch listener to use for in-layout UI controls to delay hiding the
	 * system UI. This is to prevent the jarring behavior of controls going away
	 * while interacting with activity UI.
	 */
	View.OnTouchListener mDelayHideTouchListener = new View.OnTouchListener() {
		@Override
		public boolean onTouch(View view, MotionEvent motionEvent) {
			if (AUTO_HIDE) {
				delayedHide(AUTO_HIDE_DELAY_MILLIS);
			}
			return false;
		}
	};

	Handler mHideHandler = new Handler();
	Runnable mHideRunnable = new Runnable() {
		@Override
		public void run() {
			mSystemUiHider.hide();
		}
	};

	/**
	 * Schedules a call to hide() in [delay] milliseconds, canceling any
	 * previously scheduled calls.
	 */
	private void delayedHide(int delayMillis) {
		mHideHandler.removeCallbacks(mHideRunnable);
		mHideHandler.postDelayed(mHideRunnable, delayMillis);
	}
	
    @Override 
    protected void onStop() {                                                                 
        super.onStop();                                                                       
		mAppActive = false;
    }                 

    @Override
    protected void onPause() {
        super.onPause();
		mAppActive = false;
    }

    @Override
    protected void onResume() {
        super.onResume();
        
        /* start the our main thread */
        startSotaThread();
		mAppActive = true;
    }
    
    /* This function creates temp directoy path */
    private void createTempDir() {
    	File folder = new File(Environment.getExternalStorageDirectory() + 
    			File.separator + "mota");
    	boolean success = true;
    	
    	if(folder.exists()) {
    		return;
    	}
    	
    	success = folder.mkdir();
    	if(success) {
				Log.i("MOTA", "Created \"Temp\" directory for MOTA!!");
    	} else {
				Log.i("MOTA", "Failed to create \"Temp\" directory for MOTA!!");
    	}
    }
  
    /* This function reads the client info json file from .apk file and store locally */
    private void readClientInfo() {
    	try {
			FileInputStream fis = openFileInput( "client_info.json" );
			if(fis.available() > 0) {
				Log.i("MOTA", "Client Info file already exists!!");
				return;
			}
    	} catch (IOException e1) {
			Log.i("MOTA", "Client Info file not found!!");
    		/* 
    		 * It is normal to get IO Exception for this file at start. The 
    		 * following section of the code will create it anyway! 
    		 */
		}
    	
    	/* if the file not found in ./files directory, then read it from .apk file */
    	try{
    		InputStream inputStream=this.getAssets().open("client_info.json");
    		int nBytes = inputStream.available();
    		byte data[] = new byte[nBytes];
    		String str = "";
    		
    		//read all data from file "hello_world.txt"
    		while( inputStream.read(data) != -1);
    		str = new String ( data );
			Log.i("MOTA", str);
			
			FileOutputStream fos = openFileOutput("client_info.json",
					MODE_PRIVATE);
			OutputStreamWriter osw = new OutputStreamWriter( fos );
			// Write the string to the file
			osw.write ( str );
			// Flush out anything in buffer
			osw.flush();
			osw.close();
	
    		inputStream.close();
    	} catch(IOException e) {
    		e.printStackTrace();
    	}
    }

    private void startSotaThread() {
		if(mSotaThreadActive == false) {
			mSotaThread = new Thread() {
				/* SOTA's Main Thread */
				public void run() { 
					int count = 0;
					
					// Moves the current Thread into the background
					android.os.Process.setThreadPriority(android.os.Process.THREAD_PRIORITY_BACKGROUND);

					while(mAppActive) {
						Log.i("MOTA", "sotaclient is running!! Count:" + count);
						
						checkForNetworks();
						Log.i("MOTA", "WiFi conn:" + mWifiConn + " Mobile conn:" + mMobileConn);
						if(mWifiConn || mMobileConn) {
							nativeSotaMain(); /* main function that will not return until nativeSotaStop() is called */
						}
						try {
							sleep(1000);
						} catch (InterruptedException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						}
						count++;
					}
					Log.i("MOTA", "sotaclient has returned!!");
				}  
			};
		
			mSotaThread.start();
			mSotaThreadActive = true;
		}
    }
    
    @SuppressWarnings("deprecation")
	void checkForNetworks() {
    	ConnectivityManager connMgr = (ConnectivityManager) 
    	        getSystemService(Context.CONNECTIVITY_SERVICE);
    	
    	NetworkInfo networkInfo = connMgr.getNetworkInfo(ConnectivityManager.TYPE_WIFI);
    	mWifiConn = networkInfo.isConnected();
    	
    	networkInfo = connMgr.getNetworkInfo(ConnectivityManager.TYPE_MOBILE);
    	mMobileConn = networkInfo.isConnected();
    }

	static {
		System.loadLibrary("sotajni");
		System.loadLibrary("jansson");
		System.loadLibrary("crypto");
		System.loadLibrary("ssl");
		System.loadLibrary("sotaclient");
	}
}
