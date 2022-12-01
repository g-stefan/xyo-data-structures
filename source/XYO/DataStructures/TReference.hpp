//
// Data Structures
//
// Copyright (c) Grigore Stefan <g_stefan@yahoo.com>
// Created by Grigore Stefan <g_stefan@yahoo.com>
//
// MIT License (MIT) <http://opensource.org/licenses/MIT>
//

#ifndef XYO_DATASTRUCTURES_TREFERENCE_HPP
#define XYO_DATASTRUCTURES_TREFERENCE_HPP

#ifndef XYO_DATASTRUCTURES_DEPENDENCY_HPP
#	include <XYO/DataStructures/Dependency.hpp>
#endif

namespace XYO::DataStructures {

	template <typename T, void DeleteMemoryT(T *)>
	class TReference {
		protected:
			T *object;

		public:
			inline TReference() = delete;

			inline ~TReference() {
				DeleteMemoryT(object);
			};

			//

			inline TReference(const T *value) {
				object = const_cast<T *>(value);
			};

			inline TReference(const TReference &value) = delete;
			inline TReference(TReference &&value) = delete;

			//

			inline TReference &operator=(const TReference &value) = delete;
			inline TReference &operator=(TReference &&value) = delete;

			//

			inline T *operator->() const {
				return object;
			};

			inline operator T *() const {
				return object;
			};

			inline T *value() const {
				return object;
			};
	};

};

#endif
