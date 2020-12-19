#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int dataCount, featureCount;
int* flag;//[100] = { 0, };

typedef struct candidateNode* candidate;
typedef struct candidateNode {
	int* feature;
	int label;
	candidate down;
	//candidate next;
}candidateNode;

typedef struct splitNode* split;
typedef struct splitNode {
	int feature;
	int vaule;
	int leaf;
	int* flag;
	candidate down;
	split leftChild;
	split rightChild;
	split next;
}splitNode;

typedef struct queue {
	split front;
	split rear;
	int count;
}queue;

/**
	* 	함수 [initSplit]은 구조체 "splitNode"를 초기화 하는 함수입니다. 구조체 "splitNode"는
	*	이진트리구조에서하나의 분기점을 나타냅니다. 이 함수는 주로 트리가 분기 할때 사용 됩니다.
	* 정수타입 멤버 "feature"는 데이터의 분기 기준이 되는 속성 번호를 나타냅니다.
	* 예를 들어 어떤 노드가 속성번호 1을 기준으로 값이 거짓(0)인 경우 leftChild로 참(1)인 경우 rightChild로
	* 데이터를 전달합니다.
	* 정수타입 멤버 "value"는 ...
	* "candidate" 타입 멤버 "down"은 node에 전달 된 데이터를 저장하는 헤드 역활을 합니다.
	*	NULL로 초기화 합니다.
	*/
split initSplit(int xfeature, int xvalue)
{
	split pointer = malloc(sizeof(splitNode));
	pointer->flag = calloc(featureCount , sizeof(int));
	pointer->feature = xfeature;
	pointer->vaule = xvalue; 
	pointer->leaf = -1;
	pointer->down = NULL;
	pointer->leftChild = NULL;
	pointer->rightChild = NULL;
	pointer->next = NULL;
	return pointer;
}

queue initQueue()
{
	queue top;
	top.front = NULL;
	top.rear = NULL;
	top.count = 0;
	return top;
}

void queuePush(queue* top, split node)
{
	if (top->count == 0) {
		top->front = node;
	}
	else {
		top->rear->next = node;
	}
	top->rear = node;
	top->count++;
}

split queuePop(queue* top)
{
	if (top->count == 0) {
		printf("Queue Underflow\n");
		return 0;
	}
	split node = top->front;
	top->front = node->next;
	node->next = NULL;
	top->count--;
	return node;
}

candidate initCandidate()
{
	candidate pointer = malloc(sizeof(candidateNode));
	pointer->feature = malloc(featureCount * sizeof(int));
	pointer->down = NULL;
	//pointer->next = NULL;
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
		for (int j = 0; j < featureCount; j++)
		{
			printf("%d ", pointer->feature[j]);
		}
		printf("%d ", pointer->label);
		printf("\n");
		pointer = pointer->down;
	}
}

float entropy(float probability)
{
	if (probability < 0 || probability > 1) {
		printf("entropy error: 확률의 범위가 올바르지 않습니다.");
		exit(0);
	}
	if (probability == 0) return 0;
	return -probability * (float)log2(probability);
}

/*
* 함수 [readData]는 입력 텍스트 파일 읽고 구조체 "candidate"에 저장합니다.
* 입력 파일 첫줄에 있는 데이터의 수와 속성의 수를 전역변수 "dataCount"와
* "featureCount"에 각각 저장합니다.
* 데이터의 수 만큼 반복문을 실행하여 "candidate"타입 변수 vCandidate을 선언하고,
* 정수형 포인터 타입 멤버 feature 배열에 입력파일 데이터를 저장합니다.
* 마지막으로 정수 타입 멤버 label에 데이터를 저장합니다.
* 초기화를 마친 "vCandidate"을 함수 [pushCandidate]을 호출하여
*	트리의 첫번째 노드 "root"에 저장합니다.
*/
void readData(split root)	
{
	FILE* fpInput = fopen("data.txt", "r");
	int _result = fscanf(fpInput, "%d %d", &dataCount, &featureCount);

	for (int i = 0; i < dataCount; i++)
	{
		candidate vCandidate = initCandidate();
		for (int j = 0; j < featureCount; j++)
		{
			int _result = fscanf(fpInput, "%d", &(vCandidate->feature[j]));
		}
		int _result = fscanf(fpInput, "%d", &(vCandidate->label));
		pushCandidate(root, vCandidate);
	}
}

float partition_entropy_by(split root, int index)
{
	float labelEntropy = 0;
	for (int i = 0; i < 2; i++) {
		float featureEntropy = 0;
		int totalCount;
		int featureCount;
		int labelCount;

		for (int j = 0; j < 2; j++) {
			totalCount = 0;
			featureCount = 0;
			labelCount = 0;
			candidate temp = root->down;
			while (temp) {
				totalCount++;
				if (temp->feature[index] == i) {
					featureCount++;
					if (temp->label == j) labelCount++;
				}
				temp = temp->down;
			}
			//printf("%d %d\n", featureCount, labelCount);
			float _probability = 0;
			if (featureCount) _probability = (float)labelCount / (float)featureCount;
			featureEntropy += entropy(_probability);
		}

		float probability = 0;
		if (totalCount) probability = (float)featureCount / (float)totalCount;

		//printf("확률: %f\n", probability);
		featureEntropy *= probability;
		//printf("엔트로피:%f\n", featureEntropy);
		labelEntropy += featureEntropy;
	}
	printf("INDEX: %d\t최종엔트로피:%f\n", index, labelEntropy);
	return labelEntropy;
}

int bestAttribute(split root)
{
	float min = 2;
	int minIndex = -1;
	for (int i = 0; i < featureCount; i++)
	{
		if (root->flag[i]) continue;
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
	root->flag[minIndex] = 1;
	return minIndex;
}

/*
* 함수 [labelCounts]는 노드에 저장된 candidate의 label의 수를 반환합니다.
*	함수 [buildTree]에서 labelCounts가 0이면 (if(!labelCounts(root))
*/
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
	printf("0:%d\t1:%d\t", sum0, sum1);
	if (sum0 >= sum1) return 0;
	else return 1;
}

int splitAttribute(split root)
{
	int multiply = 1;
	for (int i = 0; i < featureCount; i++)
		multiply *= root->flag[i];
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
		int _feature = node->feature[index];
		if (_feature) {
			pushCandidate(root->rightChild, temp);
		}
		else {
			pushCandidate(root->leftChild, temp);
		}
		//printf("%d", _feature);
		node = node->down;
	}
}

void freeSplit(split root)
{
	while (root->down) {
		candidate node = root->down;
		root->down = node->down;
		node->down = NULL;
		free(node);
	}
}

void printRoot(candidate tempNode)
{
	while (tempNode) {
		for (int i = 0; i < featureCount; i++) {
			printf("%d ", tempNode->feature[i]);
		}
		printf("%d\n", tempNode->label);
		tempNode = tempNode->down;
	}
}

void buildTree(queue* top, split root)
{
	showCandidateCount(root);

	int labelCount = labelCounts(root);
	if (labelCount != 2) {
		root->leaf = labelCount;
		printf("LEAF:%d\t유일한 라벨입니다. 함수 종료\n", labelCount);
		freeSplit(root);
		return;
	}
	if (splitAttribute(root)) {
		root->leaf = mostCommonLabel(root);
		printf("LEAF:%d\t모든 label을 탐색하였습니다. 함수 종료\n", root->leaf);
		freeSplit(root);
		return;
	}

	int index = bestAttribute(root);
	printf("BEST INDEX: %d\n", index);
	root->feature = index;

	root->leftChild = initSplit(index, 0);
	for (int i = 0; i < featureCount; i++)
	{
		root->leftChild->flag[i] = root->flag[i];
	}
	root->rightChild = initSplit(index, 1);
	for (int i = 0; i < featureCount; i++)
	{
		root->rightChild->flag[i] = root->flag[i];
	}

	moveCandidate(root, index);

	/*candidate tempNode = root->leftChild->down;
	printf("###INDEX:%d 의 leftChild를 출력합니다###\n", index);
	printRoot(tempNode);
	printf("########################################\n\n");

	tempNode = root->rightChild->down;
	printf("###INDEX:%d 의 rightChild를 출력합니다###\n", index);
	printRoot(tempNode);
	printf("########################################\n\n");*/

	queuePush(top, root->leftChild);
	queuePush(top, root->rightChild);
}

candidate readTestData(FILE* fpTest)
{
	candidate vCandidate = initCandidate();
	for (int j = 0; j < featureCount; j++)
	{
		int _result = fscanf(fpTest, "%d", &(vCandidate->feature[j]));
	}
	return vCandidate;
}

FILE* readTestFile(int* testCount)
{
	FILE* fpTest = fopen("test.txt", "r");
	int _result = fscanf(fpTest, "%d", testCount);
	return fpTest;
}

void fprintOutput(FILE* fpOutput, FILE* fpTest, split outputRoot)
{
	candidate testCandidate = readTestData(fpTest);

	while (outputRoot) {
		int leafValue = outputRoot->leaf;
		if (leafValue >= 0) {
			printf("%d\n", leafValue);
			fprintf(fpOutput, "%d\n", leafValue);
			break;
		}

		int index = outputRoot->feature;
		int _feature = testCandidate->feature[index];
		if (_feature)
			outputRoot = outputRoot->rightChild;
		else
			outputRoot = outputRoot->leftChild;
	}
}

void main()
{
	queue top = initQueue();

	split root = initSplit(-1, -1);
	readData(root); 

	root->flag = calloc(featureCount, sizeof(int));
	split tempRoot = root;
	while (tempRoot) {
		buildTree(&top, tempRoot);
		tempRoot = queuePop(&top);
	}

	int testCount;
	FILE* fpTest = readTestFile(&testCount);
	FILE* fpOutput = fopen("_output.txt", "w");

	for (int i = 0; i < testCount; i++) {
		split outputRoot = root;
		fprintOutput(fpOutput, fpTest, outputRoot);
	}

	return;
}