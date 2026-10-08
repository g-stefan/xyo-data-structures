# XYO Data Structures — Documentation

`xyo-data-structures` is the container layer of the XYO C++ library stack. It
sits directly on top of `xyo-managed-memory` and gives every library above it
(`xyo-multithreading`, `xyo-system`, `quantum-script`, ...) a set of
containers and object utilities that **work with the managed memory model**:

- **Managed containers.** `TStack`, `TDoubleEndedQueue`, `TDynamicArray`,
  `TRedBlackTree` (ordered map), `TRedBlackTreeOne` (ordered set) and
  `TAssociativeArray` (insertion ordered map) are `Object`s themselves. They
  hold plain values (`int`, `std::string`, structs) or managed objects
  (`TPointer<T>` / `TPointerX<T>`), and they link `TPointerX` elements to
  themselves, so **reference cycles that go through a container are
  collected** like any other cycle.
- **Pooled nodes.** Nodes come from the per-thread pools of
  `xyo-managed-memory` (`TMemory` by default): no lock, no system allocator
  call on the common path. Lifetime hooks (`activeConstructor`,
  `activeDestructor`, `initMemory`) are forwarded to the elements, so
  containers can live in active pools and be reused.
- **Intrusive building blocks.** `TXList3` (doubly linked list with tail),
  `TXList5` (tree of ordered children, DOM style) and `TXArray` (fixed
  capacity array) are static algorithms over nodes or storage you own, for
  when you need full control.
- **A lock-free ring buffer.** `TRing` is a single writer / single reader
  queue between two threads.
- **Runtime type information.** `DynamicObject` gives class hierarchies a
  cheap, DLL-safe "is a" test and checked downcast (`TDynamicCast`),
  independent of C++ RTTI.
- **Stream interfaces.** `IRead`, `IWrite`, `ISeek`, implemented by files,
  buffers, sockets, ... in the libraries above.

```
quantum-script, applications ...
xyo-system, xyo-encoding, xyo-cryptography, ...
xyo-multithreading
xyo-data-structures   <-- this library
xyo-managed-memory    (Object, TPointer, TPointerX, pools, TXList1/2, TXRedBlackTree)
xyo-platform          (macros, TAtomic, CriticalSection, Thread)
```

## Why use it

- **No `delete`, no leaks through containers.** Store `TPointerX<T>` in a
  container owned by an object, and the whole graph (objects, containers,
  cycles) is freed when the last `TPointer` to it goes away.
- **Fast.** Node allocation is a pop from a thread local free list.
  `TDynamicArray` grows by blocks, without moving existing elements.
- **Same API shape everywhere.** `push` / `pop` / `set` / `get` / `remove` /
  `empty` / `isEmpty` / `length`, `bool` results instead of exceptions.
- **Works across DLLs.** Pools and `DynamicObject` type ids are looked up by
  name in a process wide registry.

## Concepts at a glance

| Need | Use | Notes |
|------|-----|-------|
| LIFO stack | `TStack<T>` | singly linked, `push` / `pop` / `duplicate` / `swap` |
| FIFO / deque | `TDoubleEndedQueue<T>` | `push` / `pop` at the head, `pushToTail` / `popFromTail` |
| Growable array | `TDynamicArray<T>` | index access, auto grow, `push` / `insert` / `remove` / `shift` |
| Ordered map | `TRedBlackTree<TKey, TValue>` | `set` (unique), `insert` (duplicates), `get`, `find`, iterate in key order |
| Ordered set | `TRedBlackTreeOne<TKey>` | `set`, `has`, `remove` |
| Map in insertion order | `TAssociativeArray<TKey, TValue>` | tree index + two arrays; `remove` is O(n) |
| Thread to thread queue | `TRing<T>` | lock-free, exactly one writer and one reader thread |
| Your own nodes | `TXList3`, `TXList5`, `TXArray` | intrusive, static functions |
| Type test / downcast | `DynamicObject`, `TIsType`, `TDynamicCast` | no C++ RTTI needed |
| Streams | `IRead`, `IWrite`, `ISeek` | interfaces only |

## Contents

| Document | What it covers |
|----------|----------------|
| [Getting started](getting-started.md) | Build it, depend on it (DLL or static), first program, threads, building without fabricare |
| [Containers](containers.md) | The managed containers, element types and the memory rules, choosing a container |
| [Intrusive containers](intrusive.md) | `TXList3`, `TXList5`, `TXArray`, `TStackPointerUnsafe` |
| [TRing](ring.md) | The single writer / single reader lock-free ring buffer |
| [Dynamic objects and interfaces](dynamic-object.md) | `DynamicObject`, `TDynamicCast`, `TStaticCast`, `IRead` / `IWrite` / `ISeek`, `TReference` |
| [API reference](reference.md) | Every public symbol on one page |

The memory model itself (`Object`, `TPointer`, `TPointerX`, allocators,
threads) is documented in the `xyo-managed-memory` repository, `docs/`.

## Source map

```
source/XYO/DataStructures.hpp                umbrella header, include this
source/XYO/DataStructures.Amalgam.cpp        compiled part of the library in one translation unit
source/XYO/DataStructures/
    Dependency.hpp                           xyo-managed-memory, export macros
    TStack[Node].hpp                         LIFO stack
    TStackPointerUnsafe.hpp                  unchecked stack of TPointer<T>, for interpreters
    TDoubleEndedQueue[Node].hpp              deque
    TDynamicArray[Node].hpp                  block array
    TRedBlackTree[Node].hpp                  ordered map
    TRedBlackTreeOne[Node].hpp               ordered set
    TAssociativeArray.hpp                    insertion ordered map
    TRing.hpp                                SPSC lock-free ring buffer
    TXArray[T].hpp                           intrusive fixed capacity array
    TXList3[Node].hpp                        intrusive doubly linked list with tail
    TXList5[Node].hpp                        intrusive tree of ordered children
    DynamicObject[.cpp]                      runtime type information
    TStaticCast.hpp                          null safe static_cast to / from Object
    TReference.hpp                           scoped holder with a fixed deleter
    IRead / IWrite / ISeek                   stream interfaces
    Copyright / License / Version            library metadata
test/test.01.cpp                             TRedBlackTree smoke test
test/test.02.cpp                             containers: bounds, element lifetime, invariants
test/test.03.cpp                             TRing: single thread and writer / reader stress
test/test.04.cpp                             DynamicObject: concurrent type registration, type checks
```

## AI assistant skill

A Claude Code skill describing how to use this library lives in
[`.claude/skills/xyo-data-structures/`](../.claude/skills/xyo-data-structures/SKILL.md).
It is picked up automatically inside this repository; copy the folder to
`~/.claude/skills/` to have it available in the projects that depend on
`xyo-data-structures`.
