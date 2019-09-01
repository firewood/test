#include <iostream>
#include <string>
#include <vector>
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

string encode_base64(const unsigned char *src, int srclen)
{
  BIO *b64 = BIO_new(BIO_f_base64());
	BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
  BIO *bmem = BIO_new(BIO_s_mem());
  b64 = BIO_push(b64, bmem);
  BIO_write(b64, src, srclen);
  BIO_flush(b64);
  BUF_MEM *bin_result;
  BIO_get_mem_ptr(b64, &bin_result);
  string str_result(bin_result->data, bin_result->length);
  BIO_free_all(b64);
  return str_result;
}

string trim_padding(string s) {
	while (!s.empty() && s.back() == '=') {
		s.pop_back();
	}
	return s;
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

	void append(const unsigned char *src, int len) {
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
		vector<string> algos = {"ssh-rsa", "ecdsa-sha2-nistp256"};
		for (const string algo : algos) {
			char *s = strstr(buff, algo.c_str());
			if (s) {
				s += algo.length();
				if (*s == ' ') {
					char *e = strchr(++s, ' ');
					int len = e ? e - s : strlen(s);
					_length = decode_base64(s, len, _buffer);
					if (_length > 0) {
						return true;
					}
				}
			}
		}
		return false;
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

	string fingerprint_md5() const {
		char buffer[256] = {};
		string f;
		unsigned char md5[16];
		EVP_Digest(_buffer, _length, md5, NULL, EVP_md5(), NULL);
		for (int i = 0; i < 16; ++i) {
			sprintf(buffer + strlen(buffer), ":%02x", md5[i]);
		}
		return buffer + 1;
	}

	string fingerprint_sha256() const {
		char buffer[256] = {};
		string f;
		unsigned char sha2[32] = {};
		EVP_Digest(_buffer, _length, sha2, NULL, EVP_sha256(), NULL);
		return "SHA256:" + trim_padding(encode_base64(sha2, 32));
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
	do {
		if (pubkey.load(fp)) {
			break;
		}
		if (pubkey.load_pem(fp)) {
			break;
		}
		cout << "Pubkey load FAILED" << endl;
		return 1;
	} while (false);
	fclose(fp);

	pubkey.dump();
	cout << "Fingerprint (MD5/hex):" << endl << pubkey.fingerprint_md5() << endl;
	cout << "Fingerprint (SHA256/base64):" << endl << pubkey.fingerprint_sha256() << endl;

	return 0;
}
