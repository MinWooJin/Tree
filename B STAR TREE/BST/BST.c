#include<stdio.h> 
#include<stdlib.h>
typedef struct TreeNode 
{
        int key; 
        struct TreeNode* left; 
        struct TreeNode* right; 
} treeNode; 
void deleteNode(treeNode** root, int x);
treeNode* insertKey(treeNode* p, int x) 
{ 
        treeNode* newNode;
        if (p == NULL)
        { 
                newNode = (treeNode*)malloc(sizeof(treeNode)); 
                newNode->key = x; 
                newNode->left = NULL; 
                newNode->right = NULL; 
                return newNode; 
        } 
        else if (x < p->key) 
        {
                p->left = insertKey(p->left, x);
        return p;
        }
        else if (x > p->key) 
        { 
                p->right = insertKey(p->right, x); 
                return p;
        } 
        else 
        { 
                printf("\n�ߺ��� Ű���� ����\n"); 
                return p;
        } 
} 
void insert(treeNode** root, int x) 
{ 
        *root = insertKey(*root, x); 
} 
treeNode* lmaxNode(treeNode* root) 
{
        treeNode* p; 
        p = root;
        while (p->right != NULL)
        {
                p = p->right;
        }
        return p;
}
treeNode* rmaxNode(treeNode* root) 
{
        treeNode* p; 
        p = root;
        while (p->left != NULL)
        {
                p = p->left;
        }
        return p;
}
treeNode* find(treeNode* root, treeNode** parent, int x)
{    
        treeNode* p; 
        p = root; 
        *parent = root;
        while (p != NULL)
        { 
                if (x < p->key) 
                { 
                        *parent = p;
                        p = p->left; 
                }
                else if (x == p->key) 
                {
                        return p; 
                }
                else 
                {
                        *parent = p;
                        p = p->right; 
                }
        } 
        return p; 
}
void printNode(treeNode* p) 
{        
        if (p != NULL) 
        { 
                printf("("); 
                printNode(p->left);  
                printf("%d", p->key); 
                printNode(p->right);  
                printf(")"); 
        } 
} 
void printTree(treeNode* root) 
{  
        printNode(root); 
        printf("\n");
} 
treeNode* deleteKey(treeNode* root, int x)
{
        treeNode* p = NULL; 
        treeNode* q = NULL;
        treeNode* parent = NULL;
        p = find(root, &parent, x);
        if(p == NULL) 
        {
                printf("\n�Է��� ���� �������� �ʽ��ϴ�\n");
                return root;
        }
    if(p->left == NULL && p->right == NULL)
        {
                if (parent == p) 
                { 
                        p = NULL;
                        return p;
                }
                if (parent->left == p) parent->left = NULL;
                else parent->right = NULL;
        }        
        else if(p->left == NULL || p->right == NULL) 
        {
                if (parent == p) 
                { 
                        if(p->left != NULL) 
                        {
                                q = lmaxNode(p->left);
                                p->key = q->key;
                                deleteNode(&p->left, p->key);
                        }
            else 
                        {
                                q = rmaxNode(p->right);
                                p->key = q->key;
                                deleteNode(&p->right, p->key);
                        }
                }
                else 
                {
                        if(p->left != NULL) 
                        {
                                if(parent->left == p) parent->left = p->left;
                                else parent->right = p->left;
                        }
                        else 
                        {
                                if(parent->left == p) parent->left = p->right;
                                else parent->right = p->right;
                        }
                }
        }
        else if(p->left != NULL && p->right != NULL) 
        {
                q = lmaxNode(p->left);
                p->key = q->key;
                deleteNode(&p->left, p->key);
        }
        return root;
}
void deleteNode(treeNode** root, int x) 
{ 
        *root = deleteKey(*root, x); 
}
int main() 
{ 
        treeNode* root = NULL; 
    treeNode* N = NULL; 
    treeNode* P = NULL; 
    treeNode* parent = NULL; 
    int choice = 0;
    int key = 0;
        for(;;)
        {
                printf("�̿� Ž�� Ʈ�� ����\n\n");
            printf("1. ����\n");
                printf("2. ����\n");
                printf("3. Ž��\n");
                printf("9. ����\n\n");
                printf("���� : ");
                scanf("%d", &choice);
                switch(choice)
                {
                        case 1:
                                printf("\n������ ���Ұ� : ");
                scanf("%d", &key);
                insert(&root, key);
                printf("\n���� ��ȸ ��� : "); 
                printTree(root); 
                printf("\n"); 
                                break;
                        case 2:
                                printf("\n������ ���Ұ� : ");
                scanf("%d", &key);
                deleteNode(&root, key);
                printf("\n���� ��ȸ ��� : "); 
                printTree(root); 
                printf("\n"); 
                                break;
                        case 3:
                                printf("\nŽ���� ���Ұ� : ");
                scanf("%d", &key);
                N = find(root, &parent, key); 
                if (N != NULL)
                                { 
                                        printf("\n���� %d Ž����\n", N->key); 
                } 
                else 
                                { 
                                        printf("\n���� %d Ž������ ����\n", key);
                } 
                printf("\n"); 
                                break;
                      case 9:
                                exit(1);
                                break;
                        
                        default:
                                printf("\n�߸� �Է��ϼ̽��ϴ�.\n\n");
                                break;
                }
        }
        return 0;                
}