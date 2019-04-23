#pragma once

#include <string>

using namespace std;

/**
* 数字签名的封装类
* @author Frankel.Y
*/
class Signature {
public:
	Signature() {
	
	}

	Signature(const string& h, const string& s) {
		mH = h;
		mS = s;
	}

	~Signature() {
	
	}

public:
	string getH() const {
		return mH;
	}

	string getS() const {
		return mS;
	}

private:
	string mH;
	string mS;
};