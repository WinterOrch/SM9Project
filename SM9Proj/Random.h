#pragma once

using namespace std;

/**
* 目前使用默认随机数产生器，仍不确定是否符合标准要求
* @author YUAN
*/
class Random {
private:
	Random();
	~Random();

public:
	static bool init();
};