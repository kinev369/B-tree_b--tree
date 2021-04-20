#include "iomanip"
#include "istream"
#include <fstream>
#include <string>
#include <deque>
#include <fstream>
#include <iostream>
#include <istream>
#include <iterator>
#include <list>
#include <memory>
#include <ostream>
#include <queue>
#include <string>
#include <vector>
using namespace std;
const int t = 2;
int numberOfPointers=6;
#define MAX 50
struct BTreeNode {
	vector <int> keys;
	int leaf_size;
	vector <BTreeNode*> C;
	BTreeNode* prv;
	BTreeNode* next;
	bool leaf;
};
struct Block {
	int tNodes;
	Block* parentBlock;
	int value[MAX];
	Block* childBlock[MAX];
	Block() { 
		tNodes = 0;
		parentBlock = NULL;
		for (int i = 0; i < MAX; i++) {
			value[i] = INT_MAX;
			childBlock[i] = NULL;
		}
	}
};
Block* rootBlock = new Block();
void splitLeaf(Block* curBlock) {
	int x, i, j;
	if (numberOfPointers % 2)
		x = (numberOfPointers + 1) / 2;
	else x = numberOfPointers / 2;

	Block* rightBlock = new Block();
	curBlock->tNodes = x;
	rightBlock->tNodes = numberOfPointers - x;
	rightBlock->parentBlock = curBlock->parentBlock;
	for (i = x, j = 0; i < numberOfPointers; i++, j++) {
		rightBlock->value[j] = curBlock->value[i];
		curBlock->value[i] = INT_MAX;
	}
	int val = rightBlock->value[0];
	if (curBlock->parentBlock == NULL) {
		Block* parentBlock = new Block();
		parentBlock->parentBlock = NULL;
		parentBlock->tNodes = 1;
		parentBlock->value[0] = val;
		parentBlock->childBlock[0] = curBlock;
		parentBlock->childBlock[1] = rightBlock;
		curBlock->parentBlock = rightBlock->parentBlock = parentBlock;
		rootBlock = parentBlock;
		return;
	}
	else {  
		curBlock = curBlock->parentBlock;
		Block* newChildBlock = new Block();
		newChildBlock = rightBlock;
		for (i = 0; i <= curBlock->tNodes; i++) {
			if (val < curBlock->value[i]) {
				swap(curBlock->value[i], val);
			}
		}
		curBlock->tNodes++;
		for (i = 0; i < curBlock->tNodes; i++) {
			if (newChildBlock->value[0] < curBlock->childBlock[i]->value[0]) {
				swap(curBlock->childBlock[i], newChildBlock);
			}
		}
		curBlock->childBlock[i] = newChildBlock;
		for (i = 0; curBlock->childBlock[i] != NULL; i++) {
			curBlock->childBlock[i]->parentBlock = curBlock;
		}
	}
}
void splitNonLeaf(Block* curBlock) {
	int x, i, j;
	x = numberOfPointers / 2;
	Block* rightBlock = new Block();
	curBlock->tNodes = x;
	rightBlock->tNodes = numberOfPointers - x - 1;
	rightBlock->parentBlock = curBlock->parentBlock;
	for (i = x, j = 0; i <= numberOfPointers; i++, j++) {
		rightBlock->value[j] = curBlock->value[i];
		rightBlock->childBlock[j] = curBlock->childBlock[i];
		curBlock->value[i] = INT_MAX;
		if (i != x)curBlock->childBlock[i] = NULL;
	}
	int val = rightBlock->value[0];
	memcpy(&rightBlock->value, &rightBlock->value[1], sizeof(int) * (rightBlock->tNodes + 1));
	memcpy(&rightBlock->childBlock, &rightBlock->childBlock[1], sizeof(rootBlock) * (rightBlock->tNodes + 1));
	for (i = 0; curBlock->childBlock[i] != NULL; i++) {
		curBlock->childBlock[i]->parentBlock = curBlock;
	}
	for (i = 0; rightBlock->childBlock[i] != NULL; i++) {
		rightBlock->childBlock[i]->parentBlock = rightBlock;
	}
	if (curBlock->parentBlock == NULL) {
		Block* parentBlock = new Block();
		parentBlock->parentBlock = NULL;
		parentBlock->tNodes = 1;
		parentBlock->value[0] = val;
		parentBlock->childBlock[0] = curBlock;
		parentBlock->childBlock[1] = rightBlock;
		curBlock->parentBlock = rightBlock->parentBlock = parentBlock;
		rootBlock = parentBlock;
		return;
	}
	else {   
		curBlock = curBlock->parentBlock;
		Block* newChildBlock = new Block();
		newChildBlock = rightBlock;
		for (i = 0; i <= curBlock->tNodes; i++) {
			if (val < curBlock->value[i]) {
				swap(curBlock->value[i], val);
			}
		}
		curBlock->tNodes++;
		for (i = 0; i < curBlock->tNodes; i++) {
			if (newChildBlock->value[0] < curBlock->childBlock[i]->value[0]) {
				swap(curBlock->childBlock[i], newChildBlock);
			}
		}
		curBlock->childBlock[i] = newChildBlock;
		for (i = 0; curBlock->childBlock[i] != NULL; i++) {
			curBlock->childBlock[i]->parentBlock = curBlock;
		}
	}
}
void insertNode(Block* curBlock, int val) {
	for (int i = 0; i <= curBlock->tNodes; i++) {
		if (val < curBlock->value[i] && curBlock->childBlock[i] != NULL) {
			insertNode(curBlock->childBlock[i], val);
			if (curBlock->tNodes == numberOfPointers)
				splitNonLeaf(curBlock);
			return;
		}
		else if (val < curBlock->value[i] && curBlock->childBlock[i] == NULL) {
			swap(curBlock->value[i], val);
			if (i == curBlock->tNodes) {
				curBlock->tNodes++;
				break;
			}
		}
	}
	if (curBlock->tNodes == numberOfPointers) {
		splitLeaf(curBlock);
	}
}
void redistributeBlock(Block* leftBlock, Block* rightBlock, bool isLeaf, int posOfLeftBlock, int whichOneisCurBlock) {
	int PrevRightFirstVal = rightBlock->value[0];
	if (whichOneisCurBlock == 0) { 
		if (!isLeaf) {
			leftBlock->value[leftBlock->tNodes] = leftBlock->parentBlock->value[posOfLeftBlock];
			leftBlock->childBlock[leftBlock->tNodes + 1] = rightBlock->childBlock[0];
			leftBlock->tNodes++;
			leftBlock->parentBlock->value[posOfLeftBlock] = rightBlock->value[0];
			memcpy(&rightBlock->value[0], &rightBlock->value[1], sizeof(int) * (rightBlock->tNodes + 1));
			memcpy(&rightBlock->childBlock[0], &rightBlock->childBlock[1], sizeof(rootBlock) * (rightBlock->tNodes + 1));
			rightBlock->tNodes--;

		}
		else {
			leftBlock->value[leftBlock->tNodes] = rightBlock->value[0];
			leftBlock->tNodes++;
			memcpy(&rightBlock->value[0], &rightBlock->value[1], sizeof(int) * (rightBlock->tNodes + 1));
			rightBlock->tNodes--;
			leftBlock->parentBlock->value[posOfLeftBlock] = rightBlock->value[0];
		}
	}
	else { 
		if (!isLeaf) {
			memcpy(&rightBlock->value[1], &rightBlock->value[0], sizeof(int) * (rightBlock->tNodes + 1));
			memcpy(&rightBlock->childBlock[1], &rightBlock->childBlock[0], sizeof(rootBlock) * (rightBlock->tNodes + 1));
			rightBlock->value[0] = leftBlock->parentBlock->value[posOfLeftBlock];
			rightBlock->childBlock[0] = leftBlock->childBlock[leftBlock->tNodes];
			rightBlock->tNodes++;
			leftBlock->parentBlock->value[posOfLeftBlock] = leftBlock->value[leftBlock->tNodes - 1];
			leftBlock->value[leftBlock->tNodes - 1] = INT_MAX;
			leftBlock->childBlock[leftBlock->tNodes] = NULL;
			leftBlock->tNodes--;
		}
		else {
			memcpy(&rightBlock->value[1], &rightBlock->value[0], sizeof(int) * (rightBlock->tNodes + 1));
			rightBlock->value[0] = leftBlock->value[leftBlock->tNodes - 1];
			rightBlock->tNodes++;
			leftBlock->value[leftBlock->tNodes - 1] = INT_MAX;
			leftBlock->tNodes--;
			leftBlock->parentBlock->value[posOfLeftBlock] = rightBlock->value[0];
		}
	}
}

void mergeBlock(Block* leftBlock, Block* rightBlock, bool isLeaf, int posOfRightBlock) {
	if (!isLeaf) {
		leftBlock->value[leftBlock->tNodes] = leftBlock->parentBlock->value[posOfRightBlock - 1];
		leftBlock->tNodes++;
	}
	memcpy(&leftBlock->value[leftBlock->tNodes], &rightBlock->value[0], sizeof(int) * (rightBlock->tNodes + 1));
	memcpy(&leftBlock->childBlock[leftBlock->tNodes], &rightBlock->childBlock[0], sizeof(rootBlock) * (rightBlock->tNodes + 1));
	leftBlock->tNodes += rightBlock->tNodes;
	memcpy(&leftBlock->parentBlock->value[posOfRightBlock - 1], &leftBlock->parentBlock->value[posOfRightBlock], sizeof(int) * (leftBlock->parentBlock->tNodes + 1));
	memcpy(&leftBlock->parentBlock->childBlock[posOfRightBlock], &leftBlock->parentBlock->childBlock[posOfRightBlock + 1], sizeof(rootBlock) * (leftBlock->parentBlock->tNodes + 1));
	leftBlock->parentBlock->tNodes--;
	for (int i = 0; leftBlock->childBlock[i] != NULL; i++) {
		leftBlock->childBlock[i]->parentBlock = leftBlock;
	}
}
bool dataFound;
void deleteNode(Block* curBlock, int val, int curBlockPosition) {
	bool isLeaf;
	if (curBlock->childBlock[0] == NULL)
		isLeaf = true;
	else isLeaf = false;
	int prevLeftMostVal = curBlock->value[0];
	for (int i = 0; dataFound == false && i <= curBlock->tNodes; i++) {
		if (val < curBlock->value[i] && curBlock->childBlock[i] != NULL) {
			deleteNode(curBlock->childBlock[i], val, i);
		}
		else if (val == curBlock->value[i] && curBlock->childBlock[i] == NULL) {
			memcpy(&curBlock->value[i], &curBlock->value[i + 1], sizeof(int) * (curBlock->tNodes + 1));
			curBlock->tNodes--;
			dataFound = true;
			break;
		}
	}
	if (curBlock->parentBlock == NULL && curBlock->childBlock[0] == NULL) {
		return;
	}
	if (curBlock->parentBlock == NULL && curBlock->childBlock[0] != NULL && curBlock->tNodes == 0) {
		rootBlock = curBlock->childBlock[0];
		rootBlock->parentBlock = NULL;
		return;
	}
	if (isLeaf && curBlock->parentBlock != NULL) {
		if (curBlockPosition == 0) {
			Block* rightBlock = new Block();
			rightBlock = curBlock->parentBlock->childBlock[1];
			if (rightBlock != NULL && rightBlock->tNodes > (numberOfPointers + 1) / 2) {
				redistributeBlock(curBlock, rightBlock, isLeaf, 0, 0);
			}
			else if (rightBlock != NULL && curBlock->tNodes + rightBlock->tNodes < numberOfPointers) {
				mergeBlock(curBlock, rightBlock, isLeaf, 1);
			}
		}
		else {
			Block* leftBlock = new Block();
			Block* rightBlock = new Block();
			leftBlock = curBlock->parentBlock->childBlock[curBlockPosition - 1];
			rightBlock = curBlock->parentBlock->childBlock[curBlockPosition + 1];
			if (leftBlock != NULL && leftBlock->tNodes > (numberOfPointers + 1) / 2) {
				redistributeBlock(leftBlock, curBlock, isLeaf, curBlockPosition - 1, 1);
			}
			else if (rightBlock != NULL && rightBlock->tNodes > (numberOfPointers + 1) / 2) {
				redistributeBlock(curBlock, rightBlock, isLeaf, curBlockPosition, 0);
			}
			else if (leftBlock != NULL && curBlock->tNodes + leftBlock->tNodes < numberOfPointers) {
				mergeBlock(leftBlock, curBlock, isLeaf, curBlockPosition);
			}
			else if (rightBlock != NULL && curBlock->tNodes + rightBlock->tNodes < numberOfPointers) {
				mergeBlock(curBlock, rightBlock, isLeaf, curBlockPosition + 1);
			}
		}
	}
	else if (!isLeaf && curBlock->parentBlock != NULL) {

		if (curBlockPosition == 0) {
			Block* rightBlock = new Block();
			rightBlock = curBlock->parentBlock->childBlock[1];

			if (rightBlock != NULL && rightBlock->tNodes - 1 >= ceil((numberOfPointers - 1) / 2)) {
				redistributeBlock(curBlock, rightBlock, isLeaf, 0, 0);
			}
			else if (rightBlock != NULL && curBlock->tNodes + rightBlock->tNodes < numberOfPointers - 1) {
				mergeBlock(curBlock, rightBlock, isLeaf, 1);
			}
		}
		else {
			Block* leftBlock = new Block();
			Block* rightBlock = new Block();
			leftBlock = curBlock->parentBlock->childBlock[curBlockPosition - 1];
			rightBlock = curBlock->parentBlock->childBlock[curBlockPosition + 1];
			if (leftBlock != NULL && leftBlock->tNodes - 1 >= ceil((numberOfPointers - 1) / 2)) {
				redistributeBlock(leftBlock, curBlock, isLeaf, curBlockPosition - 1, 1);
			}
			else if (rightBlock != NULL && rightBlock->tNodes - 1 >= ceil((numberOfPointers - 1) / 2)) {
				redistributeBlock(curBlock, rightBlock, isLeaf, curBlockPosition, 0);
			}
			else if (leftBlock != NULL && curBlock->tNodes + leftBlock->tNodes < numberOfPointers - 1) {
				mergeBlock(leftBlock, curBlock, isLeaf, curBlockPosition);
			}
			else if (rightBlock != NULL && curBlock->tNodes + rightBlock->tNodes < numberOfPointers - 1) {
				mergeBlock(curBlock, rightBlock, isLeaf, curBlockPosition + 1);
			}
		}
	}
	Block* tempBlock = new Block();
	tempBlock = curBlock->parentBlock;
	while (tempBlock != NULL) {
		for (int i = 0; i < tempBlock->tNodes; i++) {
			if (tempBlock->value[i] == prevLeftMostVal) {
				tempBlock->value[i] = curBlock->value[0];
				break;
			}
		}
		tempBlock = tempBlock->parentBlock;
	}
}

bool SerchNode(vector < Block* > Blocks,int key) {
	vector < Block* > newBlocks;
	for (int i = 0; i < Blocks.size(); i++) {
		Block* curBlock = Blocks[i];
		int j;
		for (j = 0; j < curBlock->tNodes; j++) {
			if (curBlock->value[j]==key) {
				return true;
			 }
			if (curBlock->childBlock[j] != NULL)
				newBlocks.push_back(curBlock->childBlock[j]);
		}
		if (curBlock->value[j] == INT_MAX && curBlock->childBlock[j] != NULL)
			newBlocks.push_back(curBlock->childBlock[j]);
	}
	if (newBlocks.size() == 0) {
		Blocks.clear();
		return false;
	}
	else {
		return false;
	}
}
void print(vector < Block* > Blocks) {
	vector < Block* > newBlocks;
	for (int i = 0; i < Blocks.size(); i++) { 
		Block* curBlock = Blocks[i];
		cout << "[|";
		int j;
		for (j = 0; j < curBlock->tNodes; j++) {  
			cout << curBlock->value[j] << "|";
			if (curBlock->childBlock[j] != NULL)
				newBlocks.push_back(curBlock->childBlock[j]);
		}
		if (curBlock->value[j] == INT_MAX && curBlock->childBlock[j] != NULL)
			newBlocks.push_back(curBlock->childBlock[j]);
		cout << "]  ";
	}
	if (newBlocks.size() == 0) { 

		puts("");
		puts("");
		Blocks.clear();
	}
	else {                    
		puts("");
		puts("");
		Blocks.clear();
		print(newBlocks);
	}
}
struct BNode {
	int keys[2 * t + 1]; //элементы дерева
	BNode* children[2 * t + 2];//ссылки на дочерние элемента
	BNode* parent;//ссылка на родителя
	int count; //количество элементов в узле
	int countSons; //количество дочерних элементов
	bool leaf;// проверяет на лист 
};
BNode* root;
void Sort(BNode* node) {//метод сортировки в узле при добавлении
	int m;
	for (int i = 0; i < (2 * t + 1); i++) {
		for (int j = i + 1; j <= (2 * t + 1); j++) {
			if ((node->keys[i] != 0) && (node->keys[j] != 0)) {
				if ((node->keys[i]) > (node->keys[j])) {
					m = node->keys[i];
					node->keys[i] = node->keys[j];
					node->keys[j] = m;
				}
			}
		}
	}
	for (int i = 0; i < (2 * t); i++) {
		if (node->keys[i] == 10000)
			node->keys[i] = 0;
	}
}
void AddALement(int key, BNode* node) {//добавление 
	node->keys[node->count] = key;
	node->count = node->count + 1;
	Sort(node);
}
void restruct(BNode* node) {//разбиение узла при переполнении
	if (node->count < (2 * t - 1)) return;
	BNode* child1 = new BNode;
	int j;

	for (j = 0; j <= t; j++) child1->keys[j] = node->keys[j];
	for (j = t; j <= (2 * t); j++) child1->keys[j] = 0;
	child1->count = t;
	if (node->countSons != 0) {
		for (int i = 0; i <= (t + 1); i++) {
			child1->children[i] = node->children[i];
			child1->children[i]->parent = child1;
		}
		for (int i = t + 1; i <= (2 * t + 1); i++) child1->children[i] = NULL;
		child1->leaf = false;
		child1->countSons = t - 1;
	}
	else {
		child1->leaf = true;
		child1->countSons = 0;
		for (int i = 0; i <= (2 * t + 1); i++) child1->children[i] = NULL;
	}
	BNode* child2 = new BNode;

	for (int j = 0; j <= (t); j++) child2->keys[j] = node->keys[j + t + 1];
	for (j = t; j <= (2 * t + 1); j++) child2->keys[j] = 0;
	child2->count = t;
	if (node->countSons != 0) {
		for (int i = 0; i <= (t); i++) {
			child2->children[i] = node->children[i + t + 1];
			child2->children[i]->parent = child2;
		}
		for (int i = t + 1; i <= (2 * t + 1); i++) child2->children[i] = NULL;
		child2->leaf = false;
		child2->countSons = t;
	}
	else {
		child2->leaf = true;
		child2->countSons = 0;
		for (int i = 0; i <= (2 * t + 1); i++) child2->children[i] = NULL;
	}
	if (node->parent == NULL) {
		node->keys[0] = node->keys[t];
		for (int j = 1; j <= (2 * t + 1); j++) {
			node->keys[j] = 0;
		}
		node->children[0] = child1;
		node->children[1] = child2;
		for (int i = 2; i <= (2 * t + 1); i++) {
			node->children[i] = NULL;
		}
		node->parent = NULL;
		node->leaf = false;
		node->count = 1;
		node->countSons = 2;
		child1->parent = node;
		child2->parent = node;
	}
	else {
		AddALement(node->keys[t], node->parent);
		for (int i = 0; i <= (2 * t); i++) {
			if (node->parent->children[i] == node) node->parent->children[i] = NULL;
		}
		for (int i = 0; i <= (2 * t); i++) {
			if (node->parent->children[i] == NULL) {
				for (int j = (2 * t + 1); j > (i + 1); j--) {
					node->parent->children[j] = node->parent->children[j - 1];
				}
				node->parent->children[i + 1] = child2;
				node->parent->children[i] = child1;
				break;
			}
		}
		child1->parent = node->parent;
		child2->parent = node->parent;
		node->parent->leaf = false;
	}
}
void insert(int key) {//добавление элемента в узел
	if (root == NULL) {
		BNode* newRoot = new BNode;
		newRoot->keys[0] = key;
		for (int i = 1; i <= (2 * t + 1); i++) {
			newRoot->keys[i] = 0;
		}
		for (int j = 1; j <= (2 * t + 1); j++) {
			newRoot->children[j] = NULL;
		}
		newRoot->count = 1;
		newRoot->countSons = 0;
		newRoot->leaf = true;
		newRoot->parent = NULL;
		root = newRoot;
	}
	else {
		BNode* ptr = root;
		if (ptr != nullptr) {
			while (ptr->leaf == false) {
				for (int i = 0; i <= (2 * t); i++) {
					if (ptr->keys[i] != 0) {
						if (key <= ptr->keys[i]) {
							ptr = ptr->children[i];
							break;
						}
						if ((ptr->keys[i + 1] == 0) && (key > ptr->keys[i])) {
							ptr = ptr->children[i + 1];
							break;
						}
					}
					else break;
				}

			}
			AddALement(key, ptr);
			while (ptr->count == (2 * t + 1)) {//если идет переполнение
				if (ptr == root) {
					restruct(ptr);
					break;
				}
				else {
					restruct(ptr);
					ptr = ptr->parent;
				}
			}
		}
	}
};
void printBtree(BNode* node, int ost) {//вывод элементов дерева
	if (node != nullptr) {
		if (node->leaf == false) {
			int i;
			for (i = 0; i <= (2 * t); i++) {
				printBtree(node->children[i], ost + 4);
				cout << setw(ost);
				if (node->keys[i] != 0) {
					cout << node->keys[i] << "\n";
				}
			}
		}
		else {
			for (int j = 0; j <= (node->count); j++)
			{
				if (node->keys[j] != 0) {
					cout << setw(ost) << "\n";
					cout << setw(ost) << node->keys[j] << "\n";
					cout << "\n";
				}
			}
			return;
		}
	}
	else return;
}
void removeFromNode(int key, BNode* node) {//удаление из узла
	for (int i = 0; i < node->count; i++) {
		if (node->keys[i] == key) {
			for (int j = i; j < node->count; j++) {
				node->keys[j] = node->keys[j + 1];
				node->children[j] = node->children[j + 1];
			}
			node->keys[node->count - 1] = 0;
			node->children[node->count - 1] = node->children[node->count];
			node->children[node->count] = nullptr;
			break;
		};
	};
	node->count--;
}
void lconnect(BNode* node, BNode* othernode) {
	if (node == nullptr) return;
	for (int i = 0; i <= (othernode->count - 1); i++) {
		node->keys[node->count] = othernode->keys[i];
		node->children[node->count] = othernode->children[i];
		node->count = node->count + 1;
	}
	node->children[node->count] = othernode->children[othernode->count];
	for (int j = 0; j <= node->count; j++) {
		if (node->children[j] == nullptr) break;
		node->children[j]->parent = node;
	}
	delete othernode;
}
void repair(BNode* node) {//починка дерева, для того чтобы соответствовало свойствам дерева
	if ((node == root) && (node->count == 0)) {
		if (root->children[0] != NULL) {
			root->children[0]->parent = NULL;
			root = root->children[0];
			return;
		}
		else {
			delete root;
		} return;
	}
	BNode* ptr = node;
	int k1;
	int k2;
	int positionSon;
	BNode* parent = ptr->parent;
	for (int j = 0; j <= parent->count; j++) {
		if (parent->children[j] == ptr) {
			positionSon = j;
			break;
		}
	}
	if (positionSon == 0) {
		AddALement(parent->keys[positionSon], ptr);
		lconnect(ptr, parent->children[positionSon + 1]);
		parent->children[positionSon + 1] = ptr;
		parent->children[positionSon] = nullptr;
		removeFromNode(parent->keys[positionSon], parent);
		if (ptr->count == 2 * t + 1) {
			while (ptr->count == 2 * t + 1) {
				if (ptr == root) {
					restruct(ptr);
					break;
				}
				else {
					restruct(ptr);
					ptr = ptr->parent;
				}
			}
		}
		else if (parent->count <= (t - 1) && parent->parent != NULL)
			repair(parent);
		else if (parent->count == 0) {
			for (int i = 0; i < parent->children[0]->count; i++) {
				parent->keys[i] = parent->children[0]->keys[i];
				parent->count++;
			}
			if (parent->children[0]->leaf == true) {
				delete parent->children[0];
				parent->leaf = true;
				parent->countSons = 0;
			}
			else {
				BNode* copyChild = new BNode;
				copyChild = parent->children[0];
				for (int i = 0; i < t * 2 + 1; i++) {
					parent->children[i] = copyChild->children[i];
				}
				delete copyChild;
			}
		}
	}
	else {
		if (positionSon == parent->count) {
			AddALement(parent->keys[positionSon - 1], parent->children[positionSon - 1]);
			lconnect(parent->children[positionSon - 1], ptr);
			parent->children[positionSon] = parent->children[positionSon - 1];
			parent->children[positionSon - 1] = NULL;
			removeFromNode(parent->keys[positionSon - 1], parent);
			BNode* temp = parent->children[positionSon];
			if (ptr->count == 2 * t + 1) {
				while (temp->count == 2 * t + 1) {
					if (temp == root) {
						restruct(temp);
						break;
					}
					else {
						restruct(temp);
						temp = temp->parent;
					}
				}
			}
			else if (parent->count <= (t - 1) && parent->parent != NULL)
				repair(parent);
		}
		else {
			AddALement(parent->keys[positionSon], ptr);
			lconnect(ptr, parent->children[positionSon + 1]);
			parent->children[positionSon + 1] = ptr;
			parent->children[positionSon] = nullptr;
			removeFromNode(parent->keys[positionSon], parent);
			if (ptr->count == 2 * t + 1) {
				while (ptr->count == 2 * t + 1)
				{
					if (ptr == root) {
						restruct(ptr);
						break;
					}
					else {
						restruct(ptr);
						ptr = ptr->parent;
					}
				}
			}
			else if (parent->count <= (t - 1) && parent->parent != NULL)
				repair(parent);
		}
	}
}
void removeLeaf(int key, BNode* node) {//удаление и соединение элементов из листовых элементов
	if ((node == root) && (node->count == 2)) {
		removeFromNode(key, node);
		delete root;
		root = nullptr;
		return;
	}

	if (node == root) {
		removeFromNode(key, node);
		return;
	}
	if (node->count > t) {
		removeFromNode(key, node);
		return;
	}
	BNode* ptr = node;
	int k1;
	int k2;
	int position;
	int positionSon;
	int i;
	for (int i = 0; i <= node->count - 1; i++) {
		if (key == node->keys[i]) {
			position = i;
			break;
		}
	}
	BNode* parent = ptr->parent;
	for (int j = 0; j <= parent->count; j++) {
		if (parent->children[j] == ptr) {
			positionSon = j;
			break;
		}
	}
	if (positionSon == 0) {
		if (parent->children[positionSon + 1]->count > (t)) {
			k1 = parent->children[positionSon + 1]->keys[0];
			k2 = parent->keys[positionSon];
			AddALement(k2, ptr);
			removeFromNode(key, ptr);
			parent->keys[positionSon] = k1;
			removeFromNode(k1, parent->children[positionSon + 1]);
		}
		else {
			removeFromNode(key, ptr);
			if (ptr->count <= (t - 1)) repair(ptr);
		}
	}
	else {
		if (positionSon == parent->count) {
			if (parent->children[positionSon - 1]->count > (t)) {
				BNode* temp = parent->children[positionSon - 1];
				k1 = temp->keys[temp->count - 1]; //k1 - максимальный ключ левого брата
				k2 = parent->keys[positionSon - 1];
				AddALement(k2, ptr);
				removeFromNode(key, ptr);
				parent->keys[positionSon - 1] = k1;
				removeFromNode(k1, temp);
			}
			else {
				removeFromNode(key, ptr);
				if (ptr->count <= (t - 1))repair(ptr);
			}
		}
		else {
			if (parent->children[positionSon - 1]->count > (t)) {
				k1 = parent->children[positionSon + 1]->keys[0];
				k2 = parent->keys[positionSon];
				AddALement(k2, ptr);
				removeFromNode(key, ptr);
				parent->keys[positionSon] = k1;
				removeFromNode(k1, parent->children[positionSon + 1]);
			}
			else {
				if (parent->children[positionSon - 1]->count > t) {
					BNode* temp = parent->children[positionSon - 1];
					k1 = temp->keys[temp->count - 1];
					k2 = parent->keys[positionSon - 1];
					AddALement(k2, ptr);
					removeFromNode(key, ptr);
					parent->keys[positionSon - 1] = k1;
					removeFromNode(k1, temp);
				}
				else {
					removeFromNode(key, ptr);
					if (ptr->count <= t - 1)repair(ptr);
				}
			}
		}
	}
}
bool searchKey(int key, BNode* node) {//поиск элемента в дереве
	if (node != nullptr) {
		if (node->leaf == false) {
			int i;
			for (i = 0; i <= (2 * t); i++) {
				if (node->keys[i] != 0) {
					if (key == node->keys[i]) return true;
					if ((key < node->keys[i])) {
						return searchKey(key, node->children[i]);
						break;
					}
				}
				else break;
			}
			return searchKey(key, node->children[i]);
		}
		else {
			for (int j = 0; j <= (2 * t); j++)
				if (key == node->keys[j]) return true;
			return false;
		}
	}
	else return false;
}

void removeParent(int key, BNode* node) {//удаление из родительского элемента
	BNode* ptr = node;
	int position;
	int i;
	for (int i = 0; i <= node->count - 1; i++) {
		if (key == node->keys[i]) {
			position = i;
			break;
		}
	}
	int positionSon;
	if (ptr->parent != NULL) {
		for (int i = 0; i <= ptr->parent->count; i++) {
			if (ptr->children[i] == ptr) {
				positionSon = i;
				break;
			}
		}
	}
	ptr = ptr->children[position + 1];
	int newkey = ptr->keys[0];
	while (ptr->leaf == false) ptr = ptr->children[0];
	if (ptr->count > (t)) {
		newkey = ptr->keys[0];
		removeFromNode(newkey, ptr);
		node->keys[position] = newkey;
	}
	else {
		ptr = node;

		ptr = ptr->children[position];
		newkey = ptr->keys[ptr->count - 1];
		while (ptr->leaf == false) ptr = ptr->children[ptr->count];
		newkey = ptr->keys[ptr->count - 1];
		node->keys[position] = newkey;
		if (ptr->count > (t)) removeFromNode(newkey, ptr);
		else {

			removeLeaf(newkey, ptr);
		}
	}
}

void remove(int key) {//основной метод удаления из дерева, который определяет какой метод удаление нужно применить (удаление из removeleaf или removeParent)
	BNode* ptr = root;
	int position;
	int positionSon;
	int i;
	if (searchKey(key, ptr) == false) {
		return;
	}
	else {
		for (i = 0; i <= ptr->count - 1; i++) {
			if (ptr->keys[i] != 0) {
				if (key == ptr->keys[i]) {
					position = i;
					break;
				}
				else {
					if ((key < ptr->keys[i])) {
						ptr = ptr->children[i];
						positionSon = i + 1;
						i = -1;
					}
					else {
						if (i == (ptr->count - 1)) {
							ptr = ptr->children[i + 1];
							position = i + 1;
							i = -1;
						}
					}
				}
			}
			else break;
		}
	}
	if (ptr->leaf == true) {
		if (ptr->count > (t)) removeFromNode(key, ptr);
		else removeLeaf(key, ptr);
	}
	else removeParent(key, ptr);
}
int main()
{
	setlocale(LC_ALL, "ru");
	root = NULL;
	int elem = 0;
	int select_tree = 0;
	int choise = 0;
	string sre = "";
	vector < Block* > Blocks;
	cout << "Какое дерево вы хотите сделать ? B-tree или B+-tree"<<"\n";
	cout << "1- B-tree" << "\n";
	cout << "2- B+-tree" << "\n";
	cin >> select_tree;
	getline(cin, sre);
	if (select_tree == 1) {
		cout << "Хороший выбор!!" << "\n";
		int nus;
		do {
			cout << "Введите 1 для добавление" << "\n";
			cout << "Введите 2 для удаления" << "\n";
			cin >> nus;
			getline(cin, sre);
			if (nus == 1) {
				cout << "Откуда вы хотите добавить элементы 1-ввести вручную 2- добавить из файла" << "\n";
				cin >> choise;
				getline(cin, sre);
				if (choise == 1) {
					string d = " ";
					string s="";
					getline(cin, s);
					vector<string> v;
					int b, e = 0;
					
					while ((b = s.find_first_not_of(d, e)) != s.npos) {
						e = s.find_first_of(d, b);// идет запоминане места где был пробел
						v.push_back(s.substr(b, e - b));
						b = e;//передача этого места в следуещее значение
					}
					for (int c = 0; c < v.size(); c++) {
						insert(atoi(v[c].c_str())); 
					}
				}
				else if (choise == 2) {
					string Fils = "C:\\Users\\shalo\\Desktop\\C\\";
					string NameFiles = "";
					cin >> NameFiles;
					Fils = Fils + NameFiles;
					ifstream Files(Fils);
					while (!Files.eof()) {
						Files >> elem;
						insert(elem);
					}
				}
			}
			 else if(nus==2) {
				string s, d = " ";
				getline(cin, s);
				vector<string> v;
				int b, e = 0;
				
				while ((b = s.find_first_not_of(d, e)) != s.npos) {
					e = s.find_first_of(d, b);// идет запоминане места где был пробел
					v.push_back(s.substr(b, e - b));
					b = e;//передача этого места в следуещее значение
				}
				for (int c = 0; c < v.size(); c++) { remove(atoi(v[c].c_str())); }
			}
			printBtree(root, 4);
		} while (nus != 0);

	} else if (select_tree == 2) {
		cout << "Хороший выбор!!" << "\n";
		int nus;
		do {
			cout << "Введите 1 для добавление" << "\n";
			cout << "Введите 2 для удаления" << "\n";
			cin >> nus;
			getline(cin, sre);
			if (nus == 1) {
				cout << "Откуда вы хотите добавить элементы 1-ввести вручную 2- добавить из файла" << "\n";
				cin >> choise;
				getline(cin, sre);
				if (choise == 1) {
					string s, d = " ";
					
					getline(cin, s);
					vector<string> v;
					int b, e = 0;
					while ((b = s.find_first_not_of(d, e)) != s.npos) {
						e = s.find_first_of(d, b);// идет запоминане места где был пробел
						v.push_back(s.substr(b, e - b));
						b = e;//передача этого места в следуещее значение
					}
					for (int c = 0; c < v.size(); ++c) { insertNode(rootBlock,atoi(v[c].c_str())); }
				}
				else if (choise == 2) {
					string Fils = "C:\\Users\\shalo\\Desktop\\C\\";
					string NameFiles = "";
					cin >> NameFiles;
					Fils = Fils + NameFiles;
					ifstream Files(Fils);
					while (!Files.eof()) {
						Files >> elem;
						insertNode(rootBlock,elem);
					}
				}
				Blocks.clear();
				Blocks.push_back(rootBlock);
				print(Blocks);
			}
			else if (nus == 2) {
				string s, d = " ";
				getline(cin, s);
				vector<string> v;
				int b, e = 0;
				cout << "sd" << "\n";
				while ((b = s.find_first_not_of(d, e)) != s.npos) {
					e = s.find_first_of(d, b);// идет запоминане места где был пробел
					v.push_back(s.substr(b, e - b));
					b = e;//передача этого места в следуещее значение
				}
				for (int c = 0; c < v.size(); ++c) { deleteNode(rootBlock,atoi(v[c].c_str()),0); }
				Blocks.clear();
				Blocks.push_back(rootBlock);
				print(Blocks);
			}
		} while (nus != 0);
	}
}
/*
1 Выбор структуры при старте программы B - деревом или В + дерева.++
2 Добавление элементов с клавиатуры с вводом целых элементов в строке через
пробелы+
3 Добавление элементов из текстового файла с выбором имени файла.++
4 Удаление элементов из дерева введенных с клавиатуры через пробелы в одной
строке.
5 Отображение дерева на экране после каждой операции.*/