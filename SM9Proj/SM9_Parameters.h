#pragma once

#include <string>
using namespace std;

#define BIG_LEN 2000

extern "C"
{
#include "miracl.h"
#include "mirdef.h"
}

/**
* SM9参数及初始化、释放
* @author YUAN
*/
class ParamSM9 {

private:
	ParamSM9();
	~ParamSM9();

public:
	/**
	* 椭圆曲线子群上点的验证
	* 参考算法文档第一部分：总则中3.5
	* @param var 需要验证的点
	*/
	static bool isPointOnG1(epoint* var);

public:
	static big param_a;
	static big param_b;
	static big param_N;
	static big param_q;
	static big param_t;
	
	static epoint* param_P1;
	static ecn2 param_P2;
	static zzn2 norm_X;
	static miracl* mMip;

public:
	// 初始化及释放函数
	static bool init();
	static void release();
	
};