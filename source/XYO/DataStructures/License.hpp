// Data Structures
// Copyright (c) 2022 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2022 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_DATASTRUCTURES_LICENSE_HPP
#define XYO_DATASTRUCTURES_LICENSE_HPP

#ifndef XYO_DATASTRUCTURES_DEPENDENCY_HPP
#	include <XYO/DataStructures/Dependency.hpp>
#endif

namespace XYO::DataStructures::License {

	XYO_DATASTRUCTURES_EXPORT const char *licenseHeader();
	XYO_DATASTRUCTURES_EXPORT const char *licenseBody();
	XYO_DATASTRUCTURES_EXPORT const char *shortLicense();

};

#endif