# API reference

All public symbols, grouped by header, in namespace `XYO::DataStructures`.
Include `<XYO/DataStructures.hpp>` to get all of them.

For managed element types, the containers define (with
`TPointerTypeExclude`):

| Name | `T` = value type | `T` = `TPointer<X>` | `T` = `TPointerX<X>` |
|------|------------------|---------------------|----------------------|
| `TType` (`TKeyType`, `TValueType`) | incomplete | `X` | `X` |
| `TPointerT` (`TPointerTValue`) | incomplete | incomplete | `TPointer<X>` |
| `TPointerXT` (`TPointerXTValue`) | incomplete | `TPointerX<X>` | incomplete |

An overload that uses an incomplete type can never be called, so each
container exposes exactly the forms that make sense for its element type.

## `Dependency.hpp`

Includes `<XYO/ManagedMemory.hpp>`. Inside the namespace:
`using namespace XYO::ManagedMemory;`.

| Macro | Value |
|-------|-------|
| `XYO_DATASTRUCTURES_EXPORT` | dllexport when `XYO_DATASTRUCTURES_INTERNAL` (or `XYO_DATA_STRUCTURES_INTERNAL`) is defined, dllimport otherwise, empty when `XYO_DATASTRUCTURES_LIBRARY` is defined |

## `TStack.hpp`, `TStackNode.hpp`

```cpp
template <typename T>
struct TStackNode : TXList1Node<TStackNode<T>> {
		T value;
		void activeConstructor();
		void activeDestructor();
};

template <typename T, template <typename U> class TNodeMemory = TMemory>
class TStack : public Object {
	public:
		typedef TStackNode<T> TNode, Node;
		typedef TXList1<TNode, TNodeMemory> TXStack;

		TNode *head;

		static TNode *newNode();
		static void deleteNode(TNode *);
		static void initMemory();

		void push(const T &value);
		void push(T &&value);
		void push(const TType *value);
		void push();                       // default element
		bool pop(T &value);
		bool pop(TPointerT &value);
		bool pop(TPointerXT &value);
		bool pop();
		bool duplicate();
		bool swap();
		void empty();
		bool isEmpty() const noexcept;
		void activeDestructor();           // empty()
};
```

## `TStackPointerUnsafe.hpp`

```cpp
template <typename T, template <typename U> class TNodeMemory = TMemory>
class TStackPointerUnsafe : public Object {    // no empty checks
	public:
		typedef TStackNode<TPointer<T>> TNode, Node;
		TNode *head;

		void push(const T *value);
		void push();
		void pop(TPointer<T> &value);
		void pop(TPointerX<T> &value);
		void popExecutive(TPointerX<T> &value);  // setExecutive, no mark-and-sweep
		TTransfer<T> *popTransfer();
		void pop();
		void peek(TPointer<T> &value);
		void peek(TPointerX<T> &value);
		void peek(T *&value);
		void duplicate();
		void swap();
		void empty();
		bool isEmpty() const;
};
```

## `TDoubleEndedQueue.hpp`, `TDoubleEndedQueueNode.hpp`

```cpp
template <typename T>
struct TDoubleEndedQueueNode : TXList3Node<TDoubleEndedQueueNode<T>> {
		T value;
};

template <typename T, template <typename U> class TNodeMemory = TMemory>
class TDoubleEndedQueue : public Object {
	public:
		typedef TDoubleEndedQueueNode<T> TNode, Node;
		typedef TXList3<TNode, TNodeMemory> TXList;

		TNode *head;
		TNode *tail;

		void push(const T &value);         // head
		void push(T &&value);
		void push(TType *value);
		void push();
		bool pop(T &value);
		bool pop(TPointerT &value);
		bool pop(TPointerXT &value);
		bool pop();

		void pushToTail(const T &value);   // tail
		void pushToTail(T &&value);
		void pushToTail(const TType *value);
		void pushToTail();
		TNode *pushToTailX(const T &value);
		TNode *pushToTailX(T &&value);
		TNode *pushToTailX(const TType *value);
		bool popFromTail(T &value);
		bool popFromTail(TPointerT &value);
		bool popFromTail(TPointerXT &value);
		bool popFromTail();

		void extractNode(TNode *node);     // unlink, not freed
		void extractList(TNode *&listHead, TNode *&listTail);
		void setList(TNode *listHead, TNode *listTail);  // empty(), then adopt

		void empty();
		bool isEmpty() const noexcept;
};
```

## `TDynamicArray.hpp`, `TDynamicArrayNode.hpp`

```cpp
template <typename T, size_t dataSize2Pow = 4, template <typename U> class TNodeMemory = TMemory>
class TDynamicArray : public Object {
	public:
		static constexpr size_t dataSize = 1 << dataSize2Pow;   // elements per block
		static constexpr size_t dataMask = dataSize - 1;
		typedef TDynamicArrayNode<T, dataSize> TNode, Node;     // T value[dataSize]

		size_t indexSize;                  // blocks
		size_t itemSize;                   // capacity
		size_t length_;
		TNode **value;                     // index of blocks

		TDynamicArray(size_t indexSize_ = 1);

		T &operator[](int idx);            // grows
		T &index(size_t idx);              // grows
		bool get(size_t index, T &object) const;
		bool get(size_t index, TPointerT &object) const;
		bool get(size_t index, TPointerXT &object) const;
		T &getValue(size_t index, const T &default_);      // grows, stores default_ if missing
		T &getValue(size_t index, const TType *default_);
		void set(size_t index, const T &object);           // grows
		void set(size_t index, T &&object);
		void set(size_t index, const TType *object);

		T &push();
		void push(const T &value_);
		void push(T &&value_);
		void push(const TType *value_);
		T &insert(size_t idx);             // shift right, return slot idx
		bool remove(size_t index);         // shift left
		bool shift(T &out);                // remove index 0
		bool shift(TPointerT &out);
		bool shift(TPointerXT &out);
		void shift();

		size_t length() const noexcept;
		void setLength(size_t newLength);
		size_t arraySize() const noexcept;
		void growWith(size_t count_);      // add blocks
		void copy(const TDynamicArray &value);
		void empty();
		bool isEmpty() const noexcept;
};
```

## `TRedBlackTree.hpp`, `TRedBlackTreeNode.hpp`

```cpp
template <typename TKey, typename TValue, template <typename U> class TNodeMemory>
struct TRedBlackTreeNode : TXRedBlackTreeNode<TRedBlackTreeNode<...>, TKey> {
		// parent, left, right, color, key from TXRedBlackTreeNode
		TValue value;
		TNode *minimum();
		TNode *maximum();
		TNode *successor();
		TNode *predecessor();
};

template <typename TKey, typename TValue, template <typename U> class TNodeMemory = TMemory>
class TRedBlackTree : public Object {
	public:
		typedef TRedBlackTreeNode<TKey, TValue, TNodeMemory> TNode, Node;
		typedef TXRedBlackTree<TNode, TNodeMemory> TXRBTree;

		TNode *root;

		TNode *find(const TKey &key);
		TNode *find(const TKeyType *key);

		void set(const TKey &key, const TValue &value);     // unique: replace or insert
		void set(const TKey &key, TValue &&value);
		void set(const TKey &key, const TValueType *value);
		void set(const TKeyType *key, const TValue &value);
		void set(const TKeyType *key, const TValueType *value);

		bool get(const TKey &key, TValue &value);
		bool get(const TKey &key, TPointerTValue &value);
		bool get(const TKey &key, TPointerXTValue &value);
		bool get(const TKeyType *key, TValueType &value);
		bool get(const TKeyType *key, TPointerTValue &value);
		bool get(const TKeyType *key, TPointerXTValue &value);

		TValue getValue(const TKey &key, const TValue &default);
		TPointer<TValueType> getValue(const TKey &key, const TValueType *default);
		TValue getValue(const TKeyType *key, const TValue &default);
		TPointer<TValueType> getValue(const TKeyType *key, const TValueType *default);

		void insert(const TKey &key, const TValue &value);  // duplicates allowed
		void insert(const TKey &key, TValue &&value);
		void insert(const TKey &key, const TValueType *value);
		void insert(const TKeyType *key, const TValue &value);
		void insert(const TKeyType *key, const TValueType *value);
		TNode *insertKey(const TKey &key);
		TNode *insertKey(const TKeyType *key);
		void insertNode(TNode *node);

		bool remove(const TKey &key);
		bool remove(const TKeyType *key);
		void removeNode(TNode *node);

		TNode *begin() noexcept;
		TNode *end() noexcept;
		void empty();
};
```

## `TRedBlackTreeOne.hpp`, `TRedBlackTreeOneNode.hpp`

```cpp
template <typename TKey, template <typename U> class TNodeMemory>
struct TRedBlackTreeNodeOne : TXRedBlackTreeNode<TRedBlackTreeNodeOne<...>, TKey> {
		TNode *minimum();
		TNode *maximum();
		TNode *successor();
		TNode *predecessor();
};

template <typename TKey, template <typename U> class TNodeMemory = TMemory>
class TRedBlackTreeOne : public Object {
	public:
		typedef TRedBlackTreeNodeOne<TKey, TNodeMemory> TNode, Node;
		typedef TXRedBlackTree<TNode, TNodeMemory> TXRBTree;

		TNode *root;

		TNode *find(const TKey &key);
		TNode *find(const TKeyType *key);
		void set(const TKey &key);         // insert if missing
		void set(TKey &&key);
		void set(const TKeyType *key);
		bool has(const TKey &key);
		bool has(const TKeyType *key);
		bool remove(const TKey &key);
		bool remove(const TKeyType *key);
		TNode *begin() noexcept;
		TNode *end() noexcept;
		void empty();
};
```

## `TAssociativeArray.hpp`

```cpp
template <typename TKey, typename TValue, size_t dataSize2Pow = 4, template <typename U> class TNodeMemory = TMemory>
class TAssociativeArray : public Object {
	public:
		typedef TRedBlackTree<TKey, size_t, TNodeMemory> TMapKey, MapKey;
		typedef TDynamicArray<TKey, dataSize2Pow, TNodeMemory> TArrayKey, ArrayKey;
		typedef TDynamicArray<TValue, dataSize2Pow, TNodeMemory> TArrayValue, ArrayValue;
		typedef typename TMapKey::Node MapKeyNode;

		TPointerX<TMapKey> mapKey;         // key -> index
		TPointerX<TArrayKey> arrayKey;     // keys in insertion order
		TPointerX<TArrayValue> arrayValue; // values in insertion order
		size_t length_;

		bool get(const TKey &key, TValue &value) const;
		bool get(const TKey &key, TPointerTValue &value) const;
		bool get(const TKey &key, TPointerXTValue &value) const;
		bool get(const TKeyType *key, TPointerTValue &value) const;
		bool get(const TKeyType *key, TPointerXTValue &value) const;
		void set(const TKey &key, const TValue &value);
		void set(const TKey &key, TValue &&value);
		void set(const TKey &key, const TValueType *value);
		void set(const TKeyType *key, const TValueType *value);
		bool remove(const TKey &key);      // O(n)
		bool remove(const TKeyType *key);
		size_t length() const noexcept;
		void empty();
};
```

## `TRing.hpp`

```cpp
template <typename T>
class TRing {                              // not an Object, not copyable
	public:
		TRing(size_t size2Pow);            // 2^size2Pow slots, size2Pow >= 1

		bool write(const T &item);         // writer thread
		bool write(T &&item);

		bool read(T &item);                // reader thread
		bool peek(T *&item);               // valid until readNext()
		bool readNext();
		void empty();

		size_t capacity() const noexcept;  // 2^size2Pow - 1
		size_t length() const noexcept;
		bool isEmpty() const noexcept;

		bool resize(size_t size2Pow);      // no concurrent use
};
```

## `TXList3.hpp`, `TXList3Node.hpp`

```cpp
template <typename TNode>
struct TXList3Node { TNode *back; TNode *next; };

template <typename TNode, template <typename U> class TNodeMemory = TMemorySystem>
struct TXList3 {                           // all static
		TNode *newNode();
		void deleteNode(TNode *);
		void initMemory();
		void constructor(TNode *&head, TNode *&tail);
		void destructor(TNode *head);
		void empty(TNode *&head, TNode *&tail);
		void push(TNode *&head, TNode *&tail, TNode *node);
		TNode *pop(TNode *&head, TNode *&tail);
		TNode *popUnsafe(TNode *&head, TNode *&tail);
		void pushToTail(TNode *&head, TNode *&tail, TNode *node);
		TNode *popFromTail(TNode *&head, TNode *&tail);
		TNode *popFromTailUnsafe(TNode *&head, TNode *&tail);
		void extractNode(TNode *node);
		void extract(TNode *&head, TNode *&tail, TNode *node);
		void extractList(TNode *&head, TNode *&tail, TNode *&headX, TNode *&tailX);
		bool swap(TNode *&head, TNode *&tail);
		bool swapOnTail(TNode *&head, TNode *&tail);
		TNode *successor(TNode *node);
		TNode *predecessor(TNode *node);
		TNode *begin(TNode *node);
		TNode *end(TNode *node);
};
```

## `TXList5.hpp`, `TXList5Node.hpp`

```cpp
template <typename TNode>
struct TXList5Node {
		TNode *back;
		TNode *next;
		TNode *childHead;
		TNode *childTail;
		TNode *parent;
};

template <typename TNode, template <typename U> class TNodeMemory = TMemorySystem>
struct TXList5 {                           // all static
		TNode *newNode();                  // link fields not initialized
		void deleteNode(TNode *);
		void initMemory();
		void constructor(TNode *&root);
		void destructor(TNode *root);      // node, following siblings, descendants
		void empty(TNode *&root);
		void push(TNode *root, TNode *node);           // first child
		void pushToTail(TNode *root, TNode *node);     // last child
		TNode *pop(TNode *root);
		TNode *popUnsafe(TNode *root);
		TNode *popFromTail(TNode *root);
		TNode *popFromTailUnsafe(TNode *root);
		void extractNode(TNode *node);
		void extract(TNode *root, TNode *node);
		void extractList(TNode *root, TNode *headX, TNode *tailX);
		void addListToTail(TNode *root, TNode *headX, TNode *tailX);
		void insertNode(TNode *root, TNode *anchor, TNode *node);  // after anchor
		void transferChild(TNode *root, TNode *node);
		void transferChildAnchor(TNode *root, TNode *node_);
		TNode *successor(TNode *node);         // document order
		TNode *successorNoChild(TNode *node);  // skip the subtree
		TNode *predecessor(TNode *node);
		TNode *begin(TNode *node);             // topmost ancestor
		TNode *end(TNode *node);               // last node in document order
};
```

## `TXArray.hpp`, `TXArrayT.hpp`

```cpp
template <typename T>
struct TXArrayT { size_t size; size_t length; T *root; };

template <typename T>
struct TXArray {                           // all static
		typedef TXArrayT<T> ArrayT;
		typedef T Element;
		void initMemory();
		void constructor(ArrayT &this_, size_t size);
		void destructor(ArrayT &this_);
		void empty(ArrayT &this_);
		bool set(ArrayT &this_, size_t index, const T &value);
		bool get(ArrayT &this_, size_t index, T &value);
		void resize(ArrayT &this_, size_t size);
		bool remove(ArrayT &this_, size_t index);
		bool extract(ArrayT &this_, size_t index, T &value);
		bool insert(ArrayT &this_, size_t index, const T &value);
};
```

## `DynamicObject.hpp`

```cpp
#define XYO_DYNAMIC_TYPE_DEFINE(EXPORT, T)     // in the class body; ends with private:
#define XYO_DYNAMIC_TYPE_IMPLEMENT(T, KEY)     // in one .cpp file
#define XYO_DYNAMIC_TYPE_PUSH(T)               // in every constructor of T

struct DynamicTypeNode : TXList1Node<DynamicTypeNode> { const void *type; };

class DynamicObject : public Object {
	public:
		static const void *getType();
		static void initMemory();
		static const char *getTypeKey(const void *type);
		const char *getTypeKey();
		bool isType(const void *type);
		bool isTypeExact(const void *type) noexcept;
		bool isSameType(DynamicObject *dynamicObject) noexcept;

	protected:
		static const void *registerType(TAtomic<const void *> &type, const char *key);
		void objectTypePush(const void *type);
		bool objectTypeSearchNext(const void *type);
		DynamicTypeNode *objectType_;
};

template <typename T> bool TIsType(DynamicObject *object);
template <typename T> bool TIsTypeExact(DynamicObject *object);
template <typename T> const char *TGetTypeKey();
template <typename T> T TDynamicCast(DynamicObject *object);   // T = Derived *, nullptr if not a Derived
template <typename T> DynamicObject *TDynamicCast(T this_);    // upcast

// in XYO::ManagedMemory
template <> struct TMemory<DynamicObject> : TMemoryPoolActive<DynamicObject> {};
```

## `TStaticCast.hpp`

```cpp
template <typename T> T TStaticCast(Object *this_);      // unchecked downcast, null safe
template <typename T> Object *TStaticCast(T this_);      // upcast, null safe
```

## `IRead.hpp`, `IWrite.hpp`, `ISeek.hpp`

```cpp
class IRead : public virtual Object {
		virtual size_t read(void *output, size_t length) = 0;
};
class IWrite : public virtual Object {
		virtual size_t write(const void *input, size_t length) = 0;
};
class ISeek : public virtual Object {
		virtual bool seekFromBegin(uint64_t x) = 0;
		virtual bool seek(uint64_t x) = 0;
		virtual bool seekFromEnd(uint64_t x) = 0;
		virtual uint64_t seekTell() = 0;
};
```

## `TReference.hpp`

```cpp
template <typename T, void DeleteMemoryT(T *)>
class TReference {                         // not copyable / movable, no default constructor
	public:
		TReference(const T *value);
		~TReference();                     // DeleteMemoryT(object)
		T *operator->() const noexcept;
		operator T *() const noexcept;
		T *value() const noexcept;
};
```

## `Version.hpp`, `Copyright.hpp`, `License.hpp`

```cpp
namespace XYO::DataStructures::Version {
	const char *version();
	const char *build();
	const char *versionWithBuild();
	const char *datetime();
};
namespace XYO::DataStructures::Copyright {
	const char *copyright();
	const char *publisher();
	const char *company();
	const char *contact();
};
namespace XYO::DataStructures::License {
	std::string license();
	std::string shortLicense();
};
```
