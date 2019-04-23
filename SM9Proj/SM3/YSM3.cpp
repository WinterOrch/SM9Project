#include "YSM3.h"


#define YUINT32_SHR(x,n) ((unsigned int)x>>n)
#define YUINT32_SHL(x, n) ((unsigned int)x<<n)
#define YUINT32_ROTL(X,n)  ((((unsigned int)X)<<n) | (((unsigned int)X)>>(32-n)))
#define YUINT32_ROTR(X,n)  ((((unsigned int)X)>>n) | (((unsigned int)X)<<(32-n)))


YSM3::YSM3() : YHashEngine(BLOCK_SIZE, HASH_SIZE)
{
	_digest.reserve(HASH_SIZE);
	init();
}

YSM3::~YSM3()
{
	init();
}

void YSM3::init()
{
	int i = 0;
	_context.digest[0] = 0x7380166f;
	_context.digest[1] = 0x4914b2b9;
	_context.digest[2] = 0x172442d7;
	_context.digest[3] = 0xda8a0600;
	_context.digest[4] = 0xa96f30bc;
	_context.digest[5] = 0x163138aa;
	_context.digest[6] = 0xe38dee4d;
	_context.digest[7] = 0xb0fb0e4e;

	for( i = 0; i < 16; i ++)
		_context.T[i] = 0x79cc4519;
	for( i = 16; i < 64; i++)
		_context.T[i] = 0x7a879d8a;

	_context.countLow = 0;
	_context.countHigh = 0;
	_context.slop = 0;
	std::memset(_context.data, 0, sizeof(_context.data));
}

void YSM3::finish()
{
	const unsigned char sm3_padding[64] = {
		0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};

	uint32 high, low;
	unsigned int last, padlen;

	low = _context.countLow;
	high = _context.countHigh;

	byteReverse(&high, 4);
	byteReverse(&low, 4);

	/* Compute number of bytes mod 64 */
	last = (int)((_context.countLow >> 3) & 0x3F);
	padlen = (last < 56) ? (56 - last) : (120 - last);

	/* update padding and length in bits*/
	update(sm3_padding, padlen);
	update((void*)&high, 4);
	update((void*)&low, 4);

	byteReverse(_context.digest, HASH_SIZE);
	_digest.clear();
	_digest.insert(_digest.begin(), (unsigned char*)_context.digest, (unsigned char*)_context.digest + HASH_SIZE);
	init();
}


void YSM3::updateImpl(const void* data, std::size_t length)
{
	const unsigned char* buffer = (const unsigned char*)data;
	unsigned char* db = (unsigned char*)&_context.data[0];

	/* Update bit count */
	if ((_context.countLow + ((uint32)length << 3)) < _context.countLow)
		_context.countHigh++; /* Carry from low to high bitCount */
	_context.countLow += ((uint32)length << 3);
	_context.countHigh += ((uint32)length >> 29);

	/* Process data in BLOCK_SIZE chunks */
	while (length-- > 0)
	{
		db[_context.slop++] = *(buffer++);
		if (_context.slop == BLOCK_SIZE)
		{
			/* transform this one block */
			byteReverse(_context.data, BLOCK_SIZE);
			transform();
			_context.slop = 0;	/* no slop left */
		}
	}
}


//////////////////////////////////////////////////////////////////////////

static unsigned int FF(unsigned int X, unsigned int Y, unsigned int Z, int j) {
	if(j < 16)
		return X ^ Y ^ Z;
	else
		return (X & Y) | (X & Z) | (Y & Z);
}

static unsigned int GG(unsigned int X, unsigned int Y, unsigned int Z, int j) {
	if(j < 16)
		return X ^ Y ^ Z;
	else
		return (X & Y) | ((0xffffffff ^ X) & Z);
}


static unsigned int P0(uint32 X) {
	return X ^ YUINT32_ROTL(X, 9) ^ YUINT32_ROTL(X, 17);
}

static unsigned int P1(unsigned int X) {
	return X ^ YUINT32_ROTL(X, 15) ^ YUINT32_ROTL(X, 23);
}

static void Expand(unsigned int* W, unsigned int* W1, unsigned int* B) {
	int i;
	for (i = 0; i < 16; i++)
		W[i] = B[i];

	for( i = 16; i < 68; i ++)
		W[i] = P1(W[i - 16] ^ W[i - 9] ^ YUINT32_ROTL(W[i - 3], 15)) ^ YUINT32_ROTL(W[i - 13], 7) ^ W[i - 6];

	for( i = 0; i < 64; i ++)
		W1[i] = W[i] ^ W[i + 4]; 
}

void YSM3::transform()
{
	uint32 SS1, SS2, TT1, TT2, W[68]={0}, W1[64]={0};
	uint32 A, B, C, D, E, F, G, H;
	int j;

	Expand(W, W1, _context.data);

	A = _context.digest[0];
	B = _context.digest[1];
	C = _context.digest[2];
	D = _context.digest[3];
	E = _context.digest[4];
	F = _context.digest[5];
	G = _context.digest[6];
	H = _context.digest[7];

	for( j=0; j<64; j++ ) {
		SS1 = YUINT32_ROTL((YUINT32_ROTL(A, 12) + E + YUINT32_ROTL(_context.T[j], j % 32)), 7);
		SS2 = SS1 ^ YUINT32_ROTL(A, 12);
		TT1 = FF(A,B,C,j) + D + SS2 + W1[j];
		TT2 = GG(E, F, G, j) + H + SS1 + W[j];
		D = C;
		C = YUINT32_ROTL(B, 9);
		B = A;
		A = TT1;
		H = G;
		G = YUINT32_ROTL(F, 19);
		F = E;
		E = P0(TT2);
	}

	_context.digest[0] ^= A;
	_context.digest[1] ^= B;
	_context.digest[2] ^= C;
	_context.digest[3] ^= D;
	_context.digest[4] ^= E;
	_context.digest[5] ^= F;
	_context.digest[6] ^= G;
	_context.digest[7] ^= H;
}


int YSM3::byteArrayCmp(const unsigned char *a, const unsigned char *b, int nLen) {
	int i, nRet=0;
	for(i=0; i<nLen; i++) {
		if(a[i]>b[i])
			return 1;
		else if(a[i]<b[i])
			return -1;
	}
	return nRet;
}

bool YSM3::self_checked()
{
	static const unsigned char *puszSCabcMsg = (unsigned char *)"abc";
	static const unsigned char *puszSC512Msg = (unsigned char *)"abcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcd";

	static const unsigned char *puszSCabcHash = (unsigned char *)"66c7f0f462eeedd9d1f2d46bdc10e4e24167c4875cf2f7a2297da02b8f4ba8e0";
	static const unsigned char *puszSC512Hash = (unsigned char *)"debe9ff92275b8a138604889c18e5a4d6fdb70e5387e5765293dcba39c0c5732";

	bool result = false;
	unsigned char *puszMsg = NULL;
	unsigned char puszHash[33]={0};
	int nMsgLen = 0;

	nMsgLen = strlen((char*)puszSCabcMsg);
	puszMsg = (unsigned char *)malloc(nMsgLen+1);
	memset(puszMsg, 0, nMsgLen+1);
	memcpy(puszMsg, puszSCabcMsg, nMsgLen);

	std::string strHash = hash(puszMsg, nMsgLen);
	if(0==byteArrayCmp((const unsigned char*)strHash.c_str(), puszSCabcHash, hashLength()*2))
	{
		nMsgLen = strlen((char*)puszSC512Msg);
		free(puszMsg);
		puszMsg = (unsigned char *)malloc(nMsgLen+1);
		memset(puszMsg, 0, nMsgLen+1);
		memcpy(puszMsg, puszSC512Msg, nMsgLen);
		strHash = hash(puszMsg, nMsgLen);
		if(0==byteArrayCmp((const unsigned char*)strHash.c_str(), puszSC512Hash, hashLength()*2))
			result = true;
	}

	free(puszMsg);
	return result;
}