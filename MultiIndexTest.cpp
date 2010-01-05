
//
// MultiIndexTest.cpp
//

#include <iostream>
#include <string>
#include <algorithm>
#include <boost/mem_fn.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>

using namespace boost::multi_index;

struct Content {
	std::string url;
	std::string content;
	time_t last_access;
	Content(std::string url, std::string content) : url(url), content(content) { }
	bool operator < (const Content &c) const { return c.url < url; }
	void Print(void) const { std::cout << "  url: " << url << ", content: " << content << ", la: " << last_access << std::endl; }
};

class LRUSet {
	typedef multi_index_container<
		Content,
		indexed_by<
//			ordered_unique<tag<by_url>, member<Content, std::string, &Content::url> >,
//			ordered_non_unique<tag<by_last_access>, member<Content, time_t, &Content::last_access> >
			ordered_unique<member<Content, std::string, &Content::url> >,
			ordered_non_unique<member<Content, time_t, &Content::last_access> >
		>
	> ContentHolder;
	struct change_la {
		time_t la;
		change_la(time_t la) : la(la) {}
		void operator()(Content& e) { e.last_access = la; }
	};

	ContentHolder m_Set;
	int m_Sequence;

	typedef ContentHolder::nth_index<0>::type index_by_url;
	typedef ContentHolder::nth_index<1>::type index_by_last_access;

public:
	LRUSet() : m_Sequence(0) { }

	bool Get(Content &content) {
		index_by_url &index = m_Set.get<0>();
		index_by_url::iterator it = index.find(content.url);
		if (it == index.end()) {
			return false;
		}
		index.modify(it, change_la(++m_Sequence));
		content = *it;
		return true;
	}

	bool Has(const Content &content) const {
		const index_by_url &index = m_Set.get<0>();
		index_by_url::const_iterator it = index.find(content.url);
		return it != index.end();
	}

	void Add(Content &content) {
		content.last_access = ++m_Sequence;
		index_by_url &index = m_Set.get<0>();
		index_by_url::iterator it = index.find(content.url);
		if (it == index.end()) {
			// add
			m_Set.insert(content);
		} else {
			// update
			index.replace(it, content);
		}
	}

	template <int Index>
	void print_by_index(void) {
		typename ContentHolder::nth_index<Index>::type const &i = m_Set.get<Index>();
		std::for_each(i.begin(), i.end(), boost::mem_fn(&Content::Print));
		std::cout << std::endl;
	}

	void Dump(void) {
		std::cout << "By url:" << std::endl;
		print_by_index<0>();
		std::cout << "By last access:" << std::endl;
		print_by_index<1>();
		std::cout << std::endl;
	}
};

int main(int argc, char *argv[])
{
	LRUSet cache;
	cache.Add(Content("jkl", "jjj"));
	cache.Add(Content("def", "ddd"));
	cache.Add(Content("abc", "aaa"));
	cache.Dump();

	std::cout << "Get def" << std::endl;
	cache.Get(Content("def", ""));
	cache.Dump();

	return 0;
}

