// Created by Grigore Stefan <g_stefan@yahoo.com>
// Public domain (Unlicense) <http://unlicense.org>
// SPDX-FileCopyrightText: 2016-2026 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: Unlicense

// Containers: bounds, element lifetime, TXArray, TXList5, TRedBlackTree, TRedBlackTreeOne

#include <XYO/DataStructures.hpp>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>

using namespace XYO::DataStructures;

static int failures = 0;
#define CHECK(cond, ...)                                           \
	do {                                                       \
		if (!(cond)) {                                     \
			++failures;                                \
			printf("FAIL %s:%d ", __FILE__, __LINE__); \
			printf(__VA_ARGS__);                       \
			printf("\n");                              \
		};                                                 \
	} while (0)

// --- object with live-instance counter, system allocated so delete runs the destructor
struct Probe : Object {
		static int alive;
		int x = 0;
		Probe() { ++alive; };
		~Probe() { --alive; };
};
int Probe::alive = 0;

namespace XYO::ManagedMemory {
	template <>
	struct TMemory<Probe> : TMemorySystem<Probe> {};

	template <>
	struct TComparator<Probe> {
			static inline bool isEqual(const Probe &a, const Probe &b) {
				return a.x == b.x;
			};

			static inline bool isLess(const Probe &a, const Probe &b) {
				return a.x < b.x;
			};

			static inline int compare(const Probe &a, const Probe &b) {
				return (a.x < b.x) ? -1 : ((a.x > b.x) ? 1 : 0);
			};
	};
};

static TPointer<Probe> makeProbe(int x) {
	TPointer<Probe> p;
	p.newMemory();
	p->x = x;
	return p;
};

// ---

template <size_t P>
static void dynArrayRandom(unsigned seed, size_t initialIndex) {
	TDynamicArray<std::string, P, TMemorySystem> a(initialIndex);
	std::vector<std::string> ref;
	srand(seed);
	for (int step = 0; step < 4000; ++step) {
		int op = rand() % 9;
		std::string v = std::to_string(rand());
		switch (op) {
		case 0:
		case 1:
			a.push(v);
			ref.push_back(v);
			break;
		case 2: {
			size_t i = ref.empty() ? 0 : rand() % (ref.size() + 1);
			if (i >= ref.size()) {
				a.insert(i) = v;
				ref.resize(i + 1);
				ref[i] = v;
			} else {
				a.insert(i) = v;
				ref.insert(ref.begin() + i, v);
			};
		}; break;
		case 3:
			if (!ref.empty()) {
				size_t i = rand() % ref.size();
				CHECK(a.remove(i), "remove");
				ref.erase(ref.begin() + i);
			};
			break;
		case 4: {
			std::string out;
			bool ok = a.shift(out);
			CHECK(ok == !ref.empty(), "shift ok");
			if (ok) {
				CHECK(out == ref.front(), "shift value");
				ref.erase(ref.begin());
			};
		}; break;
		case 5:
			if (rand() % 20 == 0) {
				a.empty();
				ref.clear();
			};
			break;
		case 6:
			if (rand() % 10 == 0) {
				size_t n = ref.empty() ? 0 : rand() % ref.size();
				a.setLength(n);
				ref.resize(n);
			};
			break;
		case 7:
			a.set(ref.size(), v);
			ref.push_back(v);
			break;
		case 8:
			a.copy(a);
			break;
		};
		CHECK(a.length() == ref.size(), "length %zu vs %zu at step %d", a.length(), ref.size(), step);
		if (a.length() != ref.size()) {
			return;
		};
	};
	for (size_t k = 0; k < ref.size(); ++k) {
		std::string x;
		CHECK(a.get(k, x) && x == ref[k], "value at %zu", k);
	};
};

static void dynArrayExactFull() {
	for (size_t n = 0; n <= 64; ++n) {
		TDynamicArray<int, 4, TMemorySystem> a;
		for (size_t k = 0; k < n; ++k) {
			a.push((int)k);
		};
		a.empty(); // used to read past the index table when n % 16 == 0
		CHECK(a.length() == 0, "empty n=%zu", n);

		TDynamicArray<int, 4, TMemorySystem> b;
		for (size_t k = 0; k < n; ++k) {
			b.push((int)k);
		};
		if (n > 0) {
			b.insert(0) = -1; // used to write past the index table when full
			int x = 0;
			CHECK(b.length() == n + 1 && b.get(0, x) && x == -1, "insert n=%zu", n);
			CHECK(b.get(n, x) && x == (int)n - 1, "insert tail n=%zu", n);
		};
	};
};

static void dynArrayRetention() {
	{
		TDynamicArray<TPointer<Probe>, 4, TMemorySystem> a;
		for (int k = 0; k < 40; ++k) {
			a.push(makeProbe(k));
		};
		CHECK(Probe::alive == 40, "alive %d", Probe::alive);
		a.setLength(10);
		CHECK(Probe::alive == 10, "setLength shrink releases, alive %d", Probe::alive);
		a.setLength(20);
		TPointer<Probe> p;
		CHECK(a.get(15, p) && !p, "regrown slot is empty");
		a.setLength(1);
		TPointer<Probe> out;
		CHECK(a.shift(out) && out && out->x == 0, "shift last");
		out = nullptr;
		CHECK(Probe::alive == 0, "shift to empty releases, alive %d", Probe::alive);
		a.push(makeProbe(7));
		CHECK(a.remove(0) && Probe::alive == 0, "remove to empty releases, alive %d", Probe::alive);
	};
	CHECK(Probe::alive == 0, "final alive %d", Probe::alive);
};

static void xArray() {
	TXArray<int>::ArrayT arr;
	TXArray<int>::constructor(arr, 4);
	for (int k = 0; k < 4; ++k) {
		TXArray<int>::set(arr, k, k);
	};
	CHECK(!TXArray<int>::insert(arr, 0, 99), "insert on full array must fail");
	CHECK(arr.length == 4, "length unchanged");
	TXArray<int>::resize(arr, 8);
	CHECK(arr.size == 8 && arr.length == 4, "resize");
	CHECK(TXArray<int>::insert(arr, 0, 99), "insert after resize");
	int v = 0;
	CHECK(TXArray<int>::get(arr, 0, v) && v == 99, "insert value");
	CHECK(TXArray<int>::get(arr, 4, v) && v == 3, "shifted tail");
	TXArray<int>::resize(arr, 2);
	CHECK(arr.size == 2 && arr.length == 2, "resize shrink");
	TXArray<int>::destructor(arr);
};

static void xArrayInsertAlias() {
	typedef TXArray<std::string> XA;
	XA::ArrayT arr;
	XA::constructor(arr, 8);
	XA::set(arr, 0, "a");
	XA::set(arr, 1, "b");
	XA::set(arr, 2, "c");
	// value is an element of the array after index, the shift moves it: used to insert "a"
	CHECK(XA::insert(arr, 0, arr.root[1]), "insert own element after index");
	CHECK(arr.length == 4 && arr.root[0] == "b" && arr.root[1] == "a" && arr.root[2] == "b" && arr.root[3] == "c",
	      "insert own element after index: %s%s%s%s", arr.root[0].c_str(), arr.root[1].c_str(), arr.root[2].c_str(), arr.root[3].c_str());
	CHECK(XA::insert(arr, 1, arr.root[1]), "insert own element at index");
	CHECK(XA::insert(arr, 4, arr.root[0]), "insert own element before index");
	CHECK(XA::insert(arr, 5, arr.root[5]), "insert last element before itself");
	std::string all;
	for (size_t k = 0; k < arr.length; ++k) {
		all += arr.root[k];
	};
	CHECK(all == "baabbcc", "insert alias sequence: %s", all.c_str());
	XA::destructor(arr);
};

static void queueAndStack() {
	TDoubleEndedQueue<std::string, TMemorySystem> q;
	q.pushToTail(std::string("a"));
	q.pushToTail(std::string("b"));
	q.push(std::string("z"));
	std::string s;
	CHECK(q.pop(s) && s == "z", "deque pop");
	CHECK(q.popFromTail(s) && s == "b", "deque popFromTail");
	CHECK(q.pop(s) && s == "a", "deque pop 2");
	CHECK(!q.pop(s) && q.isEmpty(), "deque empty");

	TStack<std::string, TMemorySystem> st;
	st.push(std::string("1"));
	st.push(std::string("2"));
	CHECK(st.pop(s) && s == "2", "stack pop");
	CHECK(st.pop(s) && s == "1", "stack pop 2");
	CHECK(!st.pop(s), "stack empty");

	{
		TDoubleEndedQueue<TPointer<Probe>, TMemorySystem> qp;
		qp.push(makeProbe(1));
		TPointer<Probe> p;
		CHECK(qp.pop(p) && p->x == 1, "deque pointer pop");
		p = nullptr;
		CHECK(Probe::alive == 0, "deque pop released, alive %d", Probe::alive);
	};
};

struct Tree5Node : TXList5Node<Tree5Node> {
		int id;
};
typedef TXList5<Tree5Node> Tree5;

static Tree5Node *node5(int id) {
	Tree5Node *n = Tree5::newNode();
	n->id = id;
	n->back = n->next = n->childHead = n->childTail = n->parent = nullptr;
	return n;
};

static void xList5() {
	Tree5Node *root = node5(0);
	Tree5Node *a = node5(1);
	Tree5Node *b = node5(2);
	Tree5::pushToTail(root, a);
	Tree5::pushToTail(root, b);
	Tree5::transferChild(root, a); // a has no children: used to dereference nullptr
	Tree5Node *c = node5(3);
	Tree5::pushToTail(a, c);
	Tree5::transferChild(root, a);
	CHECK(root->childTail == c && c->parent == root && a->childHead == nullptr, "transferChild");
	Tree5::destructor(root);
};

static void rbTreePointerValue() {
	{
		TRedBlackTree<int, TPointer<Probe>, TMemorySystem> t;
		t.set(1, makeProbe(10));
		TPointer<Probe> def = makeProbe(99);
		TPointer<Probe> r = t.getValue(1, def.value());
		CHECK(r && r->x == 10, "getValue found");
		r = t.getValue(2, def.value());
		CHECK(r && r->x == 99, "getValue default");
	};
	CHECK(Probe::alive == 0, "rb alive %d", Probe::alive);
};

// Red-black invariants: parent links, no red node with a red child, same black height,
// return the black height of node
template <typename TNode>
static int rbCheck(TNode *node, TNode *parent, bool &ok) {
	if (node == nullptr) {
		return 1;
	};
	if (node->parent != parent) {
		ok = false;
	};
	if (node->color == TNode::Red) {
		if ((node->left && node->left->color == TNode::Red) || (node->right && node->right->color == TNode::Red)) {
			ok = false;
		};
	};
	int left = rbCheck(node->left, node, ok);
	int right = rbCheck(node->right, node, ok);
	if (left != right) {
		ok = false;
	};
	return left + ((node->color == TNode::Black) ? 1 : 0);
};

template <typename TNode>
static bool rbValid(TNode *root) {
	bool ok = true;
	rbCheck(root, static_cast<TNode *>(nullptr), ok);
	return ok && ((root == nullptr) || (root->color == TNode::Black));
};

// set() finds and inserts with one descent (TXRedBlackTree::insertNodeAt)
static void rbTreeSet() {
	typedef TRedBlackTree<int, int, TMemorySystem> Tree;
	typedef TRedBlackTreeOne<int, TMemorySystem> TreeOne;
	Tree t;
	TreeOne one;
	std::map<int, int> ref;
	srand(5);
	for (int step = 0; step < 20000; ++step) {
		int k = rand() % 1000;
		if (rand() % 4 == 0) {
			bool had = (ref.erase(k) > 0);
			CHECK(t.remove(k) == had, "remove %d", k);
			CHECK(one.remove(k) == had, "one remove %d", k);
		} else {
			t.set(k, step);
			one.set(k);
			ref[k] = step;
		};
	};
	CHECK(rbValid(t.root), "set keeps red-black invariants");
	CHECK(rbValid(one.root), "one set keeps red-black invariants");

	bool same = true;
	size_t count = 0;
	std::map<int, int>::iterator it = ref.begin();
	for (Tree::Node *node = t.begin(); node != nullptr; node = node->successor(), ++it, ++count) {
		if (it == ref.end() || node->key != it->first || node->value != it->second) {
			same = false;
			break;
		};
	};
	CHECK(same && count == ref.size(), "set content %zu vs %zu", count, ref.size());

	same = true;
	count = 0;
	it = ref.begin();
	for (TreeOne::Node *node = one.begin(); node != nullptr; node = node->successor(), ++it, ++count) {
		if (it == ref.end() || node->key != it->first) {
			same = false;
			break;
		};
	};
	CHECK(same && count == ref.size(), "one set content %zu vs %zu", count, ref.size());

	// Key by pointer: set through the pointer and through the raw key reach the same node
	{
		typedef TRedBlackTree<TPointer<Probe>, int, TMemorySystem> TreeP;
		TreeP tp;
		for (int k = 0; k < 64; ++k) {
			tp.set(makeProbe(k * 2), k);
		};
		// lvalue values: set(const TKeyType *, rvalue) is ambiguous with set(const TKey &, TValue &&)
		int value100 = 100;
		int value7 = 7;
		TPointer<Probe> existing = makeProbe(20);
		tp.set(existing.value(), value100);
		TPointer<Probe> missing = makeProbe(21);
		tp.set(missing.value(), value7);
		int v = 0;
		CHECK(tp.get(makeProbe(20), v) && v == 100, "raw key set existing");
		CHECK(tp.get(makeProbe(21), v) && v == 7, "raw key set missing");
		count = 0;
		int last = -1;
		same = true;
		for (TreeP::Node *node = tp.begin(); node != nullptr; node = node->successor(), ++count) {
			if (node->key->x <= last) {
				same = false;
			};
			last = node->key->x;
		};
		CHECK(same && count == 65 && rbValid(tp.root), "pointer key tree, %zu nodes", count);

		TRedBlackTreeOne<TPointer<Probe>, TMemorySystem> op;
		op.set(makeProbe(1));
		op.set(existing.value());
		op.set(existing.value());
		op.set(makeProbe(20));
		CHECK(op.has(makeProbe(1)) && op.has(existing.value()) && op.begin()->successor()->successor() == nullptr, "one pointer key set");
	};
	CHECK(Probe::alive == 0, "rb set alive %d", Probe::alive);
};

// Backward iteration with node->predecessor() gives the forward order reversed
static void rbTreePredecessor() {
	TRedBlackTree<int, int, TMemorySystem> t;
	TRedBlackTreeOne<int, TMemorySystem> one;
	for (int k = 0; k < 100; ++k) {
		t.set((k * 37) % 101, k);
		one.set((k * 37) % 101);
	};

	std::vector<int> forward;
	std::vector<int> backward;
	for (auto *node = t.begin(); node != nullptr; node = node->successor()) {
		forward.push_back(node->key);
	};
	for (auto *node = t.end(); node != nullptr; node = node->predecessor()) {
		backward.push_back(node->key);
	};
	std::reverse(backward.begin(), backward.end());
	CHECK(forward.size() == 100 && forward == backward, "predecessor order");

	forward.clear();
	backward.clear();
	for (auto *node = one.begin(); node != nullptr; node = node->successor()) {
		forward.push_back(node->key);
	};
	for (auto *node = one.end(); node != nullptr; node = node->predecessor()) {
		backward.push_back(node->key);
	};
	std::reverse(backward.begin(), backward.end());
	CHECK(forward.size() == 100 && forward == backward, "one predecessor order");
};

// A slot that is not in use reads as T(), also for types without lifetime hooks:
// never written, removed, or in a node reused from an active pool
template <size_t P, template <typename U> class TNodeMemory>
static bool dynArrayZeroFrom(TDynamicArray<int, P, TNodeMemory> &a, size_t from) {
	int x = -1;
	for (size_t k = from; k < a.length(); ++k) {
		if (!a.get(k, x) || x != 0) {
			return false;
		};
	};
	return true;
};

static void dynArraySlotReset() {
	{
		TDynamicArray<int, 2, TMemorySystem> a;
		a[20] = 1;
		CHECK(a.length() == 21 && dynArrayZeroFrom(a, 0) == false && a.index(20) == 1, "gap write");
		a.setLength(20);
		CHECK(dynArrayZeroFrom(a, 0), "never written slots are 0");
	};
	{
		TDynamicArray<int, 2, TMemorySystem> a;
		for (int k = 0; k < 10; ++k) {
			a.push(k + 1);
		};
		a.setLength(3);
		a.setLength(10);
		CHECK(dynArrayZeroFrom(a, 3), "setLength shrink resets");
		for (int k = 0; k < 10; ++k) {
			a.set(k, k + 1);
		};
		CHECK(a.remove(0), "remove");
		a.setLength(10);
		CHECK(dynArrayZeroFrom(a, 9), "remove resets the last slot");
		for (int k = 0; k < 10; ++k) {
			a.set(k, k + 1);
		};
		int out = 0;
		CHECK(a.shift(out) && out == 1, "shift");
		a.setLength(10);
		CHECK(dynArrayZeroFrom(a, 9), "shift resets the last slot");
		a.empty();
		a.setLength(10);
		CHECK(dynArrayZeroFrom(a, 0), "empty resets");
	};
	{
		TDynamicArray<std::string, 2, TMemorySystem> a;
		for (int k = 0; k < 10; ++k) {
			a.push(std::to_string(k));
		};
		a.setLength(0);
		a.setLength(10);
		bool clean = true;
		std::string x;
		for (size_t k = 0; k < 10; ++k) {
			if (!a.get(k, x) || !x.empty()) {
				clean = false;
			};
		};
		CHECK(clean, "strings regrow empty");
	};
	{
		typedef TDynamicArray<int, 2, TMemoryPoolActive> ActiveArray;
		{
			ActiveArray a;
			for (int k = 0; k < 64; ++k) {
				a.push(k + 1);
			};
		}; // nodes go back to the active pool, still constructed
		ActiveArray b;
		b.setLength(64);
		CHECK(dynArrayZeroFrom(b, 0), "node reused from an active pool is reset");
	};
};

int main(int cmdN, char *cmdS[]) {
	try {

		dynArrayExactFull();
		dynArrayRandom<2>(1, 1);
		dynArrayRandom<4>(2, 1);
		dynArrayRandom<3>(3, 0);
		dynArrayRandom<1>(4, 3);
		dynArrayRetention();
		dynArraySlotReset();
		xArray();
		xArrayInsertAlias();
		queueAndStack();
		xList5();
		rbTreePointerValue();
		rbTreeSet();
		rbTreePredecessor();

		printf(failures ? "* FAILED: %d\n" : "test.02: all passed\n", failures);
		return failures ? 1 : 0;

	} catch (const std::exception &e) {
		printf("* Error: %s\n", e.what());
	} catch (...) {
		printf("* Error: Unknown\n");
	};

	return 1;
};
