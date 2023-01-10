// Data Structures
// Copyright (c) 2016-2023 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2023 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_DATASTRUCTURES_TXLIST3_HPP
#define XYO_DATASTRUCTURES_TXLIST3_HPP

#ifndef XYO_DATASTRUCTURES_DEPENDENCY_HPP
#	include <XYO/DataStructures/Dependency.hpp>
#endif

namespace XYO::DataStructures {

	template <typename TNode>
	struct TXList3Node {
			TNode *back;
			TNode *next;
	};

	template <typename TNode, template <typename U> class TNodeMemory = TMemorySystem>
	struct TXList3 {

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

			static inline void constructor(TNode *&head, TNode *&tail) {
				head = nullptr;
				tail = nullptr;
			};

			static inline void destructor(TNode *head) {
				TNode *this_;
				while (head) {
					this_ = head;
					head = head->next;
					deleteNode(this_);
				};
			};

			static inline void empty(TNode *&head, TNode *&tail) {
				destructor(head);
				constructor(head, tail);
			};

			static inline void push(TNode *&head, TNode *&tail, TNode *node) {
				node->back = nullptr;
				node->next = head;
				if (head) {
					head->back = node;
				} else {
					tail = node;
				};
				head = node;
			};

			static inline TNode *popUnsafe(TNode *&head, TNode *&tail) {
				TNode *node = head;
				if (head->next) {
					head->next->back = nullptr;
				};
				if (head == tail) {
					tail = nullptr;
				};
				head = head->next;
				return node;
			};

			static inline TNode *pop(TNode *&head, TNode *&tail) {
				if (head) {
					return popUnsafe(head, tail);
				};
				return nullptr;
			};

			static inline void pushToTail(TNode *&head, TNode *&tail, TNode *node) {
				node->back = tail;
				node->next = nullptr;
				if (tail) {
					tail->next = node;
				} else {
					head = node;
				};
				tail = node;
			};

			static inline TNode *popFromTailUnsafe(TNode *&head, TNode *&tail) {
				TNode *node = tail;
				if (tail->back) {
					tail->back->next = nullptr;
				};
				if (tail == head) {
					head = nullptr;
				};
				tail = tail->back;
				return node;
			};

			static inline TNode *popFromTail(TNode *&head, TNode *&tail) {
				if (tail) {
					return popFromTailUnsafe(head, tail);
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
			};

			static inline void extract(TNode *&head, TNode *&tail, TNode *node) {
				if (head == node) {
					popUnsafe(head, tail);
					return;
				};
				if (tail == node) {
					popFromTailUnsafe(head, tail);
					return;
				};
				extractNode(node);
			};

			static inline void extractList(TNode *&head, TNode *&tail, TNode *&headX, TNode *&tailX) {
				if (head == headX) {
					head = tailX->next;
				};
				if (tail == tailX) {
					tail = headX->back;
				};
				if (headX->back) {
					headX->back->next = tailX->next;
				};
				if (tailX->next) {
					tailX->next->back = headX->back;
				};
				headX->back = nullptr;
				tailX->next = nullptr;
			};

			static inline bool swap(TNode *&head, TNode *&tail) {
				if (head) {
					if (head->next) {
						TNode *tmp1 = popUnsafe(head, tail);
						TNode *tmp2 = popUnsafe(head, tail);
						push(head, tail, tmp1);
						push(head, tail, tmp2);
						return true;
					};
				};
				return false;
			};

			static inline bool swapOnTail(TNode *&head, TNode *&tail) {
				if (tail) {
					if (tail->back) {
						TNode *tmp1 = popFromTailUnsafe(head, tail);
						TNode *tmp2 = popFromTailUnsafe(head, tail);
						pushToTail(head, tail, tmp1);
						pushToTail(head, tail, tmp2);
						return true;
					};
				};
				return false;
			};

			static inline TNode *successor(TNode *node) {
				return node->next;
			};

			static inline TNode *predecessor(TNode *node) {
				return node->back;
			};

			static inline TNode *begin(TNode *node) {
				TNode *scan;
				for (scan = node; scan != nullptr; scan = scan->back) {
					if (scan->back == nullptr) {
						break;
					};
				};
				return scan;
			};

			static inline TNode *end(TNode *node) {
				TNode *scan;
				for (scan = node; scan != nullptr; scan = scan->next) {
					if (scan->next == nullptr) {
						break;
					};
				};
				return scan;
			};
	};

};

#endif
