#include "Testor.h"

#include "BigMath.h"
#include "Convert.h"

#include "SM9.h"
#include "SM9_KGC.h"

#include "../SM9Proj/utils/YHex.h"

#include <fstream>

extern "C"
{
#include "miracl.h"
#include "mirdef.h"
}

Testor::Testor()
{
}


Testor::~Testor()
{
}

void Testor::KGC_Standard_Test()
{
	FILE *fp;

	big m;
	MasterKeyPair master;

	BigMath::init_big(m);

	errno_t err;
	err = fopen_s(&fp, "standardprik.txt", "r+");
	cinnum(m, fp);
	fclose(fp);

	master = SM9_KGC::genSignMasterKeyPairFromPri(Convert::puts_big(m));

	ofstream outFile1("MasterKeyPair.txt");
	if (outFile1.is_open())
	{
		outFile1 << "------ Private Key For SM9 Signing Master Key Pair ------\n";
		outFile1 << YHex::Encode(master.getPrivateKey());
		outFile1 << "\n------ Public Key For SM9 Signing Master Key Pair ------\n";
		outFile1 << YHex::Encode(master.getPublicKey()) << "\n";
		outFile1.close();
	}

	string privateK;

	privateK = SM9_KGC::genSignPrivateKey(master.getPrivateKey(), "Alice");

	ofstream outFile2("AlicePrivateKey.txt");
	if (outFile2.is_open())
	{
		outFile2 << "------ Private Key For SM9 Signing Alice------\n";
		outFile2 << YHex::Encode(privateK) << "\n";
		outFile2.close();
	}

	Signature sig;

	sig = SM9::sign(master.getPublicKey(), privateK, "Chinese IBS Standard");

	bool res = SM9::verify(master.getPublicKey(), "Alice", sig, "Chinese IBS Standard");

	if (res)
		printf("OK");
	else
		printf("NG");
}