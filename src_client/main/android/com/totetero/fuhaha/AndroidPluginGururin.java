package com.totetero.fuhaha;

import android.net.Uri;
import android.content.Intent;

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// プラグインクラス
public class AndroidPluginGururin{
	// ----------------------------------------------------------------

	// twitter投稿
	public static void jniTwitter(String post){
		Uri uri = Uri.parse("https://twitter.com/intent/tweet?text=" + post);
		Intent i = new Intent(Intent.ACTION_VIEW, uri);
		FuhahaGLView.activity.startActivity(i);
	}

	// ----------------------------------------------------------------
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

