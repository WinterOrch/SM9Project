

/** \file YHashEngine.h
*	\brief ¹þÏ£Ëã·¨»ùÀà.
*	Base class of Message Digest Algorithm. Implement some general or useful methods. All other hash algorithms should inherit this class.
*/
 

#ifndef Y_CPP_HASH_ENGINE_INCLUDE_H__
#define Y_CPP_HASH_ENGINE_INCLUDE_H__

#include <cstring>
#include <string>
#include <vector>

typedef unsigned int uint32;
typedef unsigned long long uint64;

class YHashEngine
{
public:
	YHashEngine(std::size_t blocksize, std::size_t hashsize);
	virtual ~YHashEngine();

	/// Define message digest struct with unsigned char type.
	typedef std::vector<unsigned char> Digest;

public:
	/// Resets the engine so that a new digest can be computed.
	virtual void init() = 0;

	/// Updates the digest with the given data. inline function.
	inline void update(const char data);
	inline void update(const void* data, std::size_t length);
	inline void update(const std::string& data);

	/// Finishes the computation of the digest and returns the message digest.
	/// Resets the engine and can thus only be called once for every digest.
	/// The returned reference is valid until the next time digest() is called, or the engine object is destroyed.
	virtual void finish() = 0;

	/// \brief Calculate hash use one function. 
	std::string hash(const std::string& data);

	/**
	* \brief Calculate hash use one function. Must be implement by subclass
	* It calculates hash value use subclass's functions, the implement of those functions are different in various kinds hash algorithm, even though they have the same function.
	* @param data any basic type, force convert to 'const void*'
	* @param valueMode the value mode for message digest to return. including:<br/>
	*		1. VALUE_DATA : original data mode(a string of char type).<br/>
	*		2. VALUE_HEX : hex data mode(a string of hexadecimal numbers),<br/>
	*		3. VALUE_BASE64 : base64 encoding mode(a string of base64 encoded).<br/>
	*/
	std::string hash(const void* data, std::size_t length);

protected:
	/// \brief Update the digest with the given data. Must be implemented by subclass.
	virtual void updateImpl(const void* data, std::size_t length) = 0;

public:
	/// Returns the length of the digest in bytes.
	std::size_t hashLength() { return HASH_SIZE; }

	/// Converts a message digest of the original data.
	std::string getData();


protected:
	/// Reverse byte order while little endian.
	void byteReverse(uint32 *buffer, int byteCount);
	void byteReverse(uint64 *buffer, int byteCount);

//////////////////////////////////////////////////////////////////////////
/// hash mac
public:
	inline void hmac_init(const void *key, std::size_t keylen);
	inline void hmac_init(const std::string& key);

	/// Updates the hash MAC digest with the given data.
	inline void hmac_update(const char data);
	inline void hmac_update(const void* data, std::size_t length);
	inline void hmac_update(const std::string& data);

	void hmac_finish();

	std::string hmac(const std::string& data, const std::string& key);
	std::string hmac(const void* data, std::size_t length, const void* key, std::size_t keylen);

private:
	/// \brief Hash MAC initialize.
	void hmac_init_impl(const void *key, std::size_t length);

	/// \brief Update the hash MAC digest with the given data.
	void hmac_update_impl(const void* data, std::size_t length);
	
private:
	/// Disable copy constructor and override for operator =.
	YHashEngine(const YHashEngine&);
	YHashEngine& operator = (const YHashEngine&);

protected:
	YHashEngine::Digest _digest;

private:
	std::size_t BLOCK_SIZE; /// block size per group
	std::size_t HASH_SIZE; /// digest size 

	unsigned char _hmac_inPadding[144]; ///< HMAC internal padding
	unsigned char _hmac_outPadding[144]; ///< HMAC external padding
};

//////////////////////////////////////////////////////////////////////////
/// inline

void YHashEngine::update( const void* data, std::size_t length )
{
	updateImpl(data, length);
}

void YHashEngine::update( const std::string& data )
{
	updateImpl(data.data(), data.size());
}

void YHashEngine::update(const char data)
{
	updateImpl(&data, 1);
}

/// hash mac functions
void YHashEngine::hmac_init(const void *key, std::size_t length)
{
	hmac_init_impl(key, length);
}

void YHashEngine::hmac_init(const std::string& key)
{
	hmac_init_impl(key.data(), key.size());
}

void YHashEngine::hmac_update( const void* data, std::size_t length )
{
	hmac_update_impl(data, length);
}

void YHashEngine::hmac_update( const std::string& data )
{
	hmac_update_impl(data.data(), data.size());
}

void YHashEngine::hmac_update(const char data)
{
	hmac_update_impl(&data, 1);
}


//////////////////////////////////////////////////////////////////////////

#endif // !YDIGEST_ENGINE_Y__