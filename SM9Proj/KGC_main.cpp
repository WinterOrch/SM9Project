#include "KGC_main.h"

#include "SM9.h"
#include "SM9_KGC.h"
#include "SM9_Parameters.h"
#include "MasterKeyPair.h"
#include "Signature.h"

#include "../SM9Proj/utils/YHex.h"

#include <string>
#include <map>
#include <vector>
#include <set>
#include <algorithm>

#include <cstdio>
#include <malloc.h>
#include <cstdlib>
#include <ctime>

using namespace std;

char KGC_main::SAVE_FILE[] = "state.dat";
int KGC_main::MAGIC_NUMBER = 0xAF02022;

set<string> KGC_main::Users;
string KGC_main::current_uid;
MasterKeyPair KGC_main::mainKey;
int KGC_main::magic_tag;

KGC_main::KGC_main()
{
}

KGC_main::~KGC_main()
{
}

void KGC_main::initState()
{
	Users.clear();
	mainKey = SM9_KGC::genSignMasterKeyPair();
	magic_tag = 1;
	current_uid.assign("root@localhost");
	Users.insert(current_uid);
}

bool KGC_main::loadState()
{
	int t;
	FILE* fp;
	int n, len, mxlen;
	char* buf;
	string tmp;

	fopen_s(&fp, SAVE_FILE, "r");
	if (fp == NULL) return false; // 打开文件失败

	fread(&len, sizeof(int), 1, fp);
	if (len != MAGIC_NUMBER) return fclose(fp), false; // 文件可能被破坏了

	// 获取 tag
	fread(&magic_tag, sizeof(int), 1, fp);

	// 获取 mxlen
	fread(&mxlen, sizeof(int), 1, fp);
	mxlen++;
	if (mxlen > 100000)
	{
		// puts("Error when loadState: too large!");
		fclose(fp);
		return false;
	}
	buf = (char*)malloc(mxlen);
	if (buf == NULL) return fclose(fp), false; // failed malloc

	// 获取主私钥
	fread(&len, sizeof(int), 1, fp);
	if (len > mxlen) return fclose(fp), false; // sth broken
	fread(buf, sizeof(char), len, fp);
	tmp.assign(buf, buf + len);
	// 生成主密钥对
	mainKey = SM9_KGC::genSignMasterKeyPairFromPri(tmp); 

	// 获取人数
	fread(&n, sizeof(int), 1, fp);
	while (n--) 
	{
		// 加入 uid
		fread(&len, sizeof(int), 1, fp);
		if (len > mxlen) return fclose(fp), false; // sth broken
		fread(buf, sizeof(char), len, fp);
		tmp.assign(buf, buf + len);
		Users.insert(tmp);
	}

	// 获取当前用户
	fread(&len, sizeof(int), 1, fp);
	if (len > mxlen) return fclose(fp), false; // sth broken
	fread(buf, sizeof(char), len, fp);
	current_uid.assign(buf, buf + len);

	fread(&len, sizeof(int), 1, fp);
	if (len != MAGIC_NUMBER) return fclose(fp), false; // 文件可能被破坏了

	free(buf);
	fclose(fp);
	return true;
}

bool KGC_main::saveState()
{
	FILE* fp;
	int len, mxlen;
	string prik;

	fopen_s(&fp, SAVE_FILE, "wb");
	if (fp == NULL) return false; // 创建文件失败

	fwrite(&MAGIC_NUMBER, sizeof(int), 1, fp);
	
	prik = mainKey.getPrivateKey();

	// 保存 tag
	fwrite(&magic_tag, sizeof(int), 1, fp);

	// 保存 mxlen
	mxlen = 0;
	mxlen = max(mxlen, (int)prik.length());
	for (auto uid : Users) mxlen = max(mxlen, (int)uid.length());
	fwrite(&mxlen, sizeof(int), 1, fp);

	// 保存 ke
	len = prik.length();
	fwrite(&len, sizeof(int), 1, fp);
	fwrite(prik.c_str(), sizeof(char), len, fp);
	
	// 保存用户
	len = Users.size();
	fwrite(&len, sizeof(int), 1, fp);
	for (auto uid : Users) 
	{
		len = uid.length();
		fwrite(&len, sizeof(int), 1, fp);
		fwrite(uid.c_str(), sizeof(char), len, fp);
	}

	// 保存当前用户
	len = current_uid.size();
	fwrite(&len, sizeof(int), 1, fp);
	fwrite(current_uid.c_str(), sizeof(char), len, fp);

	fwrite(&MAGIC_NUMBER, sizeof(int), 1, fp);
	
	fclose(fp);
	return true;
}

bool KGC_main::checkKeys()
{
	string tmp;
	for (auto uid : Users)
	{
		tmp = SM9_KGC::genSignPrivateKey(mainKey.getPrivateKey(), uid);
		if (tmp.empty()) return false;
	}
	return true;
}

bool KGC_main::reloadKeys()
{
	while (!checkKeys())
	{
		magic_tag++;
		mainKey = SM9_KGC::genSignMasterKeyPair();
	}
	return true;
}

void KGC_main::KGC_Boot()
{
	// 初始化 SM9 标准参数
	SM9::init();
	// 加载上次的状态
	if (!loadState()) initState();
	return;
}

bool KGC_main::createUser(const string& uid)
{
	// 用户创建
	if (Users.find(uid) != Users.end()) return false; // exist
	Users.insert(uid);
	return true;
}

bool KGC_main::switchUser(const string& uid)
{
	// 用户切换
	if (Users.find(uid) == Users.end()) return false; // not found
	current_uid = uid;
	return true;
}

string KGC_main::getPriKey()
{
	string uid, prik;
	uid = current_uid;
	prik = SM9_KGC::genSignPrivateKey(mainKey.getPrivateKey(), uid);
	return prik;
}

string KGC_main::getPriKey(const string& uid) // use for debug
{
	string prik;
	prik = SM9_KGC::genSignPrivateKey(mainKey.getPrivateKey(), uid);
	if (prik.empty())
	{
		puts("Needs Reload!");
		// reloadKeys();
		// prik = SM9_KGC::genSignPrivateKey(mainKey.getPrivateKey(), uid);
	}
	return prik;
}

Signature KGC_main::sign(const string& msg)
{
	Signature sig;
	sig = SM9::sign(mainKey.getPublicKey(), getPriKey(), msg);
	return sig;
}

bool KGC_main::verify(const string& uid, Signature sig, const string& msg)
{
	return SM9::verify(mainKey.getPublicKey(), uid, sig, msg);
}