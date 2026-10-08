// Data Structures
// Copyright (c) 2016-2026 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2026 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_DATASTRUCTURES_TREDBLACKTREE_HPP
#define XYO_DATASTRUCTURES_TREDBLACKTREE_HPP

#ifndef XYO_DATASTRUCTURES_TREDBLACKTREENODE_HPP
#	include <XYO/DataStructures/TRedBlackTreeNode.hpp>
#endif

namespace XYO::DataStructures {

	template <typename TKey, typename TValue, template <typename U> class TNodeMemory = TMemory>
	class TRedBlackTree : public Object {
			XYO_PLATFORM_DISALLOW_COPY_ASSIGN_MOVE(TRedBlackTree);

		public:
			typedef TRedBlackTreeNode<TKey, TValue, TNodeMemory> TNode;
			typedef TXRedBlackTree<TNode, TNodeMemory> TXRBTree;
			typedef TNode Node;

			typedef typename TPointerTypeExclude<TKey>::Type TKeyType;
			typedef typename TPointerTypeExclude<TValue>::Type TValueType;
			typedef typename TPointerTypeExclude<TValue>::Pointer TPointerTValue;
			typedef typename TPointerTypeExclude<TValue>::PointerX TPointerXTValue;

			static inline TNode *newNode() {
				return TXRBTree::newNode();
			};

			static inline void deleteNode(TNode *this_) {
				return TXRBTree::deleteNode(this_);
			};

			static inline void initMemory() {
				TIfHasInitMemory<TKey>::initMemory();
				TIfHasInitMemory<TValue>::initMemory();
				TXRBTree::initMemory();
			};

			TNode *root;

			inline TRedBlackTree() {
				TXRBTree::constructor(root);
			};

			inline ~TRedBlackTree() {
				TXRBTree::destructor(root);
			};

			inline void empty() {
				TXRBTree::empty(root);
			};

			[[nodiscard]] inline TNode *find(const TKey &key) {
				return TXRBTree::find(root, key);
			};

			[[nodiscard]] inline TNode *find(const TKeyType *key) {
				TNode *x;
				int compare;
				for (x = root; x;) {
					compare = TComparator<TKeyType>::compare(*key, *(x->key));
					if (compare == 0) {
						return x;
					};
					if (compare < 0) {
						x = x->left;
					} else {
						x = x->right;
					};
				};
				return x;
			};

			inline void set(const TKey &key, const TValue &value) {
				TNode *parent;
				bool isLeft;
				TNode *node = findPosition(key, parent, isLeft);
				if (node) {
					node->value = value;
					return;
				};
				node = TXRBTree::newNode();
				TIfHasPointerLink<TKey>::pointerLink(&node->key, this);
				TIfHasPointerLink<TValue>::pointerLink(&node->value, this);
				node->key = key;
				node->value = value;
				TXRBTree::insertNodeAt(root, parent, isLeft, node);
			};

			inline void set(const TKey &key, TValue &&value) {
				TNode *parent;
				bool isLeft;
				TNode *node = findPosition(key, parent, isLeft);
				if (node) {
					node->value = std::move(value);
					return;
				};
				node = TXRBTree::newNode();
				TIfHasPointerLink<TKey>::pointerLink(&node->key, this);
				TIfHasPointerLink<TValue>::pointerLink(&node->value, this);
				node->key = key;
				node->value = std::move(value);
				TXRBTree::insertNodeAt(root, parent, isLeft, node);
			};

			inline void set(const TKey &key, const TValueType *value) {
				TNode *parent;
				bool isLeft;
				TNode *node = findPosition(key, parent, isLeft);
				if (node) {
					node->value = value;
					return;
				};
				node = TXRBTree::newNode();
				TIfHasPointerLink<TKey>::pointerLink(&node->key, this);
				TIfHasPointerLink<TValue>::pointerLink(&node->value, this);
				node->key = key;
				node->value = value;
				TXRBTree::insertNodeAt(root, parent, isLeft, node);
			};

			inline void set(const TKeyType *key, const TValue &value) {
				TNode *parent;
				bool isLeft;
				TNode *node = findPosition(key, parent, isLeft);
				if (node) {
					node->value = value;
					return;
				};
				node = TXRBTree::newNode();
				TIfHasPointerLink<TKey>::pointerLink(&node->key, this);
				TIfHasPointerLink<TValue>::pointerLink(&node->value, this);
				node->key = key;
				node->value = value;
				TXRBTree::insertNodeAt(root, parent, isLeft, node);
			};

			inline void set(const TKeyType *key, const TValueType *value) {
				TNode *parent;
				bool isLeft;
				TNode *node = findPosition(key, parent, isLeft);
				if (node) {
					node->value = value;
					return;
				};
				node = TXRBTree::newNode();
				TIfHasPointerLink<TKey>::pointerLink(&node->key, this);
				TIfHasPointerLink<TValue>::pointerLink(&node->value, this);
				node->key = key;
				node->value = value;
				TXRBTree::insertNodeAt(root, parent, isLeft, node);
			};

			[[nodiscard]] inline bool get(const TKey &key, TValue &value) {
				TNode *node = TXRBTree::find(root, key);
				if (node) {
					value = node->value;
					return true;
				};
				return false;
			};

			[[nodiscard]] inline bool get(const TKey &key, TPointerTValue &value) {
				TNode *node = TXRBTree::find(root, key);
				if (node) {
					value = node->value;
					return true;
				};
				return false;
			};

			[[nodiscard]] inline bool get(const TKey &key, TPointerXTValue &value) {
				TNode *node = TXRBTree::find(root, key);
				if (node) {
					value = node->value;
					return true;
				};
				return false;
			};

			[[nodiscard]] inline bool get(const TKeyType *key, TValueType &value) {
				TNode *node = find(key);
				if (node) {
					value = *(node->value);
					return true;
				};
				return false;
			};

			[[nodiscard]] inline bool get(const TKeyType *key, TPointerTValue &value) {
				TNode *node = find(key);
				if (node) {
					value = node->value;
					return true;
				};
				return false;
			};

			[[nodiscard]] inline bool get(const TKeyType *key, TPointerXTValue &value) {
				TNode *node = find(key);
				if (node) {
					value = node->value;
					return true;
				};
				return false;
			};

			[[nodiscard]] inline TValue getValue(const TKey &key, const TValue &value) {
				TNode *node = TXRBTree::find(root, key);
				if (node) {
					return node->value;
				};
				return value;
			};

			[[nodiscard]] inline TPointer<TValueType> getValue(const TKey &key, const TValueType *value) {
				TNode *node = TXRBTree::find(root, key);
				if (node) {
					return node->value;
				};
				return value;
			};

			[[nodiscard]] inline TValue getValue(const TKeyType *key, const TValue &value) {
				TNode *node = find(key);
				if (node) {
					return node->value;
				};
				return value;
			};

			[[nodiscard]] inline TPointer<TValueType> getValue(const TKeyType *key, const TValueType *value) {
				TNode *node = find(key);
				if (node) {
					return node->value;
				};
				return value;
			};

			inline void insert(const TKey &key, const TValue &value) {
				TNode *node;
				node = TXRBTree::newNode();
				TIfHasPointerLink<TKey>::pointerLink(&node->key, this);
				TIfHasPointerLink<TValue>::pointerLink(&node->value, this);
				node->key = key;
				node->value = value;
				TXRBTree::insertNode(root, node);
			};

			inline void insert(const TKey &key, TValue &&value) {
				TNode *node;
				node = TXRBTree::newNode();
				TIfHasPointerLink<TKey>::pointerLink(&node->key, this);
				TIfHasPointerLink<TValue>::pointerLink(&node->value, this);
				node->key = key;
				node->value = std::move(value);
				TXRBTree::insertNode(root, node);
			};

			inline void insert(const TKey &key, const TValueType *value) {
				TNode *node;
				node = TXRBTree::newNode();
				TIfHasPointerLink<TKey>::pointerLink(&node->key, this);
				TIfHasPointerLink<TValue>::pointerLink(&node->value, this);
				node->key = key;
				node->value = value;
				TXRBTree::insertNode(root, node);
			};

			inline void insert(const TKeyType *key, const TValue &value) {
				TNode *node;
				node = TXRBTree::newNode();
				TIfHasPointerLink<TKey>::pointerLink(&node->key, this);
				TIfHasPointerLink<TValue>::pointerLink(&node->value, this);
				node->key = key;
				node->value = value;
				TXRBTree::insertNode(root, node);
			};

			inline void insert(const TKeyType *key, const TValueType *value) {
				TNode *node;
				node = TXRBTree::newNode();
				TIfHasPointerLink<TKey>::pointerLink(&node->key, this);
				TIfHasPointerLink<TValue>::pointerLink(&node->value, this);
				node->key = key;
				node->value = value;
				TXRBTree::insertNode(root, node);
			};

			inline TNode *insertKey(const TKey &key) {
				TNode *node;
				node = TXRBTree::newNode();
				TIfHasPointerLink<TKey>::pointerLink(&node->key, this);
				TIfHasPointerLink<TValue>::pointerLink(&node->value, this);
				node->key = key;
				TXRBTree::insertNode(root, node);
				return node;
			};

			inline TNode *insertKey(const TKeyType *key) {
				TNode *node;
				node = TXRBTree::newNode();
				TIfHasPointerLink<TKey>::pointerLink(&node->key, this);
				TIfHasPointerLink<TValue>::pointerLink(&node->value, this);
				node->key = key;
				TXRBTree::insertNode(root, node);
				return node;
			};

			[[nodiscard]] inline TNode *begin() noexcept {
				return TXRBTree::begin(root);
			};

			[[nodiscard]] inline TNode *end() noexcept {
				return TXRBTree::end(root);
			};

			inline void activeDestructor() {
				empty();
			};

			inline void insertNode(TNode *node) {
				TXRBTree::insertNode(root, node);
			};

			inline void removeNode(TNode *node) {
				TXRBTree::remove(root, node);
			};

			inline bool remove(const TKey &key) {
				return TXRBTree::remove(root, key);
			};

			inline bool remove(const TKeyType *key) {
				TNode *node = find(key);
				if (node) {
					TXRBTree::remove(root, node);
					return true;
				};
				return false;
			};

		protected:
			// Search key, on a miss return nullptr and set parent / isLeft
			// to where a node with this key must be linked (TXRBTree::insertNodeAt)
			[[nodiscard]] inline TNode *findPosition(const TKey &key, TNode *&parent, bool &isLeft) {
				TNode *x;
				int compare;
				parent = nullptr;
				isLeft = false;
				for (x = root; x;) {
					compare = TComparator<TKey>::compare(key, x->key);
					if (compare == 0) {
						return x;
					};
					parent = x;
					isLeft = (compare < 0);
					if (isLeft) {
						x = x->left;
					} else {
						x = x->right;
					};
				};
				return nullptr;
			};

			[[nodiscard]] inline TNode *findPosition(const TKeyType *key, TNode *&parent, bool &isLeft) {
				TNode *x;
				int compare;
				parent = nullptr;
				isLeft = false;
				for (x = root; x;) {
					compare = TComparator<TKeyType>::compare(*key, *(x->key));
					if (compare == 0) {
						return x;
					};
					parent = x;
					isLeft = (compare < 0);
					if (isLeft) {
						x = x->left;
					} else {
						x = x->right;
					};
				};
				return nullptr;
			};
	};

};

#endif
