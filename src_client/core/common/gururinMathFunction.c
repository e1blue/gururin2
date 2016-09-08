#include "../library.h"
#include "../includeAll.h"
#include <math.h>

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

//double utilMathSqrt(double x){return sqrt(x);}
//double utilMathCos(double x){return cos(x);}
//double utilMathSin(double x){return sin(x);}
//double utilMathAcos(double x){return acos(x);}
//double utilMathAsin(double x){return asin(x);}
//double utilMathAtan2(double y, double x){return atan2(y, x);}
double utilMathAbs(double x){return fabs(x);}
double utilMathCeil(double x){return ceil(x);}
double utilMathRound(double x){return round(x);}
double utilMathFloor(double x){return floor(x);}

// 平方根を求める
double utilMathSqrt(double x){
	if(x < 0){return NAN;}
	if(engineMathAbs(x) <= 0){return 0;}
	// ニュートン法で求める
	double t = x * 0.5;
	for(int i = 1; i < 20; i++){
		t = (t + x / t) * 0.5;
	}
	return t;
}

// 三角関数を求める
double utilMathSin(double x){
	x -= (int)(x / (2 * CONST_PI)) * (2 * CONST_PI);
	// テイラー展開を行う
	double sum = x;
	double t = x;
	for(int i = 1; i < 10; i++){
		t *= - (x * x) / ((2 * i + 1) * (2 * i));
		sum += t;
	}
	return sum;
}
double utilMathCos(double x){return utilMathSin(CONST_PI * 0.5 - x);}

// 逆三角関数を求める
static double utilMathAtan1(double x){
	if(engineMathAbs(x) <= 0){return 0;}
	if(isnan(x)){return x;}
	if(isinf(x)){return (x > 0 ? 0.5 : -0.5) * CONST_PI;}
	if(x >  1){return  CONST_PI * 0.5 - utilMathAtan1(1 / x);}
	if(x < -1){return -CONST_PI * 0.5 - utilMathAtan1(1 / x);}
	double a = 0;
	for(int i = 24; i >= 1; i--){
		a = (i * i * x * x) / (2 * i + 1 + a);
	}
	return x / (1 + a);
}
double utilMathAsin(double x){
	if(x >  1){return  CONST_PI * 0.5;}
	if(x < -1){return -CONST_PI * 0.5;}
	return utilMathAtan1(x / utilMathSqrt(1 - x * x));
}
double utilMathAcos(double x){
	if(x >  1){return 0;}
	if(x < -1){return CONST_PI;}
	if(engineMathAbs(x) <= 0){return CONST_PI * 0.5;}
	return utilMathAtan1(utilMathSqrt(1 - x * x) / x);
}
double utilMathAtan2(double y, double x){
	bool isInfy = isinf(y);
	bool isInfx = isinf(x);
	if(isInfy && isInfx){return (y > 0 ? (x > 0 ? 0.25 : 0.75) : (x < 0 ? -0.75 : -0.25)) * CONST_PI;}
	if(isInfy){return (y > 0 ? 0.5 : -0.5) * CONST_PI;}
	if(isInfx){return (x > 0 ? 0.0 :  1.0) * CONST_PI;}
	if(x > 0){return utilMathAtan1(y / x);}
	if(x < 0){return utilMathAtan1(y / x) + CONST_PI;}
	return (y >= 0 ? 0.5 : -0.5) * CONST_PI;
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

