#include <iostream>
#include <openssl/rsa.h> 
#include <openssl/pem.h> 
#include <openssl/rand.h> 
#ifdef _WIN32
#include <openssl/applink.c>
#ifdef _DEBUG
#pragma comment( lib, "libeay32MTd.lib" )
#endif
#endif

using namespace std;

struct rsa_public_key {
	RSA *_rsa;
	bool load(const char *filename) {
		FILE *fp = fopen(filename, "rt");
		if (fp == NULL) {
			return false;
		}
		do {
			_rsa = PEM_read_RSAPublicKey(fp, NULL, NULL, NULL);
			if (_rsa) {
				break;
			}
			rewind(fp);
			_rsa = PEM_read_RSA_PUBKEY(fp, NULL, NULL, NULL);
		} while (false);
		fclose(fp);
		return _rsa != NULL;
	}
};

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
	OpenSSL_add_all_algorithms();
	rsa_public_key pubkey;
	if (!pubkey.load("./pubkey")) {
		cout << "Pubkey load FAILED" << endl;
		return 1;
	}

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

	printf("Source binary:\n");
	for (int i = 0; i < len; ++i) {
		printf("%02x:", buff[i]);
	}
	printf("\n\n");

	unsigned char md5[64];
	EVP_Digest(buff, len, md5, NULL, EVP_md5(), NULL);

	printf("Fingerprint:\n");
	for (int i = 0; i < 16; ++i) {
		printf("%s%02x", i ? ":" : "", md5[i]);
	}
	printf("\n");

	return 0;
}
