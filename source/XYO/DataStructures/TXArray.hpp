//
// Data Structures
//
// Copyright (c) Grigore Stefan <g_stefan@yahoo.com>
// Created by Grigore Stefan <g_stefan@yahoo.com>
//
// MIT License (MIT) <http://opensource.org/licenses/MIT>
//

#ifndef XYO_DATASTRUCTURES_TXARRAY_HPP
#define XYO_DATASTRUCTURES_TXARRAY_HPP

#ifndef XYO_DATASTRUCTURES_DEPENDENCY_HPP
#	include <XYO/DataStructures/Dependency.hpp>
#endif

namespace XYO::DataStructures {

	template <typename T>
	struct TXArrayT {
			size_t size;
			size_t length;
			T *root;
	};

	template <typename T>
	struct TXArray {

			typedef TXArrayT<T> ArrayT;
			typedef T Element;

			static inline void initMemory() {
				TMemory<T>::initMemory();
			};

			static inline void constructor(ArrayT &this_, size_t size) {
				this_.size = size;
				this_.root = new T[size]();
				this_.length = 0;
			};

			static inline void destructor(ArrayT &this_) {
				TIfHasActiveDestructor<T>::activeDestructorArray(this_.root, this_.length);
				TIfHasActiveFinalizer<T>::activeFinalizerArray(this_.root, this_.length);
				delete[] this_.root;
			};

			static inline void empty(ArrayT &this_) {
				TIfHasActiveDestructor<T>::activeDestructorArray(this_.root, this_.length);
				this_.length = 0;
			};

			static inline bool set(ArrayT &this_, size_t index, const T &value) {
				if (index < this_.length) {
					this_.root[index] = value;
					return true;
				};
				if (index < this_.size) {
					TIfHasActiveConstructor<T>::activeConstructorArray(&this_.root[this_.length], index - this_.length + 1);
					this_.length = index + 1;
					this_.root[index] = value;
					return true;
				};
				return false;
			};

			static inline bool get(ArrayT &this_, size_t index, T &value) {
				if (index < this_.length) {
					value = this_.root[index];
					return true;
				};
				return false;
			};

			static inline void resize(ArrayT &this_, size_t size) {
				T *root = new T[size]();

				size_t index;
				size_t length;

				length = this_.length;
				if (length > size) {
					length = size;
				};

				TIfHasActiveConstructor<T>::activeConstructorArray(root, length);

				for (index = 0; index < length; ++index) {
					root[index] = std::move(this_.root[index]);
				};

				deleteArray(this_);

				this_.size = size;
				this_.root = root;
				this_.length = length;
			};

			static inline bool remove(ArrayT &this_, size_t index) {
				if (this_.length == 0) {
					return false;
				};

				if (index >= this_.length) {
					return false;
				};

				size_t scan;
				for (scan = index; scan < this_.length - 1; ++scan) {
					this_.root[scan] = std::move(this_.root[scan + 1]);
				};

				TIfHasActiveDestructor<T>::activeDestructor(&this_.root[scan]);

				--this_.length;

				return true;
			};

			static inline bool extract(ArrayT &this_, size_t index, T &value) {
				if (this_.length == 0) {
					return false;
				};

				if (index >= this_.length) {
					return false;
				};

				value = std::move(this_.root[index]);

				size_t scan;
				for (scan = index; scan < this_.length - 1; ++scan) {
					this_.root[scan] = std::move(this_.root[scan + 1]);
				};

				TIfHasActiveDestructor<T>::activeDestructor(&this_.root[scan]);

				--this_.length;

				return true;
			};

			static inline bool insert(ArrayT &this_, size_t index, const T &value) {
				if (index >= this_.size) {
					return false;
				};

				if (index >= this_.length) {
					TIfHasActiveConstructor<T>::activeConstructorArray(&this_.root[this_.length], index - this_.length + 1);
					this_.length = index + 1;
					this_.root[index] = value;
					return true;
				};

				TIfHasActiveConstructor<T>::activeConstructorArray(&this_.root[this_.length], 1);

				size_t scan;

				for (scan = this_.length; scan > index; --scan) {
					this_.root[scan] = std::move(this_.root[scan - 1]);
				};

				++this_.length;

				this_.root[index] = value;

				return true;
			};
	};

};

#endif
