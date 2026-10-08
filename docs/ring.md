# TRing — single writer / single reader ring buffer

`TRing<T>` is a lock-free queue between **exactly two threads**: one thread
writes, the other one reads (SPSC). It is the only container of the library
meant to be shared between threads.

- Capacity is `2^size2Pow - 1` items (one slot always stays empty).
  `size2Pow` below 1 is raised to 1.
- `write` and `read` never block and never allocate. They return `false`
  when the ring is full / empty; the caller decides to retry, yield, wait on
  a condition variable or drop the item.
- Items are copied or moved into preallocated slots (`new T[size]()`), in
  order.
- `TRing` is not an `Object`: put it where both threads can reach it and
  where it outlives them (a member of the object that owns both threads, a
  local of the function that joins them, ...).

## Example

```cpp
TRing<int> queue(10); // 1024 slots, 1023 items

std::thread producer([&]() {
	for (int k = 0; k < 100000; ++k) {
		while (!queue.write(k)) {
			std::this_thread::yield(); // full
		};
	};
	while (!queue.write(-1)) {          // end marker
		std::this_thread::yield();
	};
});

long long sum = 0;
int value;
for (;;) {
	if (!queue.read(value)) {
		std::this_thread::yield();      // empty
		continue;
	};
	if (value < 0) {
		break;
	};
	sum += value;
};
producer.join();
```

In real code create the threads with `xyo-multithreading` (`Thread`), and
call `XYO::ManagedMemory::Registry::registryInit()` on the main thread first.
The example uses `std::thread` only to stay short.

## Which thread may call what

| Member | Thread | Effect |
|--------|--------|--------|
| `TRing(size2Pow)` | before sharing | allocate `2^size2Pow` slots |
| `write(const T &)`, `write(T &&)` | **writer** only | append, `false` if full |
| `read(T &)` | **reader** only | move the oldest item out, release the slot, `false` if empty |
| `peek(T *&)` | **reader** only | pointer to the oldest item, valid until `readNext()`; `false` if empty |
| `readNext()` | **reader** only | drop the oldest item, `false` if empty |
| `empty()` | **reader** only | drop all items |
| `length()`, `isEmpty()` | any | exact on the reader thread, a snapshot on any other thread |
| `capacity()` | any | `2^size2Pow - 1` |
| `resize(size2Pow)` | **no other thread active** | change the size and keep the items; `false` if they do not fit |

`peek` + `readNext` let the reader process an item in place, without a copy.

## Managed objects in a ring

The slot of an item that was read is reset with the item's
`activeDestructor` / `activeConstructor` hooks, so a `TPointer<X>` item does
not stay referenced by the ring after `read` / `readNext`.

Passing managed objects between threads must still follow the rules of
`xyo-managed-memory`:

- an object graph belongs to one thread: after the reader takes an object,
  the writer must not use it any more;
- an object released on a thread other than the one that allocated it must
  come from `TMemorySystem` (for example specialize
  `TMemory<Message> : TMemorySystem<Message>`), and must not hold links into
  the writer's graph.

Plain data (`int`, structs, `std::string`) has none of these concerns. When
in doubt, prefer copying the data through `xyo-multithreading` (`Transfer`,
`Worker`), which does the copy in the receiving thread.

## Memory ordering

The writer stores the item, then publishes the write index with release.
The reader acquires the write index, then reads the item. The other
direction (slot reuse) works the same way with the read index. Each side
keeps a private copy of the other side's index and reloads it only when the
ring looks full (writer) or empty (reader), so the shared cache line is
touched rarely. Writer and reader data are on separate cache lines.
