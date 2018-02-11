#include "../includeAll.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

static unsigned long x = 123456789;
static unsigned long y = 362436069;
static unsigned long z = 521288629;
static unsigned long w = 88675123;

// 乱数初期化
void utilRandomInit(unsigned long seed){
	w ^= seed;
}

// 乱数獲得
unsigned long utilRandomGet(){
	unsigned long t = (x ^ (x << 11));
	x = y;
	y = z;
	z = w;
	w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
	return w;
}

// 乱数種取得
void utilRandomGetSeed(unsigned long seed[]){
	seed[0] = x;
	seed[1] = y;
	seed[2] = z;
	seed[3] = w;
}

// 乱数種設定
void utilRandomSetSeed(unsigned long seed[]){
	x = seed[0];
	y = seed[1];
	z = seed[2];
	w = seed[3];
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

