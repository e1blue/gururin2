
// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------
"use strict";

mergeInto(LibraryManager.library, {
	// ----------------------------------------------------------------

	// web用体験版ロック
	platformLockedSelectIndex: function(){
		return 12;
	},

	// ----------------------------------------------------------------

	// web用編集ステージ存在確認
	platformEditedExist: function(){
		return (window.editedData != null) ? 1 : 0;
	},

	// web用編集ステージ情報取得
	platformEditedGet: function(){
		if(window.editedData == null){return null;}

		var json = JSON.stringify(window.editedData);
		var length = json.length + 1;
		var buff = Module._malloc(length);
		Module.writeStringToMemory(json, buff);
		return buff;
	},

	// ----------------------------------------------------------------

	// web用リプレイモード存在確認
	platformReplayExist: function(){
		return (window.replayParams != null) ? 1 : 0;
	},

	// web用リプレイモード情報取得
	platformReplayGetStageId: function(){
		return (window.replayParams != null) ? window.replayParams.stageId : 0;
	},

	// web用リプレイモード情報取得
	platformReplayGetPuppetId: function(){
		return (window.replayParams != null) ? window.replayParams.puppetId : 0;
	},

	// web用リプレイモード情報取得
	platformReplayGetGravity: function(seed, length){
		if(window.replayParams == null){return null;}

		Module.setValue(seed +  0, window.replayParams.seed[0], "i32");
		Module.setValue(seed +  4, window.replayParams.seed[1], "i32");
		Module.setValue(seed +  8, window.replayParams.seed[2], "i32");
		Module.setValue(seed + 12, window.replayParams.seed[3], "i32");
		Module.setValue(length, window.replayParams.gravityLength, "i32");

		var length = window.replayParams.gravityList.length + 1;
		var buff = Module._malloc(length);
		Module.writeStringToMemory(window.replayParams.gravityList, buff);
		return buff;
	},

	// ----------------------------------------------------------------

	// twitter投稿
	platformTwitter: function(post){
		Module.nativePluginUtilLoadingIncrement();

		var twitStr = Pointer_stringify(post);
		var divide = twitStr.indexOf("http");
		var twitText = twitStr.substring(0, divide).replace(/\s+$/g, "");
		var twitUrl = twitStr.substring(divide);

		// ボタン背景暗幕
		var divBack = window.parent.document.body.insertBefore((function(){
			var div = document.createElement("div");
			div.style.position = "absolute";
			div.style.left = "0px";
			div.style.right = "0px";
			div.style.top = "0px";
			div.style.bottom = "0px";
			div.style.backgroundColor = "rgba(0,0,0,0.5)";
			return div;
		})(), null);

		var divWindow = window.parent.document.body.insertBefore((function(){
			// ボタンウインドウ
			var divWindow1 = document.createElement("div");
			var divWindow1Width = 200;
			var divWindow1Height = 140;
			divWindow1.style.position = "absolute";
			divWindow1.style.width = divWindow1Width + "px";
			divWindow1.style.height = divWindow1Height + "px";
			divWindow1.style.left = "50%";
			divWindow1.style.top = "50%";
			divWindow1.style.transform = "translate(-50%, -50%)";
			divWindow1.style.borderRadius = "20px";
			divWindow1.style.backgroundColor = "white";
			divWindow1.appendChild((function(){
				var divWindow2 = document.createElement("div");
				var divWindow2Width = divWindow1Width - 20;
				var divWindow2Height = divWindow1Height - 20;
				divWindow2.style.position = "absolute";
				divWindow2.style.left = "10px";
				divWindow2.style.right = "10px";
				divWindow2.style.top = "10px";
				divWindow2.style.bottom = "10px";
				divWindow2.style.borderRadius = "15px";
				divWindow2.style.backgroundColor = "black";
				divWindow2.appendChild((function(){
					var divWindow3 = document.createElement("div");
					var divWindow3Width = divWindow2Width - 20;
					var divWindow3Height = divWindow2Height - 20;
					divWindow3.style.position = "absolute";
					divWindow3.style.left = "10px";
					divWindow3.style.right = "10px";
					divWindow3.style.top = "10px";
					divWindow3.style.bottom = "10px";
					divWindow3.style.borderRadius = "10px";
					divWindow3.style.backgroundColor = "white";
					divWindow3.appendChild((function(){
						// ツイートラベル
						var divLabel = document.createElement("div");
						var divLabelWidth = 120;
						var divLabelHeight = 30;
						divLabel.style.position = "absolute";
						divLabel.style.width = divLabelWidth + "px";
						divLabel.style.height = divLabelHeight + "px";
						divLabel.style.left = ((divWindow3Width - divLabelWidth) / 2) + "px";
						divLabel.style.top = ((divWindow3Height - divLabelHeight) / 2 - 20) + "px";
						divLabel.style.textAlign = "center";
						divLabel.innerHTML = "(・∀・)b!!";
						return divLabel;
					})());
					divWindow3.appendChild((function(){
						// ツイートボタン
						var divButton = document.createElement("div");
						var divButtonWidth = 76;
						var divButtonHeight = 28;
						divButton.style.position = "absolute";
						divButton.style.width = divButtonWidth + "px";
						divButton.style.height = divButtonHeight + "px";
						divButton.style.left = ((divWindow3Width - divButtonWidth) / 2) + "px";
						divButton.style.top = ((divWindow3Height - divButtonHeight) / 2 + 20) + "px";
						divButton.appendChild((function(){
							var a = document.createElement("a");
							a.href = "https://twitter.com/share";
							a.className = "twitter-share-button";
							a.dataset.url = twitUrl;
							a.dataset.text = twitText;
							a.dataset.size = "large";
							a.innerHTML = "Tweet";
							return a;
						})());
						return divButton;
					})());
					return divWindow3;
				})());
				return divWindow2;
			})());
			return divWindow1;
		})(), null);

		// ボタンウインドウ上のタッチ処理伝搬阻止
		divWindow.addEventListener("click", function(e){
			e.preventDefault();
			e.stopPropagation();
		});

		// ボタンウインドウを閉じる
		divBack.addEventListener("click", function(e){
			divBack.parentNode.removeChild(divBack);
			divWindow.parentNode.removeChild(divWindow);
			Module.nativePluginUtilLoadingDecrement();
			e.preventDefault();
			e.stopPropagation();
		});

		// twitterのwidgets.jsを挿入
		if(!window.parent.document.getElementById("twitter-wjs")){
			window.parent.document.body.insertBefore((function(){
				var protocol = /^http:/.test(window.parent.document.location)? "http" : "https";
				var script = window.parent.document.createElement("script");
				script.src = protocol + "://platform.twitter.com/widgets.js";
				script.id = "twitter-wjs";
				return script;
			})(), null);
		}else{
			window.parent.twttr.widgets.load();
		}
	},

	// ----------------------------------------------------------------
});

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

