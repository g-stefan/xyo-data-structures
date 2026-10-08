# Containers

The managed containers are classes derived from `Object`. They are
allocated, shared and freed like any other managed object, and they know
about managed elements.

| Container | Template | Structure |
|-----------|----------|-----------|
| [`TStack`](#tstack) | `<T, TNodeMemory = TMemory>` | singly linked list (`TXList1`) |
| [`TDoubleEndedQueue`](#tdoubleendedqueue) | `<T, TNodeMemory = TMemory>` | doubly linked list with tail (`TXList3`) |
| [`TDynamicArray`](#tdynamicarray) | `<T, dataSize2Pow = 4, TNodeMemory = TMemory>` | index of blocks of `2^dataSize2Pow` elements |
| [`TRedBlackTree`](#tredblacktree) | `<TKey, TValue, TNodeMemory = TMemory>` | red-black tree (`TXRedBlackTree`) of key / value nodes |
| [`TRedBlackTreeOne`](#tredblacktreeone) | `<TKey, TNodeMemory = TMemory>` | red-black tree of keys |
| [`TAssociativeArray`](#tassociativearray) | `<TKey, TValue, dataSize2Pow = 4, TNodeMemory = TMemory>` | `TRedBlackTree<TKey, size_t>` + two `TDynamicArray` |

`TNodeMemory` is the allocator (see `xyo-managed-memory`, `docs/allocators.md`)
used for the nodes (blocks for `TDynamicArray`). The default `TMemory` is the
per-thread pool. Use `TMemorySystem` for plain new/delete (debugging with
ASan, or nodes released on another thread).

Common to all of them:

- not copyable or movable (`XYO_PLATFORM_DISALLOW_COPY_ASSIGN_MOVE`);
  `TDynamicArray` has an explicit `copy()`;
- `empty()` removes every element, `isEmpty()` tests (`TAssociativeArray`
  has `length()` only);
- results are reported with `bool` (`pop`, `get`, `remove`, ...), not
  exceptions;
- `activeDestructor()` calls `empty()`, so a container that sits in an
  active pool releases its elements when it is freed and is reused empty;
- `static initMemory()` initializes the element and node pools;
- **not thread safe**: a container belongs to one thread (see
  [TRing](ring.md) for a thread to thread queue).

## Element types

`T` (and `TKey` / `TValue`) can be:

| Element type | Stored as | Use when |
|--------------|-----------|----------|
| a value type: `int`, `std::string`, a struct | the value, copied or moved in | data |
| `TPointer<X>` | a counted reference, **a root** | the container is a local / executive container, or its elements can never lead back to whatever holds the container |
| `TPointerX<X>` | a dynamic link, **linked to the container** | the container is part of an object graph; cycles through it are collected |

For `TPointer` / `TPointerX` elements the containers offer extra overloads
that take or return the other forms, through `TPointerTypeExclude`:

- input: `const T &`, `T &&`, and **a raw `X *`** (`push(x.value())`,
  `set(key, x.value())`);
- output: `pop(TPointer<X> &)`, `pop(TPointerX<X> &)`, `get(index,
  TPointer<X> &)`, ... in addition to `T &`.

So a `TStack<TPointerX<Node>>` accepts `push(node)` with `node` a
`TPointer<Node>`, a `TPointerX<Node>` or a `Node *`, and `pop` into any of
them.

## Memory rules

These follow from the memory model of `xyo-managed-memory` (`TPointer` is
the executive link and a root, `TPointerX` is an edge of the object graph and
must be linked to the object that destroys it).

1. **A container used as a local variable** (on the stack, in `main`, in a
   function) is fine, with any element type. Prefer `TPointer<X>` elements.
2. **A container inside an object** is held through a `TPointerX` member,
   linked to the object, and allocated with `newMemory()`:

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

   Its elements are `TPointerX<X>`: each one is linked to the container
   (the containers call `pointerLink(this)` on new elements themselves), the
   container is linked to the object, and a cycle `Node → stack → Node` is
   collected.
3. **Do not put a container by value inside an `Object`.** It is not
   allocated by the library, so the collector treats it as a stack object,
   that is, a root. A cycle that goes through it is never collected. The
   same is true for `TPointer<X>` elements in any container held by an
   object: each element is a root. Both of these leak:

   ```cpp
   class Leaks : public Object {
   	public:
   		TDynamicArray<TPointerX<Leaks>> byValue;                 // container by value
   		TPointerX<TDynamicArray<TPointer<Leaks>>> rootElements; // TPointer elements
   };
   ```

4. Values stay in place: `find`, `begin`, `push()` without argument,
   `index`, `operator[]`, `insert(index)` return a pointer or a reference
   into the container. It is valid until that element is removed (for
   `TDynamicArray`, until elements are shifted by `insert`, `remove` or
   `shift`).
5. Holding a raw `X *` taken out of a container does not keep it alive. Get a
   `TPointer<X>` when the object must survive a later `remove` / `empty`.

## TStack

LIFO stack, a singly linked list of nodes `TStackNode<T>` (`next`, `value`).

```cpp
TStack<int> stack;
stack.push(1);
stack.push(2);

int x;
while (stack.pop(x)) {
	printf("%d\n", x); // 2, 1
};
```

| Member | Effect |
|--------|--------|
| `push(const T &)`, `push(T &&)`, `push(const TType *)` | push on top |
| `push()` | push a default constructed element (fill it through `head->value`) |
| `pop(T &)`, `pop(TPointerT &)`, `pop(TPointerXT &)` | pop the top into the argument, `false` if empty |
| `pop()` | drop the top, `false` if empty |
| `duplicate()` | push a copy of the top, `false` if empty |
| `swap()` | swap the two top elements, `false` if fewer than two |
| `head` | top node (`head->value`), `nullptr` if empty; walk with `node->next` |
| `empty()`, `isEmpty()` | |

## TDoubleEndedQueue

Deque, a doubly linked list with head and tail of nodes
`TDoubleEndedQueueNode<T>` (`back`, `next`, `value`). The head is the
"front": `push` / `pop` work there, `pushToTail` / `popFromTail` at the other
end. A FIFO queue is `pushToTail` + `pop`.

```cpp
TDoubleEndedQueue<std::string> queue;
queue.pushToTail("a");
queue.pushToTail("b");
queue.push("front");

std::string s;
while (queue.pop(s)) {
	printf("%s\n", s.c_str()); // front, a, b
};
```

| Member | Effect |
|--------|--------|
| `push(value)`, `push()` | insert at the head |
| `pop(value)`, `pop()` | remove from the head, `false` if empty |
| `pushToTail(value)`, `pushToTail()` | insert at the tail |
| `pushToTailX(value)` | insert at the tail and return the node |
| `popFromTail(value)`, `popFromTail()` | remove from the tail, `false` if empty |
| `head`, `tail` | first / last node; walk with `node->next` / `node->back` |
| `extractNode(node)` | unlink a node in O(1), **without freeing it** (free it with `deleteNode`) |
| `extractList(listHead, listTail)` | unlink the run of nodes `listHead..listTail` |
| `setList(listHead, listTail)` | empty the queue, then adopt a list of nodes |
| `newNode()`, `deleteNode(node)` | node allocation, for the functions above |
| `empty()`, `isEmpty()` | |

`value` can be `const T &`, `T &&` or `TType *` (`pop`: `T &`, `TPointerT &`,
`TPointerXT &`), as for `TStack`. `pushToTailX` + `extractNode` +
`deleteNode` give O(1) removal of an element you remember, for example an
LRU list.

## TDynamicArray

A growable array that never moves its elements when it grows. Storage is an
index of pointers to blocks (`TDynamicArrayNode`) of `dataSize =
2^dataSize2Pow` elements (16 by default). Growing adds blocks and copies only
the index. Index access is a shift and a mask.

```cpp
TDynamicArray<int> a;
a.push(10);
a.push(20);
a[2] = 30;          // operator[] / index() grow the array: length 3
a.set(3, 40);       // length 4
a.insert(0) = 5;    // shift right, then assign the new slot

for (size_t k = 0; k < a.length(); ++k) {
	printf("%d ", a.index(k)); // 5 10 20 30 40
};

int first;
if (a.shift(first)) { // remove index 0, first = 5
};
a.remove(0);
a.setLength(2);
```

| Member | Effect |
|--------|--------|
| `TDynamicArray(indexSize = 1)` | start with `indexSize` blocks |
| `index(i)`, `operator[](int)` | reference to element `i`; **grows** to `i + 1` if needed |
| `get(i, value)` | copy element `i` out, `false` if `i >= length()` (never grows) |
| `getValue(i, default)` | reference to element `i`; if missing, grow and store `default` |
| `set(i, value)` | store at `i`, grow if needed |
| `push(value)`, `push()` | append (the second form returns a reference to the new element) |
| `insert(i)` | shift `i..` right by one and return a reference to slot `i` (a moved-from value: assign it) |
| `remove(i)` | remove `i`, shift left, `false` if out of range |
| `shift(value)`, `shift()` | remove index 0 into `value` (O(n)), `false` if empty |
| `length()`, `setLength(n)` | set the length; see below for what shrinking and growing leave in the slots |
| `arraySize()` | current capacity |
| `growWith(blocks)` | add capacity |
| `copy(other)` | replace the content with a copy of `other` |
| `empty()`, `isEmpty()` | |

Slots are constructed once, with their block, and stay constructed. A slot
that is not in use always holds `T()`, so growing the array (with a gap,
`a[10] = x` on an empty array, or `setLength(n)` upwards) exposes `T()`
values: `0` for scalars, `""` for `std::string`, `nullptr` for `TPointer`.

- A new block is value-initialized.
- When an element is removed (`remove`, `shift`, `setLength` downwards,
  `empty`), its slot is reset. Types with lifetime hooks (`TPointer`,
  `TPointerX`, pooled objects) are reset by their `activeDestructor` /
  `activeConstructor` hooks: managed elements are released. Other types get
  `T()` assigned, so a removed `std::string` frees its buffer at once.
- A block returned to an active pool (`TNodeMemory = TMemoryPoolActive`) is
  reset the same way, so a reused block does not bring back old values.

A type with hooks is responsible for its own reset: its `activeDestructor`
must release or clear what a removed element should not keep.

`index()` and `operator[]` grow the array, they never fail. Use `get()` to
read without growing.

## TRedBlackTree

Ordered map. Each node `TRedBlackTreeNode<TKey, TValue, TNodeMemory>` has
`key`, `value`, `parent`, `left`, `right`, `color`, and the helpers
`successor()`, `predecessor()`, `minimum()`, `maximum()`. Keys are compared with
`TComparator<TKey>` (see `xyo-managed-memory`, containers): `operator<` and
`operator==` by default, `strcmp` for `const char *`, the pointed objects for
`TPointer` / `TPointerX`. Specialize `TComparator` in namespace
`XYO::ManagedMemory` for your own keys.

```cpp
TRedBlackTree<std::string, int> ages;
ages.set("Ana", 30);
ages.set("Dan", 25);
ages.set("Ana", 31);                            // replaces

int age;
if (ages.get("Ana", age)) {                     // 31
};
int bob = ages.getValue("Bob", -1);             // -1, nothing inserted

for (auto *node = ages.begin(); node; node = node->successor()) {
	printf("%s %d\n", node->key.c_str(), node->value); // key order
};

ages.remove("Dan");
```

| Member | Effect |
|--------|--------|
| `set(key, value)` | insert, or replace the value if the key exists (one descent) |
| `insert(key, value)` | always insert: **duplicate keys allowed** |
| `insertKey(key)` | insert a node with a default value and return it |
| `get(key, value)` | copy the value out, `false` if missing |
| `getValue(key, default)` | the value, or `default` if missing (nothing is inserted) |
| `find(key)` | the node, or `nullptr` |
| `remove(key)` | remove one node with that key, `false` if missing |
| `removeNode(node)` | remove and free a node |
| `insertNode(node)` | link a node from `newNode()` that you filled yourself |
| `begin()`, `end()` | first / last node in key order, `nullptr` if empty |
| `root` | root node |
| `empty()` | |

Overloads take keys and values as `TKey` or as a raw `TKeyType *` /
`TValueType *` when they are managed pointers, as for the other containers.

For a `TPointer` key or value, passing an rvalue next to a raw pointer can be
ambiguous (`set(const TKeyType *, rvalue)` against `set(const TKey &,
TValue &&)`); pass an lvalue.

Iterate backwards from `end()` with `node->predecessor()`.

## TRedBlackTreeOne

Ordered set: the same tree with `key` only.

```cpp
TRedBlackTreeOne<std::string> tags;
tags.set("b");
tags.set("a");
tags.set("b");                                  // already there, nothing happens

if (tags.has("a")) {
};
for (auto *node = tags.begin(); node; node = node->successor()) {
	printf("%s\n", node->key.c_str());          // a, b
};
tags.remove("a");
```

| Member | Effect |
|--------|--------|
| `set(key)` | insert if missing |
| `has(key)` | test |
| `find(key)` | the node, or `nullptr` |
| `remove(key)` | `false` if missing |
| `begin()`, `end()`, `root`, `empty()` | as for `TRedBlackTree` |

## TAssociativeArray

A map that keeps **insertion order** and allows index access. It is made of
a `TRedBlackTree<TKey, size_t>` from key to index (`mapKey`) and two
`TDynamicArray`s with the keys and the values (`arrayKey`, `arrayValue`), all
held by `TPointerX` members.

```cpp
TAssociativeArray<std::string, int> map;
map.set("one", 1);
map.set("two", 2);
map.set("three", 3);
map.set("one", 11);   // replaces, keeps the position
map.remove("two");

for (size_t k = 0; k < map.length(); ++k) {
	printf("%s = %d\n", map.arrayKey->index(k).c_str(), map.arrayValue->index(k));
}; // one = 11, three = 3

int v;
if (map.get("three", v)) {
};
```

| Member | Effect |
|--------|--------|
| `set(key, value)` | replace, or append a new key at index `length()` |
| `get(key, value)` | `false` if missing |
| `remove(key)` | remove and shift the following entries: **O(n)** |
| `length()` | number of entries |
| `arrayKey`, `arrayValue` | the key / value arrays, index `0..length() - 1` |
| `mapKey` | the key → index tree |
| `empty()` | |

Do not modify `arrayKey`, `arrayValue` or `mapKey` directly, only read them.

## Choosing a container

- Need order of insertion and lookup by key → `TAssociativeArray`.
- Lookup by key, sorted iteration → `TRedBlackTree` / `TRedBlackTreeOne`.
- Index access, append, mostly no removal in the middle → `TDynamicArray`.
- Queue, both ends, O(1) removal of a known node → `TDoubleEndedQueue`.
- LIFO only → `TStack`.
- Between exactly two threads → [`TRing`](ring.md).
- Full control over the node layout, no `Object` overhead →
  [intrusive containers](intrusive.md).
