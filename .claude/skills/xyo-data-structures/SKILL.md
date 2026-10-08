---
name: xyo-data-structures
description: >-
  How to use the xyo-data-structures C++ library (namespace
  XYO::DataStructures), the container layer of the XYO C++ stack on top of
  xyo-managed-memory: the managed containers TStack, TDoubleEndedQueue,
  TDynamicArray, TRedBlackTree (ordered map), TRedBlackTreeOne (ordered set),
  TAssociativeArray (insertion ordered map) holding values, TPointer or
  TPointerX elements; the intrusive TXList3 / TXList5 / TXArray and
  TStackPointerUnsafe; the single writer / single reader lock-free TRing;
  DynamicObject runtime type info (XYO_DYNAMIC_TYPE_DEFINE / IMPLEMENT / PUSH,
  TIsType, TDynamicCast); TStaticCast; the IRead / IWrite / ISeek stream
  interfaces; TReference. Use when writing or reviewing code that includes
  <XYO/DataStructures.hpp>, depends on "xyo-data-structures" in
  fabricare.json, uses any of these names, or when working inside the
  xyo-data-structures repository or libraries built on it
  (xyo-multithreading, xyo-system, quantum-script).
---

# xyo-data-structures

Container layer of the XYO C++ libraries, directly on top of
`xyo-managed-memory` (see the `xyo-managed-memory` skill: `Object`,
`TPointer`, `TPointerX`, allocators, threads; its rules all apply here).
Purpose: containers that **take part in the managed memory model**, so
object graphs that go through containers, cycles included, are still
collected; plus runtime type info and stream interfaces for the libraries
above.

Full documentation: `docs/` in the xyo-data-structures repository
(`X:\Storage\XYO\Gitea\CPP\xyo-data-structures\docs` on this machine):
README, getting-started, **containers** (element types and memory rules),
intrusive, ring, dynamic-object, reference. Read the matching page when you
need more than this summary. When in doubt read the header in
`source/XYO/DataStructures/`.

## Pick a container

| Need | Use |
|------|-----|
| LIFO | `TStack<T>`: `push`, `pop`, `duplicate`, `swap`, `head->value` |
| FIFO / deque | `TDoubleEndedQueue<T>`: `push`/`pop` at head, `pushToTail`/`popFromTail`; FIFO = `pushToTail` + `pop` |
| Array | `TDynamicArray<T, dataSize2Pow = 4>`: `push`, `index(i)`/`[i]` (grow), `get(i, v)` (no grow), `set`, `insert(i) = v`, `remove(i)`, `shift`, `length`, `setLength` |
| Ordered map | `TRedBlackTree<K, V>`: `set` (unique), `insert` (duplicates), `get`, `getValue(k, default)`, `find`, `remove`, `begin()` + `node->successor()` |
| Ordered set | `TRedBlackTreeOne<K>`: `set`, `has`, `remove` |
| Map in insertion order | `TAssociativeArray<K, V>`: `set`, `get`, `remove` (O(n)), `length()`, read `arrayKey->index(i)` / `arrayValue->index(i)` |
| Two threads, one writes one reads | `TRing<T>(size2Pow)` |
| Own node layout | `TXList3` (list + tail), `TXList5` (tree of children), `TXArray` (fixed capacity) |

The last template parameter `TNodeMemory` is the node allocator (default
`TMemory`, per-thread pool; `TMemorySystem` for new/delete, ASan, or nodes
freed on another thread). Intrusive containers default to `TMemorySystem`.

## Hard rules

1. **Element type.** Value types (`int`, `std::string`, structs) are stored
   by value. `TPointer<X>` elements are counted roots. `TPointerX<X>`
   elements are linked to the container automatically (the containers call
   `pointerLink(this)` on every new element).
2. **Container as a local** (executive): fine, prefer `TPointer<X>`
   elements.
3. **Container inside an `Object`**: hold it through a `TPointerX` member,
   `pointerLink(this)` + `newMemory()` in the constructor, and use
   `TPointerX<X>` elements. Then cycles through the container are collected:

   ```cpp
   class Node : public Object {
   		XYO_PLATFORM_DISALLOW_COPY_ASSIGN_MOVE(Node);

   	public:
   		TPointerX<TStack<TPointerX<Node>>> children;

   		inline Node() {
   			children.pointerLink(this);
   			children.newMemory();
   		};

   		static inline void initMemory() {
   			TPointerX<TStack<TPointerX<Node>>>::initMemory();
   		};
   };
   ```

4. **Never a container by value inside an `Object`**, and never
   `TPointer<X>` elements in a container owned by an object: both make
   roots, cycles through them **leak** (verified). The by-value container is
   not library allocated, so the collector treats it as a stack object.
5. Containers are `Object`s and not copyable or movable. Allocate them with
   `TPointer`/`TPointerX` `newMemory()`, or as locals. Use
   `TDynamicArray::copy()` to copy.
6. **One thread per container** (like every object graph). Only `TRing` is
   shared, and only between exactly one writer thread (`write`) and one
   reader thread (`read`, `peek`, `readNext`, `empty`). `resize` only while
   no other thread uses it. Managed objects passed through a `TRing` must be
   `TMemorySystem` allocated if the reader frees them, and must not link
   back into the writer's graph. Create threads with `xyo-multithreading`;
   call `XYO::ManagedMemory::Registry::registryInit()` first in `main`.
7. Raw pointers / references from `find`, `begin`, `index`, `[]`,
   `insert(i)`, `push()` are valid until that element is removed (for
   `TDynamicArray`, until a shift: `insert`, `remove`, `shift`). Take a
   `TPointer` to keep an object alive.
8. `TDynamicArray` slots stay constructed; a slot not in use holds `T()`
   (0, `""`, `nullptr`), also after growing with a gap. Removing an element
   resets its slot: hooked types (`TPointer`, `TPointerX`, pooled objects)
   through `activeDestructor`/`activeConstructor` (managed elements are
   released), other types by assigning `T()`. Blocks recycled by an active
   pool are reset too.
9. `index()` / `operator[]` / `set` **grow** the array, they never fail. Read
   with `get(i, v)` to stay in bounds.
10. `TRedBlackTree::insert` allows duplicate keys, `set` replaces.
    `getValue(key, default)` does not insert.
11. Backward tree iteration: `for (node = tree.end(); node; node =
    node->predecessor())`.
12. Keys are compared with `TComparator<K>` (`operator<` / `==`; `strcmp`
    for `const char *`; the pointees for `TPointer`/`TPointerX`). Specialize
    it in `namespace XYO::ManagedMemory` for custom keys (`isLess`,
    `compare` at least).
13. With `TPointer` keys or values, `set(rawPtr, rvalue)` can be ambiguous;
    pass an lvalue.

## Managed element overloads

For `T = TPointer<X>` or `TPointerX<X>`, inputs also accept a raw `X *` and
outputs accept the other pointer form: `push(x.value())`,
`pop(TPointer<X> &)`, `get(i, TPointerX<X> &)`, `set(key, x.value())`. The
types are `TType`, `TPointerT`, `TPointerXT` (`TKeyType`, `TValueType`,
`TPointerTValue`, `TPointerXTValue` for trees), from `TPointerTypeExclude`.

## DynamicObject

```cpp
class Shape : public DynamicObject {
		XYO_DYNAMIC_TYPE_DEFINE(, Shape);          // EXPORT macro, or empty in an exe; ends with private:

	public:
		Shape() {
			XYO_DYNAMIC_TYPE_PUSH(Shape);          // in EVERY constructor
		};
};
XYO_DYNAMIC_TYPE_IMPLEMENT(Shape, "{GUID}");       // one .cpp; unique key per class

TIsType<Shape>(obj);           // is a (base types included)
TIsTypeExact<Shape>(obj);      // most derived type
TDynamicCast<Shape *>(obj);    // checked downcast, nullptr if not
obj->getTypeKey(); TGetTypeKey<Shape>();
```

Type registration is thread safe and cross-DLL (by key). `TStaticCast` is an
unchecked, null safe cast to / from `Object *`.

## Interfaces

`IRead::read(void *, size_t)`, `IWrite::write(const void *, size_t)` return
the byte count (`read` returns 0 at the end). `ISeek`: `seekFromBegin`,
`seek` (relative), `seekFromEnd`, `seekTell`. All derive `virtual Object`;
implement several with `public virtual IRead, public virtual IWrite`.

## Intrusive containers

Static functions on pointers you own, node base adds the links:

- `TXList3<Node, Mem>` (`TXList3Node`: back, next) on `(head, tail)`:
  `push`/`pop`, `pushToTail`/`popFromTail`, `extract`, `extractList`,
  `swap`, `successor`/`predecessor`.
- `TXList5<Node, Mem>` (`TXList5Node`: back, next, childHead, childTail,
  parent): `push`/`pushToTail(parent, node)`, `pop`, `insertNode(parent,
  anchor, node)`, `extract`, `transferChild`, `transferChildAnchor`,
  `successor` (document order), `successorNoChild`, `predecessor`.
  **`newNode()` does not clear the five link fields; set them.**
  `destructor(node)` frees the node, its following siblings and all
  descendants.
- `TXArray<T>` on a `TXArrayT<T>` (`size`, `length`, `root`): `set`, `get`,
  `insert`, `remove`, `extract`, `resize` (only explicit growth).
- `TStackPointerUnsafe<T>`: `TStack<TPointer<T>>` without empty checks, plus
  `peek`, `popExecutive`, `popTransfer`, for interpreters.

## Using it

- `#include <XYO/DataStructures.hpp>` (umbrella). C++17.
  `XYO::DataStructures` does `using namespace XYO::ManagedMemory;`, so
  `Version`, `Copyright`, `License`, `TReference` are ambiguous: qualify
  them in full.
- fabricare consumer: `"dependency": ["xyo-data-structures"]` (DLL) or
  `["xyo-data-structures.static"]` (exports `XYO_DATASTRUCTURES_LIBRARY`).
  Install `xyo-platform`, `xyo-managed-memory`, then this library to the SDK
  before building dependents (see the `fabricare` skill).
- Without fabricare: compile `Platform.Amalgam.cpp`,
  `ManagedMemory.Amalgam.cpp`, `DataStructures.Amalgam.cpp` with
  `-DXYO_PLATFORM_LIBRARY -DXYO_MANAGEDMEMORY_LIBRARY
  -DXYO_DATASTRUCTURES_LIBRARY` (MSVC: also `/DXYO_PLATFORM_COMPILE_STATIC`),
  the three `source/` dirs on the include path, `-pthread` on Linux.

## Code style (match the repository)

- Tabs (width 8), `.clang-format` in the repo, CRLF line endings; statements
  and blocks end with `};`.
- camelCase methods, `retV` for return values, `this_` for the object /
  storage argument of static helpers, trailing `_` for some members
  (`length_`, `objectType_`).
- Headers: include guard `XYO_DATASTRUCTURES_<NAME>_HPP`, guarded includes
  (`#ifndef XYO_DATASTRUCTURES_DEPENDENCY_HPP #include ...`). Add new headers
  to `source/XYO/DataStructures.hpp` and new `.cpp` files to
  `source/XYO/DataStructures.Amalgam.cpp`. Exported functions use
  `XYO_DATASTRUCTURES_EXPORT`.
- Containers: derive `Object`, `XYO_PLATFORM_DISALLOW_COPY_ASSIGN_MOVE`,
  `static initMemory()` forwarding to `TIfHasInitMemory<T>` and the node
  memory, `activeDestructor()` calling `empty()`, nodes forwarding
  `activeConstructor`/`activeDestructor` with `TIfHasActive*`, and
  `TIfHasPointerLink<T>::pointerLink(&node->value, this)` before the first
  assignment of every new element.
- SPDX header: MIT for `source/`, Unlicense for `test/`.
- Tests: `test/test.NN.cpp` plus a `"category": "test"` project in
  `fabricare.json`; run `fabricare make` then `fabricare test`.
