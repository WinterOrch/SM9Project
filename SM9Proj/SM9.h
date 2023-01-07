#pragma once
#include <string>

#include "Signature.h"



using namespace std;

#define HID_SIGN			0x01
#define HID_KEYEXCHANGE		0x02
#define HID_ENCRYPT			0x03

/**
* SM9�㷨ʵ��
* @author YUAN
*/
class SM9 {
protected:
	SM9();
	~SM9();

public:
	static bool init();
	static void release() noexcept;
	

public :
	/**
	* ǩ��
	*
	* @param masterPublicK	����Կ
	* @param privateK		�û�ǩ��˽Կ
	* @param message		��ǩ��Ϣ
	* @return				ǩ����ʵ��
	*/
	static Signature sign(const string& masterPublicK, const string& privateK, const string& message);

	/**
	* ��֤
	*
	* @param masterPublicK	����Կ
	* @param id				�û���ʶ
	* @param message		������Ϣ
	* @return				true or false
	*/
	static bool verify(const string& masterPublicK, const string& id, const Signature& signature, const string& message);

private:
	/**
	* H_1��H_2�Ĺ��в��֣�ͨ����������ʵ��ǩ������֤�����в�ͬ�����뺯����
	* @return �ַ�����ʽ�Ĵ���
	*/
	static string H(const string&z);

	/**
	* ͨ��SM3ɢ��ִ�� 8 * ceil( keyLen ) ��ɢ�в��������һ�顣
	* @return Ha_1 || Ha_2 ... || Ha_ceil(hlen/v) || Ha!_ceil(hlen/v)
	*/
	static string H_v(const string& z, int keyLen);

	// add by qinglkyi
	static string MAC(const string& k, const string& z);
	static string KDF_bitlen(const string& z, int keyLen);
	static string true_hv(const string& z);
	static string encrypt(const string& masterPublicK, const string& privateK, const string& message);
	static string decrypt(string cipher, const string uid, const string de_B)

protected:
	/**
	* ���뺯��H_1()�����û���ʶ��ǩ��˽Կ���ɺ���ʶ����͹�ϣͷΪ���룬������֤ǩ����
	*/
	static string H_1(const string& id, char hid);

	/**
	* ���뺯��H_2()������Ϣ��G_T�е�Ԫ�غ͹�ϣͷΪ���룬��������ǩ����
	*/
	static string H_2(const string& message, const string& w);

public:
	static bool isInited;
};