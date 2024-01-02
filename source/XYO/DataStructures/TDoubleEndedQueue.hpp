// Data Structures
// Copyright (c) 2016-2024 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2024 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_DATASTRUCTURES_TDOUBLEENDEDQUEUE_HPP
#define XYO_DATASTRUCTURES_TDOUBLEENDEDQUEUE_HPP

#ifndef XYO_DATASTRUCTURES_TDOUBLEENDEDQUEUENODE_HPP
#	include <XYO/DataStructures/TDoubleEndedQueueNode.hpp>
#endif

#ifndef XYO_DATASTRUCTURES_TXLIST3_HPP
#	include <XYO/DataStructures/TXList3.hpp>
#endif

namespace XYO::DataStructures {

	template <typename T, template <typename U> class TNodeMemory = TMemory>
	class TDoubleEndedQueue : public Object {
			XYO_DISALLOW_COPY_ASSIGN_MOVE(TDoubleEndedQueue);

		public:
			typedef TDoubleEndedQueueNode<T> TNode;
			typedef TXList3<TNode, TNodeMemory> TXList;
			typedef TNode Node;

			typedef typename TPointerTypeExclude<T>::Type TType;
			typedef typename TPointerTypeExclude<T>::Pointer TPointerT;
			typedef typename TPointerTypeExclude<T>::PointerX TPointerXT;

			static inline TNode *newNode() {
				return TXList::newNode();
			};

			static inline void deleteNode(TNode *this_) {
				return TXList::deleteNode(this_);
			};

			static inline void initMemory() {
				TIfHasInitMemory<T>::initMemory();
				TXList::initMemory();
			};

			TNode *head;
			TNode *tail;

			inline TDoubleEndedQueue() {
				TXList::constructor(head, tail);
			};

			inline ~TDoubleEndedQueue() {
				TXList::destructor(head);
			};

			inline void push(const T &value) {
				TNode *node = TXList::newNode();
				TIfHasPointerLink<T>::pointerLink(&node->value, this);
				node->value = value;
				TXList::push(head, tail, node);
			};

			inline void push(TType *value) {
				TNode *node = TXList::newNode();
				TIfHasPointerLink<T>::pointerLink(&node->value, this);
				node->value = value;
				TXList::push(head, tail, node);
			};

			inline bool pop(T &value) {
				TNode *node = TXList::pop(head, tail);
				if (node) {
					value = node->value;
					TXList::deleteNode(node);
					return true;
				};
				return false;
			};

			inline bool pop(TPointerT &value) {
				TNode *node = TXList::pop(head, tail);
				if (node) {
					value = node->value;
					TXList::deleteNode(node);
					return true;
				};
				return false;
			};

			inline bool pop(TPointerXT &value) {
				TNode *node = TXList::pop(head, tail);
				if (node) {
					value = node->value;
					TXList::deleteNode(node);
					return true;
				};
				return false;
			};

			inline void push() {
				TNode *node = TXList::newNode();
				TIfHasPointerLink<T>::pointerLink(&node->value, this);
				TXList::push(head, tail, node);
			};

			inline bool pop() {
				TNode *node = TXList::pop(head, tail);
				if (node) {
					TXList::deleteNode(node);
					return true;
				};
				return false;
			};

			inline void pushToTail(const T &value) {
				TNode *node = TXList::newNode();
				TIfHasPointerLink<T>::pointerLink(&node->value, this);
				node->value = value;
				TXList::pushToTail(head, tail, node);
			};

			inline void pushToTail(const TType *value) {
				TNode *node = TXList::newNode();
				TIfHasPointerLink<T>::pointerLink(&node->value, this);
				node->value = value;
				TXList::pushToTail(head, tail, node);
			};

			inline TNode *pushToTailX(const T &value) {
				TNode *node = TXList::newNode();
				TIfHasPointerLink<T>::pointerLink(&node->value, this);
				node->value = value;
				TXList::pushToTail(head, tail, node);
				return node;
			};

			inline TNode *pushToTailX(const TType *value) {
				TNode *node = TXList::newNode();
				TIfHasPointerLink<T>::pointerLink(&node->value, this);
				node->value = value;
				TXList::pushToTail(head, tail, node);
				return node;
			};

			inline bool popFromTail(T &value) {
				TNode *node = TXList::popFromTail(head, tail);
				if (node) {
					value = node->value;
					TXList::deleteNode(node);
					return true;
				};
				return false;
			};

			inline bool popFromTail(TPointerT &value) {
				TNode *node = TXList::popFromTail(head, tail);
				if (node) {
					value = node->value;
					TXList::deleteNode(node);
					return true;
				};
				return false;
			};

			inline bool popFromTail(TPointerXT &value) {
				TNode *node = TXList::popFromTail(head, tail);
				if (node) {
					value = node->value;
					TXList::deleteNode(node);
					return true;
				};
				return false;
			};

			inline void pushToTail() {
				TNode *node = TXList::newNode();
				TIfHasPointerLink<T>::pointerLink(&node->value, this);
				TXList::pushToTail(head, tail, node);
			};

			inline bool popFromTail() {
				TNode *node = TXList::popFromTail(head, tail);
				if (node) {
					TXList::deleteNode(node);
					return true;
				};
				return false;
			};

			inline void empty() {
				TXList::empty(head, tail);
			};

			inline void activeDestructor() {
				empty();
			};

			inline bool isEmpty() const {
				return (head == nullptr);
			};

			inline void extractNode(TNode *node) {
				TXList::extract(head, tail, node);
			};

			inline void extractList(TNode *&listHead, TNode *&listTail) {
				TXList::extractList(head, tail, listHead, listTail);
			};

			inline void setList(TNode *listHead, TNode *listTail) {
				empty();
				head = listHead;
				tail = listTail;
			};
	};

};

#endif
