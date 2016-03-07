#include <iostream>
#include <string>
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

int decode_base64(const char *src, int srclen, unsigned char *dst)
{
	BIO *b64 = BIO_new(BIO_f_base64());
	BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
	BIO *bmem = BIO_new_mem_buf((void *)src, srclen);
	bmem = BIO_push(b64, bmem);
	int dstlen = BIO_read(bmem, dst, srclen);
	BIO_free_all(bmem);
	return dstlen;
}

struct rsa_public_key {
	RSA *_rsa;
	bool load(FILE *fp) {
		if (!fp) {
			return false;
		}
		rewind(fp);
		_rsa = PEM_read_RSAPublicKey(fp, NULL, NULL, NULL);
		if (_rsa) {
			return true;
		}
		rewind(fp);
		_rsa = PEM_read_RSA_PUBKEY(fp, NULL, NULL, NULL);
		return _rsa != NULL;
	}
};

struct openssh_public_key {
	unsigned char _buffer[2048];
	int _length;
	void append(const unsigned char *src, int len)
	{
		_buffer[_length++] = len >> 24;
		_buffer[_length++] = len >> 16;
		_buffer[_length++] = len >> 8;
		_buffer[_length++] = len;
		memcpy(_buffer + _length, src, len);
		_length += len;
	}
	bool load(FILE *fp) {
		if (!fp) {
			return false;
		}
		rewind(fp);
		char buff[4096] = {};
		fread(buff, 1, 4096, fp);
		char *s = strstr(buff, "ssh-rsa ");
		if (!s) {
			return false;
		}
		s += 8;
		char *e = strchr(s, ' ');
		int len = e - s;
		_length = decode_base64(s, len, _buffer);
		return _length > 0;
	}
	bool load_pem(FILE *fp) {
		rsa_public_key pubkey;
		if (!pubkey.load(fp)) {
			return false;
		}
		_length = 0;
		append((const unsigned char *)"ssh-rsa", 7);
		unsigned char temp[1024] = {};
		int len = BN_bn2bin(pubkey._rsa->e, temp + 1);
		append(temp + 1, len);
		len = BN_bn2bin(pubkey._rsa->n, temp + 1);
		int sign = (temp[1] & 0x80) != 0;
		append(temp + 1 - sign, len + sign);
		return true;
	}
	string fingerprint() const {
		char buffer[256] = {};
		string f;
		unsigned char md5[16];
		EVP_Digest(_buffer, _length, md5, NULL, EVP_md5(), NULL);
		for (int i = 0; i < 16; ++i) {
			sprintf(buffer + strlen(buffer), ":%02x", md5[i]);
		}
		return buffer + 1;
	}
	void dump() const {
		printf("Source binary:\n");
		for (int i = 0; i < _length; ++i) {
			printf("%02x:", _buffer[i]);
		}
		printf("\n\n");
	}
};

int main(int argc, char* argv[])
{
	OpenSSL_add_all_algorithms();

	const char *filename = argc >= 2 ? argv[1] : "pubkey";
	FILE *fp = fopen(filename, "rb");
	openssh_public_key pubkey;
	if (!pubkey.load(fp)) {
		if (!pubkey.load_pem(fp)) {
			cout << "Pubkey load FAILED" << endl;
			return 1;
		}
	}
	fclose(fp);

	pubkey.dump();
	cout << "Fingerprint:" << endl << pubkey.fingerprint() << endl;

	return 0;
}
