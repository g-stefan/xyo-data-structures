// Data Structures
// Copyright (c) 2016-2024 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2024 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_DATASTRUCTURES_TASSOCIATIVEARRAY_HPP
#define XYO_DATASTRUCTURES_TASSOCIATIVEARRAY_HPP

#ifndef XYO_DATASTRUCTURES_TDYNAMICARRAY_HPP
#	include <XYO/DataStructures/TDynamicArray.hpp>
#endif

#ifndef XYO_DATASTRUCTURES_TREDBLACKTREE_HPP
#	include <XYO/DataStructures/TRedBlackTree.hpp>
#endif

namespace XYO::DataStructures {

	template <typename TKey, typename TValue, size_t dataSize2Pow = 4, template <typename U> class TNodeMemory = TMemory>
	class TAssociativeArray : public Object {
			XYO_DISALLOW_COPY_ASSIGN_MOVE(TAssociativeArray);

		public:
			typedef TRedBlackTree<TKey, size_t, TNodeMemory> TMapKey;
			typedef TDynamicArray<TKey, dataSize2Pow, TNodeMemory> TArrayKey;
			typedef TDynamicArray<TValue, dataSize2Pow, TNodeMemory> TArrayValue;

			typedef typename TPointerTypeExclude<TKey>::Type TKeyType;
			typedef typename TPointerTypeExclude<TKey>::Pointer TPointerTKey;
			typedef typename TPointerTypeExclude<TKey>::PointerX TPointerXTKey;
			typedef typename TPointerTypeExclude<TValue>::Type TValueType;
			typedef typename TPointerTypeExclude<TValue>::Pointer TPointerTValue;
			typedef typename TPointerTypeExclude<TValue>::PointerX TPointerXTValue;

			typedef TMapKey MapKey;
			typedef TArrayKey ArrayKey;
			typedef TArrayValue ArrayValue;

			typedef typename TMapKey::Node MapKeyNode;

			TPointerX<TMapKey> mapKey;
			TPointerX<TArrayKey> arrayKey;
			TPointerX<TArrayValue> arrayValue;

			size_t length_;

			inline TAssociativeArray() {
				mapKey.pointerLink(this);
				arrayKey.pointerLink(this);
				arrayValue.pointerLink(this);
				mapKey.newMemory();
				arrayKey.newMemory();
				arrayValue.newMemory();
				length_ = 0;
			};

			inline ~TAssociativeArray() {
				mapKey->empty();
				arrayKey->empty();
				arrayValue->empty();
			};

			inline void empty() {
				mapKey->empty();
				arrayKey->empty();
				arrayValue->empty();
				length_ = 0;
			};

			inline void activeDestructor() {
				empty();
			};

			inline bool get(const TKey &key, TValue &value) const {
				size_t index;
				if (mapKey->get(key, index)) {
					return arrayValue->get(index, value);
				};
				return false;
			};

			inline bool get(const TKey &key, TPointerTValue &value) const {
				size_t index;
				if (mapKey->get(key, index)) {
					return arrayValue->get(index, value);
				};
				return false;
			};

			inline bool get(const TKey &key, TPointerXTValue &value) const {
				size_t index;
				if (mapKey->get(key, index)) {
					return arrayValue->get(index, value);
				};
				return false;
			};

			inline bool get(const TKeyType *key, TPointerTValue &value) const {
				size_t index;
				if (mapKey->get(key, index)) {
					return arrayValue->get(index, value);
				};
				return false;
			};

			inline bool get(const TKeyType *key, TPointerXTValue &value) const {
				size_t index;
				if (mapKey->get(key, index)) {
					return arrayValue->get(index, value);
				};
				return false;
			};

			inline void set(const TKey &key, const TValue &value) {
				size_t index;
				if (mapKey->get(key, index)) {
					arrayValue->set(index, value);
					return;
				};
				mapKey->insert(key, length_);
				arrayKey->set(length_, key);
				arrayValue->set(length_, value);
				++length_;
			};

			inline void set(const TKey &key, const TValueType *value) {
				size_t index;
				if (mapKey->get(key, index)) {
					arrayValue->set(index, value);
					return;
				};
				mapKey->insert(key, length_);
				arrayKey->set(length_, key);
				arrayValue->set(length_, value);
				++length_;
			};

			inline void set(const TKeyType *key, const TValueType *value) {
				size_t index;
				if (mapKey->get(key, index)) {
					arrayValue->set(index, value);
					return;
				};
				mapKey->insert(key, length_);
				arrayKey->set(length_, key);
				arrayValue->set(length_, value);
				++length_;
			};

			inline bool remove(const TKey &key) {
				MapKeyNode *node;

				node = mapKey->find(key);
				if (node) {
					size_t index = node->value;
					arrayKey->remove(index);
					arrayValue->remove(index);
					mapKey->removeNode(node);
					for (node = mapKey->begin(); node != nullptr; node = node->successor()) {
						if (node->value >= index) {
							node->value--;
						};
					};
					--length_;
					return true;
				};

				return false;
			};

			inline bool remove(const TKeyType *key) {
				MapKeyNode *node;

				node = mapKey->find(key);
				if (node) {
					size_t index = node->value;
					arrayKey->remove(index);
					arrayValue->remove(index);
					mapKey->removeNode(node);
					for (node = mapKey->begin(); node != nullptr; node = node->successor()) {
						if (node->value >= index) {
							node->value--;
						};
					};
					--length_;
					return true;
				};

				return false;
			};

			inline size_t length() const {
				return length_;
			};

			static inline void initMemory() {
				TPointerX<TMapKey>::initMemory();
				TPointerX<TArrayKey>::initMemory();
				TPointerX<TArrayValue>::initMemory();
			};
	};

};

#endif
