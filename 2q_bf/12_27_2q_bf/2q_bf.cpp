#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<stddef.h>
#include "bf.h"
#define Am_MAX 8856
#define A1in_MAX 2530
#define Am_h_max 8000
#define A1in_h_max 2000
using namespace bf;
using namespace std;
typedef struct node {
    unsigned long long blkno; //8
    struct node* next; //4
    struct node* prev; //4

    struct node* h_next; //4
    struct node* h_prev; //4
}node;
//40byte
node Am_head;
node Am_h_head[Am_MAX];
node A1in_head;
node A1in_h_head[A1in_MAX];

node s_am;
node s_a1in;

bf::counting_bloom_filter cb(make_hasher(3), 20971520, 2);
bf::stable_bloom_filter sbf(make_hasher(3), 20971520, 2,10000);
bf::a2_bloom_filter abf(3, 41943040, 200000);
//bf::a2_bloom_filter abf(2, 3000, 10);
/*
node *Am_head node* A1out_head node* A1in_head �� �����ؼ�
ref�ʹݿ� �Լ��� ���ϴ� �κ��� ����� ������ �Ǿ��ٰ� �ȵǾ��ٰ� �ϸ鼭 ������ �߻���
*/

int ref_num = 9000;


unsigned long long Am_hit = 0;
unsigned long long A1in_hit = 0;
unsigned long long miss = 0;

int am_c = 0;
int a1in_c = 0;


unsigned long long curtime = 0;
unsigned long long blkno = 0;

void init_head(node* head);// ��� ������ �ʱ�ȭ
int find_check(node* head, unsigned long long blkno); //blkno�� ����Ʈ�� �����ϴ� �˻��ϴ� �Լ�
void insert_node(node* head, unsigned long long blkno); // 
void del_node(node* node);// �ش� ��� ����
node* find(node* head, unsigned long long blkno);// �ش� blkno ��� �˻��ϴ� �Լ�
int count(node* head); // ����Ʈ ī��Ʈ �Լ�
void add_head(node* head, unsigned long long blkno);// ����κп� �����Լ�
int empty(node* head);  // list ����ִ��� Ȯ��
node* getLastNode(node* ptr); // ������ ��� �ֱ�
node* getnextnode(node* ptr);
int get_blkno(node* node);
void print();

void move_mru(node* head, node* node); // ���� ����Ʈ���� mru �� �̵�

void list_node_save(node* head, int max);
node* s_getlastnode(node* head); // save node �� ������ ��� ��������
void returnlastnode(node* head, node* ptr); //svae node �� ��� �ݳ�
void s_insert_node(node* head, node* ptr, unsigned long long blkno);
void s_del_node(node* node);
node* s_getnode(node* head);

int ref(unsigned long long blkno);

/*�ؽ� ��� ������ �Լ�*/ // ���� �Լ����� ���� �Լ��� h�� �߰��ؼ� ����
void hash_init(node* h_head, int max); // �ؽ� �ش� �ʱ�ȭ
void hash_head_init(node* h_head); // ����κθ� �ʱ�ȭ �Լ�
int hash_find(node* h_head, int h_Index, unsigned long long blkno); // �ؽ� ���̺��� blkno ��ã�� ==> �ؽ� function�� ���
void hash_add(node* h_head, node* newnode, int h_Index, unsigned long long blkno); //�ؽ� ���̺� blkno�� �߰�   ==> �ؽ� ���̺��� mru�� ����
void hash_del(node* node);
int get_hash_index(node* node, int max); // full�϶� ���� �Լ��� h_index�� �޴� �Լ�
int get_hash_blkno(node* node); // full�ϋ� ���� �Լ��� blkno�� ����
node* h_find(node* h_head, int h_index, unsigned long long blkno); // full�϶� lru������ ������ ��带 �콬 ���̺��� ã�� �Լ� 
node* h_getLastNode(node* ptr); // �ؽ��� ������ ��� �޴� �Լ�
node* getnextnode(node* ptr);

int main() {
    printf("2q_bf start\n");

    init_head(&Am_head); // lru ����
    init_head(&A1in_head); // FIFO ����
    hash_init(Am_h_head, Am_h_max);
    hash_init(A1in_h_head, A1in_h_max);

    //node keep node �ʱ�ȭ
    list_node_save(&s_am, Am_MAX);
    list_node_save(&s_a1in, A1in_MAX);

    FILE* fp;
    char line[1024];
    char* p;
    int count = 0, curtime = 0;
    double time = 0;
    unsigned long long offset = 0;
    int size = 0;
    int block_size = 4096;
    unsigned long long blkno = 0;
    unsigned long long total = 0;
    unsigned long long blkno_limit = 0;
    unsigned long long total_check = 0;
    fp = fopen("2016022007-LUN6.csv", "r");
    if (fp == NULL) {
        printf("file fail\n");
    }

    fgets(line, 1024, fp);
    while (!feof(fp)) {
        fgets(line, 1024, fp);
        //printf("%s\n", line);
        p = strtok(line, ",");
        while (p != NULL) {
            //printf("%s ", p);
            if (count == 0) {
                time = atof(p);
            }
            else if (*p == 'W' || *p == 'R') {
                count = 2;
            }
            else if (count == 4) {
                offset = strtoull(p, &p, 10);
            }
            else if (count == 5) {
                size = atoi(p);
            }
            count++;
            blkno = offset / block_size;
            total = offset + size;
            blkno_limit = total / block_size;
            total_check = total % block_size;

            p = strtok(NULL, ",");
        }
        if (time == 0) {
            goto start;
        }
        time = 0;
        //printf("%llu %d\n", offset, size);
        count = 0;
        if (total_check != 0) {
            blkno_limit++;
        }
        for (blkno; blkno < blkno_limit; blkno++) {
            //printf("%llu\n", blkno);
            ref(blkno);
            curtime++;
        }
        offset = 0;
    }start:
    fclose(fp);
    
    
    double rate = (double)(Am_hit + A1in_hit) * 100 / curtime;

    printf("Am_hit=%llu \n", Am_hit);
    printf("A1in_hit=%llu \n", A1in_hit);
    printf("hit=%llu\n", Am_hit + A1in_hit);
    printf("miss=%llu\n", miss);
    printf("curtime=%llu\n", curtime);
    printf("am_c=%d a1in=%d\n", am_c, a1in_c);
    printf("rate=%.3f\n", rate);
    FILE* out;
    out = fopen("2q ref_trace.txt", "w");
    fprintf(out, "2q\n");
    fprintf(out, "ref_number=%d\n", curtime);
    fprintf(out, "Am_hit=%d\n", Am_hit);
    fprintf(out, "A1in_hit=%d \n", A1in_hit);
    fprintf(out, "miss=%d\n", miss);
    fclose(out);

    return 0;
}

void init_head(node* head) { // ���� �Ϸ�
    head->next = head;
    head->prev = head;
    head->blkno = 99999999;
}

int find_check(node* head, unsigned long long blkno) { // �̰� ������� �콬 ���� ������ �Ⱦ�
    node* ptr = head;
    int done = 0; // find üũ 
    do {
        if (ptr->blkno == blkno) {
            done = 1;
            break;
        }
        ptr = ptr->next;
    } while (ptr != head);

    return done;
}

void insert_node(node* head, unsigned long long blkno) {
    node* newNode = (node*)malloc(sizeof(node));
    newNode->blkno = blkno;

    if (head->next == head) { // ��������� ( head�� next�� head�� ����ħ) == ó�� insert�϶� 
        newNode->prev = head;
        newNode->next = head;
        head->prev = newNode;
        head->next = newNode;
    }
    else { // ó�� insert �ƴ� �� 
        newNode->next = head->next;
        newNode->prev = head;
        head->next->prev = newNode;
        head->next = newNode;
    }
}

void del_node(node* node) {  // ���� �Ϸ�
    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->next = NULL;
    node->prev = NULL;
    node->h_prev->h_next = node->h_next;
    node->h_next->h_prev = node->h_prev;
    node->h_next = NULL;
    node->h_prev = NULL;
    free(node);
}

node* find(node* head, unsigned long long blkno) { // ���� �Ϸ�
    node* ptr = head;
    do {
        if (ptr->blkno == blkno) {
            return ptr;
            break;
        }
        ptr = ptr->next;
    } while (ptr != head);
    return NULL;// �������� null����
}

int count(node* head) {
    node* ptr = head;
    int count = 0;
    do {
        count++;
        ptr = ptr->next;
    } while (ptr != head);

    return count;
}

int empty(node* head) {  // ���� �Ϸ�
    if (head->next == head) {
        return 1;// ��������� head�� �ٽ� �����ļ� 1 return
    }
    return 0; //�ƴϸ� 0 return 
}

node* getLastNode(node* ptr) // ���� �Ϸ�
{
    node* cur;
    cur = ptr->prev;
    return cur;
}

node* getnextnode(node* ptr) {
    node* cur;
    cur = ptr->next;
    return cur;
}

int get_blkno(node* node) {
    int re_blkno = node->blkno;
    return re_blkno;
}

void print() {
    node* Am_ptr = &Am_head;
    printf("Am_head=>");
    do {
        printf("%d->", Am_ptr->blkno);
        Am_ptr = Am_ptr->next;
    } while (Am_ptr != &Am_head);
    printf("\n");
    printf("\n");
    node* A1in_ptr = &A1in_head;
    printf("A1in_head=>");
    do {
        printf("%d->", A1in_ptr->blkno);
        A1in_ptr = A1in_ptr->next;
    } while (A1in_ptr != &A1in_head);
    printf("\n");
    printf("\n");
    printf("am_c=%d a1in=%d", am_c, a1in_c);
    printf("--------------------------\n");
    printf("sizeof==%d ",sizeof(Am_head));

}

void move_mru(node* head, node* node) {
    node->next->prev = node->prev;
    node->prev->next = node->next;
    // lasthead ����Ʈ���� next prev ������ �� �ڷ� ����
    if (head->next == head) {  // �� ��� isnert�� ������� ���
        node->prev = head;
        node->next = head;
        head->prev = node;
        head->next = head;
    }
    else {
        node->next = head->next;
        node->prev = head;
        head->next->prev = node;
        head->next = node;
    }
}

void list_node_save(node* head, int max) {
    head->next = head;
    head->prev = head;
    for (int i = 0; i < max; i++) {
        node* newNode = (node*)malloc(sizeof(node));
        newNode->blkno = NULL;
        if (head->next == head) {
            newNode->prev = head;
            newNode->next = head;
            head->prev = newNode;
            head->next = newNode;
        }
        else {
            newNode->next = head->next;
            newNode->prev = head;
            head->next->prev = newNode;
            head->next = newNode;
        }
    }
}

node* s_getlastnode(node* head) {
    node* ptr;
    ptr = head->prev;
    return ptr;
} // save node �� ������ ��� ��������

void returnlastnode(node* head, node* ptr) {
    if (head->next == head) { // save node�� �ƹ��͵� ���� ���
        ptr->prev = head;
        ptr->next = head;
        head->prev = ptr;
        head->next = ptr;;
    }
    else {
        ptr->next = head->next;
        ptr->prev = head;
        head->next->prev = ptr;
        head->next = ptr;
    }
} //svae node �� ��� �ݳ�

void s_insert_node(node* head, node* ptr, unsigned long long blkno) {
    ptr->blkno = blkno;// blknok save node�� blkno�� ����
    if (head->next == head) {
        ptr->prev = head;
        ptr->next = head;
        head->prev = ptr;
        head->next = ptr;
    }
    else {
        ptr->next = head->next;
        ptr->prev = head;
        head->next->prev = ptr;
        head->next = ptr;
    }
}

void s_del_node(node* node) {
    node->blkno = NULL;
    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->next = NULL;
    node->prev = NULL;
    node->h_prev->h_next = node->h_next;
    node->h_next->h_prev = node->h_prev;
    node->h_next = NULL;
    node->h_prev = NULL;
}

node* s_getnode(node* head) {
    node* ptr = head->prev;
    ptr->prev->next = ptr->next;
    ptr->next->prev = ptr->prev;
    ptr->next = NULL;
    ptr->prev = NULL;
    return ptr;
}


/*�ؽ� function*/
void hash_head_init(node* h_head) { // ����κθ� �ʱ�ȭ �Լ�
    h_head->h_next = h_head;
    h_head->h_prev = h_head;
}

void hash_init(node* h_head, int max) {
    for (int i = 0; i < max; i++) {
        hash_head_init(&h_head[i]);
        h_head[i].blkno = 999999999;
    }
}

int hash_find(node* h_head, int h_Index, unsigned long long blkno) {
    int h_done = 0; // �콬 ��Ʈ ����
    node* ptr = &h_head[h_Index];
    do {
        if (ptr->blkno == blkno) {
            h_done = 1;  // �ش� �콬 ��忡 �����Ҷ� done���� 1�� ������
            break;
        }
        ptr = ptr->h_next; // 03 31 ���� ptr=ptr->next�� ptr=ptr->h_next�� ���� 
    } while (ptr != &h_head[h_Index]);
    return h_done;
}

void hash_add(node* h_head, node* newnode, int h_Index, unsigned long long blkno) {
    node* ptr = &h_head[h_Index];
    if (ptr->h_next == ptr) {//�ؽ� ���̺� ó�� �����Ҷ�
        newnode->h_next = ptr;
        newnode->h_prev = ptr;
        ptr->h_next = newnode;
        ptr->h_prev = newnode;
    }
    else { // ó�� ������ �ƴ� ��� 
        newnode->h_next = ptr->h_next;
        newnode->h_prev = ptr;
        ptr->h_next->h_prev = newnode;
        ptr->h_next = newnode;
    }
}

void hash_del(node* node) {
    node->h_prev->h_next = node->h_next;
    node->h_next->h_prev = node->h_prev;
    node->h_next = NULL;
    node->h_prev = NULL;
}
int get_hash_index(node* node, int max) {
    int index = node->blkno;
    int hash = index % max;
    return hash;
}

int get_hash_blkno(node* node) { // full�ϋ� ���� �Լ��� blkno�� ����
    int re_blkno = node->blkno;
    return re_blkno;
}

node* h_find(node* h_head, int h_index, unsigned long long blkno) { // full�϶� lru������ ������ ��带 �콬 ���̺��� ã�� �Լ�
    node* ptr = &h_head[h_index];
    do {
        if (ptr->blkno == blkno) {
            return ptr;
            break;
        }
        ptr = ptr->h_next;
    } while (ptr != &h_head[h_index]);
}

node* h_getLastNode(node* ptr) // �߰� �콬�� ������ ��� �ޱ� �Լ�
{
    node* cur;
    cur = ptr->h_prev;
    return cur;
}

int ref(unsigned long long blkno) {
    int am_hash = blkno % Am_h_max;
    int a1in_hash = blkno % A1in_h_max;
    int Am_done = 0, A1in_done = 0, A1out_done = 0; // blkno �� �����ϴ� üũ�ϴ� ����
    Am_done = hash_find(Am_h_head, am_hash, blkno);
    A1in_done = hash_find(A1in_h_head, a1in_hash, blkno);
    A1out_done = sbf.lookup(blkno);
    node* ptr; // ���� ��� ������ ����
    if (Am_done == 1) // blkno�� Am�� ����
    {
        ptr = h_find(Am_h_head, am_hash, blkno);  // �ؽ��� ã�� �ɷ� �ٲپ����
        move_mru(&Am_head, ptr);
        Am_hit++;
    }
    else if (A1in_done == 1) // blkno�� A1in ����
    {
        A1in_hit++;
        return NULL;
    }
    //else if (cb.lookup(blkno) == 1)// blkno�� A1_out ����
    //else if (sbf.lookup(blkno) == 1)
    else if (abf.lookup(blkno) == 1)
    {
        miss++;
        //cb.remove(blkno);
        //sbf.remove(blkno);
        if (am_c == Am_MAX) {
            ptr = getLastNode(&Am_head);
            s_del_node(ptr);
            returnlastnode(&s_am,ptr);

            ptr = s_getnode(&s_am);
            s_insert_node(&Am_head,ptr, blkno);
            ptr = getnextnode(&Am_head);
            hash_add(Am_h_head, ptr, am_hash, blkno);
            //bf �� blkno ���� ==> am(full) lastNode ���� �� am�� blkno ����
        }
        else {
            ptr = s_getnode(&s_am);
            s_insert_node(&Am_head, ptr, blkno);
            ptr = getnextnode(&Am_head);
            hash_add(Am_h_head, ptr, am_hash, blkno);
            am_c++;
        }
    }
    else // blkno�� ���� �� == ó������ ���Եɶ�  ==> ���⼭�� A1in A1out ����Ʈ�� �ٷ�  ==> A1in �� ����
    {
        if (a1in_c == A1in_MAX) { // A1in �� �� ä������ ��
                ptr = getLastNode(&A1in_head);
                int re_blkno = get_blkno(ptr);
                s_del_node(ptr);
                returnlastnode(&s_a1in, ptr);
                a1in_c--;
                //cb.add(re_blkno);
                //sbf.add(re_blkno);
                abf.add(re_blkno);
        }
        ptr = s_getnode(&s_a1in);
        s_insert_node(&A1in_head,ptr, blkno);
        ptr = getnextnode(&A1in_head);
        hash_add(A1in_h_head, ptr, a1in_hash, blkno);
        a1in_c++;
        miss++;
    }
}