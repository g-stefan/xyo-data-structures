// Data Structures
// Copyright (c) 2016-2026 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2026 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_DATASTRUCTURES_DYNAMICOBJECT_HPP
#define XYO_DATASTRUCTURES_DYNAMICOBJECT_HPP

#ifndef XYO_DATASTRUCTURES_DEPENDENCY_HPP
#	include <XYO/DataStructures/Dependency.hpp>
#endif

namespace XYO::DataStructures {

	struct DynamicTypeNode : TXList1Node<DynamicTypeNode> {
			const void *type;
	};

// The type is registered on first use, from any thread.
// type##T is atomic: acquire here pairs with the release in registerType,
// so a non null value always points to a fully registered type.
#define XYO_DYNAMIC_TYPE_DEFINE(EXPORT, T)                                          \
protected:                                                                          \
	EXPORT static const char *type##T##Key;                                     \
	EXPORT static XYO::Platform::Multithreading::TAtomic<const void *> type##T; \
                                                                                    \
public:                                                                             \
	static inline const void *getType() {                                       \
		const void *type = type##T.get(std::memory_order_acquire);          \
		if (type == nullptr) {                                              \
			type = registerType(type##T, type##T##Key);                 \
		};                                                                  \
		return type;                                                        \
	};                                                                          \
                                                                                    \
private:

#define XYO_DYNAMIC_TYPE_IMPLEMENT(T, KEY) \
	const char *T::type##T##Key = KEY; \
	XYO::Platform::Multithreading::TAtomic<const void *> T::type##T(nullptr);

#define XYO_DYNAMIC_TYPE_PUSH(T) \
	objectTypePush(T::getType());

	class DynamicObject : public Object {
			XYO_PLATFORM_DISALLOW_COPY_ASSIGN_MOVE(DynamicObject);
			XYO_DYNAMIC_TYPE_DEFINE(XYO_DATASTRUCTURES_EXPORT, DynamicObject);

		protected:
			XYO_DATASTRUCTURES_EXPORT static const void *registerType(TAtomic<const void *> &type, const char *key);
			XYO_DATASTRUCTURES_EXPORT void objectTypePush(const void *type);
			XYO_DATASTRUCTURES_EXPORT bool objectTypeSearchNext(const void *type);
			DynamicTypeNode *objectType_;

		public:
			XYO_DATASTRUCTURES_EXPORT DynamicObject();
			XYO_DATASTRUCTURES_EXPORT ~DynamicObject();
			XYO_DATASTRUCTURES_EXPORT static void initMemory();
			XYO_DATASTRUCTURES_EXPORT static const char *getTypeKey(const void *type);
			XYO_DATASTRUCTURES_EXPORT const char *getTypeKey();

			[[nodiscard]] inline bool isType(const void *type) {
				if (objectType_->type == type) {
					return true;
				};
				return objectTypeSearchNext(type);
			};

			[[nodiscard]] inline bool isTypeExact(const void *type) noexcept {
				if (objectType_->type == type) {
					return true;
				};
				return false;
			};

			[[nodiscard]] inline bool isSameType(DynamicObject *dynamicObject) noexcept {
				return (objectType_->type == dynamicObject->objectType_->type);
			};
	};

	template <typename T>
	[[nodiscard]] bool TIsType(DynamicObject *object) {
		return object->isType(T::getType());
	};

	template <typename T>
	[[nodiscard]] bool TIsTypeExact(DynamicObject *object) {
		return object->isTypeExact(T::getType());
	};

	template <typename T>
	[[nodiscard]] const char *TGetTypeKey() {
		return DynamicObject::getTypeKey(T::getType());
	};

	template <typename T>
	[[nodiscard]] T TDynamicCast(DynamicObject *object) {
		typedef typename std::remove_pointer<T>::type TType;
		if (object == nullptr) {
			return nullptr;
		};
		if (object->isType(TType::getType())) {
			return static_cast<T>(object);
		};
		return nullptr;
	};

	template <typename T>
	[[nodiscard]] DynamicObject *TDynamicCast(T this_) {
		if (this_ == nullptr) {
			return nullptr;
		};
		return static_cast<DynamicObject *>(this_);
	};

	template <DynamicObject *>
	[[nodiscard]] DynamicObject *TDynamicCast(DynamicObject *this_) {
		return this_;
	};
};

namespace XYO::ManagedMemory {
	template <>
	struct TMemory<XYO::DataStructures::DynamicObject> : TMemoryPoolActive<XYO::DataStructures::DynamicObject> {};
}

#endif
