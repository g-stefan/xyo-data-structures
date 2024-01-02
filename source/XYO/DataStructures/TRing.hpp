// Data Structures
// Copyright (c) 2016-2024 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2024 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_DATASTRUCTURES_TRING_HPP
#define XYO_DATASTRUCTURES_TRING_HPP

#ifndef XYO_DATASTRUCTURES_DEPENDENCY_HPP
#	include <XYO/DataStructures/Dependency.hpp>
#endif

namespace XYO::DataStructures {

	//
	// One Writer , One Reader
	//

	template <typename T>
	class TRing {
			XYO_DISALLOW_COPY_ASSIGN_MOVE(TRing);

		protected:
			size_t ringSize;
			size_t ringMask;

			T *ring;
			TAtomic<size_t> writeIndex;
			TAtomic<size_t> readIndex;

		public:
			inline TRing(size_t size2Pow) {
				ringSize = (1 << size2Pow);
				ringMask = ((1 << size2Pow) - 1);
				ring = new T[ringSize]();
				writeIndex.set(0);
				readIndex.set(0);
			};

			inline bool write(const T &item) {
				const size_t currentWriteIndex = writeIndex.get();
				const size_t nextWriteIndex = (currentWriteIndex + 1) & ringMask;
				if (nextWriteIndex == readIndex.get()) {
					return false;
				};
				ring[currentWriteIndex] = item;
				writeIndex.set(nextWriteIndex);
				return true;
			};

			inline bool read(T &item) {
				const size_t currentReadIndex = readIndex.get();
				if (readIndex == writeIndex.get()) {
					return false;
				};
				item = ring[currentReadIndex];
				readIndex.set((currentReadIndex + 1) & ringMask);
				return true;
			};

			inline bool peek(T *&item) {
				const size_t currentReadIndex = readIndex.get();
				if (readIndex == writeIndex.get()) {
					return false;
				};
				item = &ring[currentReadIndex];
				return true;
			};

			inline bool readNext() {
				const size_t currentReadIndex = readIndex.get();
				if (readIndex == writeIndex.get()) {
					return false;
				};
				readIndex.set((currentReadIndex + 1) & ringMask);
				return true;
			};

			inline bool resize(size_t size2Pow) {
				size_t newRingSize = (1 << size2Pow);
				if (newRingSize == ringSize) {
					return true;
				};
				size_t newRingMask = ((1 << size2Pow) - 1);
				T *newRing = new T[newRingSize]();
				const size_t currentWriteIndex = writeIndex.get();
				size_t currentReadIndex = readIndex.get();
				if (newRingSize < ringSize) {
					size_t currentSize = (currentWriteIndex - currentReadIndex) & ringMask;
					if (currentSize > newRingSize) {
						return false;
					};
				};
				size_t newWriteIndex = 0;
				while (true) {
					if (currentReadIndex == currentWriteIndex) {
						break;
					};
					newRing[newWriteIndex] = ring[currentReadIndex];
					currentReadIndex = (currentReadIndex + 1) & ringMask;
					newWriteIndex = (newWriteIndex + 1) & newRingMask;
				};
				delete[] ring;
				ringSize = newRingSize;
				ringMask = newRingMask;
				ring = newRing;
				writeIndex.set(newWriteIndex);
				readIndex.set(0);
				return true;
			};
	};
};

#endif
