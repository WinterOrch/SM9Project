#pragma once

#include <string>
using namespace std;

extern "C"
{
#include "miracl.h"
#include "mirdef.h"
}

class Pairing {
public:
	Pairing();
	Pairing(const Pairing& var);
	Pairing& operator=(const Pairing& var);
	~Pairing();

public:
	string toString();

private:
	void init();
	void release();
	string cout_zzn12_big(big& var);

	private:
	zzn4 a, b, c;
	BOOL unitary;				//	"unitary property means that fast squaring can be used, and inversions are just conjugates
	BOOL miller;				//	"miller" property means that arithmetic on this instance can ignore multiplications
								//	or divisions by constants - as instance will eventually be raised to (p-1).

public:
	Pairing conj();
	Pairing inverse();
	Pairing powq(zzn2& var);
	Pairing mul(Pairing& var);	//	this * var
	Pairing div(Pairing& var);	//	this / var
	Pairing pow(big var);		//	this ^ var
	bool isOrderQ();

public:
	static void q_power_frobenius(ecn2 A, zzn2 F);
	static Pairing line(ecn2 A, ecn2 *C, ecn2 *B, zzn2 slope, zzn2 extra, BOOL Doubling, big Qx, big Qy);
	static Pairing g(ecn2 *A, ecn2 *B, big Qx, big Qy);
	static bool fast_pairing(Pairing& ret, ecn2 P, big Qx, big Qy, big x, zzn2 X);
	static bool calcRatePairing(Pairing& ret, ecn2 P, epoint *Q, big x, zzn2 X);
};