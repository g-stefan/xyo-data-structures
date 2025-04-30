// Data Structures
// Copyright (c) 2016-2025 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2025 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_DATASTRUCTURES_TSTACKNODE_HPP
#define XYO_DATASTRUCTURES_TSTACKNODE_HPP

#ifndef XYO_DATASTRUCTURES_DEPENDENCY_HPP
#	include <XYO/DataStructures/Dependency.hpp>
#endif

namespace XYO::DataStructures {

	template <typename T>
	struct TStackNode : TXList1Node<TStackNode<T>> {
			typedef TStackNode TNode;

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
