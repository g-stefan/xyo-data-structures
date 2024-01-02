// Data Structures
// Copyright (c) 2016-2024 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2024 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_DATASTRUCTURES_TREDBLACKTREEONE_HPP
#define XYO_DATASTRUCTURES_TREDBLACKTREEONE_HPP

#ifndef XYO_DATASTRUCTURES_TREDBLACKTREEONENODE_HPP
#	include <XYO/DataStructures/TRedBlackTreeOneNode.hpp>
#endif

namespace XYO::DataStructures {

	template <typename TKey, template <typename U> class TNodeMemory = TMemory>
	class TRedBlackTreeOne : public Object {
			XYO_DISALLOW_COPY_ASSIGN_MOVE(TRedBlackTreeOne);

		public:
			typedef TRedBlackTreeNodeOne<TKey, TNodeMemory> TNode;
			typedef TXRedBlackTree<TNode, TNodeMemory> TXRBTree;
			typedef TNode Node;

			typedef typename TPointerTypeExclude<TKey>::Type TKeyType;

			static inline TNode *newNode() {
				return TXRBTree::newNode();
			};

			static inline void deleteNode(TNode *this_) {
				return TXRBTree::deleteNode(this_);
			};

			static inline void initMemory() {
				TIfHasInitMemory<TKey>::initMemory();
				TXRBTree::initMemory();
			};

			TNode *root;

			inline TRedBlackTreeOne() {
				TXRBTree::constructor(root);
			};

			inline ~TRedBlackTreeOne() {
				TXRBTree::destructor(root);
			};

			inline void empty() {
				TXRBTree::empty(root);
			};

			inline TNode *find(const TKey &key) {
				return TXRBTree::find(root, key);
			};

			inline TNode *find(const TKeyType *key) {
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

			inline void set(const TKey &key) {
				TNode *node = TXRBTree::find(root, key);
				if (node) {
					return;
				};
				node = TXRBTree::newNode();
				TIfHasPointerLink<TKey>::pointerLink(&node->key, this);
				node->key = key;
				TXRBTree::insertNode(root, node);
			};

			inline void set(const TKeyType *key) {
				TNode *node = find(key);
				if (node) {
					return;
				};
				node = TXRBTree::newNode();
				TIfHasPointerLink<TKey>::pointerLink(&node->key, this);
				node->key = key;
				TXRBTree::insertNode(root, node);
			};

			inline bool has(const TKey &key) {
				TNode *node = TXRBTree::find(root, key);
				if (node) {
					return true;
				};
				return false;
			};

			inline bool has(const TKeyType *key) {
				TNode *node = find(key);
				if (node) {
					return true;
				};
				return false;
			};

			inline TNode *begin() {
				return TXRBTree::begin(root);
			};

			inline TNode *end() {
				return TXRBTree::end(root);
			};

			inline void activeDestructor() {
				empty();
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
	};

};

#endif
