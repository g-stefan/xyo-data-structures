// Data Structures
// Copyright (c) 2016-2024 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2024 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_DATASTRUCTURES_TDOUBLEENDEDQUEUENODE_HPP
#define XYO_DATASTRUCTURES_TDOUBLEENDEDQUEUENODE_HPP

#ifndef XYO_DATASTRUCTURES_DEPENDENCY_HPP
#	include <XYO/DataStructures/Dependency.hpp>
#endif

#ifndef XYO_DATASTRUCTURES_TXLIST3NODE_HPP
#	include <XYO/DataStructures/TXList3Node.hpp>
#endif

namespace XYO::DataStructures {

	template <typename T>
	struct TDoubleEndedQueueNode : TXList3Node<TDoubleEndedQueueNode<T>> {
			typedef TDoubleEndedQueueNode TNode;

			T value;

			inline void activeConstructor() {
				TIfHasActiveConstructor<T>::activeConstructor(&value);
			};

			inline void activeDestructor() {
				TIfHasActiveDestructor<T>::activeDestructor(&value);
			};
	};

};

#endif
