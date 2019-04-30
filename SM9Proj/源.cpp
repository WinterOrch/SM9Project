#include <time.h>

#include "SM9.h"
#include "Testor.h"

int main()
{
	clock_t tBegin, tTestEnd;
	clock_t tInitDone, tReleaseDone;

	tBegin = clock();

	SM9::init();
	tInitDone = clock();

	Testor::KGC_Standard_Test();
	tTestEnd = clock();

	SM9::release();
	tReleaseDone = clock();

	printf("\n初始化耗时:%ld ms\n", tInitDone - tBegin);
	printf("\n测试耗时:%ld ms\n", tTestEnd - tInitDone);
	printf("\n释放耗时:%ld ms\n", tReleaseDone - tTestEnd);
}
/*
extern "C"
{
#include "miracl.h"
#include "mirdef.h"
}

int main()
{
	int i;
	big x, e, m, y;
	FILE *fp;
	clock_t tBegin, tEnd;
	miracl *mip = mirsys(1000,16);
	x = mirvar(0);
	e = mirvar(0);
	m = mirvar(0);
	y = mirvar(0);

	errno_t err;
	err = fopen_s(&fp, "data.txt", "r+");
	mip->IOBASE = 16;
	cinnum(x, fp);
	cinnum(e, fp);
	cinnum(m,fp);
	fclose(fp);
	tBegin = clock();
	for (i = 0; i < 100; i++)
		powmod(x, e, m, y);
	tEnd = clock();
	cotnum(x, stdout);
	cotnum(e, stdout);
	cotnum(m, stdout);
	cotnum(y, stdout);

	printf("\n\n进行100次1024比特的模指数运算所消耗的时间为:%ld ms\n\n", tEnd - tBegin);

	int gg;
	gg = mr_compare(e, e);
	printf("%d", false);

	return 0;
}
*/
