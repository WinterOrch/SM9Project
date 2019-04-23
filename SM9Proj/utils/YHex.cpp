
#include "YHex.h"


void YHex::Bin2Hex(bool isUpperCase, const char *bin_string, char *hex_string, int binLength) {
	int i;
	const char *HEX_CHAR = "0123456789abcdef";
	if( isUpperCase )
		HEX_CHAR = "0123456789ABCDEF";

	for( i = 0; i < binLength; i++ ) {
		hex_string[2 * i] = HEX_CHAR[(bin_string[i] >> 4) & 0x0F];
		hex_string[2 * i + 1] = HEX_CHAR[bin_string[i] & 0x0F];
	}
}


static int getHexCharPos(char ch) {
	int i = 0, nPos = -1;
	const char HEX_CHARUPP[] = "0123456789ABCDEF";
	const char HEX_CHARLOW[] = "0123456789abcdef";

	for( i = 0; i < 16; i++ ) {
		if( (ch == HEX_CHARLOW[i]) || (ch == HEX_CHARUPP[i]) ) {
			nPos = i;
			break;
		}
	}

	return nPos;
}


void YHex::Hex2Bin(const char *hex_string, char* bin_string, int hexLength)
{
	int i = 0, pos1 = 0, pos2 = 0;
	for( i = 0; i < hexLength / 2; i++ ) {
		pos1 = getHexCharPos(hex_string[i * 2]);
		pos2 = getHexCharPos(hex_string[i * 2 + 1]);
		bin_string[i] = ((pos1 & 0x0F) << 4) | (pos2 & 0x0F);
	}
}


int YHex::Check(const unsigned char* pSrc, int nSrcLength)
{
	if( pSrc == nullptr || nSrcLength <= 0 )
		return 0;

	for( int i = 0; i < nSrcLength; i++ ) {
		if( ((pSrc[i] >= '0') && (pSrc[i] <= '9'))
		   || ((pSrc[i] >= 'a') && (pSrc[i] <= 'f'))
		   || ((pSrc[i] >= 'A') && (pSrc[i] <= 'F')) )
			continue;
		else
			return i + 1;
	}

	return 0;
}


bool YHex::Check(const string& data)
{
	if( Check((const unsigned char*)data.c_str(), data.length()) == 0 )
		return true;
	else
		return false;
}


std::string YHex::Encode(const string& data, bool isUpperCase)
{
	int length = data.length();
	if( length == 0 )
		return "";

	char *hex = new char[length * 2 + 1];
	Bin2Hex(isUpperCase, data.c_str(), hex, length);
	hex[length * 2] = 0;

	std::string result;
	result.append(hex, length * 2);

	delete[] hex;
	return result;
}


std::string YHex::Decode(const string& data)
{
	int length = data.length();
	if( length == 0 )
		return "";

	char *bin = new char[length / 2 + 1];
	Hex2Bin(data.c_str(), bin, length);
	bin[length / 2] = 0;

	std::string result;
	result.append(bin, length / 2);

	delete[] bin;
	return result;
}
