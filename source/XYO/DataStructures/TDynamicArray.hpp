// Data Structures
// Copyright (c) 2016-2023 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2023 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_DATASTRUCTURES_TDYNAMICARRAY_HPP
#define XYO_DATASTRUCTURES_TDYNAMICARRAY_HPP

#ifndef XYO_DATASTRUCTURES_DEPENDENCY_HPP
#	include <XYO/DataStructures/Dependency.hpp>
#endif

namespace XYO::DataStructures {

	template <typename T, size_t dataSize>
	struct TDynamicArrayNode {
			typedef TDynamicArrayNode TNode;

			T value[dataSize];

			inline TDynamicArrayNode(){};

			inline ~TDynamicArrayNode(){};

			inline void activeConstructor() {
				TIfHasActiveConstructor<T>::activeConstructorArray(&value[0], dataSize);
			};

			inline void activeDestructor() {
				TIfHasActiveDestructor<T>::activeDestructorArray(&value[0], dataSize);
			};

			inline void empty(int count_) {
				TIfHasActiveDestructor<T>::activeDestructorArray(&value[0], count_);
				TIfHasActiveConstructor<T>::activeConstructorArray(&value[0], count_);
			};

			inline void resetIndex(size_t index) {
				TIfHasActiveDestructor<T>::activeDestructor(&value[index]);
				TIfHasActiveConstructor<T>::activeConstructor(&value[index]);
			};
	};

	template <typename T, size_t dataSize2Pow = 4, template <typename U> class TNodeMemory = TMemory>
	class TDynamicArray : public Object {
			XYO_DISALLOW_COPY_ASSIGN_MOVE(TDynamicArray);

		public:
			static const size_t dataSize = (1 << dataSize2Pow);
			static const size_t dataMask = ((1 << dataSize2Pow) - 1);

			typedef TDynamicArrayNode<T, dataSize> TNode;
			typedef TNode Node;

			typedef typename TPointerTypeExclude<T>::Type TType;
			typedef typename TPointerTypeExclude<T>::Pointer TPointerT;
			typedef typename TPointerTypeExclude<T>::PointerX TPointerXT;

			size_t indexSize;
			size_t itemSize;
			size_t length_;

			TNode **value;

			inline TDynamicArray(int indexSize_ = 1) {
				size_t k;
				indexSize = indexSize_;
				itemSize = indexSize * dataSize;
				length_ = 0;
				value = new TNode *[indexSize];
				for (k = 0; k < indexSize; ++k) {
					value[k] = TNodeMemory<TNode>::newMemory();
					TIfHasPointerLink<T>::pointerLinkArray(&value[k]->value[0], this, dataSize);
				};
			};

			inline ~TDynamicArray() {
				size_t k;
				for (k = 0; k < indexSize; ++k) {
					TNodeMemory<TNode>::deleteMemory(value[k]);
				};
				delete[] value;
			};

			inline void empty() {
				int indexHigh;
				int indexScanHigh;
				if (length_ == 0) {
					return;
				};
				indexHigh = length_ >> dataSize2Pow;
				for (indexScanHigh = 0; indexScanHigh < indexHigh; ++indexScanHigh) {
					value[indexScanHigh]->empty(dataSize);
				};
				value[indexHigh]->empty(length_ & dataMask);
				length_ = 0;
			};

			inline void growWith(int count_) {
				size_t k;
				size_t newIndexSize = indexSize + count_;
				TNode **newValue;
				TNode **oldValue;
				newValue = new TNode *[newIndexSize];
				memcpy(newValue, value, sizeof(TNode *) * indexSize);
				for (k = indexSize; k < newIndexSize; ++k) {
					newValue[k] = TNodeMemory<TNode>::newMemory();
					TIfHasPointerLink<T>::pointerLinkArray(&newValue[k]->value[0], this, dataSize);
				};
				indexSize = newIndexSize;
				itemSize = indexSize * dataSize;
				oldValue = value;
				value = newValue;
				delete[] oldValue;
			};

			inline T &operator[](int idx) {
				return index((size_t)idx);
			};

			inline T &index(size_t idx) {
				if (idx >= length_) {
					length_ = idx + 1;
					if (idx >= itemSize) {
						growWith((idx >> dataSize2Pow) - indexSize + 1);
					};
				};
				return value[idx >> dataSize2Pow]->value[idx & dataMask];
			};

			inline bool get(size_t index, T &object) const {
				if (index >= length_) {
					return false;
				};
				object = value[index >> dataSize2Pow]->value[index & dataMask];
				return true;
			};

			inline bool get(size_t index, TPointerT &object) const {
				if (index >= length_) {
					return false;
				};
				object = value[index >> dataSize2Pow]->value[index & dataMask];
				return true;
			};

			inline bool get(size_t index, TPointerXT &object) const {
				if (index >= length_) {
					return false;
				};
				object = value[index >> dataSize2Pow]->value[index & dataMask];
				return true;
			};

			inline T &getValue(size_t index, const T &default_) {
				if (index >= length_) {
					length_ = index + 1;
					if (index < itemSize) {
					} else {
						growWith((index >> dataSize2Pow) - indexSize + 1);
					};
					T &retV = value[index >> dataSize2Pow]->value[index & dataMask];
					retV = default_;
					return retV;
				};
				return value[index >> dataSize2Pow]->value[index & dataMask];
			};

			inline T &getValue(size_t index, const TType *default_) {
				if (index >= length_) {
					length_ = index + 1;
					if (index < itemSize) {
					} else {
						growWith((index >> dataSize2Pow) - indexSize + 1);
					};
					T &retV = value[index >> dataSize2Pow]->value[index & dataMask];
					retV = default_;
					return retV;
				};
				return value[index >> dataSize2Pow]->value[index & dataMask];
			};

			inline void set(size_t index, const T &object) {
				if (index >= length_) {
					length_ = index + 1;
					if (index >= itemSize) {
						growWith((index >> dataSize2Pow) - indexSize + 1);
					};
				};
				value[index >> dataSize2Pow]->value[index & dataMask] = object;
			};

			inline void set(size_t index, const TType *object) {
				if (index >= length_) {
					length_ = index + 1;
					if (index >= itemSize) {
						growWith((index >> dataSize2Pow) - indexSize + 1);
					};
				};
				value[index >> dataSize2Pow]->value[index & dataMask] = object;
			};

			inline size_t length() const {
				return length_;
			};

			inline size_t arraySize() const {
				return itemSize;
			};

			inline void setLength(size_t newLength) {
				if (newLength <= length_) {
					length_ = newLength;
					return;
				};
				length_ = newLength;
				if (newLength < itemSize) {
				} else {
					growWith((newLength >> dataSize2Pow) - indexSize + 1);
				};
			};

			inline void activeDestructor() {
				empty();
			};

			static inline void initMemory() {
				TIfHasInitMemory<T>::initMemory();
				TNodeMemory<TNode>::initMemory();
			};

			inline bool isEmpty() const {
				return (length_ == 0);
			};

			inline bool shift(T &out) {
				size_t indexHigh;
				size_t indexLow;
				size_t indexScanHigh;
				size_t indexScanLow;

				if (length_ == 0) {
					return false;
				};
				out = value[0]->value[0];
				--length_;
				if (length_ > 0) {
					indexHigh = length_ >> dataSize2Pow;
					indexLow = length_ & dataMask;
					for (indexScanHigh = 0; indexScanHigh < indexHigh; ++indexScanHigh) {
						for (indexScanLow = 0; indexScanLow < dataSize - 1; ++indexScanLow) {
							value[indexScanHigh]->value[indexScanLow] = std::move(value[indexScanHigh]->value[indexScanLow + 1]);
						};
						value[indexScanHigh]->value[dataSize - 1] = std::move(value[indexScanHigh + 1]->value[0]);
					};
					for (indexScanLow = 0; indexScanLow < dataSize - 1; ++indexScanLow) {
						value[indexHigh]->value[indexScanLow] = std::move(value[indexHigh]->value[indexScanLow + 1]);
					};
					value[indexHigh]->resetIndex(indexScanLow);
				};
				return true;
			};

			inline bool shift(TPointerT &out) {
				size_t indexHigh;
				size_t indexLow;
				size_t indexScanHigh;
				size_t indexScanLow;

				if (length_ == 0) {
					return false;
				};
				out = value[0]->value[0];
				--length_;
				if (length_ > 0) {
					indexHigh = length_ >> dataSize2Pow;
					indexLow = length_ & dataMask;
					for (indexScanHigh = 0; indexScanHigh < indexHigh; ++indexScanHigh) {
						for (indexScanLow = 0; indexScanLow < dataSize - 1; ++indexScanLow) {
							value[indexScanHigh]->value[indexScanLow] = std::move(value[indexScanHigh]->value[indexScanLow + 1]);
						};
						value[indexScanHigh]->value[dataSize - 1] = std::move(value[indexScanHigh + 1]->value[0]);
					};
					for (indexScanLow = 0; indexScanLow < dataSize - 1; ++indexScanLow) {
						value[indexHigh]->value[indexScanLow] = std::move(value[indexHigh]->value[indexScanLow + 1]);
					};
					value[indexHigh]->resetIndex(indexScanLow);
				};
				return true;
			};

			inline bool shift(TPointerXT &out) {
				size_t indexHigh;
				size_t indexLow;
				size_t indexScanHigh;
				size_t indexScanLow;

				if (length_ == 0) {
					return false;
				};
				out = value[0]->value[0];
				--length_;
				if (length_ > 0) {
					indexHigh = length_ >> dataSize2Pow;
					indexLow = length_ & dataMask;
					for (indexScanHigh = 0; indexScanHigh < indexHigh; ++indexScanHigh) {
						for (indexScanLow = 0; indexScanLow < dataSize - 1; ++indexScanLow) {
							value[indexScanHigh]->value[indexScanLow] = std::move(value[indexScanHigh]->value[indexScanLow + 1]);
						};
						value[indexScanHigh]->value[dataSize - 1] = std::move(value[indexScanHigh + 1]->value[0]);
					};
					for (indexScanLow = 0; indexScanLow < dataSize - 1; ++indexScanLow) {
						value[indexHigh]->value[indexScanLow] = std::move(value[indexHigh]->value[indexScanLow + 1]);
					};
					value[indexHigh]->resetIndex(indexScanLow);
				};
				return true;
			};

			inline void shift() {
				size_t indexHigh;
				size_t indexLow;
				size_t indexScanHigh;
				size_t indexScanLow;

				if (length_ == 0) {
					return;
				};
				--length_;
				if (length_ > 0) {
					indexHigh = length_ >> dataSize2Pow;
					indexLow = length_ & dataMask;
					for (indexScanHigh = 0; indexScanHigh < indexHigh; ++indexScanHigh) {
						for (indexScanLow = 0; indexScanLow < dataSize - 1; ++indexScanLow) {
							value[indexScanHigh]->value[indexScanLow] = std::move(value[indexScanHigh]->value[indexScanLow + 1]);
						};
						value[indexScanHigh]->value[dataSize - 1] = std::move(value[indexScanHigh + 1]->value[0]);
					};
					for (indexScanLow = 0; indexScanLow < dataSize - 1; ++indexScanLow) {
						value[indexHigh]->value[indexScanLow] = std::move(value[indexHigh]->value[indexScanLow + 1]);
					};
					value[indexHigh]->resetIndex(indexScanLow);
				};
			};

			inline bool remove(size_t index) {
				size_t indexHighX;
				size_t indexLowX;
				size_t indexHigh;
				size_t indexLow;
				size_t indexScanHigh;
				size_t indexScanLow;

				if (length_ == 0) {
					return false;
				};
				if (index >= length_) {
					return false;
				};
				--length_;
				if (length_ > 0) {
					indexHighX = index >> dataSize2Pow;
					indexLowX = index & dataMask;
					indexHigh = length_ >> dataSize2Pow;
					indexLow = length_ & dataMask;
					for (indexScanHigh = indexHighX; indexScanHigh < indexHigh; ++indexScanHigh) {
						for (indexScanLow = indexLowX; indexScanLow < dataSize - 1; ++indexScanLow) {
							value[indexScanHigh]->value[indexScanLow] = std::move(value[indexScanHigh]->value[indexScanLow + 1]);
						};
						value[indexScanHigh]->value[dataSize - 1] = value[indexScanHigh + 1]->value[0];
						for (indexScanLow = 0; indexScanLow < indexLowX; ++indexScanLow) {
							value[indexScanHigh + 1]->value[indexScanLow] = std::move(value[indexScanHigh + 1]->value[indexScanLow + 1]);
						};
					};
					for (indexScanLow = indexLowX; indexScanLow < dataSize - 1; ++indexScanLow) {
						value[indexHigh]->value[indexScanLow] = std::move(value[indexHigh]->value[indexScanLow + 1]);
					};
					value[indexHigh]->resetIndex(indexScanLow);
				};
				return true;
			};

			inline T &push() {
				size_t idx = length_;
				++length_;
				if (idx >= itemSize) {
					growWith((idx >> dataSize2Pow) - indexSize + 1);
				};
				return value[idx >> dataSize2Pow]->value[idx & dataMask];
			};

			inline T &insert(size_t idx) {
				if (idx >= length_) {
					return index(idx);
				};
				size_t k;
				++length_;
				if (idx >= itemSize) {
					growWith((idx >> dataSize2Pow) - indexSize + 1);
				};
				for (k = length_ - 1; k > idx; --k) {
					index(k) = std::move(index(k - 1));
				};
				return index(idx);
			};

			inline void push(const T &value_) {
				size_t idx = length_;
				++length_;
				if (idx >= itemSize) {
					growWith((idx >> dataSize2Pow) - indexSize + 1);
				};
				value[idx >> dataSize2Pow]->value[idx & dataMask] = value_;
			};

			inline void push(const TType *value_) {
				size_t idx = length_;
				++length_;
				if (idx >= itemSize) {
					growWith((idx >> dataSize2Pow) - indexSize + 1);
				};
				value[idx >> dataSize2Pow]->value[idx & dataMask] = value_;
			};

			inline void copy(const TDynamicArray &value) {
				size_t k;
				empty();
				for (k = 0; k < value.length(); ++k) {
					value.get(k, index(k));
				};
			};
	};

};

#endif
