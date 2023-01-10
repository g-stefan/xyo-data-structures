// Data Structures
// Copyright (c) 2016-2023 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2023 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_DATASTRUCTURES_DEPENDENCY_HPP
#define XYO_DATASTRUCTURES_DEPENDENCY_HPP

#ifndef XYO_MANAGEDMEMORY_HPP
#	include <XYO/ManagedMemory.hpp>
#endif

// -- Export

#ifndef XYO_DATASTRUCTURES_INTERNAL
#	ifdef XYO_DATA_STRUCTURES_INTERNAL
#		define XYO_DATASTRUCTURES_INTERNAL
#	endif
#endif

#ifdef XYO_DATASTRUCTURES_INTERNAL
#	define XYO_DATASTRUCTURES_EXPORT XYO_LIBRARY_EXPORT
#else
#	define XYO_DATASTRUCTURES_EXPORT XYO_LIBRARY_IMPORT
#endif

// --

namespace XYO::DataStructures {
	using namespace XYO::ManagedMemory;
};

#endif
