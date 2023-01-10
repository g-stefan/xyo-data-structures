// Data Structures
// Copyright (c) 2016-2023 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2023 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_DATASTRUCTURES_VERSION_HPP
#define XYO_DATASTRUCTURES_VERSION_HPP

#ifndef XYO_DATASTRUCTURES_DEPENDENCY_HPP
#	include <XYO/DataStructures/Dependency.hpp>
#endif

namespace XYO::DataStructures::Version {

	XYO_DATASTRUCTURES_EXPORT const char *version();
	XYO_DATASTRUCTURES_EXPORT const char *build();
	XYO_DATASTRUCTURES_EXPORT const char *versionWithBuild();
	XYO_DATASTRUCTURES_EXPORT const char *datetime();

};

#endif
