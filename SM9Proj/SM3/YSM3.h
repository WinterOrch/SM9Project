
#ifndef Y_CPP_SM3_INCLUDE_H__
#define Y_CPP_SM3_INCLUDE_H__

/** \file YSM3.h
*	\brief SM3 哈希算法C++接口.
*/

#include "YHashEngine.h"



class YSM3 : public YHashEngine
{
public:
	enum {
		BLOCK_SIZE = 64, // block size of group
		HASH_SIZE = 32 // message digest length
	};

	YSM3();
	~YSM3();

private:
	/// Disable copy constructor and override for operator =.
	YSM3(const YSM3&) = delete;
	YSM3& operator=(const YSM3&) = delete;

public:
	/// Resets so that a new digest can be computed.
	void init();
	/// Compute hash value use currently data, then initial all variable but _digest.
	void finish();

	/// Algorithm self checked.
	bool self_checked();
protected:
	/// Update new data into _context.data, then calculate.
	void updateImpl(const void* data, std::size_t length);

private:
	/// The really function to compute hash value.
	void transform();

	/// Byte array compare.
	int byteArrayCmp(const unsigned char *a, const unsigned char *b, int nLen);
private:
	/// Internal struct for compute.
	struct Context {
		uint32 digest[8]; ///< store hash value (V)
		uint32 countLow; ///< lower 32 bits to message length witch store with 64 bits
		uint32 countHigh; ///< higher 32 bits to message length witch store with 64 bits
		uint32 T[64]; ///< 算法常量，在压缩函数中使用
		uint32 data[16]; ///< a data cache of one group in this algorithm
		uint32 slop; ///< of bytes saved in data[] while 'update' message 
	};

	Context _context;
};

#endif