#include "BigMath.h"

BigMath::BigMath() {

}

BigMath::~BigMath() {

}

void BigMath::init_big(big& var) {
	var = mirvar(0);
}

void BigMath::release_big(big& var) {
	mirkill(var);
}

void BigMath::init_zzn2(zzn2& var) {
	var.a = mirvar(0);
	var.b = mirvar(0);
}

void BigMath::release_zzn2(zzn2& var) {
	mirkill(var.a);
	mirkill(var.b);
}

void BigMath::init_zzn4(zzn4& var) {
	var.a.a = mirvar(0);
	var.a.b = mirvar(0);
	var.b.a = mirvar(0);
	var.b.b = mirvar(0);
	var.unitary = FALSE;
}

void BigMath::release_zzn4(zzn4& var) {
	mirkill(var.a.a);
	mirkill(var.a.b);
	mirkill(var.b.a);
	mirkill(var.b.b);
}

void BigMath::init_ecn2(ecn2& var) {
	var.x.a = mirvar(0); var.x.b = mirvar(0);
	var.y.a = mirvar(0); var.y.b = mirvar(0);
	var.z.a = mirvar(0); var.z.b = mirvar(0);
	var.marker = MR_EPOINT_INFINITY;
}

void BigMath::release_ecn2(ecn2& var) {
	mirkill(var.x.a); mirkill(var.x.b);
	mirkill(var.y.a); mirkill(var.y.b);
	mirkill(var.z.a); mirkill(var.z.b);
}

void BigMath::init_epoint(epoint*& var) {
	var = epoint_init();
}

void BigMath::release_epoint(epoint* var) {
	epoint_free(var);
}