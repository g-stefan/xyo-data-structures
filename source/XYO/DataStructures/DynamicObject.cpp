//
// Data Structures
//
// Copyright (c) Grigore Stefan <g_stefan@yahoo.com>
// Created by Grigore Stefan <g_stefan@yahoo.com>
//
// MIT License (MIT) <http://opensource.org/licenses/MIT>
//

#include <XYO/DataStructures/DynamicObject.hpp>

namespace XYO::DataStructures {

	typedef TXList1<DynamicTypeNode, TMemoryPoolActive> DynamicTypeList;

	XYO_DYNAMIC_TYPE_IMPLEMENT(DynamicObject, "{C2BF7FE6-A879-4A2B-827F-DDE629233C5F}");

	DynamicObject::DynamicObject() {
		DynamicTypeList::constructor(objectType_);
		XYO_DYNAMIC_TYPE_PUSH(DynamicObject);
	};

	DynamicObject::~DynamicObject() {
		DynamicTypeList::destructor(objectType_);
	};

	struct TypeRegistryNode : TXRedBlackTreeNode<TypeRegistryNode, const char *> {
	};

	class TypeRegistry {
		public:
			typedef TXRedBlackTree<TypeRegistryNode, TMemorySystem> RBTree;
			TypeRegistryNode *root;

#ifdef XYO_PLATFORM_MULTI_THREAD
			CriticalSection criticalSection;
#endif

			TypeRegistry();
			~TypeRegistry();
	};

	TypeRegistry::TypeRegistry() {
		RBTree::constructor(root);
	};

	TypeRegistry::~TypeRegistry() {
		RBTree::destructor(root);
	};

	void DynamicObject::registerType(const void *&type, const char *key) {
		if (type != nullptr) {
			return;
		};

		TypeRegistry *typeRegistry = TSingletonProcess<TypeRegistry>::getValue();
		TypeRegistry::RBTree::Node *this_;
#ifdef XYO_PLATFORM_MULTI_THREAD
		typeRegistry->criticalSection.enter();
#endif
		this_ = TypeRegistry::RBTree::find(typeRegistry->root, key);
		if (this_ == nullptr) {
			this_ = TypeRegistry::RBTree::newNode();
			this_->key = key;
			TypeRegistry::RBTree::insertNode(typeRegistry->root, this_);
		};

#ifdef XYO_PLATFORM_MULTI_THREAD
		typeRegistry->criticalSection.leave();
#endif

		type = this_;
	};

	const char *DynamicObject::getTypeKey(const void *type) {
		return (reinterpret_cast<const TypeRegistry::RBTree::Node *>(type))->key;
	};

	const char *DynamicObject::getTypeKey() {
		return (reinterpret_cast<const TypeRegistry::RBTree::Node *>(objectType_->type))->key;
	};

	void DynamicObject::objectTypePush(const void *&type, const char *key) {
		registerType(type, key);
		DynamicTypeNode *node = DynamicTypeList::newNode();
		node->type = type;
		DynamicTypeList::push(objectType_, node);
	};

	bool DynamicObject::objectTypeSearchNext(const void *type) {
		DynamicTypeNode *node = objectType_->next;
		for (; node != nullptr; node = node->next) {
			if (node->type == type) {
				return true;
			};
		};
		return false;
	};

	void DynamicObject::initMemory() {
		DynamicTypeList::initMemory();
	};

};
