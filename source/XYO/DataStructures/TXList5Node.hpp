// Data Structures
// Copyright (c) 2016-2025 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2025 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_DATASTRUCTURES_TXLIST5NODE_HPP
#define XYO_DATASTRUCTURES_TXLIST5NODE_HPP

#ifndef XYO_DATASTRUCTURES_DEPENDENCY_HPP
#	include <XYO/DataStructures/Dependency.hpp>
#endif

namespace XYO::DataStructures {

	template <typename TNode>
	struct TXList5Node {
			TNode *back;
			TNode *next;
			TNode *childHead;
			TNode *childTail;
			TNode *parent;
	};

};

#endif
