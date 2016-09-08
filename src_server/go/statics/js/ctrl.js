(function(global){
	"use strict";

	// ----------------------------------------------------------------
	// ----------------------------------------------------------------
	// ----------------------------------------------------------------

	// コントローラ
	global.ctrl = (function(){
		var ctrl = {};
		ctrl.mx = 0;
		ctrl.my = 0;
		ctrl.mdn = false;
		ctrl.mmv = false;

		var mx0 = ctrl.mx;
		var my0 = ctrl.my;
		var isTouch = ('ontouchstart' in window);

		var mdnEvent = function(e){
			var rect = e.target.getBoundingClientRect();
			ctrl.mx = (isTouch ? e.changedTouches[0].clientX : e.clientX) - rect.left;
			ctrl.my = (isTouch ? e.changedTouches[0].clientY : e.clientY) - rect.top;
			ctrl.mdn = true;
			ctrl.mmv = false;
			mx0 = ctrl.mx;
			my0 = ctrl.my;
			e.preventDefault();
		};

		var mmvEvent = function(e){
			var rect = e.target.getBoundingClientRect();
			ctrl.mx = (isTouch ? e.changedTouches[0].clientX : e.clientX) - rect.left;
			ctrl.my = (isTouch ? e.changedTouches[0].clientY : e.clientY) - rect.top;
			if(ctrl.mdn && !ctrl.mmv){
				var x = ctrl.mx - mx0;
				var y = ctrl.my - my0;
				ctrl.mmv = x * x + y * y > 5 * 5;
			}
			e.preventDefault();
		};

		var mupEvent = function(e){
			var rect = e.target.getBoundingClientRect();
			ctrl.mx = (isTouch ? e.changedTouches[0].clientX : e.clientX) - rect.left;
			ctrl.my = (isTouch ? e.changedTouches[0].clientY : e.clientY) - rect.top;
			ctrl.mdn = false;
			e.preventDefault();
		};

		ctrl.init = function(element){
			if(isTouch){
				element.addEventListener("touchstart", mdnEvent, true);
				element.addEventListener("touchmove", mmvEvent, true);
				element.addEventListener("touchend", mupEvent, true);
			}else{
				element.addEventListener("mousedown", mdnEvent, true);
				element.addEventListener("mousemove", mmvEvent, true);
				element.addEventListener("mouseup", mupEvent, true);
				element.addEventListener("mouseout", mupEvent, true);
			}
		};

		return ctrl;
	})();

	// ----------------------------------------------------------------
	// ----------------------------------------------------------------
	// ----------------------------------------------------------------
})(this);
