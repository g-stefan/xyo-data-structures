// Data Structures
// Copyright (c) 2016-2026 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2026 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_DATASTRUCTURES_TRING_HPP
#define XYO_DATASTRUCTURES_TRING_HPP

#ifndef XYO_DATASTRUCTURES_DEPENDENCY_HPP
#	include <XYO/DataStructures/Dependency.hpp>
#endif

namespace XYO::DataStructures {

	//
	// Lock-free ring buffer, one writer thread and one reader thread (SPSC)
	//
	// - write() only from the writer thread
	// - read(), peek(), readNext(), empty() only from the reader thread
	// - length(), isEmpty() are exact from the reader thread, a snapshot from any other thread
	// - resize() is not concurrent, call it only while no other thread uses the ring
	// - a ring of (1 << size2Pow) slots holds at most (1 << size2Pow) - 1 items
	//
	// Memory ordering
	//   writer: store item, then release writeIndex  -> reader acquire writeIndex, then load item
	//   reader: consume item, then release readIndex -> writer acquire readIndex, then reuse slot
	// Each side keeps a private copy of the other side index and reloads it only when
	// the ring looks full (writer) or empty (reader), so the shared cache line is touched rarely.
	//

	template <typename T>
	class TRing {
			XYO_PLATFORM_DISALLOW_COPY_ASSIGN_MOVE(TRing);

		protected:
			// Keep writer and reader data on different cache lines, avoid false sharing.
			// Padding instead of alignas, so the ring works with any allocator.
			static constexpr size_t cacheLineSize = 64;

			// Shared, read only while both threads are running
			T *ring;
			size_t ringSize;
			size_t ringMask;
			char padding0[cacheLineSize];

			// Writer thread
			TAtomic<size_t> writeIndex;
			size_t readIndexCache;
			char padding1[cacheLineSize];

			// Reader thread
			TAtomic<size_t> readIndex;
			size_t writeIndexCache;
			char padding2[cacheLineSize];

			static inline size_t sizeFromPow(size_t size2Pow) {
				// a ring of 1 slot can not hold any item
				if (size2Pow < 1) {
					size2Pow = 1;
				};
				return (static_cast<size_t>(1) << size2Pow);
			};

			[[nodiscard]] inline bool canWrite(size_t nextWriteIndex) noexcept {
				if (nextWriteIndex == readIndexCache) {
					readIndexCache = readIndex.get(std::memory_order_acquire);
					if (nextWriteIndex == readIndexCache) {
						return false;
					};
				};
				return true;
			};

			[[nodiscard]] inline bool canRead(size_t currentReadIndex) noexcept {
				if (currentReadIndex == writeIndexCache) {
					writeIndexCache = writeIndex.get(std::memory_order_acquire);
					if (currentReadIndex == writeIndexCache) {
						return false;
					};
				};
				return true;
			};

			// Release what the consumed slot still references (managed pointers)
			inline void resetSlot(size_t index) {
				TIfHasActiveDestructor<T>::activeDestructor(&ring[index]);
				TIfHasActiveConstructor<T>::activeConstructor(&ring[index]);
			};

		public:
			inline TRing(size_t size2Pow) {
				ringSize = sizeFromPow(size2Pow);
				ringMask = (ringSize - 1);
				ring = new T[ringSize]();
				writeIndex.set(0, std::memory_order_relaxed);
				readIndex.set(0, std::memory_order_relaxed);
				readIndexCache = 0;
				writeIndexCache = 0;
			};

			inline ~TRing() {
				delete[] ring;
			};

			// Writer thread

			[[nodiscard]] inline bool write(const T &item) {
				const size_t currentWriteIndex = writeIndex.get(std::memory_order_relaxed);
				const size_t nextWriteIndex = (currentWriteIndex + 1) & ringMask;
				if (!canWrite(nextWriteIndex)) {
					return false;
				};
				ring[currentWriteIndex] = item;
				writeIndex.set(nextWriteIndex, std::memory_order_release);
				return true;
			};

			[[nodiscard]] inline bool write(T &&item) {
				const size_t currentWriteIndex = writeIndex.get(std::memory_order_relaxed);
				const size_t nextWriteIndex = (currentWriteIndex + 1) & ringMask;
				if (!canWrite(nextWriteIndex)) {
					return false;
				};
				ring[currentWriteIndex] = std::move(item);
				writeIndex.set(nextWriteIndex, std::memory_order_release);
				return true;
			};

			// Reader thread

			[[nodiscard]] inline bool read(T &item) {
				const size_t currentReadIndex = readIndex.get(std::memory_order_relaxed);
				if (!canRead(currentReadIndex)) {
					return false;
				};
				item = std::move(ring[currentReadIndex]);
				resetSlot(currentReadIndex);
				readIndex.set((currentReadIndex + 1) & ringMask, std::memory_order_release);
				return true;
			};

			// The item stays valid until readNext()
			[[nodiscard]] inline bool peek(T *&item) {
				const size_t currentReadIndex = readIndex.get(std::memory_order_relaxed);
				if (!canRead(currentReadIndex)) {
					return false;
				};
				item = &ring[currentReadIndex];
				return true;
			};

			[[nodiscard]] inline bool readNext() {
				const size_t currentReadIndex = readIndex.get(std::memory_order_relaxed);
				if (!canRead(currentReadIndex)) {
					return false;
				};
				resetSlot(currentReadIndex);
				readIndex.set((currentReadIndex + 1) & ringMask, std::memory_order_release);
				return true;
			};

			// Drop all items currently in the ring
			inline void empty() {
				while (readNext()) {
				};
			};

			// State

			[[nodiscard]] inline size_t capacity() const noexcept {
				return ringMask;
			};

			[[nodiscard]] inline size_t length() const noexcept {
				const size_t currentReadIndex = readIndex.get(std::memory_order_acquire);
				const size_t currentWriteIndex = writeIndex.get(std::memory_order_acquire);
				return (currentWriteIndex - currentReadIndex) & ringMask;
			};

			[[nodiscard]] inline bool isEmpty() const noexcept {
				return (readIndex.get(std::memory_order_acquire) == writeIndex.get(std::memory_order_acquire));
			};

			// Not concurrent

			[[nodiscard]] inline bool resize(size_t size2Pow) {
				const size_t newRingSize = sizeFromPow(size2Pow);
				if (newRingSize == ringSize) {
					return true;
				};
				const size_t newRingMask = (newRingSize - 1);
				const size_t currentWriteIndex = writeIndex.get();
				size_t currentReadIndex = readIndex.get();
				if (newRingSize < ringSize) {
					const size_t currentSize = (currentWriteIndex - currentReadIndex) & ringMask;
					if (currentSize >= newRingSize) {
						return false;
					};
				};
				T *newRing = new T[newRingSize]();
				size_t newWriteIndex = 0;
				while (currentReadIndex != currentWriteIndex) {
					newRing[newWriteIndex] = std::move(ring[currentReadIndex]);
					currentReadIndex = (currentReadIndex + 1) & ringMask;
					newWriteIndex = (newWriteIndex + 1) & newRingMask;
				};
				delete[] ring;
				ringSize = newRingSize;
				ringMask = newRingMask;
				ring = newRing;
				writeIndex.set(newWriteIndex);
				readIndex.set(0);
				readIndexCache = 0;
				writeIndexCache = newWriteIndex;
				return true;
			};
	};
};

#endif
