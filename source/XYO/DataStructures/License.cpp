// Data Structures
// Copyright (c) 2022 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2022 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <XYO/DataStructures/License.hpp>

namespace XYO::DataStructures::License {

	const char *license() {
		return XYO::ManagedMemory::License::license();
	};

	const char *shortLicense() {
		return XYO::ManagedMemory::License::shortLicense();
	};

};
