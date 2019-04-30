#include "SM9_KGC.h"

#include "Status.h"
#include "BigMath.h"
#include "Convert.h"
#include "SM9_Parameters.h"
#include "../SM9Proj/SM3/YSM3.h"

#include <math.h>

MasterKeyPair SM9_KGC::genSignMasterKeyPair() {
	big ke = NULL;
	ecn2 p_pub;

	string privateK, publicK;

	if (!SM9::isInited) {
		throw exception(Status::getStatusTip(SM9_NOT_INITTED).c_str());
	}

	BigMath::init_big(ke);
	BigMath::init_ecn2(p_pub);

	bigrand(ParamSM9::param_N, ke);

	ecn2_copy(&ParamSM9::param_P2, &p_pub);
	ecn2_mul(ke, &p_pub);

	privateK = Convert::puts_big(ke);
	publicK = Convert::puts_ecn2(p_pub);

	BigMath::release_big(ke);
	BigMath::release_ecn2(p_pub);

	return MasterKeyPair(privateK, publicK);
}

MasterKeyPair SM9_KGC::genSignMasterKeyPairFromPri(const string& masterPrivateK) {
	big ke = NULL;
	ecn2 p_pub;

	string privateK, publicK;

	if (!SM9::isInited) {
		throw exception(Status::getStatusTip(SM9_NOT_INITTED).c_str());
	}

	BigMath::init_big(ke);
	BigMath::init_ecn2(p_pub);

	Convert::gets_big(ke, masterPrivateK.c_str(), masterPrivateK.length());

	ecn2_copy(&ParamSM9::param_P2, &p_pub);
	ecn2_mul(ke, &p_pub);

	privateK = Convert::puts_big(ke);
	publicK = Convert::puts_ecn2(p_pub);

	BigMath::release_big(ke);
	BigMath::release_ecn2(p_pub);

	return MasterKeyPair(privateK, publicK);
}

std::string SM9_KGC::genSignPrivateKey(const string& masterPrivateK, const string& id) {
	big t_2 = NULL;
	epoint *dsa = NULL;
	string res, st_2;

	BigMath::init_big(t_2);
	BigMath::init_epoint(dsa);

	st_2 = calcT2(masterPrivateK, id, HID_SIGN);

	if (st_2.empty())
		goto END;
	Convert::gets_big(t_2, st_2.c_str(), st_2.length());

	ecurve_mult(t_2, ParamSM9::param_P1, dsa);
	res = Convert::puts_epoint(dsa);

END:
	BigMath::release_big(t_2);
	BigMath::release_epoint(dsa);

	return res;
}

bool SM9_KGC::calcT1(big& t_1, big& ke, const string& id, int hid) {
	big h_1 = NULL;
	big zero = NULL;

	string hashH_1;

	hashH_1 = H_1(id, hid);											// H_1( ID_B || hid, N)
	
	BigMath::init_big(h_1);
	BigMath::init_big(zero);

	Convert::gets_big(h_1, hashH_1.c_str(), hashH_1.length());

	add(h_1, ke, t_1);												// t_1 = H_1( ID_B || hid, N) + ke

	BigMath::release_big(h_1);
	BigMath::release_big(zero);

	if (mr_compare(t_1, zero) == 0)
		return false;
	else
		return true;
}

std::string SM9_KGC::calcT2(const string& masterPrivateK, const string& id, int hid) {
	bool t_1_equal_zero = false;
	int status;

	big ke = NULL;
	big t_1 = NULL;
	big t_2 = NULL;
	big tmp = NULL;

	string res;

	BigMath::init_big(ke);
	BigMath::init_big(t_1);
	BigMath::init_big(t_2);
	BigMath::init_big(tmp);
	
	Convert::gets_big(ke, masterPrivateK.c_str(), masterPrivateK.length());

	if ( !calcT1(t_1, ke, id, hid) ) {
		t_1_equal_zero = true;
		status = SM9_KGC_T1_EQUAL_ZERO;
		goto END;
	}

	xgcd(t_1, ParamSM9::param_N, t_1, t_1, t_1);		//	t_1 ^(-1)
	multiply(ke, t_1, t_2);								//	t_2 = ke * t_1 ^(-1)
	divide(t_2, ParamSM9::param_N, tmp);				//	mod N
	res = Convert::puts_big(t_2);

END:
	BigMath::release_big(ke);
	BigMath::release_big(t_1);
	BigMath::release_big(t_2);
	BigMath::release_big(tmp);

	if (t_1_equal_zero) {
		throw exception(Status::getStatusTip(status).c_str());
	}

	return res;
}