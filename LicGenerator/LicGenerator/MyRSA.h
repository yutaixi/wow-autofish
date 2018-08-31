/*
* MyRSA.h
*
*  Created on: 2013-3-7
*      Author: wzb
*/

#ifndef MYRSA_H_
#define MYRSA_H_

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <iostream>
#include <string>

#include "cryptopp/rsa.h"
#include "cryptopp/hex.h"
#include "cryptopp/osrng.h"
#include "cryptopp/files.h"
#include "cryptopp/md5.h"
#pragma comment(lib,"cryptlib")
using namespace std;
using namespace CryptoPP;

class MyRSA
{
public:
	MyRSA();
	~MyRSA();

	string MD5(const char * message);
	string MD5File(const char * filename);

	void GenerateRSAKey(unsigned int keyLength, const char *privFilename,
		const char *pubFilename);

	string Encrypt(const char * pubFilename, const char * message);
	string Decrypt(const char * privFilename, const char * cipher);

	void SignFile(const char * privFilename, const char * messageFilename,
		const char * signatureFilename);
	bool VerifyFile(const char *pubFilename, const char * messageFilename,
		const char * signatureFilename);

	SecByteBlock SignString(const char *privFilename, const char * message);
	bool VerifyString(const char * pubFilename, const char * messsage,
		const SecByteBlock &SecByteBlock);
private:
	AutoSeededRandomPool _rng;
};

#endif /* MYRSA_H_ */