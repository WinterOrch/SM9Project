#pragma once

#include "SM9.h"
#include "MasterKeyPair.h"

#include <string>

using namespace std;

class SM9_KGC : public SM9 {
public:
	SM9_KGC() {};
	~SM9_KGC() {};

public:
	static MasterKeyPair genSignMasterKeyPair();
	static string genSignPrivateKey(const string& masterPrivateK, const string& id);

private:
	/**
	* 在G1上生成私钥时计算t_1。
	*
	* @param var		t_1
	* @param ke			私钥
	* @param id			用户标识
	* @param hid		私钥生成函数识别符
	*
	* @return			true : 生成t_1不为0； false : 生成t_1为0。
	* @note		如果t1为0，则KGC需要重新产生签名或加密主密钥对，并更新已用用户的签名或加密私钥。
	*/
	static bool calcT1(big& var, big& ke, const string& id, int hid);

	/**
	* 在G1上生成私钥时计算t_2。
	*
	* @param masterPrivateK	签名主私钥
	* @param id				用户标识
	* @param hid			私钥生成函数识别符
	*
	* @return				t_1不为0时返回t_2，t_1为0时返回空。
	* @note		如果t1为0，则KGC需要重新产生签名或加密主密钥对，并更新已用用户的签名或加密私钥。
	*/
	static string calcT2(const string& masterPrivateK, const string& id, int hid);
};