#include "Interact.h"
#include "KGC_main.h"
#include "Signature.h"

#include <cstdio>
#include <cstring>

#include <string>
#include <iostream>
#include <fstream>

#include "../SM9Proj/utils/YHex.h"

using namespace std;

#define INTERACT_DEBUG 1

Interact::Interact()
{
}

Interact::~Interact()
{
}

const int SHELL_WIDTH = 82;
const int PAR = 10;

const int BUF_SIZE = 1 << 14;
char Interact::buf[BUF_SIZE + 1];

void Interact::cutline(char c, int len)
{
	while (len--) putchar(c);
	puts("");
}

// 居中输出一行 不能超过 SHELL_WIDTH-2
void Interact::puts_middle(const char* s, char c)
{
	int n = strlen(s);
	int l, r;
	l = (SHELL_WIDTH - 2 - n) >> 1;
	r = SHELL_WIDTH - 2 - n - l;
	putchar(c);
	while (l--) putchar(' ');
	while (n--) putchar(*s++);
	while (r--) putchar(' ');
	putchar(c);
	puts("");
}

// 输出一个段落
// 段落从第 12 格开始，换行后从 10 格开始，并留出 10 格的空位
void Interact::puts_para(const char* s, char c)
{
	int n = strlen(s), len = SHELL_WIDTH - 2 - PAR * 2;
	// 第一行 SHELL_WIDTH-22 个字符
	for (int i = 1; *s; i++) 
	{
		putchar(c);
		int L, p, m = 0;
		L = i == 1 ? len - 2 : len;
		p = i == 1 ? PAR + 2 : PAR;
		while (p--) putchar(' ');
		while (L > 0 && *s != 0)
		{
			if (m == 0 && *s != ' ')
			{
				m = 0;
				while (*(s + m) != ' ' && *(s + m) != 0) m++;
				if (m > L) break;
			}
			putchar(*s++);
			L--;
			if (m > 0) m--;
		}
		p = PAR + L;
		while (p--) putchar(' ');
		putchar(c);
		puts("");
	}
}

void Interact::welcome()
{
	const char c = '#';
	cutline(c, SHELL_WIDTH);
	puts_middle(" ", c);
	puts_middle("***************************************", c);
	puts_middle("* Welcome to Security Mail Generator! *", c);
	puts_middle("***************************************", c);
	puts_middle(" ", c);
	puts_para("This is a digit signature system base on SM9. "
		"Register identity of you and your friends (such as email address) to our system, "
		"and we will be able to generate a signature for your message, "
		"and others can verify this through us."
		, c);
	puts_para("Types `help' for more infomation", c);
	puts_middle(" ", c);
	cutline(c, SHELL_WIDTH);
	puts("");
}

string Interact::readline(const char* s)
{
	string r;
	printf(s);
	getline(cin, r);
	fflush(stdin);
	return r;
}

string Interact::wait_input()
{
	cout << "[" << KGC_main::magic_tag << "] " << KGC_main::current_uid << "$ ";
	string s;
	getline(cin, s);
	fflush(stdin);
	return s;
}

void Interact::do_help()
{
	puts("Supports following commands:");
	puts(" help  ---  to show this infomation");
	puts(" ls    ---  to list all registered uid");
	puts(" reg   ---  to start a register transaction");
	puts("            this will add a new uid to our system");
	puts(" su    ---  to switch current uid");
	puts(" sig   ---  to start a signature transaction");
	puts("            this will generate a msg text with your signature");
	puts(" ver   ---  to start a verify transaction");
	puts("            this will read the safe mail and verify it");
	puts(" save  ---  to save KGC state");
	puts(" exit  ---  to exit our system");
}

void Interact::do_ls()
{
	cout << "Total: " << KGC_main::Users.size() << endl;
	for (auto uid : KGC_main::Users)
	{
		cout << uid << "  ";
	}
	cout << endl;
}

void Interact::do_reg()
{
	string s;
	cout << "Input the uid you want to regster:" << endl;
	cout << "[e.g. qinglkyi@qq.com]: ";
	getline(cin, s); 
	fflush(stdin);
	if (!KGC_main::createUser(s))
	{
		cout << "Error: " << s << " has been registered before!" << endl;
	}
	else
	{
		cout << "Success: " << s << " has been registered!" << endl;
	}
}

void Interact::do_su()
{
	string s;
	cout << "Input the uid you want to switch:" << endl;
	cout << "[e.g. qinglkyi@qq.com]: ";
	getline(cin, s);
	fflush(stdin);
	if (!KGC_main::switchUser(s))
	{
		cout << "Error: " << s << " isn't registered! Type `reg' to start registering!" << endl;
	}
	else
	{
		cout << "Success! Welcome back, " << s << "!" << endl;
	}
}

void Interact::do_save()
{
	if (!KGC_main::saveState()) puts("Error: Failed to save state!");
	else puts("Successfully saved!");
}

// Warning: '\n' means '\r' and '\n' in windows text file!
#define __CR 1 

const char header_line[] = "====================== Signature Header ======================\r\n";

const int HEAD_LEN = 8 * 9 + 4 + 1 + __CR;

string Interact::gen_signed_text(const string &msg, const Signature &sig)
{
	// TODO: Forget to add magic tags into the text
	string all;
	// Header Message: ".hd ....\n"
	// TODO: A (A<<8)^B (B'<<8)^C ...
	unsigned int pH, lH, pS, lS, pU, lU, pM, lM;
	// deal with cr
	int _cr = 0;
	string T;

	all += header_line;
	_cr += __CR;

	pH = all.length() + HEAD_LEN + _cr;
	T = YHex::Encode(sig.getH());
	lH = T.length(); // fix bugs
	all += T + "\n";
	_cr += __CR;
	
	pS = all.length() + HEAD_LEN + _cr;
	T = YHex::Encode(sig.getS());
	lS = T.length();
	all += T + "\n";
	_cr += __CR;

	all += "Signed by ";
	pU = all.length() + HEAD_LEN + _cr;
	lU = KGC_main::current_uid.length();
	all += KGC_main::current_uid + "\n";
	_cr += __CR;
	
	all += header_line;
	_cr += __CR;

	pM = all.length() + HEAD_LEN + _cr;
	lM = msg.length();
	all += msg;

	sprintf_s(buf, 
		".hd %9d%9d%9d%9d%9d%9d%9d%9d\n",
		pH, lH, pS, lS, pU, lU, pM, lM);

#ifdef INTERACT_DEBUG
	printf(".hd %d %d %d %d %d %d %d %d\n", pH, lH, pS, lS, pU, lU, pM, lM);
#endif
	all = string(buf) + all;
	return all;
}

void Interact::do_sig()
{
	int tmp;
	string fname, msg, text;
	FILE* fp;
	Signature sig;

	puts("Input the text file you want to sign:");
	fname = readline("[e.g. mail.txt]: ");
	
	// get msg
	fopen_s(&fp, fname.c_str(), "r");
	if (fp == NULL)
	{
		puts("Error: File not found!");
		return;
	}
	while ((tmp = fread(buf, 1, BUF_SIZE, fp)) == BUF_SIZE)
	{
		msg += string(buf, buf + tmp);
	}
	msg += string(buf, buf + tmp);
	fclose(fp);

	// get signature
	sig = KGC_main::sign(msg);

#ifdef INTERACT_DEBUG
	cout << "debug info:" << endl;
	cout << YHex::Encode(sig.getH()) << endl;
	cout << YHex::Encode(sig.getS()) << endl;
	cout << KGC_main::current_uid << endl;
	cout << msg << endl;
	cout << msg.length() << endl;
#endif

	// generate signing txt
	fname += ".signed";
	text = gen_signed_text(msg, sig);
	ofstream outf(fname.c_str(), ios::out | ios::trunc);
	if (outf.is_open())
	{
		outf << text;
		cout << "Success: generate `" << fname << "'." << endl;
	}
	else
	{
		puts("Error: Failed to write file!");
	}
	puts("");
}

string Interact::get_string_from_file(FILE* fp, unsigned int off, unsigned int siz)
{
	int tmp;
	string s;
	
	fseek(fp, off, SEEK_SET);
	while (siz > BUF_SIZE)
	{
		tmp = fread(buf, 1, BUF_SIZE, fp);
		if (tmp != BUF_SIZE)
		{
			fclose(fp);
			puts("Error when get string from file!");
			s.clear();
			return s;
		}
		s += string(buf, buf + tmp);
		siz -= BUF_SIZE;
	}
	tmp = fread(buf, 1, siz, fp);
	s += string(buf, buf + tmp);
	
	return s;
}

void Interact::do_ver()
{
	int tmp;
	string fname;
	FILE* fp;
	Signature sig;
	unsigned int pH, lH, pS, lS, pU, lU, pM, lM;
	string H, S, uid, msg;

	puts("Input the text file you want to verify:");
	fname = readline("[e.g. mail.signed]: ");

	fopen_s(&fp, fname.c_str(), "r");
	if (fp == NULL)
	{
		puts("Error: Failed to open file!");
		return;
	}
	tmp = fread(buf, 1, HEAD_LEN, fp);
	if (tmp != HEAD_LEN)
	{
		fclose(fp);
		puts("Error: Wrong format of signed file!");
		return;
	}
	
	tmp = sscanf_s(buf,
		".hd %d %d %d %d %d %d %d %d\n",
		&pH, &lH, &pS, &lS, &pU, &lU, &pM, &lM);
	if (tmp != 8)
	{
		fclose(fp);
		puts("Error: Wrong format of signed file!");
		return;
	}

#ifdef INTERACT_DEBUG
	printf(".hd %d %d %d %d %d %d %d %d\n", pH, lH, pS, lS, pU, lU, pM, lM);
#endif

	// get H S uid msg
	H = YHex::Decode(get_string_from_file(fp, pH, lH));
	S = YHex::Decode(get_string_from_file(fp, pS, lS));
	uid = get_string_from_file(fp, pU, lU);
	msg = get_string_from_file(fp, pM, lM);

#ifdef INTERACT_DEBUG
	cout << "debug info:" << endl;
	cout << YHex::Encode(H) << endl;
	cout << YHex::Encode(S) << endl;
	cout << uid << endl;
	cout << msg << endl;
	cout << msg.length() << endl;
#endif

	sig = Signature(H, S);

	tmp = KGC_main::verify(uid, sig, msg);

	if (tmp)
	{
		cout << "Verify successfully! It's signed by " << uid << "!" << endl;
	}
	else
	{
		cout << "Oops! It seems not verified!" << endl;
	}

	puts("");
	fclose(fp);
}

void Interact::main()
{
	KGC_main::KGC_Boot();
	welcome();
	while (1)
	{
		string cmd = wait_input();
		if (cmd == "help")
		{
			do_help();
		}
		else if (cmd == "ls")
		{
			do_ls();
		}
		else if (cmd == "reg")
		{
			do_reg();
		}
		else if (cmd == "su")
		{
			do_su();
		}
		else if (cmd == "save")
		{
			do_save();
		}
		else if (cmd == "sig")
		{
			do_sig();
		}
		else if (cmd == "ver")
		{
			do_ver();
		}
		else if (cmd == "exit")
		{
			cout << "See you next time, " << KGC_main::current_uid << " !" << endl;
			return;
		}
		else if (cmd == "")
		{
			// puts("");
		}
		else
		{
			puts("Unsupported commands. Just type command itself.");
		}
	}
}