#pragma once

#include <string>
using namespace std;

#define SM9_NORMAL					0x00000001
#define SM9_NOT_INITTED				0x00000002
#define SM9_INITTING_FAILED			0x00000003
#define SM9_RATE_CAL_FAILED			0x00000004
#define SM9_VERIFY_H_OUTRANGE		0x00000005
#define SM9_VERIFY_S_OUTOF_G1		0x00000006
#define SM9_VERIFY_H2_UNEQUAL_H		0x00000007
#define SM9_KGC_T1_EQUAL_ZERO		0x00000008

/**
* “Ï≥£¿‡
* @author YUAN
*/
class Status {
private:
	Status() {};
	~Status() {};

public:
	static string getStatusTip(int statusID) {
		string msg = "Error Unknown";
		switch (statusID) {
			case SM9_NORMAL:
				msg = "Normally Operate";
				break;
			case SM9_NOT_INITTED:
				msg = "SM9 Has Not Been Inited";
				break;
			case SM9_INITTING_FAILED:
				msg = "Something Went Wrong When Being Initted";
				break;
			case SM9_RATE_CAL_FAILED:
				msg = "Something Went Wrong When Calculating R-ate Pairing";
				break;
			case SM9_VERIFY_H_OUTRANGE:
				msg = "h' Is Expected to Be In Range of [1,N-1]";
				break;
			case SM9_VERIFY_S_OUTOF_G1:
				msg = "S Is Expected to Be On G1";
				break;
			case SM9_VERIFY_H2_UNEQUAL_H:
				msg = "h_2 != h' Verify Failed";
				break;
			case SM9_KGC_T1_EQUAL_ZERO:
				msg = "t_1 = 0 Please Regenerate The Master Key Pair";
				break;
		}
	}


};