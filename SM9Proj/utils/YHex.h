
#ifndef YY_UTILS_YHEX_INCLUDE_H__
#define YY_UTILS_YHEX_INCLUDE_H__

/** \file YHex.h
* \brief C++-十六进制格式字符串转换.
* 将字符串转换为十六进制时，结果为小写；逆向则可以是大写或小写.
* 若函数的形参中有长度参数，则对于该长度是否大于传入字符串的实际长度是不作判断的，需调用者自己保证.
*/

#include <string>
using namespace std;

/**
* HEX CODEC.
*/
class YHex
{
public:
	/**
	* HEX encoding.
	*/
	static string Encode(const string& data, bool isUpperCase);

	/**
	* HEX encoding(LowerCase).
	*/
	static string Encode(const string& data) {
		return Encode(data, false);
	}

	/**
	* HEX decoding.
	*/
	static string Decode(const string& data);

	/**
	* HEX data format check.
	*
	* A valid character is in ["0123456789abcdef"], case-insensitive.
	*/
	static bool Check(const string& data);

private:
	static void Bin2Hex(bool isUpperCase, const char *bin_string, char *hex_string, int binLength);
	static void Hex2Bin(const char *hex_string, char* bin_string, int hexLength);

	/**
	* Hex Format check.
	* A valid hex character is in "0123456789ABCDEFabcdef".
	* @return 0 if all characters is valid; or the position of first invalid character, 1 represent the first character;
	* if nSrcLength<=0, or pSrc=nullptr, return 0.
	*/
	static int Check(const unsigned char* pSrc, int nSrcLength);

private:
	YHex() = delete;
	~YHex() = delete;
	YHex(const YHex&) = delete;
	YHex& operator=(const YHex&) = delete;

};
#endif