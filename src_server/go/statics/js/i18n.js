(function(global){
	"use strict";

	// ----------------------------------------------------------------
	// ----------------------------------------------------------------
	// ----------------------------------------------------------------

	var i18nList = {};

	// ----------------------------------------------------------------
	// ----------------------------------------------------------------
	// ----------------------------------------------------------------

	// 日本語
	i18nList["ja"] = {
		"i18n_spanId_getApp_caption":          "いますぐアプリを手に入れよう!!",
		"i18n_spanId_getApp_asterisk_free":    "※無料であそべるよ。課金要素なしだよ。",
		"i18n_spanId_getWeb_caption1":         "アプリで遊べない人はWeb体験版を遊ぼう!!",
		"i18n_spanId_getWeb_caption2":         "アプリで遊べない人はWeb版を遊ぼう!!",
		"i18n_spanId_getWeb_caption3":         "体験版は廃止してweb版も全機能解放したよ。",
		"i18n_spanId_getWeb_asterisk_reset1":  "※ゲームが上手く動かない方はデータをリセットしてください。",
		"i18n_spanId_getWeb_asterisk_reset2":  "データをリセット",
		"i18n_spanId_getWeb_asterisk_webgl":   "※ブラウザがwebGLに対応している必要があります。最新のならたぶんいける。",
		"i18n_spanId_getWeb_asterisk_appData": "※アプリへのデータの引き継ぎはできません。",
		"i18n_spanId_movie_caption":           "公式動画",
		"i18n_spanId_credit_caption":          "クレジット",
		"i18n_spanId_credit_game":             "ゲーム作成",
		"i18n_spanId_credit_movie":            "動画作成",
		"i18n_spanId_credit_sound":            "音楽効果音",
		"i18n_spanId_replay_caption":          "リプレイ データ",
		"i18n_spanId_backLink_top":            "トップへ",
	};

	// ----------------------------------------------------------------
	// ----------------------------------------------------------------
	// ----------------------------------------------------------------

	// 英語
	i18nList["en"] = {
		"i18n_spanId_getApp_caption":          "get app now!!",
		"i18n_spanId_getApp_asterisk_free":    "free to play!!",
		"i18n_spanId_getWeb_caption1":         "web trial version",
		"i18n_spanId_getWeb_caption2":         "web version",
		"i18n_spanId_getWeb_caption3":         "",
		"i18n_spanId_getWeb_asterisk_reset1":  "",
		"i18n_spanId_getWeb_asterisk_reset2":  "reset",
		"i18n_spanId_getWeb_asterisk_webgl":   "",
		"i18n_spanId_getWeb_asterisk_appData": "",
		"i18n_spanId_movie_caption":           "official movie",
		"i18n_spanId_credit_caption":          "credit",
		"i18n_spanId_credit_game":             "game creator",
		"i18n_spanId_credit_movie":            "movie creator",
		"i18n_spanId_credit_sound":            "sound creator",
		"i18n_spanId_replay_caption":          "replay data",
		"i18n_spanId_backLink_top":            "go to top",
	};

	// ----------------------------------------------------------------
	// ----------------------------------------------------------------
	// ----------------------------------------------------------------

	// 言語判定
	var langCode = (window.navigator.userLanguage || window.navigator.language || window.navigator.browserLanguage);
	if(langCode == null){langCode = "en";}
	langCode = langCode.substr(0, 2);
	if(i18nList[langCode] == null){langCode = "en";}
	global.i18n = i18nList[langCode];

	// cssルール作成
	var style = document.createElement('style');
	style.type = "text/css";
	document.head.appendChild(style);
	var sheet = style.sheet;
	if(langCode != "ja"){sheet.insertRule(".i18n_ja{display:none;}", sheet.cssRules.length);}
	if(langCode != "en"){sheet.insertRule(".i18n_en{display:none;}", sheet.cssRules.length);}

	window.addEventListener("DOMContentLoaded", function(){
		// span要素のid検索
		for(var key in global.i18n){
			if(key.substr(0, 12) == "i18n_spanId_"){
				var span = document.getElementById(key);
				if(span == null){continue;}
				span.innerHTML = global.i18n[key];
			}
		}
	}, false);

	// ----------------------------------------------------------------
	// ----------------------------------------------------------------
	// ----------------------------------------------------------------
})(this);
