(function(global){
	"use strict";

	// ----------------------------------------------------------------
	// ----------------------------------------------------------------
	// ----------------------------------------------------------------

	var enc = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
	var dec = [];

	// base64エンコード
	global.utilBase64encode = function(data){
		console.log("enc!!");
		var buff = [];
		var rawlen = data.length;
		var b64len = ((rawlen + 2) / 3) * 4;

		// base64エンコード
		var x = 0;
		var i = 0;
		var j = 0;
		while(i < rawlen){
			x = (x << 8) | data[i++];
			switch(i % 3){
				case 1: buff[j++] = enc[(x >> 2) & 0x3f]; break;
				case 2: buff[j++] = enc[(x >> 4) & 0x3f]; break;
				case 0: buff[j++] = enc[(x >> 6) & 0x3f]; buff[j++] = enc[x & 0x3f]; break;
			}
		}
		switch(i % 3){
			case 1: buff[j++] = enc[(x << 4) & 0x3f]; buff[j++] = enc[0x40]; buff[j++] = enc[0x40]; break;
			case 2: buff[j++] = enc[(x << 2) & 0x3f]; buff[j++] = enc[0x40]; break;
			case 0: break;
		}
		buff[j++] = '\0';

		return buff;
	};

	// base64デコード
	global.utilBase64decode = function(data){
		var buff = [];
		var b64len = data.length;
		var rawlen = Math.floor((b64len / 4) * 3);
		if(rawlen > 0 && data[b64len - 1] == enc[0x40]){rawlen -= 1;}
		if(rawlen > 0 && data[b64len - 2] == enc[0x40]){rawlen -= 1;}

		// デコードテーブル作成
		for(var i = 0; i < 0x41; i++){dec[enc[i]] = i % 0x40;}

		// base64デコード
		for(var i = 0, j = 0; i < rawlen; i++){
			switch(i % 3){
				case 0: buff[i] = ((dec[data[j + 0]] << 2) & 0xff) | ((dec[data[j + 1]] >> 4) & 0xff); break;
				case 1: buff[i] = ((dec[data[j + 1]] << 4) & 0xff) | ((dec[data[j + 2]] >> 2) & 0xff); break;
				case 2: buff[i] = ((dec[data[j + 2]] << 6) & 0xff) | ((dec[data[j + 3]] >> 0) & 0xff); j += 4; break;
			}
		}
		return buff;
	};

	// ----------------------------------------------------------------
	// ----------------------------------------------------------------
	// ----------------------------------------------------------------
})(this);
