#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<stddef.h>
#include<iostream>
#include<iterator>
#include <string>
#include <time.h>
#include <windows.h>
// ���Ƿ� ���� recliT2 �ϱ�����
#define capacity 244922
#define T1_h_max 244922
#define T2_h_max 244922
#define b1_h_max 244922
#define b2_h_max 244922


#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))
    
typedef struct node {
    unsigned long long blkno; //8
    struct node* next; //8
    struct node* prev; //8
    int* data; //8
    struct node* h_next; //8
    struct node* h_prev; //8
}node;

node T2_head; //t2
node T1_head; //t1
node b1_head; //b1
node b2_head;

node s_T_head;
node s_B_head;

//hashtable
node T2_h_head[T2_h_max];
node T1_h_head[T1_h_max];
node b2_h_head[b2_h_max];
node b1_h_head[b1_h_max];

unsigned long long T1_hit = 0, T2_hit = 0;
unsigned long long B1_hit = 0, B2_hit = 0;
unsigned long long miss = 0;
int ratio = 0;
int t1_c = 0, t2_c = 0;
int b1_c = 0, b2_c = 0;
unsigned long long curtime = 0;
unsigned long long blkno = 0;

void init_head(node* head);// ��� ������ �ʱ�ȭ
int find_check(node* head, unsigned long long blkno); //blkno�� ����Ʈ�� �����ϴ� �˻��ϴ� �Լ�
void insert_node(node* head, unsigned long long blkno); // 
void del_node(node* node);// �ش� ��� ����
node* find(node* head, unsigned long long blkno);// �ش� blkno ��� �˻��ϴ� �Լ�
node* getLastNode(node* ptr); // ������ ��� �ֱ�
int get_blkno(node* node);
void print();
void move_mru(node* head, node* node); // ���� ����Ʈ���� mru �� �̵�
void move_list_mru(node* head, node* node); // hot cache �� �̵�

// node �ʱ�ȭ 
void list_node_save(node* head, int max);
node* s_getlastnode(node* head); // save node �� ������ ��� ��������
void returnlastnode(node* head, node* ptr); //svae node �� ��� �ݳ�
void s_insert_node(node* head, node* ptr, unsigned long long blkno);
void s_del_node(node* node);
node* s_getnode(node* head);

//hash function
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

void replace(unsigned long long blkno, int b2_done, node* T1_head, node* T2_head, int ratio);

int ref(unsigned long long blkno);

int main() {
    printf("arc start\n");
    LARGE_INTEGER start_time, end_time, frequency;
    double elapsed_time = 0, total_time = 0;
    init_head(&T2_head);
    init_head(&T1_head);
    init_head(&b1_head);
    init_head(&b2_head);

    hash_init(T2_h_head, T2_h_max);
    hash_init(T1_h_head, T1_h_max);
    hash_init(b2_h_head, b2_h_max);
    hash_init(b1_h_head, b1_h_max);

    list_node_save(&s_T_head, capacity);
    list_node_save(&s_B_head, capacity);

    /*
    ref(1); print();
    ref(2); print();
    ref(3); print();
    ref(4); print();
    ref(5); print();
    ref(2); print();
    ref(4); print();
    ref(3); print();

    unsigned long long random = 0;
    for (int i = 0; i < 100; i++) { // 10�� �ݺ�

        random = rand() % 9; // ���� ����
        ref(random); print();
    }
    */
    /*
    ref(6); print();
    ref(7); print();
    ref(5); print();
    ref(7); print();
    ref(8); print();
    ref(3); print();
    ref(9); print();
    ref(9); print();
    ref(6); print();
    ref(5); print();
    ref(0); print();
    ref(0); print();
    ref(6); print();
    ref(5); print(); // T2=>B2 �̵� but 0�� 2�� b2�� insert �� �����ʿ�
    ref(0); print();
    ref(4); print();
    ref(7); print();
    ref(6); print();
    ref(5); print();
    */
    /*
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
    fp = fopen("2016021707-LUN2.csv", "r");
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
            goto jump;
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
            //printf("ratio=%d T1_c=%d T2_c=%d B1_c=%d B2_c=%d\n\n", ratio, t1_c, t2_c, b1_c, b2_c);
            curtime++;
            print();
        }
        offset = 0;
    }jump:
    fclose(fp);
    */
    unsigned long long blkno = 0;
    FILE* fp = fopen("trace0.3.txt", "r");
    while (fscanf(fp, "%llu", &blkno) == 1) {
        QueryPerformanceFrequency(&frequency); // Ÿ�̸��� ���ļ� ���
        QueryPerformanceCounter(&start_time);  // �ڵ� ���� �� �ð� ���� ����
        ref(blkno);
        QueryPerformanceCounter(&end_time); // �ڵ� ���� �� �ð� ���� ����
        elapsed_time = (double)(end_time.QuadPart - start_time.QuadPart) / frequency.QuadPart * 1000000.0;
        //printf("�ڵ� ���� �ð�: %.2f ����ũ����\n", elapsed_time);

        total_time += elapsed_time;
        curtime++;
    }
    fclose(fp);

    double rate = (double)(T2_hit + T1_hit) * 100 / curtime;
    printf("arc\n");
    printf("T2_hit=%llu \n", T2_hit);
    printf("T1_hit=%llu \n", T1_hit);
    printf("hit=%llu\n", T2_hit + T1_hit);
    printf("B2_hit=%llu \n", B2_hit);
    printf("b1_hit=%llu\n", B1_hit);
    printf("miss=%llu\n", miss);
    printf("Total=%d\n", curtime);
    printf("T1_c %d T2_c %d b1_c %d b2_c %d ", t1_c, t2_c, b1_c, b2_c);
    printf("rate=%.3f\n", rate);
    double avg_e_time = total_time / curtime;
    printf("��� ���� �ð�: %.2f ����ũ����\n", avg_e_time);

    /*

    FILE* out;
    out = fopen("2q ref_trace.txt", "w");
    fprintf(out, "2q\n");
    fprintf(out, "ref_number=%d\n", curtime);
    fprintf(out, "T2_hit=%d\n", T2_hit);
    fprintf(out, "T1_hit=%d \n", T1_hit);
    fprintf(out, "b1_hit=%d \n", B1_hit);
    fprintf(out, "miss=%d\n", miss);
    fclose(out);
    */
    return 0;
}

void init_head(node* head) { // ���� �Ϸ�
    head->next = head;
    head->prev = head;
    head->blkno = 9999999;
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

void move_mru(node* head, node* node) {
    node->next->prev = node->prev; // error
    node->prev->next = node->next;
    // lasthead ����Ʈ���� next prev ������ �� �ڷ� ����
    node->next = head->next;
    node->prev = head;
    head->next->prev = node;
    head->next = node;
}

void move_list_mru(node* head, node* node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->h_prev->h_next = node->h_next;
    node->h_next->h_prev = node->h_prev;
    // hash ��� list ��� next prev ������ �� �ڷ� ����
    if (head->next == head) {  // �� ��� isnert�� ������� ���
        node->prev = head;
        node->next = head;
        head->prev = node;
        head->next = node;;
    }
    else {
        node->next = head->next;
        node->prev = head;
        head->next->prev = node;
        head->next = node;
    }
}

void print() {
    node* T2_ptr = &T2_head;
    printf("T2_head=>");
    do {
        printf("%llu->", T2_ptr->blkno);
        T2_ptr = T2_ptr->next;
    } while (T2_ptr != &T2_head);
    printf("\n");
    node* T1_ptr = &T1_head;
    printf("T1_head=>");
    do {
        printf("%llu->", T1_ptr->blkno);
        T1_ptr = T1_ptr->next;
    } while (T1_ptr != &T1_head);
    printf("\n");
    node* b2_ptr = &b2_head;
    printf("b2_head=>");
    do {
        printf("%llu->", b2_ptr->blkno);
        b2_ptr = b2_ptr->next;
    } while (b2_ptr != &b2_head);
    printf("\n");
    node* b1_ptr = &b1_head;
    printf("b1_head=>");
    do {
        printf("%llu->", b1_ptr->blkno);
        b1_ptr = b1_ptr->next;
    } while (b1_ptr != &b1_head);
    printf("\n");
    printf("t2=%d t1=%d b2=%d b1=%d\n", t2_c, t1_c, b2_c, b1_c);
    printf("T2_hit=%d T1_hit=%d B2_hit=%d B1_hit=%d miss=%d\n", T2_hit, T1_hit, B2_hit, B1_hit, miss);
    printf("--------------------------\n");

}

void list_node_save(node* head, int max) {
    head->next = head;
    head->prev = head;
    for (int i = 0; i < max+1; i++) {
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
    ptr->next = head->next;
    ptr->prev = head;
    head->next->prev = ptr;
    head->next = ptr;
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


//hash function 
void hash_head_init(node* h_head) { // ����κθ� �ʱ�ȭ �Լ�
    h_head->h_next = h_head;
    h_head->h_prev = h_head;
}

/*�ؽ� function*/
void hash_init(node* h_head, int max) {
    for (int i = 0; i < max; i++) {
        hash_head_init(&h_head[i]);
        h_head[i].blkno = 999999999;
    }
}

int hash_find(node* h_head, int h_Index, unsigned long long blkno) {
    int h_done = 0; // �콬 ��Ʈ ����
    node* ptr = &h_head[h_Index];
    ptr = ptr->h_next;
    while (ptr != &h_head[h_Index])
    {
        if (ptr->blkno == blkno) {
            h_done = 1;  // �ش� �콬 ��忡 �����Ҷ� done���� 1�� ������
            break;
        }
        ptr = ptr->h_next; // 03 31 ���� ptr=ptr->next�� ptr=ptr->h_next�� ���� 
    };
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

void replace(unsigned long long blkno, int b2_done, node* T1_head, node* T2_head, int ratio) {
    int c = t1_c;
    node* ptr;
    if ((c > 0) && ((ratio < c) || ((c == ratio) && (b2_done == 0)))) {
        ptr = getLastNode(T1_head);
        int return_blkno = get_blkno(ptr);
        int return_hash = return_blkno % b1_h_max; //t1_h_max���� b1_h_max�� ���� 8.30
        s_del_node(ptr);
        returnlastnode(&s_T_head, ptr);
        t1_c--;
        ptr = s_getnode(&s_B_head);
        s_insert_node(&b1_head,ptr, return_blkno);
        ptr = getnextnode(&b1_head);
        hash_add(b1_h_head, ptr, return_hash, return_blkno);
        b1_c++;
    }
    else {
            ptr = getLastNode(T2_head);
            int return_blkno = get_blkno(ptr);
            int return_hash = return_blkno % b2_h_max; //t1_h_max���� b1_h_max�� ���� 8.30
            s_del_node(ptr); 
            returnlastnode(&s_T_head,ptr);
            t2_c--;
            ptr = s_getnode(&s_B_head); // �� �κп��� s_B_head�� B2_head �����Ͱ� ���Ե� ������ �𸣁���
            s_insert_node(&b2_head,ptr, return_blkno); // s_b_head �� ���Ե� check�ʿ�
            ptr = getnextnode(&b2_head);
            hash_add(b2_h_head, ptr, return_hash, return_blkno);
            b2_c++;
    }
}

int ref(unsigned long long blkno) {
    int T2_done = 0, T1_done = 0, b2_done = 0, b1_done = 0; // blkno �� �����ϴ� üũ�ϴ� ����
    int T1_hash = blkno % T1_h_max;
    int T2_hash = blkno % T2_h_max;
    int b1_hash = blkno % b1_h_max;
    int b2_hash = blkno % b2_h_max;
    T2_done = hash_find(T2_h_head, T2_hash, blkno);
    T1_done = hash_find(T1_h_head, T1_hash, blkno);
    b2_done = hash_find(b2_h_head, b2_hash, blkno);
    b1_done = hash_find(b1_h_head, b1_hash, blkno);
    //printf("T2_done=%d T1_done=%d B2_done=%d B1_done=%d miss=%llu blkno=%llu ratio=%d \n", T2_done, T1_done, b2_done, b1_done, miss,blkno,ratio);

    node* ptr; // ���� ��� ������ ����
    if (T2_done == 1) // blkno�� T2�� ����
    {
        T2_hit++;
        ptr = h_find(T2_h_head, T1_hash, blkno);
        move_mru(&T2_head, ptr);
    }
    else if (T1_done == 1) // blkno�� t1 ����
    {
        T1_hit++;
        ptr = h_find(T1_h_head, T1_hash, blkno);
        t1_c--;
        hash_del(ptr);
        move_mru(&T2_head, ptr); // �ذ� �Ϸ�
        ptr = getnextnode(&T2_head);
        hash_add(T2_h_head, ptr, T2_hash, blkno);
        t2_c++;
    }
    else if (b1_done == 1)// blkno�� b1 ����
    {
        B1_hit++;
        ratio = min(capacity, ratio + max(1, (b2_c / b1_c)));
        replace(blkno, b2_done, &T1_head, &T2_head, ratio); //t2_insert
        //b1���� ���� 
        ptr = h_find(b1_h_head, b1_hash, blkno);
        s_del_node(ptr);
        returnlastnode(&s_B_head, ptr); //�� ����Ʈ ������ 009��
        b1_c--;
        //b1 list ����

        ptr = s_getnode(&s_T_head);
        s_insert_node(&T2_head,ptr, blkno);
        ptr = getnextnode(&T2_head);
        hash_add(T2_h_head, ptr, T2_hash, blkno);
        t2_c++;
    }
    else if (b2_done == 1) {
        B2_hit++;
        ratio = max(0, ratio - max(1, (b1_c / b2_c)));
        replace(blkno, b2_done, &T1_head, &T2_head, ratio);
        ptr = h_find(b2_h_head, b2_hash, blkno);
        s_del_node(ptr);
        returnlastnode(&s_B_head, ptr);
        b2_c--;
        ptr = s_getnode(&s_T_head);
        s_insert_node(&T2_head,ptr, blkno);
        ptr = getnextnode(&T2_head);
        hash_add(T2_h_head, ptr, T2_hash, blkno);
        t2_c++;
    }

    else // blkno�� ���� �� == ó������ ���Եɶ�  ==> ���⼭�� A1in b1 ����Ʈ�� �ٷ�  ==> A1in �� ����
    {
        miss++;
        if ((t1_c + b1_c) == capacity) {// t1 & b1 �� ����� �� ä������ ���
            if (t1_c < capacity) {// t1 �� �뷮�� ��� ä�� ��찡 �ƴ� ���
                ptr = getLastNode(&b1_head);
                s_del_node(ptr);//b1'tail remove // error point 
                returnlastnode(&s_B_head, ptr);
                b1_c--;
                replace(blkno, b2_done, &T1_head, &T2_head, ratio);
            }
            else { // t1�� �� ä�� ��� 
                ptr = getLastNode(&T1_head);
                s_del_node(ptr);
                returnlastnode(&s_T_head, ptr);
                t1_c--;
            }
        }
        else {
            //sm==> t1 t2 b1 b2 �� ��� �뷮
            int sm = (t1_c + t2_c + b1_c + b2_c);
            if (sm >= capacity) {// �� �뷮�� ������ �뷮���� ū ���
                if (sm == (2 * capacity)) {// sm �� ���� �뷮���� 2�谡 �Ǵ� ���
                    ptr = getLastNode(&b2_head);
                    s_del_node(ptr);//b2 ' tail remove
                    returnlastnode(&s_B_head, ptr);
                    b2_c--;
                }
                replace(blkno, b2_done, &T1_head, &T2_head, ratio);
            }
        }
        ptr = s_getnode(&s_T_head);
        s_insert_node(&T1_head, ptr, blkno); // s node ���� �Ϸ�
        ptr = getnextnode(&T1_head);
        hash_add(T1_h_head, ptr, T1_hash, blkno);
        t1_c++;
    }
    //printf("T2_done=%d T1_done=%d B2_done=%d B1_done=%d miss=%llu blkno=%llu ratio=%d \n", T2_done, T1_done, b2_done, b1_done, miss, blkno, ratio);

    //printf("T2_done=%d T1_done=%d B2_done=%d B1_done=%d miss=%llu", T2_done, T1_done, b2_done, b1_done, miss);
    return 0;
}
