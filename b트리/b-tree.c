#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#define DEGREE 5    //B-트리의 차수 
#define MAX_ELEMENTS 1000   //입력되는 KEY의 최대 수
#define DELETE_COUNT 10     //삭제시 빼줄 원소 수

//전역변수
FILE* pInputStream;  //input 화일의 핸들
FILE* pOutputStream; //output 화일의 핸들 

struct NODE { //B트리 노드 구조 
    int bRoot;
    int nElm[DEGREE-1];
    void* pChild[DEGREE];
};

struct OVERFLOW_NODE { //B트리의 오버플로 노드 구조
    int bRoot;
    int nElm[DEGREE];
    void* pChild[DEGREE+1];
};

struct STACK_NODE { //stack의 노드구조
    struct NODE* pCurr;
    void* pPrev;
};

//----------------------구조 끝 함수 생성------------------------
 
struct NODE* CreateNode() { //노드생성 - 함수 
    struct NODE* pNode;
    pNode = (struct NODE*)malloc(sizeof(struct NODE));
    memset(pNode, 0, sizeof(struct NODE));
    return pNode;
}

struct OVERFLOW_NODE* CreateOverflowNode() { //오버플로우 노드 생성 - 함수
    struct OVERFLOW_NODE* pNode;
    pNode = (struct OVERFLOW_NODE*)malloc(sizeof(struct OVERFLOW_NODE));
    memset(pNode, 0, sizeof(struct OVERFLOW_NODE));
    return pNode;
}

struct STACK_NODE* CreateStackNode() { //스텍노드 생성 - 함수
    struct STACK_NODE* pStackNode;
    pStackNode = (struct STACK_NODE*)malloc(sizeof(struct STACK_NODE));
    memset(pStackNode, 0, sizeof(struct STACK_NODE));
    return pStackNode;
}

struct  STACK_NODE* PushIntoStack (struct STACK_NODE* pStackTop, struct NODE* pNode) { 
    //스택에 push
    struct STACK_NODE* pNewTop;
    if (pStackTop == NULL||pNode == NULL) {
        return NULL;
    }
    
    if (pStackTop->pCurr == NULL) {
        //top이 비어있는 경우
        pStackTop->pCurr = pNode;
        pStackTop->pPrev = NULL;
        return pStackTop;
    }
    else { //새 top을 생성
        pNewTop = CreateStackNode();
        pNewTop->pPrev = (void*) pStackTop;
        pNewTop->pCurr = pNode;
        return pNewTop;
    }
}

struct STACK_NODE* PopFromStack(struct STACK_NODE* pStackTop) {
    //스택에서 pop
    struct STACK_NODE* pPrevTop;
    struct NODE* pNode;
    if (pStackTop == NULL) return NULL;
    if (pStackTop->pCurr == NULL) return NULL;
    else if (pStackTop->pPrev == NULL) {
        //현재 top
        pNode = pStackTop->pCurr;
        pStackTop->pCurr = NULL;
        return pStackTop;
    }
    else {
        //pCurr, pPrev 모두 존재
        pNode = pStackTop->pCurr;
        pPrevTop = pStackTop;
        pStackTop = (struct STACK_NODE*) pStackTop->pPrev;
        //이전 top 삭제
        free((void*)pPrevTop);
        return pStackTop;
    }
}

struct NODE* PeepStackTop (struct STACK_NODE* pStackTop) {
    //stack의 top을 peep
    if (pStackTop == NULL) return NULL;
    return pStackTop->pCurr;
}

int CheckElementExist(struct NODE* pNode, int key) {
    //노드에 key가 있는지 검사
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
    //노드의 원소 수 반환
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
    //노드의 key값 출력
    int i;
    struct NODE* pChild;
    //post-order로 출력
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
    //leftmost child 출력: node 내의 key와 다른 child 출력
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
  int nPos;     //삽입 위치를 저장하기 위한 임시변수
  int nKey;     //삽입 및 삭제를 해줄 key값
  int bFinished;    //삽입, 삭제가 일어났는지의 flag
  int bFound;   //B-트리에서 해당 key가 발견되었는지의 flag
  int bOverflow;    //해당 node에서 오버플로가 발생했는지의 flag
  int nCount;   //해당 node의 key 수
  int nTemp, nTemp2, nTemp3;    //임시변수
  struct NODE* pRoot;   //root node
  struct NODE* pCurr;   //현재 작업 수행중인 node
  struct NODE* pChild;  //현재 node의 child node  
  struct NODE* pNewNode;    //생성할 새 node
  struct NODE* pInkey;   //in-key의 pointer
  struct STACK_NODE* pStackTop; //stack의 top pointer
  struct OVERFLOW_NODE* pOverflow;  //오버플로의 경우를 처리할 임시 node
  int nInput[MAX_ELEMENTS]; //화일 접근을 줄이기 위해 배열에 저장
  int nElementCnt;
  int menu, mi;
  int serchkey;
  int success = 0;
  //초기화
  
  nKey = 0;
  bFinished = 0;
  bFound = 0;
  bOverflow = 0;
  nCount = 0;
  for (i=0; i<MAX_ELEMENTS; ++i) nInput[i] = 0;
  nElementCnt = 0;
  
  //공백 b 트리 생성
  pRoot = CreateNode();
  pRoot->bRoot = 1;
  pCurr = pRoot;
  
  //input.txt 화일 열기
  if ((pInputStream = fopen("input.txt", "r")) == NULL) {
        //오류처리
        printf("input.txt file cannot be opened\n");
        return -1;
    }
    
   //output.txt. 화일 열기
  if ((pOutputStream = fopen("output.txt", "w")) == NULL) {
        //오류처리 
        printf("output.txt file cannot be opened\n");
        return -1;
    }
    
  
  //b-트리로 삽입
  printf("[B-Tree] Insertion Process started\n");
  pStackTop = NULL;
  while(1) {
        nKey = 0;
        if (fscanf( pInputStream, "%d", &nKey) == EOF) {
            break;
        }
        
        //key값을 배열에 저장
        nInput[nElementCnt] = nKey;
        nElementCnt++;
        pCurr = pRoot;
        
        //stack 초기화
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
                //root node는 초기에 ket값이 0개일 수 있음
                pChild = pCurr->pChild[0];
            } else if (CheckElementExist(pCurr, nKey) == 1) {
                bFound = 1;
            } else if (nKey < pCurr->nElm[0]) {
                pChild = pCurr->pChild[0];
            } else if (nKey > pCurr->nElm[ (nCount-1 > 0)? nCount-1 : 0]) {
                pChild = pCurr->pChild[nCount];
            } else {
                //현재 node의 key의 중간에 위치해야 함
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
            //Key를 B 트리에 삽입 
            //만약 pInkey가 존재하면 In-key와 같이 취급 - 즉, 1개의 원소 소유
            pChild = NULL;
            pInkey = NULL;
            bFinished = 0;
            do {
                nCount = GetElementCount(pCurr);
                if (pInkey) nKey = pInkey->nElm[0];
                if (nCount < DEGREE-1) //만원이 아닌 경우 
                {
                    //해당위치에 바로 삽입
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
                    
                    //기존 key들을 shift
                    for (i=DEGREE-2; i>nPos; --i) {
                        pCurr->nElm[i] = pCurr->nElm[i-1];
                        pCurr->pChild[i+1] = pCurr->pChild[i];
                    }
                    
                    //삽입 nkey가 삽입되는 경우는 리프뿐임
                    pCurr->nElm[nPos] = nKey;
                    pCurr->pChild[nPos+1] = NULL;
                    if (pInkey) {
                        pCurr->pChild[nPos] = pInkey->pChild[0];
                        pCurr->pChild[nPos+1] = pInkey->pChild[1];
                    }
                    bFinished = 1;
                } else {
                    //OVERFLOW_NODE에 복사
                    pOverflow = CreateOverflowNode();
                    for (i=0; i<DEGREE-1; ++i) {
                        pOverflow->nElm[i] = pCurr->nElm[i];
                        pOverflow->pChild[i] = pCurr->pChild[i];
                    }
                    pOverflow->pChild[DEGREE-1] = pCurr->pChild[DEGREE-1];
                    //nkey 삽입
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
                    
                    //기존 key들을 shift
                    for (i=DEGREE-1; i >= nPos; --i) {
                        pOverflow->nElm[i] = pOverflow->nElm[i-1];
                        pOverflow->pChild[i+1] = pOverflow->pChild[i];
                    }
                    
                    //삽입
                    pOverflow->nElm[nPos] = nKey;
                    pOverflow->pChild[nPos+1] = NULL;
                    if (pInkey) {
                        pOverflow->pChild[nPos] = pInkey->pChild[0];
                        pOverflow->pChild[nPos+1] = pInkey->pChild[1];
                    }
                    //중간값 구하기
                    nPos = (DEGREE-1) / 2;
                    nKey = pOverflow->nElm[nPos];
                    //오버플로를 pCurr과 새 노드에 분할
                    //pCurr를 clear한 후 pOverflow의 1st half를 복사
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
                    //새 노드에 2nd half를 복사
                    pNewNode = CreateNode();
                    for (i=nPos+1; i<DEGREE; ++i) {
                        pNewNode->nElm[i-nPos-1] = pOverflow->nElm[i];
                        pNewNode->pChild[i-nPos-1] = pOverflow->pChild[i];
                    }
                    pNewNode->pChild[DEGREE-nPos-1] = pOverflow->pChild[DEGREE];
                    //pInkey를 생성하여 key와 child를 삽입
                    pInkey = CreateNode();
                    pInkey->nElm[0] = nKey;
                    pInkey->pChild[0] = pCurr;
                    pInkey->pChild[1] = pNewNode;
                    //위로 올라가며 삽입 반복
                    if (pStackTop && pStackTop->pCurr) {
                        //부모 node로 올려 보냄
                        pCurr = PeepStackTop(pStackTop);
                        pStackTop = PopFromStack(pStackTop);
                    } else {
                        //트리레벨 증가
                        //pInkey가 새 root가 됨
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
    //b트리에서 삭제(초기화)
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
    
    //삭제 시작
    printf("[B-Tree] Deletion Process started\n");
    for (j=(nElementCnt - DELETE_COUNT > 0? nElementCnt-DELETE_COUNT: 0); j<nElementCnt; ++j) {
        //key값 가져오기
        nKey = nInput[j];
        printf("Key (%d) deleted \n", nKey);
        fprintf(pOutputStream, "Key (%d) deleted \n", nKey);
        bFound = 0;
        //Stack에 쌓으면서 key값 찾기
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
                //현재 node의 key의 중간에 위치해야 함
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
            //리프가 노드에 있지 않은 경우
            //이경우, pCurr는 NULL이 아니고,
            //최소한 pCurr의 leftmost child도 null이 아님
            if (pCurr && pCurr->pChild[0]) {
                pChild = NULL;
                nPos = 0;
                for (i=0; i<DEGREE-1; ++i) {
                    if (nKey == pCurr->nElm[i]) {
                        //pCurr이 리프 노드가 아니므로
                        //선행자는 pCurr내의 Key값의 좌측 child의 맨 우측 자손의 맨 우측 key값이 됨
                        nPos = i;
                        pChild = pCurr->pChild[i];
                        break;
                    }
                }
                if (pChild) pStackTop = PushIntoStack(pStackTop, pChild);
                //경로는 stack에 저장
                //경로는 pCurr까지 동일하므로, pStackTop에 첨가
                while (pChild) {
                    bFound = 0;     //선행자의 pos
                    //pChild에서 가장 큰 key를 찾고, 해당 노드의 우측 child 따라 검색
                    for (i=DEGREE-2; i>=0; --i) {
                        if (pChild->nElm[i] > 0) {
                            bFound = i;
                            break;
                        }
                    }
                    if (pChild->pChild[bFound+1]) {
                        pStackTop = PushIntoStack(pStackTop, pChild);
                        pChild = pChild->pChild[bFound+1];
                    } else { //리프노드
                        //key를 swap하고 loop 탈출
                        nTemp3 = pChild->nElm[bFound];
                        pChild->nElm[bFound] = pCurr->nElm[nPos];
                        pCurr->nElm[nPos] = nTemp3;
                        pCurr = pChild;
                        break;
                    }
                }
            }
            
            //key를 삭제하고 shift
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
            
            //트리 재조정
            bFinished = 0;
            do {
                if (pCurr->bRoot == 1 || GetElementCount(pCurr) >= ((DEGREE+1)/2-1) ) {
                    //root node이거나 언더플로가 발생하지 않으면
                    bFinished = 1;
                } else {
                    //재분배 가능 여부 판별
                    bFound = 0;
                    nTemp = 0;  //0: impossible, 1: left sibling, 2: right sibling
                    pChild = NULL;
                    //root가 아니므로 parent존재
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
                    //sibling을 찾아서 재분배 가능 여부 결정
                    if (nPos == 0) { //이 경우에는 무조건 우측 sibling결정
                        if (GetElementCount(pNewNode->pChild[nPos+1]) > ((DEGREE+1)/2-1) ) {
                            bFound = 1;
                            nTemp = 2;
                        }
                    } else if (nPos == DEGREE-1) {
                        //이 경우에는 무조건 좌측 sibling 결정
                        if (GetElementCount(pNewNode->pChild[nPos-1]) > ((DEGREE+1)/2-1) ) {
                            bFound = 1;
                            nTemp = 1;
                        }
                    } else { //left sibling 결정
                        if (GetElementCount(pNewNode->pChild[nPos-1]) > ((DEGREE+1)/2-1) ) {
                            bFound = 1;
                            nTemp = 1;
                        } else if (GetElementCount(pNewNode->pChild[nPos+1]) > ((DEGREE+1)/2-1) ) {
                            bFound = 1;
                            nTemp = 2;
                        }
                    }
                    if (bFound) { //재분배 기능
                        //TWOBNODE 사용하지 않음
                        if (nTemp == 1) {
                            //좌측 sibling, parent와 합쳐 반으로 나누기
                            //key값, child만 이동
                            pChild = pNewNode->pChild[nPos-1];
                            nTemp3 = GetElementCount(pChild);
                            nTemp2 = (nTemp3 - GetElementCount(pCurr) ) / 2;
                            //균형을 맞추기 위해 left sibling에서 nTemp 수만큼 pCurr에 분배
                            //pCurr에서 nTemp2 만큼 우측으로 shift
                            for (i=DEGREE-2; i>=nTemp2; --i) {
                                pCurr->nElm[i] = pCurr->nElm[i-nTemp2];
                                pCurr->pChild[i+1] = pCurr->pChild[i+1-nTemp2];
                            }
                            pCurr->pChild[nTemp2] = pCurr->pChild[0];
                            //부모 key값 내려 보내기
                            pCurr->nElm[nTemp2-1] = pNewNode->nElm[nPos-1];
                            //left sibling에서 nTemp2-1만큼 pCurr로 이동
                            for (i=0; i<nTemp2-1; ++i) {
                                pCurr->nElm[nTemp2-i-2] = pChild->nElm[nTemp3 -i -1];
                                pCurr->pChild[nTemp2-i-1] = pChild->pChild[nTemp3-i];
                            }
                            pCurr->pChild[0] = pChild->pChild[nTemp3-nTemp2+1];
                            //left sibling에서 부모로 key올려 주고 제거
                            pNewNode->nElm[nPos-1] = pChild->nElm[nTemp3 - nTemp2];
                            for (i=nTemp3-1; i>nTemp3-nTemp2-1; --i) {
                                pChild->nElm[i] = 0;
                                pChild->pChild[i+1] = NULL;
                            }
                        } else {
                            //우측 sibling, parent와 합쳐 반으로 나누기
                            //key값 child만 이동
                            pChild = pNewNode->pChild[nPos+1];
                            nTemp3 = GetElementCount(pChild);
                            nTemp2 = (nTemp3 - GetElementCount(pCurr) )/2;
                            //균형을 맞추기 위해 right sibling에서 nTemp 수만큼 pCurr에 분배
                            //    부모 key값 내려 보내기
                            pCurr->nElm[GetElementCount(pCurr)] = pNewNode->nElm[nPos];
                            //right sibling에서 nTemp2-1만큼 pCurr로 이동
                            for (i=0; i<nTemp2-1; ++i) {
                                pCurr->nElm[GetElementCount(pCurr)+i+1] = pChild->nElm[i];
                                pCurr->pChild[GetElementCount(pCurr)+i+1] = pChild->pChild[i];
                            }
                            pCurr->pChild[GetElementCount(pCurr)+nTemp2-1] = pChild->pChild[nTemp2-1];
                            //right sibling에서 부모로 key 올려 주고 left shift
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
                            //left sibling과 합병
                            pChild = pNewNode->pChild[nPos-1];
                            nTemp3 = GetElementCount(pChild);
                            nTemp2 = GetElementCount(pCurr);
                            //parent 합병 
                            pChild->nElm[nTemp3] = pNewNode->nElm[nPos-1];
                            pChild->pChild[nTemp3+1] = pCurr->pChild[0];
                            //pCurr의 key와 child를 left sibling에 합병 
                            for (i=0; i<nTemp2; ++i) {
                                pChild->nElm[i+nTemp3+1] = pCurr->nElm[i];
                                pChild->pChild[i+nTemp3+2] = pCurr->pChild[i+1];
                            }
                            //parent에서 key, child 삭제
                            for (i=nPos-1; i<DEGREE-1; ++i) {
                                pNewNode->nElm[i] = pNewNode->nElm[i+1];
                                pNewNode->pChild[i+1] = pNewNode->pChild[i+2];
                            }
                            pNewNode->nElm[DEGREE-2] = 0;
                            pNewNode->pChild[DEGREE-1] = NULL;
                            pCurr = pNewNode;
                        } else { //right sibling과 합병
                            pChild = pNewNode->pChild[nPos+1];
                            nTemp3 = GetElementCount(pChild);
                            nTemp2 = GetElementCount(pCurr);
                            //parent합병
                            pCurr->nElm[nTemp2] = pNewNode->nElm[nPos];
                            pCurr->pChild[nTemp2+1] = pChild->pChild[0];
                            //right sibling의 key와 child를 pCurr에 합병
                            for (i=0; i<nTemp3; ++i) {
                                pCurr->nElm[i+nTemp2+1] = pChild->nElm[i];
                                pCurr->pChild[i+nTemp2+2] = pChild->pChild[i+1];
                            }
                            //parent에서 key, child 삭제
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
            
            //root에 key가 없으면 트리 레벨을 감소
            nTemp2 = GetElementCount(pRoot);
            if (nTemp2 == 0) {
                if (pRoot->pChild[0]) {
                    //정상인경우
                }
                if (pRoot->pChild[1]) {
                    //비정상인 경우
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
