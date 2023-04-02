// Data Structures
// Copyright (c) 2016-2023 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2023 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_DATASTRUCTURES_TXLIST5_HPP
#define XYO_DATASTRUCTURES_TXLIST5_HPP

#ifndef XYO_DATASTRUCTURES_TXLIST5NODE_HPP
#	include <XYO/DataStructures/TXList5Node.hpp>
#endif

namespace XYO::DataStructures {

	template <typename TNode, template <typename U> class TNodeMemory = TMemorySystem>
	struct TXList5 {

			typedef TNode Node;

			static inline TNode *newNode() {
				return TNodeMemory<TNode>::newMemory();
			};

			static inline void deleteNode(TNode *this_) {
				return TNodeMemory<TNode>::deleteMemory(this_);
			};

			static inline void initMemory() {
				TNodeMemory<TNode>::initMemory();
			};

			static inline void constructor(TNode *&root) {
				root = nullptr;
			};

			static inline void destructor(TNode *root) {
				TNode *this_;
				while (root) {
					destructor(root->childHead);
					this_ = root;
					root = root->next;
					deleteNode(this_);
				};
			};

			static inline void empty(TNode *&root) {
				destructor(root);
				constructor(root);
			};

			static inline void push(TNode *root, TNode *node) {
				node->back = nullptr;
				node->next = root->childHead;
				if (root->childHead) {
					root->childHead->back = node;
				} else {
					root->childTail = node;
				};
				root->childHead = node;
				node->parent = root;
			};

			static inline void pushToTail(TNode *root, TNode *node) {
				node->back = root->childTail;
				node->next = nullptr;
				if (root->childTail) {
					root->childTail->next = node;
				} else {
					root->childHead = node;
				};
				root->childTail = node;
				node->parent = root;
			};

			static inline TNode *popUnsafe(TNode *root) {
				TNode *node = root->childHead;
				if (root->childHead->next) {
					root->childHead->next->back = nullptr;
				};
				if (root->childHead == root->childTail) {
					root->childTail = nullptr;
				};
				root->childHead = root->childHead->next;
				node->parent = nullptr;
				node->back = nullptr;
				node->next = nullptr;
				return node;
			};

			static inline TNode *pop(TNode *root) {
				if (root->childHead) {
					return popUnsafe(root);
				};
				return nullptr;
			};

			static inline TNode *popFromTailUnsafe(TNode *root) {
				TNode *node = root->childTail;
				if (root->childTail->back) {
					root->childTail->back->next = nullptr;
				};
				if (root->childTail == root->childHead) {
					root->childHead = nullptr;
				};
				root->childTail = root->childTail->back;
				node->parent = nullptr;
				node->back = nullptr;
				node->next = nullptr;
				return node;
			};

			static inline TNode *popFromTail(TNode *root) {
				if (root->childTail) {
					return popFromTailUnsafe(root);
				};
				return nullptr;
			};

			static inline void extractNode(TNode *node) {
				if (node->next) {
					node->next->back = node->back;
				};
				if (node->back) {
					node->back->next = node->next;
				};
				node->parent = nullptr;
				node->back = nullptr;
				node->next = nullptr;
			};

			static inline void extract(TNode *root, TNode *node) {
				if (root->childHead == node) {
					popUnsafe(root);
					return;
				};
				if (root->childTail == node) {
					popFromTailUnsafe(root);
					return;
				};
				extractNode(node);
			};

			static inline TNode *successor(TNode *node) {
				if (node->childHead) {
					return node->childHead;
				};
				if (node->next) {
					return node->next;
				};
				while (node->parent) {
					if (node->parent->next == nullptr) {
						node = node->parent;
						continue;
					};
					return node->parent->next;
				}
				return nullptr;
			};

			static inline TNode *successorNoChild(TNode *node) {
				if (node->next) {
					return node->next;
				};
				while (node->parent) {
					if (node->parent->next == nullptr) {
						node = node->parent;
						continue;
					};
					return node->parent->next;
				}
				return nullptr;
			};

			static inline TNode *predecessor(TNode *node) {
				if (node->back) {
					if (node->back->childTail) {
						node = node->back->childTail;
						while (node->childTail) {
							node = node->childTail;
						};
						return node;
					};
					return node->back;
				};
				return node->parent;
			};

			static inline TNode *begin(TNode *node) {
				TNode *scan;
				for (scan = node; scan != nullptr; scan = scan->parent) {
					if (scan->parent == nullptr) {
						break;
					};
				};
				return scan;
			};

			static inline TNode *end(TNode *node) {
				TNode *scan;
				scan = begin(node);
				while (scan != nullptr) {
					if (scan->next == nullptr) {
						if (scan->childTail) {
							scan = scan->childTail;
							continue;
						};
						break;
					};
					scan = scan->next;
				};
				return scan;
			};

			static inline void extractList(TNode *root, TNode *headX, TNode *tailX) {
				if (root->childHead == headX) {
					root->childHead = tailX->next;
				};
				if (root->childTail == tailX) {
					root->childTail = headX->back;
				};
				if (headX->back) {
					headX->back->next = tailX->next;
				};
				if (tailX->next) {
					tailX->next->back = headX->back;
				};
				headX->back = nullptr;
				tailX->next = nullptr;

				for (TNode *node = headX; node != nullptr; node = node->next) {
					node->parent = nullptr;
				};
			};

			static inline void addListToTail(TNode *root, TNode *headX, TNode *tailX) {
				for (TNode *node = headX; node != nullptr; node = node->next) {
					node->parent = root;
				};

				headX->back = root->childTail;
				if (root->childTail) {
					root->childTail->next = headX;
				} else {
					root->childHead = headX;
				};
				root->childTail = tailX;
				tailX->next = nullptr;
			};

			static inline void insertNode(TNode *root, TNode *anchor, TNode *node) {
				if (anchor == nullptr) {
					push(root, node);
					return;
				};

				node->parent = root;
				node->back = anchor;
				if (anchor->next) {
					anchor->next->back = node;
				};
				node->next = anchor->next;
				anchor->next = node;

				if (root->childTail == anchor) {
					root->childTail = node;
				};
			};

			static inline void transferChild(TNode *root, TNode *node) {
				TNode *headX = node->childHead;
				TNode *tailX = node->childTail;

				extractList(node, headX, tailX);
				addListToTail(root, headX, tailX);
			};

			static inline void transferChildAnchor(TNode *root, TNode *node_) {
				TNode *headX = node_->childHead;
				TNode *tailX = node_->childTail;

				if (headX == nullptr) {
					return;
				};
				if (tailX == nullptr) {
					return;
				};

				extractList(node_, headX, tailX);

				for (TNode *node = headX; node != nullptr; node = node->next) {
					node->parent = root;
				};

				headX->back = node_;
				if (node_->next) {
					node_->next->back = tailX;
				};
				tailX->next = node_->next;
				node_->next = headX;

				if (root->childTail == node_) {
					root->childTail = tailX;
				};
			};
	};
};

#endif
