#pragma once
#include <string>

#include "Signature.h"



using namespace std;

#define HID_SIGN			0x01
#define HID_KEYEXCHANGE		0x02
#define HID_ENCRYPT			0x03

/**
* SM9算法实现
* @author YUAN
*/
class SM9 {
protected:
	SM9();
	~SM9();

public:
	static bool init();
	static void release() noexcept;
	

public :
	/**
	* 签名
	*
	* @param masterPublicK	主公钥
	* @param privateK		用户签名私钥
	* @param message		待签消息
	* @return				签名类实例
	*/
	static Signature sign(const string& masterPublicK, const string& privateK, const string& message);

	/**
	* 验证
	*
	* @param masterPublicK	主公钥
	* @param id				用户标识
	* @param message		待验消息
	* @return				true or false
	*/
	static bool verify(const string& masterPublicK, const string& id, const Signature& signature, const string& message);

private:
	/**
	* H_1和H_2的共有部分，通过调整输入实现签名和验证过程中不同的密码函数。
	* @return 字符串格式的大数
	*/
	static string H(const string&z);

	/**
	* 通过SM3散列执行 8 * ceil( keyLen ) 轮散列并处理最后一组。
	* @return Ha_1 || Ha_2 ... || Ha_ceil(hlen/v) || Ha!_ceil(hlen/v)
	*/
	static string H_v(const string& z, int keyLen);

	// add by qinglkyi
	static string MAC(const string& k, const string& z);
	static string KDF_bitlen(const string& z, int keyLen);
	static string true_hv(const string& z);
	static string encrypt(const string& masterPublicK, const string& privateK, const string& message);

protected:
	/**
	* 密码函数H_1()，以用户标识、签名私钥生成函数识别符和哈希头为输入，用于验证签名。
	*/
	static string H_1(const string& id, char hid);

	/**
	* 密码函数H_2()，以消息、G_T中的元素和哈希头为输入，用于生成签名。
	*/
	static string H_2(const string& message, const string& w);

public:
	static bool isInited;
};