// Data Structures
// Copyright (c) 2016-2025 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2025 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_DATASTRUCTURES_IWRITE_HPP
#define XYO_DATASTRUCTURES_IWRITE_HPP

#ifndef XYO_DATASTRUCTURES_DEPENDENCY_HPP
#	include <XYO/DataStructures/Dependency.hpp>
#endif

namespace XYO::DataStructures {

	class IWrite : public virtual Object {
			XYO_PLATFORM_INTERFACE(IWrite);

		public:
			virtual size_t write(const void *input, size_t length) = 0;
	};
};

#endif
