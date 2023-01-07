#include "Interact.h"
#include "SM3/YSM3.h"
#include "utils/YHex.h"

#include <string>
#include <iostream>

#include <cstdio>

using namespace std;

int main()
{
	// Interact::main();
	YSM3 digestSM3;
	digestSM3.init();
	// 616263 每个比特都是大端	
	digestSM3.update(YHex::Decode("61"));
	digestSM3.update(YHex::Decode("62"));
	digestSM3.update(YHex::Decode("63"));
	digestSM3.finish();
	string s = YHex::Encode(digestSM3.getData());
	cout << s << endl;
	s = YHex::Decode("12345678");
	printf("%x", *(unsigned int *)s.c_str());
	return 0;
}