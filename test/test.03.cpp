// Created by Grigore Stefan <g_stefan@yahoo.com>
// Public domain (Unlicense) <http://unlicense.org>
// SPDX-FileCopyrightText: 2016-2026 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: Unlicense

// TRing: single thread behaviour and one writer / one reader stress

#include <XYO/DataStructures.hpp>
#include <chrono>
#include <cstdio>
#include <string>
#include <thread>

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

struct Probe : Object {
		static int alive;
		int x = 0;
		Probe() { ++alive; };
		~Probe() { --alive; };
};
int Probe::alive = 0;

namespace XYO::ManagedMemory {
	template <>
	struct TMemory<Probe> : TMemorySystem<Probe> {};
};

static TPointer<Probe> makeProbe(int x) {
	TPointer<Probe> p;
	p.newMemory();
	p->x = x;
	return p;
};

static void singleThread() {
	TRing<std::string> r(3);
	CHECK(r.capacity() == 7 && r.length() == 0 && r.isEmpty(), "initial state");
	for (int k = 0; k < 4; ++k) {
		CHECK(r.write(std::to_string(k)), "write");
	};
	CHECK(r.length() == 4 && !r.isEmpty(), "length 4");
	CHECK(!r.resize(2), "4 items must not fit a 4-slot ring");
	CHECK(r.resize(4) && r.capacity() == 15 && r.length() == 4, "grow");
	CHECK(r.resize(3) && r.length() == 4, "shrink back");
	for (int k = 4; k < 7; ++k) {
		CHECK(r.write(std::to_string(k)), "write after resize");
	};
	CHECK(!r.write("x") && r.length() == 7, "full");
	std::string s;
	std::string *ps = nullptr;
	CHECK(r.peek(ps) && *ps == "0", "peek");
	CHECK(r.readNext(), "readNext");
	for (int k = 1; k < 7; ++k) {
		CHECK(r.read(s) && s == std::to_string(k), "read order %d", k);
	};
	CHECK(!r.read(s) && !r.peek(ps) && !r.readNext() && r.isEmpty(), "empty");

	// wrap around many times
	for (int k = 0; k < 1000; ++k) {
		CHECK(r.write(std::to_string(k)), "wrap write");
		CHECK(r.read(s) && s == std::to_string(k), "wrap read");
	};

	for (int k = 0; k < 3; ++k) {
		CHECK(r.write(std::to_string(k)), "write");
	};
	CHECK(r.resize(2), "3 items fit a 4-slot ring");
	for (int k = 0; k < 3; ++k) {
		CHECK(r.read(s) && s == std::to_string(k), "read after shrink %d", k);
	};

	static_cast<void>(r.write("a"));
	static_cast<void>(r.write("b"));
	r.empty();
	CHECK(r.isEmpty() && r.length() == 0, "empty()");

	TRing<int> tiny(0); // clamped to 2 slots, 1 item
	CHECK(tiny.capacity() == 1 && tiny.write(1) && !tiny.write(2), "size2Pow 0 clamp");

	// consumed slots must not keep objects alive
	{
		TRing<TPointer<Probe>> rp(2);
		CHECK(rp.write(makeProbe(1)) && rp.write(makeProbe(2)), "write probes");
		TPointer<Probe> p;
		CHECK(rp.read(p) && p->x == 1, "read probe");
		p = nullptr;
		CHECK(Probe::alive == 1, "read releases slot, alive %d", Probe::alive);
		CHECK(rp.readNext(), "readNext probe");
		CHECK(Probe::alive == 0, "readNext releases slot, alive %d", Probe::alive);
		CHECK(rp.write(makeProbe(3)), "write probe 3");
		rp.empty();
		CHECK(Probe::alive == 0, "empty releases, alive %d", Probe::alive);
		CHECK(rp.write(makeProbe(4)), "write probe 4");
	};
	CHECK(Probe::alive == 0, "destructor releases, alive %d", Probe::alive);
};

// Message with fields that must be seen consistently by the reader
struct Message {
		size_t seq = 0;
		size_t check = 0;
		std::string payload;
};

static size_t checkOf(size_t seq) {
	return seq * 2654435761u + 12345;
};

static void stress(size_t size2Pow, size_t count, bool usePeek) {
	TRing<Message> r(size2Pow);
	size_t errors = 0;
	auto t0 = std::chrono::steady_clock::now();

	std::thread writer([&]() {
		Message m;
		for (size_t k = 0; k < count; ++k) {
			m.seq = k;
			m.check = checkOf(k);
			m.payload = std::to_string(k);
			while (!r.write(std::move(m))) {
				std::this_thread::yield();
			};
		};
	});

	std::thread reader([&]() {
		Message m;
		Message *pm;
		for (size_t k = 0; k < count; ++k) {
			if (usePeek) {
				while (!r.peek(pm)) {
					std::this_thread::yield();
				};
				if (pm->seq != k || pm->check != checkOf(k) || pm->payload != std::to_string(k)) {
					++errors;
				};
				static_cast<void>(r.readNext());
			} else {
				while (!r.read(m)) {
					std::this_thread::yield();
				};
				if (m.seq != k || m.check != checkOf(k) || m.payload != std::to_string(k)) {
					++errors;
				};
			};
		};
	});

	writer.join();
	reader.join();
	double ms = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - t0).count();
	CHECK(errors == 0, "stress size2Pow=%zu peek=%d errors=%zu", size2Pow, usePeek, errors);
	CHECK(r.isEmpty(), "stress ring empty at end");
	printf("  stress size2Pow=%2zu %s: %zu msgs in %7.1f ms (%.1f M msg/s), errors=%zu\n", size2Pow, usePeek ? "peek/readNext" : "read        ", count, ms, count / ms / 1000.0, errors);
};

int main(int cmdN, char *cmdS[]) {
	try {

		printf("TAtomic<size_t>::isAtomic=%d isAlwaysLockFree=%d\n", (int)TAtomic<size_t>::isAtomic, (int)TAtomic<size_t>::isAlwaysLockFree);
		singleThread();
		stress(1, 200000, false);
		stress(4, 2000000, false);
		stress(4, 2000000, true);
		stress(10, 2000000, false);
		stress(10, 2000000, true);

		printf(failures ? "* FAILED: %d\n" : "test.03: all passed\n", failures);
		return failures ? 1 : 0;

	} catch (const std::exception &e) {
		printf("* Error: %s\n", e.what());
	} catch (...) {
		printf("* Error: Unknown\n");
	};

	return 1;
};
