#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int dataCount, attributeCount;
int flag[100] = { 0, };

typedef struct candidateNode* candidate;
typedef struct candidateNode {
	int* attribute;
	int label;
	candidate down;
	candidate next;
}candidateNode;

typedef struct splitNode* split;
typedef struct splitNode {
	int attribute;
	int vaule;
	candidate down;
	split leftChild;
	split rightChild;
}splitNode;

split initSplit(int xattribute, int xvalue)
{
	split pointer = malloc(sizeof(splitNode));
	pointer->attribute = xattribute;
	pointer->vaule = xvalue; 
	pointer->down = NULL; 
	pointer->leftChild = NULL;
	pointer->rightChild = NULL;
	return pointer;
}

candidate initCandidate()
{
	candidate pointer = malloc(sizeof(candidateNode));
	pointer->attribute = malloc(attributeCount * sizeof(int));
	pointer->down = NULL;
	pointer->next = NULL;
	return pointer;
}

void pushCandidate(split root, candidate pointer)
{
	pointer->down = root->down;
	root->down = pointer;
}

candidate popCandidate(split root)
{
	candidate node = root->down;
	root->down = node->down;
	node->down = NULL;
	return node;
}

void readHead(candidate head)
{
	candidate pointer = head->down;

	for (int i = 0; i < dataCount; i++)
	{
		for (int j = 0; j < attributeCount; j++)
		{
			printf("%d ", pointer->attribute[j]);
		}
		printf("%d ", pointer->label);
		printf("\n");
		pointer = pointer->down;
	}
}

float entropy(float f)
{
	if (f < 0 || f > 1) {
		printf("entropy error: 확률의 범위가 올바르지 않습니다.");
		exit(0);
	}
	if (f == 0) return 0;
	return -f * (float)log2(f);
}



void readData(split root)
{
	FILE* fpInput = fopen("_data.txt", "r");
	int _result = fscanf(fpInput, "%d %d", &dataCount, &attributeCount);

	//candidate head = initCandidate(root);
	for (int i = 0; i < dataCount; i++)
	{
		candidate candidate = initCandidate();
		for (int j = 0; j < attributeCount; j++)
		{
			int _result = fscanf(fpInput, "%d", &(candidate->attribute[j]));
		}
		int _result = fscanf(fpInput, "%d", &(candidate->label));
		pushCandidate(root, candidate);
	}
}

float trueProbability(candidate head, int index)
{
	candidate pointer = head->down;
	int sum = 0;
	for (int i = 0; i < dataCount; i++)
	{
		sum += pointer->attribute[index];
		pointer = pointer->down;
	}
	float probability = (float)sum / (float)dataCount;
	return probability;
}

float dataEntropy(candidate head, int index)
{
	float trueProb = trueProbability(head, index);
	float falseProb = 1 - trueProb;
	float value = entropy(trueProb);
	value += entropy(falseProb);
	return value;
}

int trueCounter(candidate head, int index)
{
	candidate pointer = head->down;
	int total = 0;
	for (int i = 0; i < dataCount; i++)
	{
		total += pointer->attribute[index];
		pointer = pointer->down;
	}
	return total;
}

int isMuchTrue(candidate head, int index)
{
	int total = trueCounter(head, index);
	if (total > dataCount - total)
		return 1;
	return 0;
}



float partition_entropy_by(split root, int index)
{
	float _entropy = 0;
	for (int i = 0; i < 2; i++) {
		float __entropy = 0;
		int count;
		int _count;
		for (int j = 0; j < 2; j++) {
			count = 0;
			_count = 0;
			int __count = 0;
			candidate temp = root->down;
			while (temp) {
				count++;
				if (temp->attribute[index] == i) {
					_count++;
					if (temp->label == j) __count++;
				}
				temp = temp->down;
			}
			//printf("%d %d\n", _count, __count);
			float _probability = (float)__count / (float)_count;
			__entropy += entropy(_probability);
		}
		float probability = (float)_count / (float)count;
		//printf("확률: %f\n", probability);
		__entropy *= probability;
		//printf("엔트로피:%f\n", __entropy);
		_entropy += __entropy;
	}
	//printf("최종엔트로피:%f\n", _entropy);
	return _entropy;
}

int bestAttribute(split root)
{
	candidate head = root->down;
	float min = 2;
	int minIndex = attributeCount;
	for (int i = 0; i < attributeCount; i++)
	{
		if (flag[i]) continue;
		float entropy = partition_entropy_by(head, i);
		if (min > entropy) {
			min = entropy;
			minIndex = i;
		}
	}
	flag[minIndex] = 1;
	return minIndex;
}

int labelCounts(split root)
{
	candidate temp = root->down;
	int sum0 = 0, sum1 = 0;
	while (temp)
	{
		if (temp->label == 0)
			sum0++;
		else if (temp->label == 1)
			sum1++;
		temp = temp->down;
	}
	return sum0 * sum1;
}

int splitAttribute()
{
	int multiply = 1;
	for (int i = 0; i < attributeCount; i++)
		multiply *= flag[i];
	return multiply;
}

void buildTree(split root)
{

	if (!labelCounts) {
		return;
	}
	if (splitAttribute()) {
		return;
	}

	int index = bestAttribute(root);
	root->attribute = index;

	root->leftChild = initSplit(index, 0);
	root->rightChild = initSplit(index, 1);

	//candidate node = root->down;
	while (root->down)
	{
		candidate node = root->down;
		candidate temp = popCandidate(root);
		int attr = node->attribute[index];
		if (attr) {
			pushCandidate(root->leftChild, temp);
		}
		else {
			pushCandidate(root->rightChild, temp);
		}
		//printf("%d", attr);
		//node = node->down;
	}
	buildTree(root->leftChild);
	buildTree(root->rightChild);
}



void main()
{



	split root = initSplit(-1, -1);
	readData(root); 

	buildTree(root);
		
	return;
}