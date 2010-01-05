
//
// LRUSet.cpp
//

#include <iostream>
#include <string>
#include <memory>
#include <set>
#include <map>
#include <assert.h>

struct Content {
	std::string url;
	std::string content;
	Content(std::string url, std::string content) : url(url), content(content) { }
	bool operator < (const Content &c) const { return c.url < url; }
	void Print(void) const { std::cout << "  url: " << url << ", content: " << content << std::endl; }
};

template <class Value>
class LRUSet {
	struct SList {
		SList *next;
		Value *value;
		SList() : next(NULL), value(NULL) { }
	};
	typedef typename SList *SListPtr;
	struct SListPtrOps {
		// sort by value of next node
		bool operator()( const SListPtr &a, const SListPtr &b ) const { return *(a->next->value) < *(b->next->value); }
	};
	typedef typename std::set<SListPtr, SListPtrOps> SListPtrSet;
	SListPtrSet m_Set;
	size_t m_Size;
	SList *m_Oldest;
	SList *m_Newest;

	typename SListPtrSet::const_iterator Find(const Value &value) const {
		SList a, b;
		a.next = &b;
		b.value = const_cast<Value *>(&value);
		return m_Set.find(&a);
	}

public:
	LRUSet() : m_Size(0) {
		m_Oldest = new SList();
		m_Newest = m_Oldest;
	}

	void Add(Value &value) {
		SList *previous_node, *current_node, *next_node, *terminal_node;
		Value *previous_value, *current_value;
		typename SListPtrSet::const_iterator current_pos = Find(value);
		if (current_pos != m_Set.end()) {
			// update
			// 自ノードを取得
			current_node = *current_pos;
			// 次ノードを取得
			next_node = current_node->next;
			assert(next_node != NULL);
			if (next_node == m_Newest) {
				// current is the newest
				assert(next_node->next == NULL);
				return;
			}
			previous_value = current_node->value;
			current_value = next_node->value;

			// valueは次ノードに入っている値と同じはず
//			assert(value == *current_value);
			assert(!(value < *current_value) && !(*current_value < value));

			if (previous_value == NULL) {
				// current is the oldest
				assert(current_node == m_Oldest);
				m_Oldest = next_node;
			} else {
				typename SListPtrSet::const_iterator previous_pos = Find(*previous_value);
				// 値があるので前ノードが存在するはず
				assert(previous_pos != m_Set.end());
				// 前ノードを取得
				previous_node = *previous_pos;
				// 前ノードは自ノードを指しているはず
				assert(previous_node->next == current_node);
				// 前ノードが次ノードを指すように変更
				previous_node->next = next_node;
			}
			// 次ノードの値に自ノードが保持していた値（前ノードの値）を設定
			next_node->value = previous_value;
		} else {
			// insert
			current_node = new SList();
			current_value = new Value(value);
			SList temp_node;
			current_node->next = &temp_node;
			temp_node.value = current_value;
			m_Set.insert(current_node);
			++m_Size;
		}

		// 終端ノードを取得
		terminal_node = m_Newest->next;
		if (terminal_node != NULL) {
			// 1つ以上のノードが存在
			m_Newest->next = current_node;
		} else {
			// 値を持ったノードをはじめて追加する
			assert(m_Size == 1);
			assert(m_Oldest == m_Newest);
			m_Oldest = current_node;
			terminal_node = m_Newest;
		}
		// 元々終端ノードに入っていた値を自ノードに設定
		current_node->value = terminal_node->value;
		// 自ノードの次を終端ノードに設定
		current_node->next = terminal_node;
		// 終端ノードに自ノードの値を設定
		terminal_node->value = current_value;
		// 自ノードをNewestに設定
		m_Newest = current_node;
	}

	void Dump(void) const {
		std::cout << "Dump:" << std::endl;
		int i;
		SList *p;
		for (i = 0, p = m_Oldest; p != NULL; ++i, p = p->next) {
			std::cout << "  " << i << ": next: " << p->next;
			if (p->value) {
				p->value->Print();
			} else {
				std::cout << "  (null)" << std::endl;
			}
		}
	}
};

int main(int argc, char *argv[])
{
	LRUSet<Content> cache;

	cache.Dump();

	cache.Add(Content("abc", "aaa"));
	cache.Dump();

	cache.Add(Content("def", "ddd"));
	cache.Dump();

	cache.Add(Content("ghi", "ggg"));
	cache.Dump();

	cache.Add(Content("jkl", "jjj"));
	cache.Dump();

	cache.Add(Content("def", "ddd"));
	cache.Dump();

	return 0;
}

