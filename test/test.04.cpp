// Created by Grigore Stefan <g_stefan@yahoo.com>
// Public domain (Unlicense) <http://unlicense.org>
// SPDX-FileCopyrightText: 2016-2026 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: Unlicense

// DynamicObject: type registration on first use from many threads, type checks

#include <XYO/DataStructures.hpp>
#include <cstdio>
#include <cstring>
#include <atomic>
#include <thread>
#include <vector>

using namespace XYO::DataStructures;

static int failures = 0;
#define CHECK(cond, ...)                                           \
	do {                                                       \
		if (!(cond)) {                                     \
			++failures;                                \
			printf("FAIL %s:%d ", __FILE__, __LINE__); \
			printf(__VA_ARGS__);                       \
			printf("\n");                              \
		};                                                 \
	} while (0)

class ShapeA : public DynamicObject {
		XYO_DYNAMIC_TYPE_DEFINE(, ShapeA);

	public:
		ShapeA() {
			XYO_DYNAMIC_TYPE_PUSH(ShapeA);
		};
};

class ShapeB : public ShapeA {
		XYO_DYNAMIC_TYPE_DEFINE(, ShapeB);

	public:
		ShapeB() {
			XYO_DYNAMIC_TYPE_PUSH(ShapeB);
		};
};

class ShapeC : public DynamicObject {
		XYO_DYNAMIC_TYPE_DEFINE(, ShapeC);

	public:
		ShapeC() {
			XYO_DYNAMIC_TYPE_PUSH(ShapeC);
		};
};

XYO_DYNAMIC_TYPE_IMPLEMENT(ShapeA, "{7E0B1C52-3D59-4F0A-9A3B-2B8E6C1D4A01}");
XYO_DYNAMIC_TYPE_IMPLEMENT(ShapeB, "{7E0B1C52-3D59-4F0A-9A3B-2B8E6C1D4A02}");
XYO_DYNAMIC_TYPE_IMPLEMENT(ShapeC, "{7E0B1C52-3D59-4F0A-9A3B-2B8E6C1D4A03}");

// No type is used before the threads start, so they all race on the first registration
static void concurrentFirstUse() {
	const size_t threadCount = 16;
	std::atomic<bool> start(false);
	std::vector<const void *> seenA(threadCount), seenB(threadCount), seenC(threadCount);
	std::vector<std::thread> threads;
	for (size_t k = 0; k < threadCount; ++k) {
		threads.emplace_back([&, k]() {
			while (!start.load(std::memory_order_acquire)) {
				std::this_thread::yield();
			};
			// different order per thread
			if (k & 1) {
				seenC[k] = ShapeC::getType();
				seenB[k] = ShapeB::getType();
				seenA[k] = ShapeA::getType();
			} else {
				seenA[k] = ShapeA::getType();
				seenB[k] = ShapeB::getType();
				seenC[k] = ShapeC::getType();
			};
		});
	};
	start.store(true, std::memory_order_release);
	for (std::thread &thread : threads) {
		thread.join();
	};

	const void *typeA = ShapeA::getType();
	const void *typeB = ShapeB::getType();
	const void *typeC = ShapeC::getType();
	CHECK(typeA != nullptr && typeB != nullptr && typeC != nullptr, "types registered");
	CHECK(typeA != typeB && typeA != typeC && typeB != typeC, "types are distinct");
	for (size_t k = 0; k < threadCount; ++k) {
		CHECK(seenA[k] == typeA && seenB[k] == typeB && seenC[k] == typeC, "thread %zu saw another type", k);
	};
	CHECK(strcmp(TGetTypeKey<ShapeB>(), "{7E0B1C52-3D59-4F0A-9A3B-2B8E6C1D4A02}") == 0, "type key");
};

static void typeChecks() {
	ShapeA a;
	ShapeB b;
	ShapeC c;
	CHECK(TIsTypeExact<ShapeA>(&a) && TIsType<DynamicObject>(&a), "a type");
	CHECK(TIsTypeExact<ShapeB>(&b) && TIsType<ShapeA>(&b) && !TIsTypeExact<ShapeA>(&b), "b is a ShapeA");
	CHECK(!TIsType<ShapeB>(&a) && !TIsType<ShapeA>(&c), "unrelated types");
	CHECK(TDynamicCast<ShapeA *>(static_cast<DynamicObject *>(&b)) == &b, "cast to base");
	CHECK(TDynamicCast<ShapeB *>(static_cast<DynamicObject *>(&a)) == nullptr, "cast to derived fails");
	CHECK(b.isSameType(&b) && !b.isSameType(&a), "same type");
	CHECK(strcmp(b.getTypeKey(), TGetTypeKey<ShapeB>()) == 0, "object type key");
};

int main(int cmdN, char *cmdS[]) {
	try {

		// Register the process and this thread in the XYO registry before any other thread uses it,
		// else the first std::thread to touch it becomes the registered one
		Registry::registryInit();

		concurrentFirstUse();
		typeChecks();

		printf(failures ? "* FAILED: %d\n" : "test.04: all passed\n", failures);
		return failures ? 1 : 0;

	} catch (const std::exception &e) {
		printf("* Error: %s\n", e.what());
	} catch (...) {
		printf("* Error: Unknown\n");
	};

	return 1;
};
