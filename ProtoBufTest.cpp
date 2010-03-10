
#define WIN32_LEAN_AND_MEAN
#pragma warning( disable : 4244 )
#pragma warning( disable : 4267 )

#include <iostream>
#include <fstream>
#include <string>
#include "Hoge.pb.h"

using namespace std;
using namespace Hogehoge;

#ifdef _DEBUG
#pragma comment(lib, "libprotobufd.lib")
#else
#pragma comment(lib, "libprotobuf.lib")
#endif

int main(int argc, char* argv[]) {
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	const char *filename = "Hoge.dat";
	if (argc == 2) {
		filename = argv[1];
	}

	{
		Hoge hoge;
		hoge.set_hoge("HOGE");
		hoge.add_hogege();
		hoge.mutable_hogege(0)->set_fuga("FUGA");
		hoge.mutable_hogege(0)->set_fugaga("FUGAGA");

		fstream output(filename, ios::out | ios::trunc | ios::binary);
		if (output.is_open() && hoge.SerializeToOstream(&output)) {
			cout << "save OK" << endl;
		}
	}

	{
		Hoge hoge;
		fstream input(filename, ios::in | ios::binary);
		if (input.is_open() && hoge.ParseFromIstream(&input)) {
			cout << "Hoge loaded" << endl;
			cout << "  hoge: " << hoge.hoge() << endl;
			size_t i;
			for (i = 0; i < hoge.hogege_size(); ++i) {
				cout << "  hogege #" << i << endl;
				cout << "    fuga: " << hoge.hogege(0).fuga() << endl;
				cout << "    fugaga: " << hoge.hogege(0).fugaga() << endl;
			}
		}
	}

	google::protobuf::ShutdownProtobufLibrary();

	return 0;
}

