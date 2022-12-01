//
// Data Structures
//
// Copyright (c) Grigore Stefan <g_stefan@yahoo.com>
// Created by Grigore Stefan <g_stefan@yahoo.com>
//
// MIT License (MIT) <http://opensource.org/licenses/MIT>
//

#ifndef XYO_DATASTRUCTURES_TSTACKPOINTERUNSAFE_HPP
#define XYO_DATASTRUCTURES_TSTACKPOINTERUNSAFE_HPP

#ifndef XYO_DATASTRUCTURES_TSTACK_HPP
#	include <XYO/DataStructures/TStack.hpp>
#endif

namespace XYO::DataStructures {

	template <typename T, template <typename U> class TNodeMemory = TMemory>
	class TStackPointerUnsafe : public Object {
			XYO_DISALLOW_COPY_ASSIGN_MOVE(TStackPointerUnsafe);

		public:
			typedef TStackNode<TPointer<T>> TNode;
			typedef TXList1<TNode, TNodeMemory> TXStack;
			typedef TNode Node;

			static inline TNode *newNode() {
				return TXStack::newNode();
			};

			static inline void deleteNode(TNode *this_) {
				return TXStack::deleteNode(this_);
			};

			static inline void initMemory() {
				TMemory<T>::initMemory();
				TXStack::initMemory();
			};

			TNode *head;

			inline TStackPointerUnsafe() {
				TXStack::constructor(head);
			};

			inline ~TStackPointerUnsafe() {
				TXStack::destructor(head);
			};

			inline void push(const T *value) {
				TNode *node = TXStack::newNode();
				node->value = value;
				TXStack::push(head, node);
			};

			inline void pop(TPointer<T> &value) {
				TNode *node = TXStack::popUnsafe(head);
				value = std::move(node->value);
				TXStack::deleteNode(node);
			};

			inline void pop(TPointerX<T> &value) {
				TNode *node = TXStack::popUnsafe(head);
				value = std::move(node->value);
				TXStack::deleteNode(node);
			};

			inline void popExecutive(TPointerX<T> &value) {
				TNode *node = TXStack::popUnsafe(head);
				value.setExecutive(std::move(node->value));
				TXStack::deleteNode(node);
			};

			inline TTransfer<T> *popTransfer() {
				TTransfer<T> *retV;
				TNode *node = TXStack::popUnsafe(head);
				retV = node->value.transfer();
				TXStack::deleteNode(node);
				return retV;
			};

			inline void empty() {
				TXStack::empty(head);
			};

			inline void activeDestructor() {
				empty();
			};

			inline bool isEmpty() const {
				return (head == nullptr);
			};

			inline void duplicate() {
				TNode *node = TXStack::newNode();
				node->value = head->value;
				TXStack::push(head, node);
			};

			inline void swap() {
				TXStack::swap(head);
			};

			inline void push() {
				TNode *node = TXStack::newNode();
				TXStack::push(head, node);
			};

			inline void pop() {
				TNode *node = TXStack::pop(head);
				TXStack::deleteNode(node);
			};

			inline void peek(TPointer<T> &value) {
				value = head->value;
			};

			inline void peek(TPointerX<T> &value) {
				value = head->value;
			};

			inline void peek(T *&value) {
				value = head->value;
			};
	};

};

#endif
