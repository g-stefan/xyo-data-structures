// Data Structures
// Copyright (c) 2022 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2022 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_DATASTRUCTURES_TSTATICCAST_HPP
#define XYO_DATASTRUCTURES_TSTATICCAST_HPP

#ifndef XYO_DATASTRUCTURES_DEPENDENCY_HPP
#	include <XYO/DataStructures/Dependency.hpp>
#endif

namespace XYO::DataStructures {

	template <typename T>
	T TStaticCast(Object *this_) {
		if (this_ == nullptr) {
			return nullptr;
		};
		return static_cast<T>(this_);
	};

	template <typename T>
	Object *TStaticCast(T this_) {
		if (this_ == nullptr) {
			return nullptr;
		};
		return static_cast<Object *>(this_);
	};

	template <Object *>
	Object *TStaticCast(Object *this_) {
		return this_;
	};

};

#endif
