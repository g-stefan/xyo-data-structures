// Data Structures
// Copyright (c) 2016-2025 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2025 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_DATASTRUCTURES_IREAD_HPP
#define XYO_DATASTRUCTURES_IREAD_HPP

#ifndef XYO_DATASTRUCTURES_DEPENDENCY_HPP
#	include <XYO/DataStructures/Dependency.hpp>
#endif

namespace XYO::DataStructures {

	class IRead : public virtual Object {
			XYO_PLATFORM_INTERFACE(IRead);

		public:
			virtual size_t read(void *output, size_t length) = 0;
	};

};

#endif
