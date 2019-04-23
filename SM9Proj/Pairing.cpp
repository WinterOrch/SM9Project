#include "Pairing.h"
#include "SM9_Parameters.h"
#include "BigMath.h"


Pairing::Pairing()
{
	init();
}

Pairing::Pairing(const Pairing& var)
{
	init();
	zzn4_copy((zzn4*)&var.a, &a);
	zzn4_copy((zzn4*)&var.b, &b);
	zzn4_copy((zzn4*)&var.c, &c);
	miller = var.miller;
	unitary = var.unitary;
}

Pairing& Pairing::operator=(const Pairing& var)
{
	zzn4_copy((zzn4*)&var.a, &a);
	zzn4_copy((zzn4*)&var.b, &b);
	zzn4_copy((zzn4*)&var.c, &c);
	miller = var.miller;
	unitary = var.unitary;
	return *this;
}

Pairing::~Pairing()
{
	release();
}


void Pairing::init()
{
	a.a.a = mirvar(0); a.a.b = mirvar(0);
	a.b.a = mirvar(0); a.b.b = mirvar(0); a.unitary = FALSE;
	b.a.a = mirvar(0); b.a.b = mirvar(0);
	b.b.a = mirvar(0); b.b.b = mirvar(0); b.unitary = FALSE;
	c.a.a = mirvar(0); c.a.b = mirvar(0);
	c.b.a = mirvar(0); c.b.b = mirvar(0); c.unitary = FALSE;
	unitary = FALSE; miller = FALSE;
}

void Pairing::release()
{
	mirkill(a.a.a); mirkill(a.a.b);
	mirkill(a.b.a); mirkill(a.b.b);
	mirkill(b.a.a); mirkill(b.a.b);
	mirkill(b.b.a); mirkill(b.b.b);
	mirkill(c.a.a); mirkill(c.a.b);
	mirkill(c.b.a); mirkill(c.b.b);
}

std::string Pairing::toString()
{
	string result;
	result.append(cout_zzn12_big(c.b.b));
	result.append(cout_zzn12_big(c.b.a));
	result.append(cout_zzn12_big(c.a.b));
	result.append(cout_zzn12_big(c.a.a));
	result.append(cout_zzn12_big(b.b.b));
	result.append(cout_zzn12_big(b.b.a));
	result.append(cout_zzn12_big(b.a.b));
	result.append(cout_zzn12_big(b.a.a));
	result.append(cout_zzn12_big(a.b.b));
	result.append(cout_zzn12_big(a.b.a));
	result.append(cout_zzn12_big(a.a.b));
	result.append(cout_zzn12_big(a.a.a));
	return result;
}

string Pairing::cout_zzn12_big(big& var)
{
	big tmp = NULL;
	tmp = mirvar(0);
	redc(var, tmp);

	int length = tmp->len * sizeof(tmp->w);
	char *buffer = new char[length];
	int ret = big_to_bytes(length, tmp, buffer, TRUE);
	string result(buffer, ret);

	delete[] buffer;
	mirkill(tmp);
	return result;
}

Pairing Pairing::mul(Pairing& var)
{
	// Karatsuba
	zzn4 Z0, Z1, Z2, Z3, T0, T1;
	Pairing ret(*this);
	BOOL zero_c, zero_b;

	BigMath::init_zzn4(Z0);
	BigMath::init_zzn4(Z1);
	BigMath::init_zzn4(Z2);
	BigMath::init_zzn4(Z3);
	BigMath::init_zzn4(T0);
	BigMath::init_zzn4(T1);

	if (zzn4_compare(&a, &var.a) && zzn4_compare(&a, &var.a) && zzn4_compare(&a, &var.a))
	{
		if (unitary == TRUE)
		{
			zzn4_copy(&a, &Z0); zzn4_mul(&a, &a, &ret.a); zzn4_copy(&ret.a, &Z3); zzn4_add(&ret.a, &ret.a, &ret.a);
			zzn4_add(&ret.a, &Z3, &ret.a); zzn4_conj(&Z0, &Z0); zzn4_add(&Z0, &Z0, &Z0); zzn4_sub(&ret.a, &Z0, &ret.a);
			zzn4_copy(&c, &Z1); zzn4_mul(&Z1, &Z1, &Z1); zzn4_tx(&Z1);
			zzn4_copy(&Z1, &Z3); zzn4_add(&Z1, &Z1, &Z1); zzn4_add(&Z1, &Z3, &Z1);
			zzn4_copy(&b, &Z2); zzn4_mul(&Z2, &Z2, &Z2);
			zzn4_copy(&Z2, &Z3); zzn4_add(&Z2, &Z2, &Z2); zzn4_add(&Z2, &Z3, &Z2);
			zzn4_conj(&b, &ret.b); zzn4_add(&ret.b, &ret.b, &ret.b);
			zzn4_conj(&c, &ret.c); zzn4_add(&ret.c, &ret.c, &ret.c); zzn4_negate(&ret.c, &ret.c);
			zzn4_add(&ret.b, &Z1, &ret.b); zzn4_add(&ret.c, &Z2, &ret.c);
		}
		else
		{
			if (!miller)
			{// Chung-Hasan SQR2
				zzn4_copy(&a, &Z0); zzn4_mul(&Z0, &Z0, &Z0);
				zzn4_mul(&b, &c, &Z1); zzn4_add(&Z1, &Z1, &Z1);
				zzn4_copy(&c, &Z2); zzn4_mul(&Z2, &Z2, &Z2);
				zzn4_mul(&a, &b, &Z3); zzn4_add(&Z3, &Z3, &Z3);
				zzn4_add(&a, &b, &ret.c); zzn4_add(&ret.c, &c, &ret.c); zzn4_mul(&ret.c, &ret.c, &ret.c);
				zzn4_tx(&Z1); zzn4_add(&Z0, &Z1, &ret.a);
				zzn4_tx(&Z2); zzn4_add(&Z3, &Z2, &ret.b);
				zzn4_add(&Z0, &Z1, &T0); zzn4_add(&T0, &Z2, &T0);
				zzn4_add(&T0, &Z3, &T0); zzn4_sub(&ret.c, &T0, &ret.c);
			}
			else
			{// Chung-Hasan SQR3 - actually calculate 2x^2 !
			 // Slightly dangerous - but works as will be raised to p^{k/2}-1
			 // which wipes out the 2.
				zzn4_copy(&a, &Z0); zzn4_mul(&Z0, &Z0, &Z0);// a0^2 = S0
				zzn4_copy(&c, &Z2); zzn4_mul(&Z2, &b, &Z2); zzn4_add(&Z2, &Z2, &Z2); // 2a1.a2 = S3
				zzn4_copy(&c, &Z3); zzn4_mul(&Z3, &Z3, &Z3);; // a2^2 = S4
				zzn4_add(&c, &a, &ret.c); // a0+a2
				zzn4_copy(&b, &Z1); zzn4_add(&Z1, &ret.c, &Z1); zzn4_mul(&Z1, &Z1, &Z1);// (a0+a1+a2)^2 =S1
				zzn4_sub(&ret.c, &b, &ret.c); zzn4_mul(&ret.c, &ret.c, &ret.c);// (a0-a1+a2)^2 =S2
				zzn4_add(&Z2, &Z2, &Z2); zzn4_add(&Z0, &Z0, &Z0); zzn4_add(&Z3, &Z3, &Z3);
				zzn4_sub(&Z1, &ret.c, &T0); zzn4_sub(&T0, &Z2, &T0);
				zzn4_sub(&Z1, &Z0, &T1); zzn4_sub(&T1, &Z3, &T1); zzn4_add(&ret.c, &T1, &ret.c);
				zzn4_tx(&Z3); zzn4_add(&T0, &Z3, &ret.b);
				zzn4_tx(&Z2); zzn4_add(&Z0, &Z2, &ret.a);
			}
		}
	}
	else
	{
		// Karatsuba
		zero_b = zzn4_iszero(&var.b);
		zero_c = zzn4_iszero(&var.c);
		zzn4_mul(&a, &var.a, &Z0); //9
		if (!zero_b) zzn4_mul(&b, &var.b, &Z2); //+6
		zzn4_add(&a, &b, &T0);
		zzn4_add(&var.a, &var.b, &T1);
		zzn4_mul(&T0, &T1, &Z1); //+9
		zzn4_sub(&Z1, &Z0, &Z1);
		if (!zero_b) zzn4_sub(&Z1, &Z2, &Z1);
		zzn4_add(&b, &c, &T0);
		zzn4_add(&var.b, &var.c, &T1);
		zzn4_mul(&T0, &T1, &Z3);//+6
		if (!zero_b) zzn4_sub(&Z3, &Z2, &Z3);
		zzn4_add(&a, &c, &T0);
		zzn4_add(&var.a, &var.c, &T1);
		zzn4_mul(&T0, &T1, &T0);//+9=39 for "special case"
		if (!zero_b) zzn4_add(&Z2, &T0, &Z2);
		else zzn4_copy(&T0, &Z2);
		zzn4_sub(&Z2, &Z0, &Z2);
		zzn4_copy(&Z1, &ret.b);
		if (!zero_c)
		{ // exploit special form of BN curve line function
			zzn4_mul(&c, &var.c, &T0);
			zzn4_sub(&Z2, &T0, &Z2);
			zzn4_sub(&Z3, &T0, &Z3); zzn4_tx(&T0);
			zzn4_add(&ret.b, &T0, &ret.b);
		}
		zzn4_tx(&Z3);
		zzn4_add(&Z0, &Z3, &ret.a);
		zzn4_copy(&Z2, &ret.c);
		if (!var.unitary) ret.unitary = FALSE;
	}

	BigMath::release_zzn4(Z0);
	BigMath::release_zzn4(Z1);
	BigMath::release_zzn4(Z2);
	BigMath::release_zzn4(Z3);
	BigMath::release_zzn4(T0);
	BigMath::release_zzn4(T1);

	return ret;
}

Pairing Pairing::conj()
{
	Pairing ret;
	zzn4_conj(&a, &ret.a);
	zzn4_conj(&b, &ret.b);
	zzn4_negate(&ret.b, &ret.b);
	zzn4_conj(&c, &ret.c);
	ret.miller = miller;
	ret.unitary = unitary;
	return ret;
}

Pairing Pairing::inverse()
{
	zzn4 tmp1, tmp2;
	Pairing ret;
	BigMath::init_zzn4(tmp1);
	BigMath::init_zzn4(tmp2);

	if (unitary)
	{
		ret = conj();
		goto END;
	}
	//ret.a=a*a-tx(b*c);
	zzn4_mul(&a, &a, &ret.a);
	zzn4_mul(&b, &c, &ret.b); zzn4_tx(&ret.b);
	zzn4_sub(&ret.a, &ret.b, &ret.a);

	//ret.b=tx(c*c)-a*b;
	zzn4_mul(&c, &c, &ret.c); zzn4_tx(&ret.c);
	zzn4_mul(&a, &b, &ret.b); zzn4_sub(&ret.c, &ret.b, &ret.b);

	//ret.c=b*b-a*c;
	zzn4_mul(&b, &b, &ret.c); zzn4_mul(&a, &c, &tmp1); zzn4_sub(&ret.c, &tmp1, &ret.c);

	//tmp1=tx(b*ret.c)+a*ret.a+tx(c*ret.b);
	zzn4_mul(&b, &ret.c, &tmp1); zzn4_tx(&tmp1);
	zzn4_mul(&a, &ret.a, &tmp2); zzn4_add(&tmp1, &tmp2, &tmp1);
	zzn4_mul(&c, &ret.b, &tmp2); zzn4_tx(&tmp2); zzn4_add(&tmp1, &tmp2, &tmp1);
	zzn4_inv(&tmp1);
	zzn4_mul(&ret.a, &tmp1, &ret.a);
	zzn4_mul(&ret.b, &tmp1, &ret.b);
	zzn4_mul(&ret.c, &tmp1, &ret.c);

END:
	BigMath::release_zzn4(tmp1);
	BigMath::release_zzn4(tmp2);
	return ret;
}

Pairing Pairing::powq(zzn2& var)
{
	Pairing ret(*this);
	zzn2 X2, X3;

	BigMath::init_zzn2(X2);
	BigMath::init_zzn2(X3);

	zzn2_mul(&var, &var, &X2);
	zzn2_mul(&X2, &var, &X3);
	zzn4_powq(&X3, &ret.a); zzn4_powq(&X3, &ret.b); zzn4_powq(&X3, &ret.c);
	zzn4_smul(&ret.b, &ParamSM9::norm_X, &ret.b);
	zzn4_smul(&ret.c, &X2, &ret.c);

	BigMath::release_zzn2(X2);
	BigMath::release_zzn2(X3);
	return ret;
}

Pairing Pairing::div(Pairing& var)
{
	Pairing y = var.inverse();
	return mul(y);
}

Pairing Pairing::pow(big k)
{
	Pairing ret;
	big zero, tmp, tmp1;
	int nb, i;
	BOOL invert_it;

	BigMath::init_big(zero);
	BigMath::init_big(tmp);
	BigMath::init_big(tmp1);

	copy(k, tmp1);
	invert_it = FALSE;
	if (mr_compare(tmp1, zero) == 0)
	{
		tmp = get_mip()->one;
		zzn4_from_big(tmp, &ret.a);
		goto END;
	}
	if (mr_compare(tmp1, zero) < 0)
	{
		negify(tmp1, tmp1); invert_it = TRUE;
	}
	nb = logb2(k);

	ret = *this;
	if (nb > 1) for (i = nb - 2; i >= 0; i--)
	{
		ret = ret.mul(ret);
		if (mr_testbit(k, i)) ret = ret.mul(*this);
	}
	if (invert_it) ret = ret.inverse();

END:
	BigMath::release_big(zero);
	BigMath::release_big(tmp);
	BigMath::release_big(tmp1);
	return ret;
}

bool Pairing::isOrderQ()
{
	bool result = false;
	Pairing v(*this);
	Pairing w(*this);
	big six;

	BigMath::init_big(six);

	convert(6, six);

	w = w.powq(ParamSM9::norm_X);
	v = v.pow(ParamSM9::param_t);
	v = v.pow(ParamSM9::param_t);
	v = v.pow(six);

	if (zzn4_compare(&w.a, &v.a) && zzn4_compare(&w.a, &v.a) && zzn4_compare(&w.a, &v.a)) {
		result = true;
		goto END;
	}

END:
	BigMath::release_big(six);

	return result;
}


void Pairing::q_power_frobenius(ecn2 A, zzn2 F)
{
	// Fast multiplication of A by q (for Trace-Zero group members only)
	zzn2 x, y, z, w, r;

	BigMath::init_zzn2(x);
	BigMath::init_zzn2(y);
	BigMath::init_zzn2(z);
	BigMath::init_zzn2(w);
	BigMath::init_zzn2(r);

	ecn2_get(&A, &x, &y, &z);
	zzn2_copy(&F, &r);//r=F
	if (get_mip()->TWIST == MR_SEXTIC_M) zzn2_inv(&r); // could be precalculated
	zzn2_mul(&r, &r, &w);//w=r*r
	zzn2_conj(&x, &x); zzn2_mul(&w, &x, &x);
	zzn2_conj(&y, &y); zzn2_mul(&w, &r, &w); zzn2_mul(&w, &y, &y);
	zzn2_conj(&z, &z);
	ecn2_setxyz(&x, &y, &z, &A);

	BigMath::release_zzn2(x);
	BigMath::release_zzn2(y);
	BigMath::release_zzn2(z);
	BigMath::release_zzn2(w);
	BigMath::release_zzn2(r);
}



Pairing Pairing::line(ecn2 A, ecn2 *C, ecn2 *B, zzn2 slope, zzn2 extra, BOOL Doubling, big Qx, big Qy)
{
	Pairing ret;
	zzn2 X, Y, Z, Z2, U, QY, CZ;
	big QX;

	BigMath::init_big(QX);
	BigMath::init_zzn2(X);
	BigMath::init_zzn2(Y);
	BigMath::init_zzn2(Z);
	BigMath::init_zzn2(Z2);
	BigMath::init_zzn2(U);
	BigMath::init_zzn2(QY);
	BigMath::init_zzn2(CZ);

	ecn2_getz(C, &CZ);
	// Thanks to A. Menezes for pointing out this optimization...
	if (Doubling)
	{
		ecn2_get(&A, &X, &Y, &Z);
		zzn2_mul(&Z, &Z, &Z2); //Z2=Z*Z
							   //X=slope*X-extra
		zzn2_mul(&slope, &X, &X);
		zzn2_sub(&X, &extra, &X);
		zzn2_mul(&CZ, &Z2, &U);
		//(-(Z*Z*slope)*Qx);
		nres(Qx, QX);
		zzn2_mul(&Z2, &slope, &Y);
		zzn2_smul(&Y, QX, &Y);
		zzn2_negate(&Y, &Y);
		if (get_mip()->TWIST == MR_SEXTIC_M)
		{ // "multiplied across" by i to simplify
			zzn2_from_big(Qy, &QY);
			zzn2_txx(&QY);
			zzn2_mul(&U, &QY, &QY);
			zzn4_from_zzn2s(&QY, &X, &ret.a);
			zzn2_copy(&Y, &(ret.c.b));
		}
		if (get_mip()->TWIST == MR_SEXTIC_D)
		{
			zzn2_smul(&U, Qy, &QY);
			zzn4_from_zzn2s(&QY, &X, &ret.a);
			zzn2_copy(&Y, &(ret.b.b));
		}
	}
	else
	{ //slope*X-Y*Z
		ecn2_getxy(B, &X, &Y);
		zzn2_mul(&slope, &X, &X);
		zzn2_mul(&Y, &CZ, &Y);
		zzn2_sub(&X, &Y, &X);
		//(-slope*Qx)
		nres(Qx, QX);
		zzn2_smul(&slope, QX, &Z);
		zzn2_negate(&Z, &Z);
		if (get_mip()->TWIST == MR_SEXTIC_M)
		{
			zzn2_from_big(Qy, &QY);
			zzn2_txx(&QY);
			zzn2_mul(&CZ, &QY, &QY);
			zzn4_from_zzn2s(&QY, &X, &ret.a);
			zzn2_copy(&Z, &(ret.c.b));
		}
		if (get_mip()->TWIST == MR_SEXTIC_D)
		{
			zzn2_smul(&CZ, Qy, &QY);
			zzn4_from_zzn2s(&QY, &X, &ret.a);
			zzn2_copy(&Z, &(ret.b.b));
		}
	}

	BigMath::release_big(QX);
	BigMath::release_zzn2(X);
	BigMath::release_zzn2(Y);
	BigMath::release_zzn2(Z);
	BigMath::release_zzn2(Z2);
	BigMath::release_zzn2(U);
	BigMath::release_zzn2(QY);
	BigMath::release_zzn2(CZ);
	return ret;
}

Pairing Pairing::g(ecn2 *A, ecn2 *B, big Qx, big Qy)
{
	Pairing ret;
	zzn2 lam, extra;
	ecn2 P;
	BOOL Doubling;

	BigMath::init_zzn2(lam);
	BigMath::init_zzn2(extra);
	BigMath::init_ecn2(P);

	ecn2_copy(A, &P);
	Doubling = ecn2_add2(B, A, &lam, &extra);
	if (A->marker == MR_EPOINT_INFINITY)
	{
		zzn4_from_int(1, &ret.a);
		ret.miller = FALSE;
		ret.unitary = TRUE;
	}
	else {
		ret = line(P, A, B, lam, extra, Doubling, Qx, Qy);
	}

	BigMath::release_zzn2(lam);
	BigMath::release_zzn2(extra);
	BigMath::release_ecn2(P);
	return ret;
}

bool Pairing::fast_pairing(Pairing& ret, ecn2 P, big Qx, big Qy, big x, zzn2 X)
{
	bool result = false;
	int i, nb;
	big n, zero, negify_x;
	ecn2 A, KA;
	Pairing t0, x0, x1, x2, x3, x4, x5, res, tmp;

	BigMath::init_big(n);
	BigMath::init_big(zero);
	BigMath::init_big(negify_x);
	BigMath::init_ecn2(A);
	BigMath::init_ecn2(KA);

	premult(x, 6, n); incr(n, 2, n);//n=(6*x+2);
	if (mr_compare(x, zero) < 0) //x<0
		negify(n, n); //n=-(6*x+2);
	ecn2_copy(&P, &A);
	nb = logb2(n);
	zzn4_from_int(1, &res.a);
	res.unitary = TRUE; //res=1
	res.miller = TRUE; //Short Miller loop

	for (i = nb - 2; i >= 0; i--)
	{
		res = res.mul(res);
		tmp = g(&A, &A, Qx, Qy);
		res = res.mul(tmp);
		if (mr_testbit(n, i)) {
			tmp = g(&A, &P, Qx, Qy);
			res = res.mul(tmp);
		}
	}
	// Combining ideas due to Longa, Aranha et al. and Naehrig
	ecn2_copy(&P, &KA);
	q_power_frobenius(KA, X);
	if (mr_compare(x, zero) < 0)
	{
		ecn2_negate(&A, &A);
		res = res.conj();
	}
	tmp = g(&A, &KA, Qx, Qy);
	res = res.mul(tmp);

	q_power_frobenius(KA, X);
	ecn2_negate(&KA, &KA);
	tmp = g(&A, &KA, Qx, Qy);
	res = res.mul(tmp);

	if (zzn4_iszero(&res.a) && zzn4_iszero(&res.b) && zzn4_iszero(&res.c))
		goto END;

	// The final exponentiation
	res = res.conj().div(res);
	res.miller = FALSE; res.unitary = FALSE;

	res = res.powq(X).powq(X).mul(res);
	res.miller = FALSE; res.unitary = TRUE;

	// Newer new idea...
	// See "On the final exponentiation for calculating pairings on ordinary elliptic curves"
	// Michael Scott and Naomi Benger and Manuel Charlemagne and Luis J. Dominguez Perez and Ezekiel J. Kachisa
	t0 = res.powq(X);
	x0 = t0.powq(X);
	x1 = res.mul(t0);

	x0 = x0.mul(x1).powq(X);
	x1 = res.inverse();

	negify(x, negify_x);
	x4 = res.pow(negify_x);
	x3 = x4.powq(X);
	x2 = x4.pow(negify_x);

	x5 = x2.inverse();
	t0 = x2.pow(negify_x);

	x2 = x2.powq(X);
	x4 = x4.div(x2);
	x2 = x2.powq(X);

	res = t0.powq(X);
	t0 = t0.mul(res);

	t0 = t0.mul(t0).mul(x4).mul(x5);
	res = x3.mul(x5).mul(t0);

	t0 = t0.mul(x2);
	res = res.mul(res).mul(t0);

	res = res.mul(res);
	t0 = res.mul(x1);

	res = res.mul(x0);
	t0 = t0.mul(t0).mul(res);

	ret = t0;
	result = true;

END:
	BigMath::release_big(n);
	BigMath::release_big(zero);
	BigMath::release_big(negify_x);
	BigMath::release_ecn2(A);
	BigMath::release_ecn2(KA);
	return result;
}

bool Pairing::calcRatePairing(Pairing& ret, ecn2 P, epoint *Q, big x, zzn2 X)
{
	bool result = false;
	big Qx, Qy;

	BigMath::init_big(Qx);
	BigMath::init_big(Qy);

	ecn2_norm(&P);
	epoint_get(Q, Qx, Qy);
	result = fast_pairing(ret, P, Qx, Qy, x, X);
	if (result) result = ret.isOrderQ();

	BigMath::release_big(Qx);
	BigMath::release_big(Qy);
	return result;
}