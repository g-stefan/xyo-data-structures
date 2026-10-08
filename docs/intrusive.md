# Intrusive containers

Like `TXList1`, `TXList2` and `TXRedBlackTree` in `xyo-managed-memory`, these
are **static algorithms over storage you own**:

- you define the node type by deriving from the node base, which adds the
  link fields;
- the container is a `struct` of static functions working on your head /
  tail / root pointers, not an object;
- the optional `TNodeMemory` template parameter is the allocator used by
  `newNode()` / `deleteNode()` and by the functions that free nodes
  (`destructor`, `empty`). The default is `TMemorySystem`; pass `TMemory` for
  pooled nodes.

Use them for data structures with a special node layout, or inside your own
`Object`s, where the managed containers would add an object per container.
If a node holds a `TPointerX`, link it to the object that frees the node
(see `xyo-managed-memory`, containers, "Managed values in nodes").

## TXList3 — doubly linked list with head and tail

Node base `TXList3Node<Node>` adds `back` and `next`. The list is two
pointers, `head` and `tail`. It is the list behind `TDoubleEndedQueue`.

```cpp
struct Item : TXList3Node<Item> {
	int value;
};

typedef TXList3<Item, TMemory> ItemList;

Item *head;
Item *tail;
ItemList::constructor(head, tail);

for (int k = 0; k < 3; ++k) {
	Item *item = ItemList::newNode();
	item->value = k;
	ItemList::pushToTail(head, tail, item);
};

ItemList::deleteNode(ItemList::popFromTail(head, tail)); // removes 2

for (Item *scan = head; scan; scan = ItemList::successor(scan)) {
	printf("%d\n", scan->value); // 0, 1
};

ItemList::destructor(head);
```

| Function | Effect |
|----------|--------|
| `newNode()`, `deleteNode(node)`, `initMemory()` | node allocation with `TNodeMemory` |
| `constructor(head, tail)` | both `nullptr` |
| `destructor(head)` | delete every node (does not reset the pointers) |
| `empty(head, tail)` | delete every node and reset the pointers |
| `push(head, tail, node)` / `pop(head, tail)` | at the head; `pop` returns `nullptr` if empty |
| `pushToTail(head, tail, node)` / `popFromTail(head, tail)` | at the tail |
| `popUnsafe`, `popFromTailUnsafe` | without the empty check |
| `extract(head, tail, node)` | unlink any node in O(1) (does not free it) |
| `extractNode(node)` | unlink a node that is neither head nor tail |
| `extractList(head, tail, headX, tailX)` | unlink the run `headX..tailX` |
| `swap(head, tail)`, `swapOnTail(head, tail)` | swap the first / last two nodes, `false` if fewer than two |
| `successor(node)`, `predecessor(node)` | `next` / `back` |
| `begin(node)`, `end(node)` | walk from any node to the first / last node |

Popped and extracted nodes keep stale `back` / `next` values. The push
functions set them, so a node can be pushed again as is.

## TXList5 — tree of ordered children

Node base `TXList5Node<Node>` adds `back`, `next` (siblings), `childHead`,
`childTail` (children) and `parent`. Every node is a list of children: a
DOM / XML style tree, an outline, a scene graph. The root is a node too; its
`parent`, `back` and `next` are `nullptr`.

`newNode()` does not clear the link fields. Set all five before linking a
node:

```cpp
struct Element : TXList5Node<Element> {
	const char *name;
};

typedef TXList5<Element, TMemory> Document;

Element *newElement(const char *name) {
	Element *element = Document::newNode();
	element->name = name;
	element->back = element->next = element->parent = nullptr;
	element->childHead = element->childTail = nullptr;
	return element;
};

Element *root = newElement("html");
Element *body = newElement("body");
Document::pushToTail(root, newElement("head"));
Document::pushToTail(root, body);
Document::pushToTail(body, newElement("p"));

// depth first, document order: html, head, body, p
for (Element *scan = root; scan; scan = Document::successor(scan)) {
	printf("%s\n", scan->name);
};

Document::destructor(root); // the root and all its descendants
```

| Function | Effect |
|----------|--------|
| `newNode()`, `deleteNode(node)`, `initMemory()` | node allocation with `TNodeMemory` |
| `constructor(root)` | `root = nullptr` |
| `destructor(node)` | delete `node`, its following siblings and all their descendants (recursive) |
| `empty(root)` | `destructor` + `constructor` |
| `push(parent, node)` / `pushToTail(parent, node)` | add `node` as the first / last child of `parent` |
| `pop(parent)` / `popFromTail(parent)` | unlink the first / last child, `nullptr` if none; the returned node's `parent`, `back`, `next` are cleared |
| `insertNode(parent, anchor, node)` | insert `node` after the child `anchor` (`nullptr`: as first child) |
| `extract(parent, node)` | unlink a child (with its subtree), does not free it |
| `extractList(parent, headX, tailX)` | unlink the run of children `headX..tailX` |
| `addListToTail(parent, headX, tailX)` | append a run of nodes as children |
| `transferChild(parent, node)` | move all children of `node` to the end of `parent`'s children |
| `transferChildAnchor(parent, node)` | move all children of `node` right after `node`, in `parent` (unwrap an element) |
| `successor(node)` | next node in depth first (document) order |
| `successorNoChild(node)` | next node, skipping the subtree of `node` |
| `predecessor(node)` | previous node in document order |
| `begin(node)` | the root of the tree `node` is in |
| `end(node)` | the last node in document order |

## TXArray — fixed capacity array

A plain array with a capacity (`size`) and a length, in a small struct you
own, `TXArrayT<T>` (`size`, `length`, `root`). Storage is `new T[size]()`,
grown only by an explicit `resize`.

```cpp
typedef TXArray<std::string> Strings;

Strings::ArrayT array;
Strings::constructor(array, 4);              // capacity 4, length 0

Strings::set(array, 0, "a");
Strings::set(array, 1, "b");
Strings::insert(array, 0, "first");          // first, a, b

if (!Strings::insert(array, 9, "x")) {       // past the capacity: false
	Strings::resize(array, 16);
};

for (size_t k = 0; k < array.length; ++k) {
	printf("%s\n", array.root[k].c_str());
};

Strings::destructor(array);
```

| Function | Effect |
|----------|--------|
| `constructor(array, size)` | allocate `size` slots, length 0 |
| `destructor(array)` | free the storage |
| `empty(array)` | length 0 |
| `set(array, i, value)` | store at `i`; extends the length if `i < size`; `false` if `i >= size` |
| `get(array, i, value)` | `false` if `i >= length` |
| `insert(array, i, value)` | insert at `i`, shifting right; `false` if full or `i >= size`; `value` may be an element of the array |
| `remove(array, i)` / `extract(array, i, value)` | remove at `i`, shifting left; `false` if out of range |
| `resize(array, size)` | new capacity; the length is cut if it does not fit |

## TStackPointerUnsafe — unchecked stack of `TPointer<T>`

A `TStack<TPointer<T>>` without the safety checks, for code that keeps the
stack balanced itself, such as an interpreter's evaluation stack. `pop`,
`peek`, `duplicate` and `swap` on an empty stack are **undefined behavior**.
Elements are `TPointer<T>`, not linked (roots).

| Member | Effect |
|--------|--------|
| `push(const T *)`, `push()` | push (the second form pushes `nullptr`) |
| `pop(TPointer<T> &)`, `pop(TPointerX<T> &)`, `pop()` | pop, no empty check |
| `popExecutive(TPointerX<T> &)` | pop into a `TPointerX` without a mark-and-sweep (`setExecutive`) |
| `popTransfer()` | pop and return the reference as `TTransfer<T> *` without inc / dec |
| `peek(TPointer<T> &)`, `peek(TPointerX<T> &)`, `peek(T *&)` | read the top |
| `duplicate()`, `swap()` | no checks |
| `head`, `empty()`, `isEmpty()` | |

`popExecutive` and `popTransfer` are for code that manages the reference
balance itself (see `TTransfer` in `xyo-managed-memory`).
