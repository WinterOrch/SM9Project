#include "Random.h"
#include<random>

extern "C"
{
#include "miracl.h"
#include "mirdef.h"
}

Random::Random() {

}

Random::~Random() {

}

bool Random::init() {
	std::random_device rd;
	std::default_random_engine mGenerator;
	std::uniform_int_distribution<unsigned int> mDistribute(0, UINT_MAX);
	mGenerator.seed(rd());
	irand(mDistribute(mGenerator));

	return TRUE;
}