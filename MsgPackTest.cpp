
#define WIN32_LEAN_AND_MEAN
#pragma warning( disable : 4003 )
#pragma warning( disable : 4244 )
#pragma warning( disable : 4267 )

#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <msgpack.hpp>

#pragma comment(lib, "ws2_32.lib")
#ifdef _DEBUG
#pragma comment(lib, "msgpackd.lib")
#else
#pragma comment(lib, "msgpack.lib")
#endif

namespace Hogehoge {

	struct Hoge {
		std::string hoge;

		struct Fuga {
			std::string fuga;
			std::string fugaga;

			MSGPACK_DEFINE( fuga, fugaga );
		};

		std::vector<Fuga> hogege;

		MSGPACK_DEFINE( hoge, hogege );
	};
}

using namespace std;
using namespace Hogehoge;

int main(int argc, char *argv[])
{
	const char *filename = "Hoge.dat";
	if (argc == 2) {
		filename = argv[1];
	}

	{
		Hoge hoge;
		hoge.hoge = "HOGE";
		Hoge::Fuga hogege;
		hogege.fuga = "FUGA";
		hogege.fugaga = "FUGAGA";
		hoge.hogege.push_back( hogege );
		msgpack::sbuffer buffer;
		msgpack::pack(buffer, hoge);

		fstream output(filename, ios::out | ios::trunc | ios::binary);
		if (output.is_open()) {
			output.write( buffer.data(), buffer.size() );
		}
	}

	try {
		struct _stat64 info;
		fstream input(filename, ios::in | ios::binary);
		if ( _stat64(filename, &info) == 0 && input.is_open() ) {
			msgpack::unpacker pac;
			pac.reserve_buffer( info.st_size );
			input.read( pac.buffer(), info.st_size );
			pac.buffer_consumed( info.st_size );
			if ( pac.execute() ) {
				msgpack::object o = pac.data();
				Hoge hoge;
				o.convert(&hoge);
				cout << "Hoge loaded" << endl;
				cout << "  hoge: " << hoge.hoge << endl;
				size_t i;
				for (i = 0; i < hoge.hogege.size(); ++i) {
					cout << "  hogege #" << i << endl;
					cout << "    fuga: " << hoge.hogege[0].fuga << endl;
					cout << "    fugaga: " << hoge.hogege[0].fugaga << endl;
				}
			}
		}
	} catch ( std::exception &err ) {
		cout << "ERROR: " << err.what() << endl;
	}

	return 0;
}

