
#include <openssl/pem.h> 
#include <openssl/rand.h> 
#ifdef _DEBUG
#pragma comment( lib, "gtestd.lib" )
#pragma comment( lib, "libeay32MTd.lib" )
#endif
#ifdef _WIN32
#include <openssl/applink.c>
#endif
#include <iostream>

using namespace std;

static void append(unsigned char *&dst, const unsigned char *src, int len)
{
	*dst++ = len >> 24;
	*dst++ = len >> 16;
	*dst++ = len >> 8;
	*dst++ = len;
	memcpy(dst, src, len);
	dst += len;
}

int main(int argc, char* argv[])
{
	rsa_public_key pubkey;
	if (!pubkey.load("./pubkey")) {
		cout << "Pubkey load FAILED" << endl;
		return 1;
	}

	cout << "Pubkey load OK" << endl;

	unsigned char buff[4096];

	unsigned char *dst = buff;
	append(dst, (const unsigned char *)"ssh-rsa", 7);
	unsigned char temp[1024] = {};
	int len = BN_bn2bin(pubkey._rsa->e, temp+1);
	append(dst, temp+1, len);
	len = BN_bn2bin(pubkey._rsa->n, temp+1);
	int sign = (temp[1] & 0x80) != 0;
	append(dst, temp+1-sign, len+sign);
	len = dst - buff;

	printf("binary:\n");
	for (int i = 0; i < len; ++i) {
		printf("%02x:", buff[i]);
	}
	printf("\n");

	unsigned char sum[64];
	MD5(buff, len, sum);

	printf("fingerprint (MD5):\n");
	for (int i = 0; i < 16; ++i) {
		printf("%02x:", sum[i]);
	}
	printf("\n");

	return 0;
}
