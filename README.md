# Data Structures

C++ library
- Containers that work with the XYO managed memory model: stack, deque, dynamic array,
red-black tree map and set, associative array; cycles through containers are collected.
- Intrusive lists and arrays, a lock-free single writer / single reader ring buffer.
- Runtime type information (`DynamicObject`) and stream interfaces.

Built on `xyo-managed-memory`; used by `xyo-multithreading`, `xyo-system`
and the rest of the XYO C++ libraries.

## Documentation

- [Overview](docs/README.md) - purpose and design
- [Getting started](docs/getting-started.md) - build, depend on it, first program, threads
- [Containers](docs/containers.md) - `TStack`, `TDoubleEndedQueue`, `TDynamicArray`, `TRedBlackTree`, `TRedBlackTreeOne`, `TAssociativeArray`, memory rules
- [Intrusive containers](docs/intrusive.md) - `TXList3`, `TXList5`, `TXArray`, `TStackPointerUnsafe`
- [TRing](docs/ring.md) - single writer / single reader lock-free ring buffer
- [Dynamic objects and interfaces](docs/dynamic-object.md) - `DynamicObject`, `TDynamicCast`, `IRead` / `IWrite` / `ISeek`
- [API reference](docs/reference.md)

A Claude Code skill for this library is in
[.claude/skills/xyo-data-structures](.claude/skills/xyo-data-structures/SKILL.md).

## License

Copyright (c) 2016-2026 Grigore Stefan
Licensed under the [MIT](LICENSE) license.
