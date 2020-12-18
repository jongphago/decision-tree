#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int dataCount, attributeCount;
int* flag;//[100] = { 0, };

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
	int leaf;
	candidate down;
	split leftChild;
	split rightChild;
}splitNode;



split initSplit(int xattribute, int xvalue)
{
	/**
	* 	함수 [initSplit]은 구조체 "splitNode"를 초기화 하는 함수입니다. 구조체 "splitNode"는 
	*	이진트리구조에서하나의 분기점을 나타냅니다. 이 함수는 주로 트리가 분기 할때 사용 됩니다. 
	* 정수타입 멤버 "attribute"는 데이터의 분기 기준이 되는 속성 번호를 나타냅니다.
	* 예를 들어 어떤 노드가 속성번호 1을 기준으로 값이 거짓(0)인 경우 leftChild로 참(1)인 경우 rightChild로
	* 데이터를 전달합니다.
	* 정수타입 멤버 "value"는 ...
	* "candidate" 타입 멤버 "down"은 node에 전달 된 데이터를 저장하는 헤드 역활을 합니다. 
	*	NULL로 초기화 합니다.
	*/
	split pointer = malloc(sizeof(splitNode));
	pointer->attribute = xattribute;
	pointer->vaule = xvalue; 
	pointer->leaf = -1;
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



void readData(split root)							// 입력파일경로
/*
* 함수 [readData]는 입력 텍스트 파일 읽고 구조체 "candidate"에 저장합니다.
* 입력 파일 첫줄에 있는 데이터의 수와 속성의 수를 전역변수 "dataCount"와
* "attributeCount"에 각각 저장합니다.
* 데이터의 수 만큼 반복문을 실행하여 "candidate"타입 변수 vCandidate을 선언하고,
* 정수형 포인터 타입 멤버 attribute 배열에 입력파일 데이터를 저장합니다.
* 마지막으로 정수 타입 멤버 label에 데이터를 저장합니다.
* 초기화를 마친 "vCandidate"을 함수 [pushCandidate]을 호출하여
*	트리의 첫번째 노드 "root"에 저장합니다.
*/
{
	FILE* fpInput = fopen("_data.txt", "r");
	int _result = fscanf(fpInput, "%d %d", &dataCount, &attributeCount);

	for (int i = 0; i < dataCount; i++)
	{
		candidate vCandidate = initCandidate();
		for (int j = 0; j < attributeCount; j++)
		{
			int _result = fscanf(fpInput, "%d", &(vCandidate->attribute[j]));
		}
		int _result = fscanf(fpInput, "%d", &(vCandidate->label));
		pushCandidate(root, vCandidate);
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
			printf("%d %d\n", _count, __count);
			float _probability;
			if (_count || __count) {
				_probability = (float)__count / (float)_count;
			}
			else _probability = 0;
			__entropy += entropy(_probability);
		}
		float probability = (float)_count / (float)count;
		printf("확률: %f\n", probability);
		__entropy *= probability;
		printf("엔트로피:%f\n", __entropy);
		_entropy += __entropy;
	}
	printf("최종엔트로피:%f\n", _entropy);
	return _entropy;
}

int bestAttribute(split root)
{
	//DEL//candidate head = root->down;
	float min = 2;
	int minIndex = -1;
	for (int i = 0; i < attributeCount; i++)
	{
		if (flag[i]) continue;
		float entropy = partition_entropy_by(root, i);
		if (min > entropy) {
			min = entropy;
			minIndex = i;
		}
	}
	if (minIndex < 0) {
		printf("function bestAttribute Error");
		exit(0);
	}
	flag[minIndex] = 1;
	return minIndex;
}

int labelCounts(split root)
/*
* 함수 [labelCounts]는 노드에 저장된 candidate의 label의 수가 1개면 0(False)를,
* 2개면 1(True)이상의 수를 반환합니다. 
*	함수 [buildTree]에서 labelCounts가 0이면 (if(!labelCounts(root))
*/
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
	if (sum0 * sum1) return 2;
	else {
		if (sum0) return 0;
		else return 1;
	}
}

int mostCommonLabel(split root)
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
	if (sum0 >= sum1) return 0;
	else return 1;
}


int splitAttribute()
{
	int multiply = 1;
	for (int i = 0; i < attributeCount; i++)
		multiply *= flag[i];
	return multiply;
}

void showCandidateCount(split root)
{
	candidate node = root->down;
	int count = 0;
	while (node)
	{
		count++;
		node = node->down;
	}
	printf("노드의수: %d\n", count);
}

void moveCandidate(split root, int index)
{
	while (root->down)
	{
		candidate node = root->down;
		candidate temp = popCandidate(root);
		int attr = node->attribute[index];
		if (attr) {
			pushCandidate(root->rightChild, temp);
		}
		else {
			pushCandidate(root->leftChild, temp);
		}
		//printf("%d", attr);
		node = node->down;
	}
}

void buildTree(split root)
{
	showCandidateCount(root);

	int labelCount = labelCounts(root);
	if (labelCount != 2) {
		root->leaf = labelCount;
		printf("유일한 라벨입니다. 함수를 종료합니다.\n");
		return;
	}
	if (splitAttribute()) {
		root->leaf = mostCommonLabel(root);
		printf("모든 label을 탐색하였습니다. 함수를 종료합니다.\n");
		return;
	}

	int index = bestAttribute(root);
	root->attribute = index;

	root->leftChild = initSplit(index, 0);
	root->rightChild = initSplit(index, 1);

	//candidate node = root->down;
	moveCandidate(root, index);

	printf("INDEX:%d leftChild 계산을 시작합니다.\n", index);
	buildTree(root->leftChild);
	printf("INDEX:%d rightChild 계산을 시작합니다.\n", index);
	buildTree(root->rightChild);
	printf("INDEX:%d 함수를 종료합니다.\n", index);
}



void main()
{

	split root = initSplit(-1, -1);
	readData(root); 

	flag = calloc(attributeCount, sizeof(int));

	buildTree(root);
		
	return;
}