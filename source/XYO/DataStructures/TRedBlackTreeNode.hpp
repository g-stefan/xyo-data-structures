// Data Structures
// Copyright (c) 2016-2024 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2024 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_DATASTRUCTURES_TREDBLACKTREENODE_HPP
#define XYO_DATASTRUCTURES_TREDBLACKTREENODE_HPP

#ifndef XYO_DATASTRUCTURES_DEPENDENCY_HPP
#	include <XYO/DataStructures/Dependency.hpp>
#endif

namespace XYO::DataStructures {

	template <typename TKey, typename TValue, template <typename U> class TNodeMemory>
	struct TRedBlackTreeNode : TXRedBlackTreeNode<TRedBlackTreeNode<TKey, TValue, TNodeMemory>, TKey> {
			typedef TRedBlackTreeNode TNode;
			typedef TXRedBlackTree<TNode, TNodeMemory> TXRBTree;

			TValue value;

			inline TNode *minimum() {
				return TXRBTree::minimum(this);
			};

			inline TNode *maximum() {
				return TXRBTree::maximum(this);
			};

			inline TNode *successor() {
				return TXRBTree::successor(this);
			};

			inline TNode *predecesor() {
				return TXRBTree::predecesor(this);
			};

			inline void activeConstructor() {
				TIfHasActiveConstructor<TKey>::activeConstructor(&this->key);
				TIfHasActiveConstructor<TValue>::activeConstructor(&this->value);
			};

			inline void activeDestructor() {
				TIfHasActiveDestructor<TKey>::activeDestructor(&this->key);
				TIfHasActiveDestructor<TValue>::activeDestructor(&this->value);
			};
	};

};

#endif
