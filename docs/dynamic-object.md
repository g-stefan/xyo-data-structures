# Dynamic objects and interfaces

## DynamicObject — runtime type information

`DynamicObject` is an `Object` that knows its type and all its base types.
It gives class hierarchies an "is a" test and a checked downcast without C++
RTTI (`dynamic_cast`, `typeid`), and the type identity is the same in every
DLL of the process. Script engines and object models use it to check the
type of values held as `DynamicObject *` / `Object *`.

### Declaring a type

Every class in the hierarchy does three things:

1. `XYO_DYNAMIC_TYPE_DEFINE(EXPORT, ClassName)` in the class body. `EXPORT`
   is your library's export macro, or empty in an executable.
2. `XYO_DYNAMIC_TYPE_PUSH(ClassName)` in **every** constructor.
3. `XYO_DYNAMIC_TYPE_IMPLEMENT(ClassName, "key")` in one `.cpp` file. The key
   is a unique string, by convention a GUID.

```cpp
class Shape : public DynamicObject {
		XYO_DYNAMIC_TYPE_DEFINE(, Shape);

	public:
		Shape() {
			XYO_DYNAMIC_TYPE_PUSH(Shape);
		};
};

class Circle : public Shape {
		XYO_DYNAMIC_TYPE_DEFINE(, Circle);

	public:
		double radius = 1;

		Circle() {
			XYO_DYNAMIC_TYPE_PUSH(Circle);
		};
};

// in a .cpp file
XYO_DYNAMIC_TYPE_IMPLEMENT(Shape, "{A1B2C3D4-0000-4000-8000-000000000001}");
XYO_DYNAMIC_TYPE_IMPLEMENT(Circle, "{A1B2C3D4-0000-4000-8000-000000000002}");
```

In a library, export the statics: `XYO_DYNAMIC_TYPE_DEFINE(XYO_MYLIBRARY_EXPORT,
Shape);`.

`XYO_DYNAMIC_TYPE_DEFINE` ends with `private:`. Put it first in the class,
before `public:`, or restate the access you want after it.

### Using it

```cpp
TPointer<Circle> circle;
circle.newMemory();
TPointer<DynamicObject> any(circle.value());

TIsType<Shape>(any);       // true: Circle is a Shape
TIsTypeExact<Shape>(any);  // false: it is exactly a Circle
TIsType<Circle>(any);      // true

Circle *c = TDynamicCast<Circle *>(any);  // nullptr if any is not a Circle
const char *key = any->getTypeKey();      // the Circle key
const char *key2 = TGetTypeKey<Circle>(); // same
```

| Symbol | Effect |
|--------|--------|
| `T::getType()` | the type id, a `const void *`; registered on first use, from any thread |
| `object->isType(type)` | `object`'s type or one of its base types is `type` |
| `object->isTypeExact(type)` | `object`'s most derived type is `type` |
| `object->isSameType(other)` | same most derived type |
| `object->getTypeKey()` | the key of the most derived type |
| `DynamicObject::getTypeKey(type)` | the key of a type id |
| `TIsType<T>(object)`, `TIsTypeExact<T>(object)` | the same with `T::getType()` |
| `TGetTypeKey<T>()` | key of `T` |
| `TDynamicCast<T *>(object)` | `static_cast` if `object` is a `T`, else `nullptr`; `nullptr` in, `nullptr` out |
| `TDynamicCast(T *)` | upcast to `DynamicObject *`, null safe |

How it works: `getType()` returns a pointer to the node of the key in a
process wide registry (a red-black tree keyed by the key string, guarded by
a critical section). The first call registers the key, later calls read an
atomic. Each object keeps a small list of the types pushed by its
constructors, most derived first. `isTypeExact` compares the first entry,
`isType` walks the list.

Costs and rules:

- each object holds one small list node per class in its hierarchy;
- two different classes must never use the same key. Type ids are unified
  by key in the process wide registry, so a class whose statics end up
  duplicated in several modules (a static library linked into two DLLs)
  still has one type id;
- a constructor that forgets `XYO_DYNAMIC_TYPE_PUSH` makes the object
  report its base type as its exact type.

## TStaticCast

Null safe `static_cast` to and from `Object *`, for code that stores managed
objects as `Object *`:

```cpp
Object *o = TStaticCast(node);           // Node * -> Object *, nullptr stays nullptr
Node *n = TStaticCast<Node *>(o);        // Object * -> Node *, unchecked
```

Unlike `TDynamicCast`, the downcast is not checked: use it only when the
type is known.

## Stream interfaces: IRead, IWrite, ISeek

Pure interfaces, derived virtually from `Object`, so a class can implement
several of them and still be one managed object:

```cpp
class IRead : public virtual Object {
	public:
		virtual size_t read(void *output, size_t length) = 0;   // bytes read, 0 at the end
};

class IWrite : public virtual Object {
	public:
		virtual size_t write(const void *input, size_t length) = 0; // bytes written
};

class ISeek : public virtual Object {
	public:
		virtual bool seekFromBegin(uint64_t x) = 0;
		virtual bool seek(uint64_t x) = 0;             // relative to the current position
		virtual bool seekFromEnd(uint64_t x) = 0;
		virtual uint64_t seekTell() = 0;               // current position
};
```

They are implemented by files, memory buffers, sockets, compressors, ... in
the libraries above (`xyo-system`, `xyo-encoding`, ...), so code written
against `IRead *` / `IWrite *` works with all of them.

```cpp
class MemoryStream : public virtual IRead, public virtual IWrite {
	public:
		std::string data;
		size_t position = 0;

		size_t read(void *output, size_t length) override {
			if (length > data.size() - position) {
				length = data.size() - position;
			};
			memcpy(output, data.data() + position, length);
			position += length;
			return length;
		};

		size_t write(const void *input, size_t length) override {
			data.append(static_cast<const char *>(input), length);
			return length;
		};
};

void copyAll(IRead *in, IWrite *out) {
	char buffer[4096];
	size_t n;
	while ((n = in->read(buffer, sizeof(buffer))) > 0) {
		out->write(buffer, n);
	};
};
```

## TReference

`TReference<T, void DeleteMemoryT(T *)>` is a scoped, non-copyable holder: it
stores a `T *` and calls `DeleteMemoryT` on it in its destructor. It has no
default constructor. It is the same as `XYO::ManagedMemory::TReference`;
with both headers included and both namespaces in use, qualify the name.

```cpp
void closeFile(FILE *file) {
	if (file) {
		fclose(file);
	};
};

TReference<FILE, closeFile> file(fopen("data.bin", "rb"));
if (file) {
	fread(buffer, 1, sizeof(buffer), file);
};
```
