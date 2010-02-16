
#include <msgpack.hpp>
#include <iostream>
#include <string>

#define NUM_OF_SERIALIZE_MEMBERS(n) \
		msgpack::packer<msgpack::sbuffer>(buffer).pack_map(n);
#define SERIALIZE_MEMBER(m) \
		SerializeMember(buffer, #m, m);
#define DESERIALIZE_MEMBER(m) \
		DeserializeMember(o, k, #m, m);

template <typename T> void SerializeMember(msgpack::sbuffer &buffer, const char *name, T &m)
{
	msgpack::pack(buffer, std::string(name));
	msgpack::pack(buffer, m);
}

template <typename T> void DeserializeMember(msgpack::object &o, std::string &key, const char *name, T &m)
{
	if (key.compare(name) == 0) {
		o.convert(&m);
	}
}

struct Foo {
	std::string hoge;
	std::string fuga;
	int hogera;

	void Store(msgpack::sbuffer &buffer) {
		NUM_OF_SERIALIZE_MEMBERS(3)
		SERIALIZE_MEMBER(hoge)
		SERIALIZE_MEMBER(fuga)
		SERIALIZE_MEMBER(hogera)
	}
};

struct Bar {
	std::string hoge;
	std::string fuga;

	void Restore(msgpack::object &o) {
		if (o.type == msgpack::type::MAP) {
			msgpack::object_kv *p = o.via.map.ptr;
			for (size_t i = 0; i < o.via.map.size; ++i, ++p) {
				std::string k;
				p->key.convert(&k);
				msgpack::object &o = p->val;
				DESERIALIZE_MEMBER(hoge)
				DESERIALIZE_MEMBER(fuga)
			}
		}
		std::cout << "hoge: " << hoge << ", fuga: " << fuga << std::endl;
	}
};

int main(int argc, char* argv[])
{
	Foo foo;
	foo.hoge = "hoge1";
	foo.fuga = "fuga2";
	foo.hogera = 123;
	msgpack::sbuffer buffer;
	foo.Store(buffer);

	Bar bar;
	msgpack::unpacker pac;
	pac.reserve_buffer( buffer.size() );
	memcpy( pac.buffer(), buffer.data(), buffer.size() );
	pac.buffer_consumed( buffer.size() );
	if ( pac.execute() ) {
		msgpack::object o = pac.data();
		std::cout << o << std::endl;
		bar.Restore(o);
		pac.reset();
	}

	return 0;
}

