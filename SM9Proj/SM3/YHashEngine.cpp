
#include "YHashEngine.h"

#include "../utils/YHex.h"

static unsigned int swap(unsigned int n)
{
	return (n << 24) | (n >> 24) | ((n & 0xff00) << 8) | ((n & 0xff0000) >> 8);
}


static uint64 swap(uint64 value)
{
	return (((uint64)swap((uint32)value)) << 32) | swap((uint32)(value >> 32));
}

YHashEngine::YHashEngine(std::size_t blocksize, std::size_t hashsize) : BLOCK_SIZE(blocksize), HASH_SIZE(hashsize)
{

}


YHashEngine::~YHashEngine()
{

}

void YHashEngine::byteReverse( uint32 *buffer, int byteCount )
{
	int counts = byteCount / sizeof(unsigned int);
	for( int count = 0; count < counts; count++ ) {
		buffer[count] = swap(buffer[count]);
	}
}

void YHashEngine::byteReverse(uint64 *buffer, int byteCount)
{
	int counts = byteCount / sizeof(uint64);
	for( int count = 0; count < counts; count++ ) {
		buffer[count] = swap(buffer[count]);
	}
}

std::string YHashEngine::getData()
{
	std::string result(_digest.begin(), _digest.end());
	return result;
}



std::string YHashEngine::hash( const std::string& data)
{
	return hash(data.data(), data.size());
}

std::string YHashEngine::hash(const void* data, std::size_t length)
{
	init();
	update(data, length);
	finish();
	return getData();
}


//////////////////////////////////////////////////////////////////////////
void YHashEngine::hmac_init_impl(const void *key, std::size_t keylen)
{
	const unsigned char* buffer = (const unsigned char*)key;
	std::string keyhash; // must define out of if sentence
	if (keylen > BLOCK_SIZE) {
		keyhash = hash(buffer, keylen); // variable returned can not be temporary
		buffer = (const unsigned char*)keyhash.c_str();
		keylen = HASH_SIZE;
	}

	std::memset(_hmac_inPadding, 0x36, BLOCK_SIZE);
	std::memset(_hmac_outPadding, 0x5c, BLOCK_SIZE);
	

	std::size_t i = 0;
	for (i = 0; i < keylen; i++) {
		_hmac_inPadding[i] ^= buffer[i];
		_hmac_outPadding[i] ^= buffer[i];
	}

	init();
	updateImpl(_hmac_inPadding, BLOCK_SIZE);
}

void YHashEngine::hmac_update_impl(const void* data, std::size_t length)
{
	updateImpl(data, length);
}

void YHashEngine::hmac_finish()
{
	finish();
	init();
	updateImpl(_hmac_outPadding, BLOCK_SIZE);
	updateImpl(getData().c_str(), HASH_SIZE);
	finish();
}

std::string YHashEngine::hmac(const std::string& data, const std::string& key)
{
	return hmac(data.data(), data.size(), key.data(), key.size());
}

std::string YHashEngine::hmac(const void* data, std::size_t length, const void* key, std::size_t keylen)
{
	hmac_init(key, keylen);
	hmac_update(data, length);
	hmac_finish();
	return getData();
}


