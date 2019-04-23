#pragma once

using namespace std;

extern "C"
{
#include "miracl.h"
#include "mirdef.h"
}

/**
* 封装大数、椭圆曲线等类的初始化和释放
* @author YUAN
*/
class BigMath {
private:
	BigMath();
	~BigMath();

public:
	static void init_big(big& var);
	static void release_big(big& var);

	static void init_zzn2(zzn2& var);
	static void release_zzn2(zzn2& var);

	static void init_zzn4(zzn4& var);
	static void release_zzn4(zzn4& var);

	static void init_ecn2(ecn2& var);
	static void release_ecn2(ecn2& var);

	static void init_epoint(epoint*& var);
	static void release_epoint(epoint* var);
};
