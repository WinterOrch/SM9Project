#pragma once

#include <string>
#include <map>
#include <vector>
#include <set>

#include <cstdio>

#include "MasterKeyPair.h"
#include "Signature.h"

using namespace std;

class KGC_main {
public:
	KGC_main();
	~KGC_main();
public:
	static void KGC_Boot(); // 启动 KGC，执行初始化和例行检查
	static bool switchUser(const string& uid); // 切换当前用户
	static bool createUser(const string& uid); // 创建新用户
	static bool loadState();
	static bool saveState();

	static Signature sign(const string& msg); // 当前用户对 msg 签名
	static bool verify(const string &uid, Signature sig, const string &msg); // 验证 uid 的签名


private:
	static void initState();
	static bool reloadKeys();
	static bool checkKeys();
	static string getPriKey();
	static string getPriKey(const string& uid);

public:
	static set<string> Users;
	static string current_uid;
	static MasterKeyPair mainKey;
	static int magic_tag;  // 用来标定 KGC 的版本

private:
	static char SAVE_FILE[];
	static int MAGIC_NUMBER;
};