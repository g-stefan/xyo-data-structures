//
// Data Structures
//
// Copyright (c) Grigore Stefan <g_stefan@yahoo.com>
// Created by Grigore Stefan <g_stefan@yahoo.com>
//
// MIT License (MIT) <http://opensource.org/licenses/MIT>
//

#ifndef XYO_DATASTRUCTURES_ISEEK_HPP
#define XYO_DATASTRUCTURES_ISEEK_HPP

#ifndef XYO_DATASTRUCTURES_DEPENDENCY_HPP
#	include <XYO/DataStructures/Dependency.hpp>
#endif

namespace XYO::DataStructures {

	class ISeek : public virtual Object {
			XYO_INTERFACE(ISeek);

		public:
			virtual bool seekFromBegin(uint64_t x) = 0;
			virtual bool seek(uint64_t x) = 0;
			virtual bool seekFromEnd(uint64_t x) = 0;
			virtual uint64_t seekTell() = 0;
	};

};

#endif
