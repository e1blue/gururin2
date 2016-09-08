package com.totetero.fuhaha;

import android.app.Activity;
import android.os.Bundle;
import android.view.Window;
import android.view.KeyEvent;
import android.widget.LinearLayout;

import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.AdSize;
import com.google.android.gms.ads.AdView;

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// アクティビティ プログラムはここから
public class FuhahaActivity extends Activity{
	private FuhahaGLView glView;
	private AdView adView;

	public void onCreate(Bundle savedInstanceState){
		super.onCreate(savedInstanceState);
		this.requestWindowFeature(Window.FEATURE_NO_TITLE);

		LinearLayout layout = new LinearLayout(this);
		layout.setLayoutParams(new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.MATCH_PARENT));
		layout.setOrientation(LinearLayout.VERTICAL);
		this.setContentView(layout);

		// ゲーム本体
		this.glView = new FuhahaGLView(this);
		this.glView.setLayoutParams(new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT, 1.0f));
		layout.addView(this.glView);

		// 広告
		this.adView = new AdView(this);
		this.adView.setAdUnitId(AndroidPluginSecret.gamePluginSecretGet("goodfox", 10010001));
		this.adView.setAdSize(AdSize.BANNER);
		AdRequest adRequest = new AdRequest.Builder()
			.addTestDevice(AndroidPluginSecret.gamePluginSecretGet("goodfox", 10020001))
			.build();
		this.adView.loadAd(adRequest);
		layout.addView(this.adView);
	}

	protected void onPause(){
		this.glView.onPause();
		this.adView.pause();
		super.onPause();
	}

	protected void onResume(){
		this.glView.onResume();
		this.adView.resume();
		super.onResume();
	}

	protected void onDestroy(){
		this.glView.onDestroy();
		this.adView.destroy();
		super.onDestroy();
	}

	public boolean dispatchKeyEvent(KeyEvent event){
		return this.glView.dispatchKeyEvent(event) || super.dispatchKeyEvent(event);
	}
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

