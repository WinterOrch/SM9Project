#include "SM9.h"

#include <math.h>
#include "SM9_Parameters.h"
#include "Status.h"
#include "BigMath.h"
#include "Convert.h"
#include "Pairing.h"
#include "../SM9Proj/SM3/YSM3.h"


bool SM9::isInited = false;

bool SM9::init() {
	if (!isInited) {
		bool res = ParamSM9::init();
		if (res)
			isInited = true;
		else {
			throw exception(Status::getStatusTip(SM9_INITTING_FAILED).c_str);
		}
		return res;
	}
	else
		return true;
}

void SM9::release() noexcept {
	if (isInited) {
		ParamSM9::release();
		isInited = false;
	}
}

string SM9::H_v(const string& z, int keyLen) {
	int ct = 0x00000001;											//	Step 1: Init the Counter
	int hashLen = YSM3::HASH_SIZE; 
	int hlen = 8 * keyLen;											//	Step 2: Calculate hlen
	int v = hashLen * 8;
	int num = (hlen + (v - 1)) / v;									//	To ceil( hlen / v )
	
	int temp = hlen - v * (hlen / v);

	YSM3 digestSM3;
	unsigned char* buffer = new unsigned char[num * hashLen];

	string res;

	for (int i = 0; i < num; i++) {									//	Step 3: Execute Hash Rounds

		digestSM3.init();											//	Ha_i = Hv( Z||ct )
		digestSM3.update(z.c_str, z.length());						//	Append Z
		digestSM3.update((ct >> 24) & 0xFF);						//	Append ct
		digestSM3.update((ct >> 16) & 0xFF);
		digestSM3.update((ct >> 8) & 0xFF);
		digestSM3.update((ct >> 0) & 0xFF);
		digestSM3.finish();

		ct++;
		string hash = digestSM3.getData();

		if( (i != ( num - 1 )) || (temp == 0) )
			memcpy(buffer + i * hashLen, hash.c_str(), hashLen);	//	Fill Buffer with Hash Result
		else 
			memcpy(buffer + i * hashLen, hash.c_str(), temp / 8);	//	Step 4: Calculate Ha!_ceil(hlen/v) When hlen/v Isn't An Integer
	}

	res = string((char*)buffer, keyLen);

	delete[] buffer;
	return res;
}

string SM9::H(const string& Z) {
	big	N_Sub_1 = NULL;
	big bHa = NULL;
	big tmp = NULL;
	big h = NULL;

	string ha, res;

	int hlen = (int)ceil((5.0*logb2(ParamSM9::param_N)) / 32.0);

	BigMath::init_big(N_Sub_1);
	BigMath::init_big(bHa);
	BigMath::init_big(tmp);
	BigMath::init_big(h);

	ha = H_v(Z, hlen);												//	Step 1 - 4

	decr(ParamSM9::param_N, 1, N_Sub_1);							//	N_Sub_1 = N - 1

	Convert::gets_big(bHa, ha.c_str, ha.length());					//	Step 5: Convert (String)Hash to Big

	divide(bHa, N_Sub_1, tmp);										//	Step 6: h_1=(Ha mod (n-1))+1
	incr(bHa, 1, h);
	res = Convert::puts_big(h);
	
	BigMath::release_big(N_Sub_1);
	BigMath::release_big(bHa);
	BigMath::release_big(tmp);
	BigMath::release_big(h);

	return res;
}

string SM9::H_1(const string& id, char hid) {
	string buffer;

	buffer.append(1, 0x01);											//	0x01 || ID_A || hid
	buffer.append(id);
	buffer.append(1, hid);

	return H(buffer);
}

string SM9::H_2(const string& message, const string& w) {
	string buffer;

	buffer.append(1, 0x02);											//	0x02 || M || w
	buffer.append(message);
	buffer.append(w);

	return H(buffer);
}

Signature SM9::sign(const string& masterPublicK, const string& privateK, const string& message) {

	Signature res;
	string h, s, m;
	ecn2 p_pubs;
	epoint* ds_A = NULL;
	epoint* S = NULL;
	Pairing g;
	Pairing w;
	big h_2 = NULL;
	big r = NULL;
	big l = NULL;
	big tmp = NULL;
	big zero = NULL;
	
	if (!isInited) {
		throw exception(Status::getStatusTip(SM9_NOT_INITTED).c_str);
	}

	BigMath::init_ecn2(p_pubs);
	BigMath::init_epoint(ds_A);
	BigMath::init_epoint(S);
	BigMath::init_big(h_2);
	BigMath::init_big(r);
	BigMath::init_big(l);
	BigMath::init_big(tmp);
	BigMath::init_big(zero);

	Convert::gets_ecn2_byte128(p_pubs, masterPublicK.c_str());						// Get P_pubs
	if (!Pairing::calcRatePairing(g, p_pubs, ParamSM9::param_P1,					// A1: g = e( P_1, P_pubs )
									ParamSM9::param_t, ParamSM9::norm_X)) {
		throw exception(Status::getStatusTip(SM9_RATE_CAL_FAILED).c_str);
		goto END;
	}

	while (true) {
		bigrand(ParamSM9::param_N, r);												// A2: Generate Random r

		w = g.pow(r);
		m = w.toString();															// A3: w = g ^ r

		h = H_2(message, m);														// A4: h = H_2( M||w, N )
		Convert::gets_big(h_2, h.c_str(), h.length);

		subtract(r, h_2, l);														// A5: l = ( r - h ) mod N
		divide(l, ParamSM9::param_N, tmp);											// mod N

		while (mr_compare(l, zero) < 0)
			add(l, ParamSM9::param_N, l);
		if (mr_compare(l, zero) != 0)												// Make Sure l != 0
			break;
	}

	Convert::gets_epoint(ds_A, privateK.c_str());									// A6: S = [l]ds_A
	ecurve_mult(l, ds_A, S);
	s = Convert::puts_epoint(S);

	res = Signature(h, s);															// A7: Signature( h, s )
	

END:
	BigMath::release_ecn2(p_pubs);
	BigMath::release_epoint(ds_A);
	BigMath::release_epoint(S);
	BigMath::release_big(h_2);
	BigMath::release_big(r);
	BigMath::release_big(l);
	BigMath::release_big(tmp);
	BigMath::release_big(zero);

	return res;
}

bool SM9::verify(const string& masterPublicK, const string& id, const Signature& signature, const string& message)
{
	bool res = false;
	int numError;

	big N_sub_1 = NULL;
	big h = NULL;
	big one = NULL;
	epoint* S = NULL;
	ecn2 p_pubs;
	ecn2 p;
	Pairing g;
	Pairing t;
	Pairing u;
	Pairing w;
	big h_1 = NULL;

	string mH, mS;
	string mH_1, mH_2, m;

	BigMath::init_big(N_sub_1);
	BigMath::init_big(h);
	BigMath::init_big(one);
	BigMath::init_epoint(S);
	BigMath::init_ecn2(p_pubs);
	BigMath::init_ecn2(p);
	BigMath::init_big(h_1);

	if (!isInited) {
		throw exception(Status::getStatusTip(SM9_NOT_INITTED).c_str);
	}

	mH = signature.getH();
	mS = signature.getS();

	decr(ParamSM9::param_N, 1, N_sub_1);										// B1: Check If h' Belongs to [1,N-1]
	convert(1, one);
	Convert::gets_big(h, mH.c_str(), mH.length);
	if ((mr_compare(h, one) < 0) | (mr_compare(h, N_sub_1) > 0)) {
		numError = SM9_VERIFY_H_OUTRANGE;
		goto END;
	}

	Convert::gets_epoint(S, mS.c_str());										// B2: Check If S' Is On G1
	if (!ParamSM9::isPointOnG1(S)) {
		numError = SM9_VERIFY_S_OUTOF_G1;
		goto END;
	}

	Convert::gets_ecn2_byte128(p_pubs, masterPublicK.c_str());					// B3: g = e( P_1, P_pub-s )
	if (!Pairing::calcRatePairing(g, p_pubs, ParamSM9::param_P1,
									ParamSM9::param_t, ParamSM9::norm_X)) {
		numError = SM9_RATE_CAL_FAILED;
		throw exception(Status::getStatusTip(SM9_RATE_CAL_FAILED).c_str);
		goto END;
	}

	t = g.pow(h);																// B4: t = g ^ h

	mH_1 = H_1(id, HID_SIGN);													// B5: h_1 = H_1( ID_A || hid, N )
	Convert::gets_big(h_1, mH_1.c_str(), mH_1.length);

	ecn2_copy(&ParamSM9::param_P2, &p);											// B6: P = [h_1]P_2 + P_pub-s
	ecn2_mul(h_1, &p);
	ecn2_add(&p_pubs, &p);

	if (!Pairing::calcRatePairing(u, p, S, ParamSM9::param_t,					// B7: u = e( S', P )
									ParamSM9::norm_X)) {
		numError = SM9_RATE_CAL_FAILED;
		throw exception(Status::getStatusTip(SM9_RATE_CAL_FAILED).c_str);
		goto END;
	}

	w = u.mul(t);
	m = w.toString();

	mH_2 = H_2(message, m);
	if (mH_2.compare(mH) == 0) {
		res = true;
	} else {
		numError = SM9_VERIFY_H2_UNEQUAL_H;
	}

END:
	BigMath::release_big(N_sub_1);
	BigMath::release_big(h);
	BigMath::release_big(one);
	BigMath::release_epoint(S);
	BigMath::release_ecn2(p_pubs);
	BigMath::release_ecn2(p);
	BigMath::release_big(h_1);

	return res;
}