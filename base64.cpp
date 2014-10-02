
#include <string>


//

std::string bin_to_hex(const unsigned char *buffer, int bytes)
{
	std::string hex;
	int i;
	for (i = 0; i < bytes; ++i) {
		char _hex[4];
		sprintf(_hex, "%02X", buffer[i]);
		hex += _hex;
	}
	return hex;
}


//

std::string bin_to_base32(const unsigned char *buffer, int bytes)
{
	const char table[] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','2','3','4','5','6','7'};
	unsigned int a = 0, b = 0;
	int size = 0;
	char result[1024] = {};
	while (bytes > 0) {
		a = (a << 8) | *buffer++;
		b += 8;
		--bytes;
		while (b >= 5) {
			b -= 5;
			result[size++] = table[a >> b];
			a &= ((1 << b) - 1);
		}
	}
	if (b > 0) {
		result[size++] = table[a << (5-b)];
	}
	return result;
}


//

std::string bin_to_base64(const unsigned char *buffer, int bytes)
{
	const char table[] = {
		'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
		'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
		'0','1','2','3','4','5','6','7','8','9','+','/'
	};
	unsigned char src[1024] = {};
	char dst[1024] = {};
	memcpy(src, buffer, bytes);
	for (int i = 0, w = 0; i < bytes; i += 3, w += 4) {
		unsigned char a = src[i];
		unsigned char b = src[i+1];
		unsigned char c = src[i+2];
		dst[w] = table[a >> 2];
		dst[w + 1] = table[((a & 3) << 4) | (b >> 4)];
		dst[w + 2] = (i+1 >= bytes) ? '=' : table[((b & 15) << 2) | (c >> 6)];
		dst[w + 3] = (i+2 >= bytes) ? '=' : table[c & 63];
	}
	return dst;
}


//

int hex_to_bin(const char *hex, unsigned char *buffer, int buffer_size)
{
	int length = 0;
	while (hex && *hex && (buffer_size < 0 || length < buffer_size)) {
		unsigned int c;
		if (sscanf(hex, "%02X", &c) != 1) {
			return -1;
		}
		*buffer++ = (unsigned char)c;
		hex += 2;
		++length;
	}
	return length;
}


//

int base64_to_bin(const char *encoded, unsigned char *buffer, int buffer_size)
{
	const char table[] = {
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,62,0,0,0,63, 52,53,54,55,56,57,58,59,60,61,0,0,0,0,0,0,
		0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14, 15,16,17,18,19,20,21,22,23,24,25,0,0,0,0,0,
		0,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40, 41,42,43,44,45,46,47,48,49,50,51,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	};

	int len = strlen(encoded);
	if (len % 4) {
		return -1;
	}
	int written = 0;
	for (int i = 0; i < len; i += 4, written += 3) {
		int a = table[encoded[i]];
		int b = table[encoded[i+1]];
		int c = table[encoded[i+2]];
		int d = table[encoded[i+3]];
		buffer[written] = (a << 2) | (b >> 4);
		buffer[written+1] = (b << 4) | (c >> 2);
		buffer[written+2] = (c << 6) | d;
		written -= encoded[i+2] == '=';
		written -= encoded[i+3] == '=';
	}
	return written;
}

