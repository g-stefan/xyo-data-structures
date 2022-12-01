// Created by Grigore Stefan <g_stefan@yahoo.com>
// Public domain (Unlicense) <http://unlicense.org>
// SPDX-FileCopyrightText: 2022 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: Unlicense

#include <XYO/DataStructures.hpp>

using namespace XYO::DataStructures;

void test() {
	TRedBlackTree<std::string, std::string> tree;

	tree.set("Apples", "Pie");
	tree.set("Pears", "Juice");
	tree.set("Grapefruits", "Juice");
	tree.set("Raspberries", "Pie");
	tree.set("Blueberries", "Pie");

	TRedBlackTree<std::string, std::string>::TNode *node;

	for (node = tree.begin(); node != nullptr; node = node->successor()) {
		printf("%s - %s\n", node->key.c_str(), node->value.c_str());
	};
};

int main(int cmdN, char *cmdS[]) {
	try {

		test();

		return 0;

	} catch (const std::exception &e) {
		printf("* Error: %s\n", e.what());
	} catch (...) {
		printf("* Error: Unknown\n");
	};

	return 1;
};
