#pragma once

#include <string>

using namespace std;

class MasterKeyPair {
public:
	MasterKeyPair() {};
	MasterKeyPair(const string& privateK, const string& publicK) : mPrivateK(privateK), mPublicK(publicK) {};
	~MasterKeyPair() {};

public:
	string getPrivateKey() const { return mPublicK; }
	string getPublicKey() const { return mPublicK; }

private:
	string mPrivateK;
	string mPublicK;
};