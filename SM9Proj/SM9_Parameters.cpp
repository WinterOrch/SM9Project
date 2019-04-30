#include "SM9_Parameters.h"
#include "BigMath.h"
#include "Convert.h"
#include "Random.h"

#include <string>
#include <random>

ParamSM9::ParamSM9()
{

}

ParamSM9::~ParamSM9()
{

}

//	基域特征q
unsigned char SM9_q[32] = {
	0xB6, 0x40, 0x00, 0x00, 0x02, 0xA3, 0xA6, 0xF1,
	0xD6, 0x03, 0xAB, 0x4F, 0xF5, 0x8E, 0xC7, 0x45,
	0x21, 0xF2, 0x93, 0x4B, 0x1A, 0x7A, 0xEE, 0xDB,
	0xE5, 0x6F, 0x9B, 0x27, 0xE3, 0x51, 0x45, 0x7D
};

//	群G1, G2的阶N
unsigned char SM9_N[32] = {
	0xB6, 0x40, 0x00, 0x00, 0x02, 0xA3, 0xA6, 0xF1,
	0xD6, 0x03, 0xAB, 0x4F, 0xF5, 0x8E, 0xC7, 0x44,
	0x49, 0xF2, 0x93, 0x4B, 0x18, 0xEA, 0x8B, 0xEE,
	0xE5, 0x6E, 0xE1, 0x9C, 0xD6, 0x9E, 0xCF, 0x25
};

//	坐标xP1, yP1
unsigned char SM9_xP1[32] = {
	0x93, 0xDE, 0x05, 0x1D, 0x62, 0xBF, 0x71, 0x8F,
	0xF5, 0xED, 0x07, 0x04, 0x48, 0x7D, 0x01, 0xD6,
	0xE1, 0xE4, 0x08, 0x69, 0x09, 0xDC, 0x32, 0x80,
	0xE8, 0xC4, 0xE4, 0x81, 0x7C, 0x66, 0xDD, 0xDD
};

unsigned char SM9_yP1[32] = {
	0x21, 0xFE, 0x8D, 0xDA, 0x4F, 0x21, 0xE6, 0x07,
	0x63, 0x10, 0x65, 0x12, 0x5C, 0x39, 0x5B, 0xBC,
	0x1C, 0x1C, 0x00, 0xCB, 0xFA, 0x60, 0x24, 0x35,
	0x0C, 0x46, 0x4C, 0xD7, 0x0A, 0x3E, 0xA6, 0x16
};

//	坐标xP2, yP2
unsigned char SM9_xP2[64] = {
	0x85, 0xAE, 0xF3, 0xD0, 0x78, 0x64, 0x0C, 0x98,
	0x59, 0x7B, 0x60, 0x27, 0xB4, 0x41, 0xA0, 0x1F,
	0xF1, 0xDD, 0x2C, 0x19, 0x0F, 0x5E, 0x93, 0xC4,
	0x54, 0x80, 0x6C, 0x11, 0xD8, 0x80, 0x61, 0x41,

	0x37, 0x22, 0x75, 0x52, 0x92, 0x13, 0x0B, 0x08,
	0xD2, 0xAA, 0xB9, 0x7F, 0xD3, 0x4E, 0xC1, 0x20,
	0xEE, 0x26, 0x59, 0x48, 0xD1, 0x9C, 0x17, 0xAB,
	0xF9, 0xB7, 0x21, 0x3B, 0xAF, 0x82, 0xD6, 0x5B
};

unsigned char SM9_yP2[64] = {
	0x17, 0x50, 0x9B, 0x09, 0x2E, 0x84, 0x5C, 0x12,
	0x66, 0xBA, 0x0D, 0x26, 0x2C, 0xBE, 0xE6, 0xED,
	0x07, 0x36, 0xA9, 0x6F, 0xA3, 0x47, 0xC8, 0xBD,
	0x85, 0x6D, 0xC7, 0x6B, 0x84, 0xEB, 0xEB, 0x96,

	0xA7, 0xCF, 0x28, 0xD5, 0x19, 0xBE, 0x3D, 0xA6,
	0x5F, 0x31, 0x70, 0x15, 0x3D, 0x27, 0x8F, 0xF2,
	0x47, 0xEF, 0xBA, 0x98, 0xA7, 0x1A, 0x08, 0x11,
	0x62, 0x15, 0xBB, 0xA5, 0xC9, 0x99, 0xA7, 0xC7
};

unsigned char SM9_t[32] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x60, 0x00, 0x00, 0x00, 0x00, 0x58, 0xF9, 0x8A
};

unsigned char SM9_a[32] = {
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

unsigned char SM9_b[32] = {
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05
};

big ParamSM9::param_N = NULL;
big ParamSM9::param_a = NULL;
big ParamSM9::param_b = NULL;
big ParamSM9::param_q = NULL;
big ParamSM9::param_t = NULL;
epoint* ParamSM9::param_P1 = NULL;
ecn2 ParamSM9::param_P2;
zzn2 ParamSM9::norm_X;
miracl* ParamSM9::mMip = NULL;

bool ParamSM9::isPointOnG1(epoint* var) {
	bool res = false;
	big x = NULL;
	big y = NULL;
	big x_3 = NULL;
	big tmp = NULL;
	epoint* buf = NULL;

	BigMath::init_big(x);
	BigMath::init_big(y);
	BigMath::init_big(x_3);
	BigMath::init_big(tmp);
	BigMath::init_epoint(buf);

	epoint_get(var, x, y);

	// 检验 y^2 = x^3 + b
	power(x, 3, param_q, x_3);	//	x_3 = x^3 mod p
	multiply(x, param_a, x);	//	x = a * x
	divide(x, param_q, tmp);	//	mod p
	add(x_3, x, x);				//	x = x^3 + ax
	add(x, param_b, x);			//	x = x^3 + ax + b
	divide(x, param_q, tmp);	//	x = x^3 + ax + b mod p
	power(y, 2, param_q, y);	//	y = y^2 mod p

	if ( mr_compare(x, y) != 0 ) {
		res = false;
		goto END;
	}

	// 令Q = ( x, y ),验证[N]Q = O;				
	ecurve_mult(param_N, var, buf);				//	buf = [N]Q
	if (point_at_infinity(buf) == TRUE) {		//	检验buf是否为无穷点
		res = true;
		goto END;
	}
		
END:
	BigMath::release_big(x);
	BigMath::release_big(y);
	BigMath::release_big(x_3);
	BigMath::release_big(tmp);
	BigMath::release_epoint(buf);

	return res;
}

bool ParamSM9::init() {

	bool res = false;
	big temp = NULL;
	big P1_x = NULL;
	big P1_y = NULL;
	BOOL br = FALSE;

	mMip = mirsys(BIG_LEN, 16);
	mMip->IOBASE = 16;
	mMip->TWIST = MR_SEXTIC_M;

	Random::init();

	BigMath::init_big(temp);
	BigMath::init_big(P1_x);
	BigMath::init_big(P1_y);

	BigMath::init_big(param_a);
	BigMath::init_big(param_b);
	BigMath::init_big(param_q);
	BigMath::init_big(param_t);
	BigMath::init_big(param_N);
	BigMath::init_epoint(param_P1);
	BigMath::init_ecn2(param_P2);
	BigMath::init_zzn2(norm_X);

	Convert::gets_big(param_N, (char*)SM9_N, sizeof(SM9_N));
	Convert::gets_big(param_q, (char*)SM9_q, sizeof(SM9_q));
	Convert::gets_big(param_t, (char*)SM9_t, sizeof(SM9_t));
	Convert::gets_big(param_a, (char*)SM9_a, sizeof(SM9_a));
	Convert::gets_big(param_b, (char*)SM9_b, sizeof(SM9_b));

	ecurve_init(param_a, param_b, param_q, MR_PROJECTIVE);
	Convert::gets_big(P1_x, (char*)SM9_xP1, sizeof(SM9_xP1));
	Convert::gets_big(P1_y, (char*)SM9_yP1, sizeof(SM9_yP1));

	br = epoint_set(P1_x, P1_y, 0, param_P1);
	res = br ? true : false;

	if (res) {
		res = Convert::gets_ecn2_byte128_xy(param_P2, (const char*)SM9_xP2, (const char*)SM9_yP2);
		if (!res) {
			goto END;
		}
	}
	else
		goto END;

	setFrobeniusNormConstant();

	res = true;

END:
	BigMath::release_big(P1_x);
	BigMath::release_big(P1_y);

	if (!res) {
		release();
	}

	return res;
}

void ParamSM9::release() {
	mirkill(param_N);
	mirkill(param_a);
	mirkill(param_b);
	mirkill(param_q);
	mirkill(param_t);
	BigMath::release_epoint(param_P1);
	BigMath::release_ecn2(param_P2);
	BigMath::release_zzn2(norm_X);
	mirexit();

}

void ParamSM9::setFrobeniusNormConstant()
{
	big p, zero, one, two;
	zzn2 tmp_norm_X;

	BigMath::init_big(p);
	BigMath::init_big(zero);
	BigMath::init_big(one);
	BigMath::init_big(two);
	BigMath::init_zzn2(tmp_norm_X);

	convert(0, zero);
	convert(1, one);
	convert(2, two);
	copy(mMip->modulus, p);
	switch (get_mip()->pmod8)
	{
	case 5:
		zzn2_from_bigs(zero, one, &tmp_norm_X);// = (sqrt(-1)^(p-1)/2
		break;
	case 3:
		zzn2_from_bigs(one, one, &tmp_norm_X); // = (1 + sqrt(-1))^(p-1)/2
		break;
	case 7:
		zzn2_from_bigs(two, one, &tmp_norm_X);// = (2 + sqrt(-1))^(p-1)/2
	default: break;
	}
	decr(p, 1, p);
	subdiv(p, 6, p);
	zzn2_pow(tmp_norm_X, p, norm_X);

	BigMath::release_big(p);
	BigMath::release_big(zero);
	BigMath::release_big(one);
	BigMath::release_big(two);
	BigMath::release_zzn2(tmp_norm_X);
}

void ParamSM9::zzn2_pow(zzn2& x, big& k, zzn2& r)
{
	int i, j, nb, n, nbw, nzs;
	big zero;
	zzn2 u2, t[16];

	BigMath::init_big(zero);
	BigMath::init_zzn2(u2);
	for (i = 0; i < 16; i++)
	{
		BigMath::init_zzn2(t[i]);
	}

	if (zzn2_iszero(&x))
	{
		zzn2_zero(&r);
		goto END;
	}
	if (size(k) == 0)
	{
		zzn2_from_int(1, &r);
		goto END;
	}
	if (size(k) == 1) {
		zzn2_copy(&x, &r);
		goto END;
	}

	// Prepare table for windowing
	zzn2_mul(&x, &x, &u2);
	zzn2_copy(&x, &t[0]);
	for (i = 1; i < 16; i++)
	{
		zzn2_mul(&t[i - 1], &u2, &t[i]);
	}
	// Left to right method - with windows
	zzn2_copy(&x, &r);
	nb = logb2(k);
	if (nb > 1) for (i = nb - 2; i >= 0;)
	{
		//Note new parameter of window_size=5. Default to 5, but reduce to 4 (or even 3) to save RAM
		n = mr_window(k, i, &nbw, &nzs, 5);
		for (j = 0; j < nbw; j++) zzn2_mul(&r, &r, &r);
		if (n > 0) zzn2_mul(&r, &t[n / 2], &r);
		i -= nbw;
		if (nzs)
		{
			for (j = 0; j < nzs; j++) zzn2_mul(&r, &r, &r);
			i -= nzs;
		}
	}

END:
	BigMath::release_big(zero);
	BigMath::release_zzn2(u2);
	for (i = 0; i < 16; i++) {
		BigMath::release_zzn2(t[i]);
	}
}