// Data Structures
// Copyright (c) 2022 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2022 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_DATASTRUCTURES_COPYRIGHT_HPP
#define XYO_DATASTRUCTURES_COPYRIGHT_HPP

#ifndef XYO_DATASTRUCTURES_DEPENDENCY_HPP
#	include <XYO/DataStructures/Dependency.hpp>
#endif

namespace XYO::DataStructures::Copyright {
	XYO_DATASTRUCTURES_EXPORT const char *copyright();
	XYO_DATASTRUCTURES_EXPORT const char *publisher();
	XYO_DATASTRUCTURES_EXPORT const char *company();
	XYO_DATASTRUCTURES_EXPORT const char *contact();
};

#endif
