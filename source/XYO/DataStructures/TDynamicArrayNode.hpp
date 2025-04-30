// Data Structures
// Copyright (c) 2016-2025 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2025 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_DATASTRUCTURES_TDYNAMICARRAYNODE_HPP
#define XYO_DATASTRUCTURES_TDYNAMICARRAYNODE_HPP

#ifndef XYO_DATASTRUCTURES_DEPENDENCY_HPP
#	include <XYO/DataStructures/Dependency.hpp>
#endif

namespace XYO::DataStructures {

	template <typename T, size_t dataSize>
	struct TDynamicArrayNode {
			typedef TDynamicArrayNode TNode;

			T value[dataSize];

			inline TDynamicArrayNode(){};

			inline ~TDynamicArrayNode(){};

			inline void activeConstructor() {
				TIfHasActiveConstructor<T>::activeConstructorArray(&value[0], dataSize);
			};

			inline void activeDestructor() {
				TIfHasActiveDestructor<T>::activeDestructorArray(&value[0], dataSize);
			};

			inline void empty(int count_) {
				TIfHasActiveDestructor<T>::activeDestructorArray(&value[0], count_);
				TIfHasActiveConstructor<T>::activeConstructorArray(&value[0], count_);
			};

			inline void resetIndex(size_t index) {
				TIfHasActiveDestructor<T>::activeDestructor(&value[index]);
				TIfHasActiveConstructor<T>::activeConstructor(&value[index]);
			};
	};

};

#endif
