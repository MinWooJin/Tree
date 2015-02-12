#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#define DEGREE 5    //B-Ʈ���� ���� 
#define MAX_ELEMENTS 1000   //�ԷµǴ� KEY�� �ִ� ��
#define DELETE_COUNT 10     //������ ���� ���� ��

//��������
FILE* pInputStream;  //input ȭ���� �ڵ�
FILE* pOutputStream; //output ȭ���� �ڵ� 

struct NODE { //BƮ�� ��� ���� 
    int bRoot;
    int nElm[DEGREE-1];
    void* pChild[DEGREE];
};

struct OVERFLOW_NODE { //BƮ���� �����÷� ��� ����
    int bRoot;
    int nElm[DEGREE];
    void* pChild[DEGREE+1];
};

struct STACK_NODE { //stack�� ��屸��
    struct NODE* pCurr;
    void* pPrev;
};

//----------------------���� �� �Լ� ����------------------------
 
struct NODE* CreateNode() { //������ - �Լ� 
    struct NODE* pNode;
    pNode = (struct NODE*)malloc(sizeof(struct NODE));
    memset(pNode, 0, sizeof(struct NODE));
    return pNode;
}

struct OVERFLOW_NODE* CreateOverflowNode() { //�����÷ο� ��� ���� - �Լ�
    struct OVERFLOW_NODE* pNode;
    pNode = (struct OVERFLOW_NODE*)malloc(sizeof(struct OVERFLOW_NODE));
    memset(pNode, 0, sizeof(struct OVERFLOW_NODE));
    return pNode;
}

struct STACK_NODE* CreateStackNode() { //���س�� ���� - �Լ�
    struct STACK_NODE* pStackNode;
    pStackNode = (struct STACK_NODE*)malloc(sizeof(struct STACK_NODE));
    memset(pStackNode, 0, sizeof(struct STACK_NODE));
    return pStackNode;
}

struct  STACK_NODE* PushIntoStack (struct STACK_NODE* pStackTop, struct NODE* pNode) { 
    //���ÿ� push
    struct STACK_NODE* pNewTop;
    if (pStackTop == NULL||pNode == NULL) {
        return NULL;
    }
    
    if (pStackTop->pCurr == NULL) {
        //top�� ����ִ� ���
        pStackTop->pCurr = pNode;
        pStackTop->pPrev = NULL;
        return pStackTop;
    }
    else { //�� top�� ����
        pNewTop = CreateStackNode();
        pNewTop->pPrev = (void*) pStackTop;
        pNewTop->pCurr = pNode;
        return pNewTop;
    }
}

struct STACK_NODE* PopFromStack(struct STACK_NODE* pStackTop) {
    //���ÿ��� pop
    struct STACK_NODE* pPrevTop;
    struct NODE* pNode;
    if (pStackTop == NULL) return NULL;
    if (pStackTop->pCurr == NULL) return NULL;
    else if (pStackTop->pPrev == NULL) {
        //���� top
        pNode = pStackTop->pCurr;
        pStackTop->pCurr = NULL;
        return pStackTop;
    }
    else {
        //pCurr, pPrev ��� ����
        pNode = pStackTop->pCurr;
        pPrevTop = pStackTop;
        pStackTop = (struct STACK_NODE*) pStackTop->pPrev;
        //���� top ����
        free((void*)pPrevTop);
        return pStackTop;
    }
}

struct NODE* PeepStackTop (struct STACK_NODE* pStackTop) {
    //stack�� top�� peep
    if (pStackTop == NULL) return NULL;
    return pStackTop->pCurr;
}

int CheckElementExist(struct NODE* pNode, int key) {
    //��忡 key�� �ִ��� �˻�
    int i;
    int bExist;
    if (pNode == NULL) return 0;
    bExist = 0;
    for (i=0; i<DEGREE-1; ++i) {
        if (key == pNode->nElm[i]) {
            bExist = 1;
            break;
        }
    }
    return bExist;
}

int GetElementCount(struct NODE* pNode) {
    //����� ���� �� ��ȯ
    int i;
    int count;
    if (pNode == NULL) return 0;
    count = 0;
    for (i=0; i<DEGREE-1; ++i) {
        if (pNode->nElm[i] > 0) count++;
        else break;
    }
    return count;
}

void PrintNodeElement(struct NODE* pNode) {
    //����� key�� ���
    int i;
    struct NODE* pChild;
    //post-order�� ���
    pChild = pNode->pChild[0];
    if (pChild == NULL) {
        //leaf
        fprintf(pOutputStream, "[");
        for (i=0; i<DEGREE-1; ++i) {
            if (pNode->nElm[i] > 0) fprintf(pOutputStream,"%d ", pNode->nElm[i]);
        }
        fprintf(pOutputStream, "] ");
        return;
    }
    PrintNodeElement(pNode->pChild[0]);
    //leftmost child ���: node ���� key�� �ٸ� child ���
    for (i=1; i<DEGREE; ++i) {
        if (pNode->pChild[i] == NULL) return;
        PrintNodeElement(pNode->pChild[i]);
        fprintf(pOutputStream, "%d ", pNode->nElm[i-1]);
    }
    return;
}

int main(int argc, char *argv[])
{
  int i, j;
  int nPos;     //���� ��ġ�� �����ϱ� ���� �ӽú���
  int nKey;     //���� �� ������ ���� key��
  int bFinished;    //����, ������ �Ͼ������ flag
  int bFound;   //B-Ʈ������ �ش� key�� �߰ߵǾ������� flag
  int bOverflow;    //�ش� node���� �����÷ΰ� �߻��ߴ����� flag
  int nCount;   //�ش� node�� key ��
  int nTemp, nTemp2, nTemp3;    //�ӽú���
  struct NODE* pRoot;   //root node
  struct NODE* pCurr;   //���� �۾� �������� node
  struct NODE* pChild;  //���� node�� child node  
  struct NODE* pNewNode;    //������ �� node
  struct NODE* pInkey;   //in-key�� pointer
  struct STACK_NODE* pStackTop; //stack�� top pointer
  struct OVERFLOW_NODE* pOverflow;  //�����÷��� ��츦 ó���� �ӽ� node
  int nInput[MAX_ELEMENTS]; //ȭ�� ������ ���̱� ���� �迭�� ����
  int nElementCnt;
  int menu, mi;
  int serchkey;
  int success = 0;
  //�ʱ�ȭ
  
  nKey = 0;
  bFinished = 0;
  bFound = 0;
  bOverflow = 0;
  nCount = 0;
  for (i=0; i<MAX_ELEMENTS; ++i) nInput[i] = 0;
  nElementCnt = 0;
  
  //���� b Ʈ�� ����
  pRoot = CreateNode();
  pRoot->bRoot = 1;
  pCurr = pRoot;
  
  //input.txt ȭ�� ����
  if ((pInputStream = fopen("input.txt", "r")) == NULL) {
        //����ó��
        printf("input.txt file cannot be opened\n");
        return -1;
    }
    
   //output.txt. ȭ�� ����
  if ((pOutputStream = fopen("output.txt", "w")) == NULL) {
        //����ó�� 
        printf("output.txt file cannot be opened\n");
        return -1;
    }
    
  
  //b-Ʈ���� ����
  printf("[B-Tree] Insertion Process started\n");
  pStackTop = NULL;
  while(1) {
        nKey = 0;
        if (fscanf( pInputStream, "%d", &nKey) == EOF) {
            break;
        }
        
        //key���� �迭�� ����
        nInput[nElementCnt] = nKey;
        nElementCnt++;
        pCurr = pRoot;
        
        //stack �ʱ�ȭ
        while (pStackTop && pStackTop->pCurr) {
            pStackTop = PopFromStack(pStackTop);
        }
        pStackTop = CreateStackNode();
        
        pChild = NULL;
        printf("Key (%d) inserted \n", nKey);
        fprintf(pOutputStream, "Key (%d) inserted \n", nKey);
        do {
            nCount = GetElementCount(pCurr);
            if (nCount == 0) {
                //root node�� �ʱ⿡ ket���� 0���� �� ����
                pChild = pCurr->pChild[0];
            } else if (CheckElementExist(pCurr, nKey) == 1) {
                bFound = 1;
            } else if (nKey < pCurr->nElm[0]) {
                pChild = pCurr->pChild[0];
            } else if (nKey > pCurr->nElm[ (nCount-1 > 0)? nCount-1 : 0]) {
                pChild = pCurr->pChild[nCount];
            } else {
                //���� node�� key�� �߰��� ��ġ�ؾ� ��
                for (i=0; i<nCount-1; ++i) {
                    if (nKey > pCurr->nElm[i] && nKey < pCurr->nElm[i+1]) {
                        pChild = pCurr->pChild[i+1];
                        break;
                    }
                }
            }
            if (pChild != NULL) {
                pStackTop = PushIntoStack(pStackTop, pCurr);
                pCurr = pChild;
            }
        }
        while (!bFound && pChild != NULL);
        if (bFound == 1) {
            printf("[Insertion] Key (%d) already exists in the B-Tree\n", nKey);
        } else {
            //Key�� B Ʈ���� ���� 
            //���� pInkey�� �����ϸ� In-key�� ���� ��� - ��, 1���� ���� ����
            pChild = NULL;
            pInkey = NULL;
            bFinished = 0;
            do {
                nCount = GetElementCount(pCurr);
                if (pInkey) nKey = pInkey->nElm[0];
                if (nCount < DEGREE-1) //������ �ƴ� ��� 
                {
                    //�ش���ġ�� �ٷ� ����
                    if (nCount == 0) {
                        nPos = 0;
                    } else if (nKey < pCurr->nElm[0]) {
                        nPos = 0;
                    } else if (nKey > pCurr->nElm[(nCount-1 > 0) ? nCount-1 : 0])
                    {
                        nPos = nCount;
                    } else {
                        for (i=0; i<nCount-1; ++i) {
                            if (nKey > pCurr->nElm[i] && nKey < pCurr->nElm[i+1]) {
                                nPos = i+1;
                                break;
                            }
                        }
                    }
                    
                    //���� key���� shift
                    for (i=DEGREE-2; i>nPos; --i) {
                        pCurr->nElm[i] = pCurr->nElm[i-1];
                        pCurr->pChild[i+1] = pCurr->pChild[i];
                    }
                    
                    //���� nkey�� ���ԵǴ� ���� ��������
                    pCurr->nElm[nPos] = nKey;
                    pCurr->pChild[nPos+1] = NULL;
                    if (pInkey) {
                        pCurr->pChild[nPos] = pInkey->pChild[0];
                        pCurr->pChild[nPos+1] = pInkey->pChild[1];
                    }
                    bFinished = 1;
                } else {
                    //OVERFLOW_NODE�� ����
                    pOverflow = CreateOverflowNode();
                    for (i=0; i<DEGREE-1; ++i) {
                        pOverflow->nElm[i] = pCurr->nElm[i];
                        pOverflow->pChild[i] = pCurr->pChild[i];
                    }
                    pOverflow->pChild[DEGREE-1] = pCurr->pChild[DEGREE-1];
                    //nkey ����
                    nCount = DEGREE-1;
                    if (nKey < pOverflow->nElm[0]) {
                        nPos = 0;
                    }
                    else if (nKey > pOverflow->nElm[(nCount-1>0)? nCount-1: 0]) {
                        nPos = nCount;
                    } else {
                        for (i=0; i<nCount-1; ++i) {
                            if (nKey>pOverflow->nElm[i] && nKey < pOverflow->nElm[i+1]) {
                                nPos = i + 1;
                                break;
                            }
                        }
                    }
                    
                    //���� key���� shift
                    for (i=DEGREE-1; i >= nPos; --i) {
                        pOverflow->nElm[i] = pOverflow->nElm[i-1];
                        pOverflow->pChild[i+1] = pOverflow->pChild[i];
                    }
                    
                    //����
                    pOverflow->nElm[nPos] = nKey;
                    pOverflow->pChild[nPos+1] = NULL;
                    if (pInkey) {
                        pOverflow->pChild[nPos] = pInkey->pChild[0];
                        pOverflow->pChild[nPos+1] = pInkey->pChild[1];
                    }
                    //�߰��� ���ϱ�
                    nPos = (DEGREE-1) / 2;
                    nKey = pOverflow->nElm[nPos];
                    //�����÷θ� pCurr�� �� ��忡 ����
                    //pCurr�� clear�� �� pOverflow�� 1st half�� ����
                    for (i=0; i<DEGREE-1; ++i) {
                        pCurr->nElm[i] = 0;
                        pCurr->pChild[i] = NULL;
                    }
                    pCurr->pChild[DEGREE-1] = NULL;
                    for (i=0; i<nPos; ++i) {
                        pCurr->nElm[i] = pOverflow->nElm[i];
                        pCurr->pChild[i] = pOverflow->pChild[i];
                    }
                    pCurr->pChild[nPos] = pOverflow->pChild[nPos];
                    //�� ��忡 2nd half�� ����
                    pNewNode = CreateNode();
                    for (i=nPos+1; i<DEGREE; ++i) {
                        pNewNode->nElm[i-nPos-1] = pOverflow->nElm[i];
                        pNewNode->pChild[i-nPos-1] = pOverflow->pChild[i];
                    }
                    pNewNode->pChild[DEGREE-nPos-1] = pOverflow->pChild[DEGREE];
                    //pInkey�� �����Ͽ� key�� child�� ����
                    pInkey = CreateNode();
                    pInkey->nElm[0] = nKey;
                    pInkey->pChild[0] = pCurr;
                    pInkey->pChild[1] = pNewNode;
                    //���� �ö󰡸� ���� �ݺ�
                    if (pStackTop && pStackTop->pCurr) {
                        //�θ� node�� �÷� ����
                        pCurr = PeepStackTop(pStackTop);
                        pStackTop = PopFromStack(pStackTop);
                    } else {
                        //Ʈ������ ����
                        //pInkey�� �� root�� ��
                        pCurr->bRoot = 0;
                        pRoot = pInkey;
                        pInkey->bRoot = 1;
                        bFinished = 1;
                    }
                }
            }
            while (bFinished == 0);
        }
        PrintNodeElement(pRoot);
        fprintf(pOutputStream, " \n");
    }
    printf("[B-Tree] Insertion Process Ended\n");
    fclose(pInputStream);
    
    printf("Please enter the menu(1-serch, 2-delete)\n");
    scanf("%d", &menu);
    
    if (menu == 1) {
        printf("Please enter serch key\n");
        scanf("%d", &serchkey);
        for(mi = 0; mi<nElementCnt; mi++) {
            if (nInput[mi] == serchkey) {
                printf("Success!\n");
                success = 1;
                break;
            }
        }
        if (success == 0) printf("Fail!\n");
    }
    
    else {
    //bƮ������ ����(�ʱ�ȭ)
    pCurr = NULL;
    pChild = NULL;
    pNewNode = NULL;
    pInkey = NULL;
    pStackTop = NULL;
    pOverflow = NULL;
    nKey = 0;
    bFinished = 0;
    bFound = 0;
    bOverflow = 0;
    nCount = 0;
    
    //���� ����
    printf("[B-Tree] Deletion Process started\n");
    for (j=(nElementCnt - DELETE_COUNT > 0? nElementCnt-DELETE_COUNT: 0); j<nElementCnt; ++j) {
        //key�� ��������
        nKey = nInput[j];
        printf("Key (%d) deleted \n", nKey);
        fprintf(pOutputStream, "Key (%d) deleted \n", nKey);
        bFound = 0;
        //Stack�� �����鼭 key�� ã��
        while (pStackTop && pStackTop->pCurr) {
            pStackTop = PopFromStack(pStackTop);
        }
        pStackTop = CreateStackNode();
        pCurr = pRoot;
        pChild = NULL;
        do {
            nCount = GetElementCount(pCurr);
            if (CheckElementExist(pCurr, nKey) == 1) {
                bFound = 1;
                pChild = pCurr;
            } else if (nKey < pCurr->nElm[0]) {
                pChild = pCurr->pChild[0];
            } else if (nKey > pCurr->nElm[ (nCount-1 > 0) ? nCount-1 : 0]) {
                pChild = pCurr->pChild[nCount];
            } else {
                //���� node�� key�� �߰��� ��ġ�ؾ� ��
                for (i=0; i<nCount-1; ++i) {
                    if (nKey > pCurr->nElm[i] && nKey < pCurr->nElm[i+1]) {
                        pChild = pCurr->pChild[i+1];
                        break;
                    }
                }
            }
            if (pChild != NULL) {
                pStackTop = PushIntoStack(pStackTop, pCurr);
                pCurr = pChild;
            }
        }
        while (!bFound);
        if (bFound != 1) {
            printf("[Deletion] Key (%d) does not exist in the B-Tree\n", nKey);
        } else {
            //������ ��忡 ���� ���� ���
            //�̰��, pCurr�� NULL�� �ƴϰ�,
            //�ּ��� pCurr�� leftmost child�� null�� �ƴ�
            if (pCurr && pCurr->pChild[0]) {
                pChild = NULL;
                nPos = 0;
                for (i=0; i<DEGREE-1; ++i) {
                    if (nKey == pCurr->nElm[i]) {
                        //pCurr�� ���� ��尡 �ƴϹǷ�
                        //�����ڴ� pCurr���� Key���� ���� child�� �� ���� �ڼ��� �� ���� key���� ��
                        nPos = i;
                        pChild = pCurr->pChild[i];
                        break;
                    }
                }
                if (pChild) pStackTop = PushIntoStack(pStackTop, pChild);
                //��δ� stack�� ����
                //��δ� pCurr���� �����ϹǷ�, pStackTop�� ÷��
                while (pChild) {
                    bFound = 0;     //�������� pos
                    //pChild���� ���� ū key�� ã��, �ش� ����� ���� child ���� �˻�
                    for (i=DEGREE-2; i>=0; --i) {
                        if (pChild->nElm[i] > 0) {
                            bFound = i;
                            break;
                        }
                    }
                    if (pChild->pChild[bFound+1]) {
                        pStackTop = PushIntoStack(pStackTop, pChild);
                        pChild = pChild->pChild[bFound+1];
                    } else { //�������
                        //key�� swap�ϰ� loop Ż��
                        nTemp3 = pChild->nElm[bFound];
                        pChild->nElm[bFound] = pCurr->nElm[nPos];
                        pCurr->nElm[nPos] = nTemp3;
                        pCurr = pChild;
                        break;
                    }
                }
            }
            
            //key�� �����ϰ� shift
            nPos = 0;
            for (i=0; i<DEGREE-1; ++i) {
                if (pCurr->nElm[i] == nKey) {
                    nPos = i;
                    break;
                }
            }
            
            if (nPos == DEGREE-2) {
                pCurr->nElm[nPos] = 0;
            } else {
                for (i=nPos+1; i<DEGREE-1; ++i) {
                    pCurr->nElm[i-1] = pCurr->nElm[i];
                }
                pCurr->nElm[DEGREE-2] = 0;
            }
            
            //Ʈ�� ������
            bFinished = 0;
            do {
                if (pCurr->bRoot == 1 || GetElementCount(pCurr) >= ((DEGREE+1)/2-1) ) {
                    //root node�̰ų� ����÷ΰ� �߻����� ������
                    bFinished = 1;
                } else {
                    //��й� ���� ���� �Ǻ�
                    bFound = 0;
                    nTemp = 0;  //0: impossible, 1: left sibling, 2: right sibling
                    pChild = NULL;
                    //root�� �ƴϹǷ� parent����
                    pNewNode = PeepStackTop(pStackTop);
                    if (pCurr == pNewNode) {
                        pStackTop = PopFromStack(pStackTop);
                        pNewNode = PeepStackTop(pStackTop);
                    }
                    if (pNewNode == NULL) printf("Error wrong Situation\n");
                    nPos = -1;
                    for (i=0; i<= GetElementCount(pNewNode); ++i) {
                        if (pCurr == pNewNode->pChild[i]) {
                            nPos = i;
                            break;
                        }
                    }
                    if (nPos == -1) printf("Error, the Node doesn`t match\n");
                    //sibling�� ã�Ƽ� ��й� ���� ���� ����
                    if (nPos == 0) { //�� ��쿡�� ������ ���� sibling����
                        if (GetElementCount(pNewNode->pChild[nPos+1]) > ((DEGREE+1)/2-1) ) {
                            bFound = 1;
                            nTemp = 2;
                        }
                    } else if (nPos == DEGREE-1) {
                        //�� ��쿡�� ������ ���� sibling ����
                        if (GetElementCount(pNewNode->pChild[nPos-1]) > ((DEGREE+1)/2-1) ) {
                            bFound = 1;
                            nTemp = 1;
                        }
                    } else { //left sibling ����
                        if (GetElementCount(pNewNode->pChild[nPos-1]) > ((DEGREE+1)/2-1) ) {
                            bFound = 1;
                            nTemp = 1;
                        } else if (GetElementCount(pNewNode->pChild[nPos+1]) > ((DEGREE+1)/2-1) ) {
                            bFound = 1;
                            nTemp = 2;
                        }
                    }
                    if (bFound) { //��й� ���
                        //TWOBNODE ������� ����
                        if (nTemp == 1) {
                            //���� sibling, parent�� ���� ������ ������
                            //key��, child�� �̵�
                            pChild = pNewNode->pChild[nPos-1];
                            nTemp3 = GetElementCount(pChild);
                            nTemp2 = (nTemp3 - GetElementCount(pCurr) ) / 2;
                            //������ ���߱� ���� left sibling���� nTemp ����ŭ pCurr�� �й�
                            //pCurr���� nTemp2 ��ŭ �������� shift
                            for (i=DEGREE-2; i>=nTemp2; --i) {
                                pCurr->nElm[i] = pCurr->nElm[i-nTemp2];
                                pCurr->pChild[i+1] = pCurr->pChild[i+1-nTemp2];
                            }
                            pCurr->pChild[nTemp2] = pCurr->pChild[0];
                            //�θ� key�� ���� ������
                            pCurr->nElm[nTemp2-1] = pNewNode->nElm[nPos-1];
                            //left sibling���� nTemp2-1��ŭ pCurr�� �̵�
                            for (i=0; i<nTemp2-1; ++i) {
                                pCurr->nElm[nTemp2-i-2] = pChild->nElm[nTemp3 -i -1];
                                pCurr->pChild[nTemp2-i-1] = pChild->pChild[nTemp3-i];
                            }
                            pCurr->pChild[0] = pChild->pChild[nTemp3-nTemp2+1];
                            //left sibling���� �θ�� key�÷� �ְ� ����
                            pNewNode->nElm[nPos-1] = pChild->nElm[nTemp3 - nTemp2];
                            for (i=nTemp3-1; i>nTemp3-nTemp2-1; --i) {
                                pChild->nElm[i] = 0;
                                pChild->pChild[i+1] = NULL;
                            }
                        } else {
                            //���� sibling, parent�� ���� ������ ������
                            //key�� child�� �̵�
                            pChild = pNewNode->pChild[nPos+1];
                            nTemp3 = GetElementCount(pChild);
                            nTemp2 = (nTemp3 - GetElementCount(pCurr) )/2;
                            //������ ���߱� ���� right sibling���� nTemp ����ŭ pCurr�� �й�
                            //    �θ� key�� ���� ������
                            pCurr->nElm[GetElementCount(pCurr)] = pNewNode->nElm[nPos];
                            //right sibling���� nTemp2-1��ŭ pCurr�� �̵�
                            for (i=0; i<nTemp2-1; ++i) {
                                pCurr->nElm[GetElementCount(pCurr)+i+1] = pChild->nElm[i];
                                pCurr->pChild[GetElementCount(pCurr)+i+1] = pChild->pChild[i];
                            }
                            pCurr->pChild[GetElementCount(pCurr)+nTemp2-1] = pChild->pChild[nTemp2-1];
                            //right sibling���� �θ�� key �÷� �ְ� left shift
                            pNewNode->nElm[nPos] = pChild->nElm[nTemp2-1];
                            for (i=0; i<DEGREE-1-nTemp2; ++i) {
                                pChild->nElm[i] = pChild->nElm[i+nTemp2];
                                pChild->pChild[i] = pChild->pChild[i+nTemp2];
                            }
                            pChild->pChild[DEGREE-1-nTemp2] = pChild->pChild[DEGREE-1];
                            for (i=nTemp3; i>nTemp3-nTemp2; --i) {
                                pChild->nElm[i-1] = 0;
                                pChild->pChild[i] = NULL;
                            }
                        }
                        bFinished = 1;
                    } else {
                        //cancatenation
                        nTemp = (nPos == 0)? 2: 1;
                        if (nTemp == 1) { 
                            //left sibling�� �պ�
                            pChild = pNewNode->pChild[nPos-1];
                            nTemp3 = GetElementCount(pChild);
                            nTemp2 = GetElementCount(pCurr);
                            //parent �պ� 
                            pChild->nElm[nTemp3] = pNewNode->nElm[nPos-1];
                            pChild->pChild[nTemp3+1] = pCurr->pChild[0];
                            //pCurr�� key�� child�� left sibling�� �պ� 
                            for (i=0; i<nTemp2; ++i) {
                                pChild->nElm[i+nTemp3+1] = pCurr->nElm[i];
                                pChild->pChild[i+nTemp3+2] = pCurr->pChild[i+1];
                            }
                            //parent���� key, child ����
                            for (i=nPos-1; i<DEGREE-1; ++i) {
                                pNewNode->nElm[i] = pNewNode->nElm[i+1];
                                pNewNode->pChild[i+1] = pNewNode->pChild[i+2];
                            }
                            pNewNode->nElm[DEGREE-2] = 0;
                            pNewNode->pChild[DEGREE-1] = NULL;
                            pCurr = pNewNode;
                        } else { //right sibling�� �պ�
                            pChild = pNewNode->pChild[nPos+1];
                            nTemp3 = GetElementCount(pChild);
                            nTemp2 = GetElementCount(pCurr);
                            //parent�պ�
                            pCurr->nElm[nTemp2] = pNewNode->nElm[nPos];
                            pCurr->pChild[nTemp2+1] = pChild->pChild[0];
                            //right sibling�� key�� child�� pCurr�� �պ�
                            for (i=0; i<nTemp3; ++i) {
                                pCurr->nElm[i+nTemp2+1] = pChild->nElm[i];
                                pCurr->pChild[i+nTemp2+2] = pChild->pChild[i+1];
                            }
                            //parent���� key, child ����
                            for (i=nPos; i<DEGREE-1; ++i) {
                                pNewNode->nElm[i] = pNewNode->nElm[i+1];
                                pNewNode->pChild[i+1] = pNewNode->pChild[i+2];
                            }
                            pNewNode->nElm[DEGREE-2] = 0;
                            pNewNode->pChild[DEGREE-1] = NULL;
                            pCurr = pNewNode;
                        }
                    }
                }
            }
            while (!bFinished);
            
            //root�� key�� ������ Ʈ�� ������ ����
            nTemp2 = GetElementCount(pRoot);
            if (nTemp2 == 0) {
                if (pRoot->pChild[0]) {
                    //�����ΰ��
                }
                if (pRoot->pChild[1]) {
                    //�������� ���
                    printf("wrong situation\n");
                }
                if (pRoot->pChild[0]) {
                    pRoot = pRoot->pChild[0];
                    pRoot->bRoot = 1;
                }
            }
        }
        if (GetElementCount(pRoot) ) {
            PrintNodeElement(pRoot);
        } else {
            printf("There`s no keys in the B-Tree\n");
            fprintf(pOutputStream, "There`s no keys in the B-Tree");
        }
        fprintf(pOutputStream, " \n");
    }
    fclose(pOutputStream);
    printf("[B-Tree] Deletion Process Ended\n");
}      
    system("PAUSE");	
    return 0;
}
