// Data Structures
// Copyright (c) 2016-2026 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2026 Grigore Stefan <g_stefan@yahoo.com>
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

			// A slot that is not in use holds T(): types with lifetime hooks (TPointer, TPointerX,
			// pooled objects) are reset by their hooks, other types by assigning T(),
			// so a removed element does not keep its value or its resources
			static constexpr bool hasActiveHooks = THasActiveConstructor<T>::value || THasActiveDestructor<T>::value;

			T value[dataSize];

			// Value initialization, a slot never written reads as T() (0 for scalars)
			inline TDynamicArrayNode() : value(){};

			inline ~TDynamicArrayNode(){};

			inline void activeConstructor() {
				TIfHasActiveConstructor<T>::activeConstructorArray(&value[0], dataSize);
			};

			// Before the node is freed or returned to an active pool,
			// a node reused from an active pool must not bring back old values
			inline void activeDestructor() {
				if constexpr (hasActiveHooks) {
					TIfHasActiveDestructor<T>::activeDestructorArray(&value[0], dataSize);
				} else {
					resetArray(dataSize);
				};
			};

			inline void empty(size_t count_) {
				if constexpr (hasActiveHooks) {
					TIfHasActiveDestructor<T>::activeDestructorArray(&value[0], count_);
					TIfHasActiveConstructor<T>::activeConstructorArray(&value[0], count_);
				} else {
					resetArray(count_);
				};
			};

			inline void resetIndex(size_t index) {
				if constexpr (hasActiveHooks) {
					TIfHasActiveDestructor<T>::activeDestructor(&value[index]);
					TIfHasActiveConstructor<T>::activeConstructor(&value[index]);
				} else {
					value[index] = T();
				};
			};

		protected:
			inline void resetArray(size_t count_) {
				size_t k;
				for (k = 0; k < count_; ++k) {
					value[k] = T();
				};
			};
	};

};

#endif
