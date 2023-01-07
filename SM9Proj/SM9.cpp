#include "SM9.h"

#include <math.h>
#include <string>
#include "SM9_Parameters.h"
#include "Status.h"
#include "BigMath.h"
#include "Convert.h"
#include "Pairing.h"
#include "../SM9Proj/SM3/YSM3.h"

#ifdef IN_TEST
#include <fstream>
#include "../SM9Proj/utils/YHex.h"
#endif

bool SM9::isInited = false;

bool SM9::init() {
	if (!isInited) {
		bool res = ParamSM9::init();
		if (res)
			isInited = true;
		else {
			throw exception(Status::getStatusTip(SM9_INITTING_FAILED).c_str());
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
		digestSM3.update(z.c_str(), z.length());						//	Append Z
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

	Convert::gets_big(bHa, ha.c_str(), ha.length());					//	Step 5: Convert (String)Hash to Big

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

#ifdef IN_TEST
	ofstream outFile3("Signed.txt");
#endif
	
	if (!isInited) {
		throw exception(Status::getStatusTip(SM9_NOT_INITTED).c_str());
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
		throw exception(Status::getStatusTip(SM9_RATE_CAL_FAILED).c_str());
		goto END;
	}

	while (true) {
#ifdef IN_TEST
		string rHex = YHex::Decode("033C8616B06704813203DFD00965022ED15975C662337AED648835DC4B1CBE");
		Convert::gets_big(r, rHex.c_str(), rHex.length());
#else
		bigrand(ParamSM9::param_N, r);												// A2: Generate Random r
#endif
		w = g.pow(r);
		m = w.toString();															// A3: w = g ^ r

		h = H_2(message, m);														// A4: h = H_2( M||w, N )
		Convert::gets_big(h_2, h.c_str(), h.length());

		subtract(r, h_2, l);														// A5: l = ( r - h ) mod N
		divide(l, ParamSM9::param_N, tmp);											// mod N

		while (mr_compare(l, zero) < 0)
			add(l, ParamSM9::param_N, l);
		if (mr_compare(l, zero) != 0)												// Make Sure l != 0
		{
#ifdef IN_TEST
			if (outFile3.is_open())
			{
				outFile3 << "------ Signiture g For SM9 Signing Alice ------\n";
				outFile3 << YHex::Encode(g.toString()) << "\n";
				outFile3 << "------ Signiture r For SM9 Signing Alice ------\n";
				outFile3 << YHex::Encode(Convert::puts_big(r)) << "\n";
				outFile3 << "------ Signiture w For SM9 Signing Alice ------\n";
				outFile3 << YHex::Encode(m) << "\n";
			}
#endif
			break;
		}	
	}

	Convert::gets_epoint(ds_A, privateK.c_str());									// A6: S = [l]ds_A
	ecurve_mult(l, ds_A, S);
	s = Convert::puts_epoint(S);

	res = Signature(h, s);															// A7: Signature( h, s )
	
#ifdef IN_TEST
	if (outFile3.is_open())
	{
		outFile3 << "------ Signiture h For SM9 Signing Alice ------\n";
		outFile3 << YHex::Encode(h) << "\n";
		outFile3 << "------ Signiture l For SM9 Signing Alice ------\n";
		outFile3 << YHex::Encode(Convert::puts_big(l)) << "\n";
		outFile3 << "------ Signiture s For SM9 Signing Alice ------\n";
		outFile3 << YHex::Encode(Convert::puts_epoint(S)) << "\n";

		outFile3.close();
	}
#endif

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
		throw exception(Status::getStatusTip(SM9_NOT_INITTED).c_str());
	}

	mH = signature.getH();
	mS = signature.getS();

	decr(ParamSM9::param_N, 1, N_sub_1);										// B1: Check If h' Belongs to [1,N-1]
	convert(1, one);
	Convert::gets_big(h, mH.c_str(), mH.length());
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
		throw exception(Status::getStatusTip(SM9_RATE_CAL_FAILED).c_str());
		goto END;
	}

	t = g.pow(h);																// B4: t = g ^ h

	mH_1 = H_1(id, HID_SIGN);													// B5: h_1 = H_1( ID_A || hid, N )
	Convert::gets_big(h_1, mH_1.c_str(), mH_1.length());

	ecn2_copy(&ParamSM9::param_P2, &p);											// B6: P = [h_1]P_2 + P_pub-s
	ecn2_mul(h_1, &p);
	ecn2_add(&p_pubs, &p);

	if (!Pairing::calcRatePairing(u, p, S, ParamSM9::param_t,					// B7: u = e( S', P )
									ParamSM9::norm_X)) {
		numError = SM9_RATE_CAL_FAILED;
		throw exception(Status::getStatusTip(SM9_RATE_CAL_FAILED).c_str());
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

string SM9::MAC(const string& k, const string& z)
{
	YSM3 digestSM3;
	digestSM3.init();
	digestSM3.update(z.c_str(), z.length());
	digestSM3.update(k.c_str(), k.length());
	digestSM3.finish();
	return digestSM3.getData();
}

string SM9::KDF_bitlen(const string& z, int klen)
{
	return H_v(z, klen / 8);
}

string SM9::true_hv(const string& z)
{
	YSM3 digestSM3;
	digestSM3.init();
	digestSM3.update(z.c_str(), z.length());
	digestSM3.finish();
	return digestSM3.getData();
}

bool str_not_zero(const string& a)
{
	for (auto x : a) if (x) return 1;
	return 0;
}

string str_xor(const string& a, const string& b)
{
	string c = a;
	for (int i = 0; i < c.length(); i++)
		c[i] ^= b[i];
	return c;
}

string SM9::encrypt(const string& masterPublicK, const string& uid, const string& message)
{
	epoint* Q_B;
	epoint* p;
	epoint* pub;
	big h_1;
	string mH_1;
	big r;
	epoint* C_1;
	string mC_1;
	Pairing g;
	Pairing w;
	string mw;
	int mlen;
	int klen;
	string K;
	string K_1;
	string K_2;

	BigMath::init_epoint(Q_B);
	BigMath::init_epoint(p);
	BigMath::init_epoint(pub);
	BigMath::init_big(h_1);
	BigMath::init_big(r);
	BigMath::init_epoint(C_1);


	// A1: Q_B = [H_1 (ID_B || hid, N)] P_1 + P_{pub-e}
	// h_1 = H_1 (ID_B || hid, N)
	mH_1 = H_1(uid, HID_ENCRYPT);
	Convert::gets_big(h_1, mH_1.c_str(), mH_1.length());
	// p = [h_1] P_1
	ecurve_mult(h_1, ParamSM9::param_P1, p);
	// Q_B = p + pub
	Convert::gets_epoint(pub, masterPublicK.c_str());
	ecurve_add(pub, p);

	while (1) {
		// A2: generate random number r \in [1, N - 1]
		bigrand(ParamSM9::param_N, r);

		// A3: C_1 = [r] Q_B
		ecurve_mult(r, Q_B, C_1);
		mC_1 = Convert::puts_epoint(C_1);

		// A4: g = e(pub, P_2)
		Pairing::calcRatePairing(g, ParamSM9::param_P2, pub, ParamSM9::param_t, ParamSM9::norm_X);

		// A5: w = g^r
		w = g.pow(r);
		mw = w.toString();

		// A6: 
		// klen = mlen + k2_len (0x100)
		mlen = message.length();
		klen = mlen + (0x100 / 8);
		// K = KDF(C_1 || w || ID_B, klen)
		K = H_v(mC_1 + mw + uid, klen);
		// K_1 = K.substr(0, mlen) K_2 = rest of K
		K_1 = K.substr(0, mlen);
		K_2 = K.substr(mlen);


	}

}

string SM9::decrypt(string cipher, const string uid, const string mPrivateK)
{
	epoint *C_1, *C_3;
	ecn2 de_B;
	string mC_1, mC_2, mC_3;
	Pairing w;
	string mw;
	int mlen;
	int klen;
	string K;
	string K_1;
	string K_2;
	string u;
	string message;

	BigMath::init_epoint(C_1);
	BigMath::init_epoint(C_3);

	// B1: get C1
	mC_1 = cipher.substr(0, BIG_LEN);
	Convert::gets_epoint(C_1, mC_1.c_str());
	if (!ParamSM9::isPointOnG1(C_1))
		return NULL;

	// B2: w = e(C_1, de_B)
	Convert::gets_ecn2_byte128(de_B, mPrivateK.c_str());
	Pairing::calcRatePairing(w, de_B, C_1, ParamSM9::param_t, ParamSM9::norm_X);
	mw = w.toString();

	// B3: decrypt 
	mC_2 =
		cipher.substr(BIG_LEN + 0x100);	// 0x100 is the length of SM3 output
	mlen = mC_2.length();
	klen = mlen + (0x100 / 8);	
	K = H_v(mC_1, klen);
	K_1 = K.substr(0, mlen);
	if (!str_not_zero(K_1))
		return NULL;
	message = str_xor(mC_2, K_1);

	// B4: varify hash
	K_2 = K.substr(mlen);
	u = SM9::MAC(K_2, mC_2);
	mC_3 =
		cipher.substr(BIG_LEN, 0x100);	// 0x100 is the length of SM3 output
	if (!u.compare(mC_3))
		return NULL;

	return message;
}