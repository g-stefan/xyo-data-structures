# Getting started

## 1. Build and install

The library is built with [fabricare](https://github.com/g-stefan/fabricare),
the build tool used by all XYO C++ projects. `xyo-platform` and
`xyo-managed-memory` must be installed to the SDK first. From the repository
root:

```bash
fabricare make       # build into output/
fabricare test       # build and run test/test.*.cpp (run make first)
fabricare install    # copy output/{bin,include,lib} to ~/.fabricare/<platform>
fabricare clean      # remove output/ and temp/
```

Two libraries are produced:

| Project                        | Kind                                | Use it when                               |
|--------------------------------|-------------------------------------|-------------------------------------------|
| `xyo-data-structures`          | DLL / shared library (`dll-or-lib`) | default, shared between several libraries |
| `xyo-data-structures.static`   | static library, static CRT          | self-contained executables                |

Most of the library is templates in headers. The compiled part is small:
`DynamicObject` and the version / copyright / license functions.

## 2. Depend on it from another fabricare project

In the consumer's `fabricare.json`:

```json
{
	"name": "my-library",
	"make": "dll-or-lib",
	"sourcePath": "XYO/MyLibrary",
	"dependency": [
		"xyo-data-structures"
	]
}
```

For the static variant use `"xyo-data-structures.static"`. It exports
`XYO_DATASTRUCTURES_LIBRARY` to the consumer (`dependencyDefines`), which
turns `XYO_DATASTRUCTURES_EXPORT` into nothing. `xyo-managed-memory` and
`xyo-platform` come in as transitive dependencies.

## 3. Include

```cpp
#include <XYO/DataStructures.hpp>
```

The umbrella header pulls in `<XYO/ManagedMemory.hpp>` (and through it
`<XYO/Platform.hpp>`) and every public header of this library.

Namespace `XYO::DataStructures` contains `using namespace
XYO::ManagedMemory;`, so `using namespace XYO::DataStructures;` also brings in
`Object`, `TPointer`, `TPointerX`, `TMemory`, ... and the platform names. The
metadata namespaces then exist three times (`Version`, `Copyright`,
`License`): write `XYO::DataStructures::Version::version()` in full.

`TReference` exists in both `XYO::DataStructures` and `XYO::ManagedMemory`
(the latter only if `<XYO/ManagedMemory/TReference.hpp>` is included). With
both included and both namespaces used, qualify it.

Libraries built on top usually re-export the names in their own
`Dependency.hpp`:

```cpp
#include <XYO/DataStructures.hpp>

namespace XYO::MyLibrary {
	using namespace XYO::DataStructures;
};
```

## 4. First program

A task graph: every task has a list of tasks that follow it. Two tasks point
to each other, so there is a cycle going through two containers, and it is
still freed.

```cpp
#include <XYO/DataStructures.hpp>

using namespace XYO::DataStructures;

class Task : public Object {
		XYO_PLATFORM_DISALLOW_COPY_ASSIGN_MOVE(Task);

	public:
		std::string name;
		TPointerX<TDynamicArray<TPointerX<Task>>> next; // container owned by this object

		inline Task() {
			next.pointerLink(this); // required for every TPointerX member
			next.newMemory();
		};

		static inline void initMemory() {
			TPointerX<TDynamicArray<TPointerX<Task>>>::initMemory();
		};
};

TPointer<Task> newTask(const char *name) {
	TPointer<Task> retV;
	retV.newMemory();
	retV->name = name;
	return retV;
};

int main(int, char *[]) {
	TPointer<Task> build = newTask("build");
	TPointer<Task> test = newTask("test");

	build->next->push(test); // stored as TPointerX, linked to the array
	test->next->push(build); // a cycle through two containers

	TRedBlackTree<std::string, TPointer<Task>> byName; // a local container
	byName.set("build", build);
	byName.set("test", test);

	TPointer<Task> found;
	if (byName.get("test", found)) {
		printf("%s has %zu next\n", found->name.c_str(), found->next->length());
	};

	for (auto *node = byName.begin(); node; node = node->successor()) {
		printf("%s\n", node->key.c_str()); // build, test
	};

	return 0; // everything is freed when the TPointers go out of scope
};
```

Three things to notice, all explained in [Containers](containers.md#memory-rules):

- the container inside `Task` is held by a `TPointerX` linked to `this`, not
  as a plain member;
- its elements are `TPointerX<Task>`, which the container links to itself;
- the local `byName` holds `TPointer<Task>` elements: it is part of the
  executive, its elements are roots.

As a fabricare test project:

```json
{
	"name": "test.05",
	"make": "exe",
	"category": "test",
	"dependency": [
		"xyo-data-structures"
	]
}
```

with the source in `test/test.05.cpp`.

## 5. Threads

Everything in `xyo-managed-memory` about threads applies here (see its
`docs/getting-started.md`):

- create threads and move data between them with **`xyo-multithreading`**
  (`Thread`, `Worker`, `WorkerQueue`, `Transfer`);
- the main thread must use the library before any thread starts. Call
  `XYO::ManagedMemory::Registry::registryInit()` at the start of `main` when
  in doubt;
- **a container, like any object graph, is used by one thread only.** None of
  the containers is synchronized, except `TRing`.

`TRing` is the exception by design: one thread writes, another reads,
without a lock. See [TRing](ring.md).

`DynamicObject` types may be registered from any thread (the registration is
thread safe), but the registry rule above still applies.

## 6. Building without fabricare

Compile the three amalgams with your sources:

1. Put `source/` of `xyo-platform`, `xyo-managed-memory` and
   `xyo-data-structures` on the include path.
2. Provide the configuration headers of `xyo-platform` and
   `xyo-managed-memory` (see their documentation; for the default
   configuration `XYO/ManagedMemory/Config.hpp` is a copy of
   `Config.Template.hpp`).
3. Compile `Platform.Amalgam.cpp`, `ManagedMemory.Amalgam.cpp` and
   `DataStructures.Amalgam.cpp` together with your sources, and define
   `XYO_PLATFORM_LIBRARY`, `XYO_MANAGEDMEMORY_LIBRARY` and
   `XYO_DATASTRUCTURES_LIBRARY` everywhere (plain static linking).
4. Link `pthread` on Linux.

Example on Linux, with the repositories side by side:

```bash
g++ -std=c++17 \
    -Ixyo-platform/source -Ixyo-managed-memory/source -Ixyo-data-structures/source \
    -DXYO_PLATFORM_LIBRARY -DXYO_MANAGEDMEMORY_LIBRARY -DXYO_DATASTRUCTURES_LIBRARY \
    xyo-platform/source/XYO/Platform.Amalgam.cpp \
    xyo-managed-memory/source/XYO/ManagedMemory.Amalgam.cpp \
    xyo-data-structures/source/XYO/DataStructures.Amalgam.cpp \
    main.cpp -o main -pthread
```

On Windows with MSVC, also define `XYO_PLATFORM_COMPILE_STATIC`:

```bat
cl /EHsc /std:c++17 /Ixyo-platform\source /Ixyo-managed-memory\source /Ixyo-data-structures\source ^
   /DXYO_PLATFORM_COMPILE_STATIC /DXYO_PLATFORM_LIBRARY /DXYO_MANAGEDMEMORY_LIBRARY /DXYO_DATASTRUCTURES_LIBRARY ^
   xyo-platform\source\XYO\Platform.Amalgam.cpp ^
   xyo-managed-memory\source\XYO\ManagedMemory.Amalgam.cpp ^
   xyo-data-structures\source\XYO\DataStructures.Amalgam.cpp ^
   main.cpp
```
