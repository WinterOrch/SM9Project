#pragma once

#include <string>
using namespace std;

#define MR_LITTLE_ENDIAN
#define MIRACL 64
#define mr_utype __int64
#define MR_IBITS 32
#define MR_LBITS 32
#define mr_unsign32 unsigned int
#define mr_unsign64 unsigned __int64
#define MR_FLASH 52
#define MAXBASE ((mr_small)1<<(MIRACL-1))
#define BITSINCHAR 8
#define BN_LEN 32

extern "C"
{
#include "miracl.h"
#include "mirdef.h"
}

#define BIG_LEN 2000

/**
* 椭圆曲线、大数等数据类型与string的互相转化
* @author YUAN
*/
class Convert
{
public:
	Convert();
	~Convert();

public:
	static string puts_big(big& var);
	static void gets_big(big& var, const unsigned char* buf, int length);
	static void gets_big(big& var, const char* buf, int length) { 
		gets_big(var, (const unsigned char*)buf, length); }
	
	static string puts_ecn2(ecn2& var);
	static bool gets_ecn2_byte128(ecn2& var, const char* buf);

	static string puts_epoint(epoint* var);
	static void gets_epoint(epoint* var, const char* buf);

private:
	static string puts_ecn2_big(big& var);

};

