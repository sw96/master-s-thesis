#define _CRT_SECURE_NO_WARNINGS
#include<time.h>

#include<stdio.h>
#include<string.h>
#include <stdlib.h>
#include<stddef.h>
#include <string.h>
#include <windows.h> // ����ũ�� �ð� ���� ��� ����
#include <time.h>

#define MAX   256106
#define H_MAX 256106

unsigned long long hit = 0, miss = 0;
int done = 0; // find �Լ� ���� hit=>1 // miss=>0
int c = 0; // lru�� ���� ī��Ʈ
int curtime = 0;
int blkno = 0;
int if_count = 0;
clock_t start, finish;

typedef struct node {
    unsigned long long blkno; //8
    struct node* next; //8
    struct node* prev; //8
    
    int* data; //8
    struct node* h_next;
    struct node* h_prev;
}node;

struct TraceRecord {
    int asu;
    int lba;
    int size;
    char opcode;
    double timestamp;
};

struct node head;
struct node h_head[H_MAX];
struct node save_node;
node* getLastNode(node* ptr);// ������ ��� ���
void init_head(node* head);// ��� ������ �ʱ�ȭk
int find_check(node* head, unsigned long long blkno); //blkno�� ����Ʈ�� �����ϴ� �˻��ϴ� �Լ�
void insert_node(node* head, unsigned long long blkno); // 
void del_node(node* node);// �ش� ��� ����
int ref(unsigned long long blkno);

void move_mru(node* head, node* node); // ���� ����Ʈ���� mru �� �̵�


// node �ʱ�ȭ 
void list_node_save(node* head, int max);
int s_getlastnode(node* head); // save node �� ������ ��� ��������
void returnlastnode(node* head, node* ptr); //svae node �� ��� �ݳ�
void s_insert_node(node* head, node* ptr, unsigned long long blkno);
void s_del_node(node* node);
node* s_getnode(node* head);

/*�ؽ� ��� ������ �Լ�*/ // ���� �Լ����� ���� �Լ��� h�� �߰��ؼ� ����
void hash_init(node* h_head); // �ؽ� �ش� �ʱ�ȭ
void hash_head_init(node* h_head); // ����κθ� �ʱ�ȭ �Լ�
int hash_find(node* h_head, unsigned long long h_Index, unsigned long long blkno); // �ؽ� ���̺��� blkno ��ã�� ==> �ؽ� function�� ���
void hash_add(node* h_head, unsigned long long h_Index, unsigned long long blkno); //�ؽ� ���̺� blkno�� �߰�   ==> �ؽ� ���̺��� mru�� ����
void hash_del(node* h_head); //�ؽ� ���̺� blkno�� ����   ==> �ؽ� ���̺��� lru�� ���� 

int get_hash_index(node* node); // full�϶� ���� �Լ��� h_index�� �޴� �Լ�
int get_hash_blkno(node* node); // full�ϋ� ���� �Լ��� blkno�� ����
node* h_find(node* h_head, unsigned long long h_index, unsigned long long blkno); // full�϶� lru������ ������ ��带 �콬 ���̺��� ã�� �Լ� 
node* h_getLastNode(node* ptr); // �ؽ��� ������ ��� �޴� �Լ�
node* getnextnode(node* ptr);

void hash_print();
void print();
int main() {
    LARGE_INTEGER start_time, end_time, frequency;
    double elapsed_time=0, total_time=0;
    printf("lru start\n");
    init_head(&head);
    hash_init(&h_head);
    list_node_save(&save_node, MAX);

    /*
    FILE* fp;
    FILE* fp1;
    char line[1024];
    char* p, * endptr;
    int count = 0, curtime = 0;
    double time = 0;
    unsigned long long offset = 0;
    int size = 0;
    int block_size = 4096;
    int previous_line_empty = 0;
    unsigned long long blkno = 0;
    unsigned long long total = 0;
    unsigned long long blkno_limit = 0;
    unsigned long long total_check = 0;
    fp = fopen("2016021612-LUN0.csv", "r");
    fp1 = fopen("2016021911-LUN0.csv", "r");
    if (fp == NULL) {
        printf("file fail\n");
    }
    fgets(line, 1024, fp);
    while (!feof(fp)) {
        fgets(line, 1024, fp);
        if (strcmp(line, "\n") == 0) {
            continue;
        }
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
                offset = strtoull(p, &endptr, 10);
            }
            else if (count == 5) {
                size = atoi(p);
            }
            count++;

            p = strtok(NULL, ",");
        }
        if (time == -1) {
            goto jump;
        }
        time = -1;
        blkno = offset / block_size;
        total = offset + size;
        blkno_limit = total / block_size;
        total_check = total % block_size;
        
        //printf("%llu %d\n", offset, size);
        count = 0;
        if (total_check != 0) {
            blkno_limit++;
        }
        for (blkno; blkno < blkno_limit; blkno++) {
            //printf("%llu\n", blkno);
            QueryPerformanceFrequency(&frequency); // Ÿ�̸��� ���ļ� ���
            QueryPerformanceCounter(&start_time);  // �ڵ� ���� �� �ð� ���� ����

            ref(blkno);

            QueryPerformanceCounter(&end_time); // �ڵ� ���� �� �ð� ���� ����
            elapsed_time = (double)(end_time.QuadPart - start_time.QuadPart) / frequency.QuadPart * 1000000.0;
            //printf("�ڵ� ���� �ð�: %.2f ����ũ����\n", elapsed_time);

            total_time += elapsed_time;
            curtime++;
        }
    }jump:
    fclose(fp);
    
    fgets(line, 1024, fp1);
    while (!feof(fp1)) {
        fgets(line, 1024, fp1);
        if (strcmp(line, "\n") == 0) {
            continue;
        }
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
                offset = strtoull(p, &endptr, 10);
            }
            else if (count == 5) {
                size = atoi(p);
            }
            count++;

            p = strtok(NULL, ",");
        }
        if (time == -1) {
            goto jump1;
        }
        time = -1;
        blkno = offset / block_size;
        total = offset + size;
        blkno_limit = total / block_size;
        total_check = total % block_size;

        //printf("%llu %d\n", offset, size);
        count = 0;
        if (total_check != 0) {
            blkno_limit++;
        }
        for (blkno; blkno < blkno_limit; blkno++) {
            //printf("%llu\n", blkno);
            QueryPerformanceFrequency(&frequency); // Ÿ�̸��� ���ļ� ���
            QueryPerformanceCounter(&start_time);  // �ڵ� ���� �� �ð� ���� ����

            ref(blkno);

            QueryPerformanceCounter(&end_time); // �ڵ� ���� �� �ð� ���� ����
            elapsed_time = (double)(end_time.QuadPart - start_time.QuadPart) / frequency.QuadPart * 1000000.0;
            //printf("�ڵ� ���� �ð�: %.2f ����ũ����\n", elapsed_time);

            total_time += elapsed_time;
            curtime++;
        }
    }jump1:
    fclose(fp1);
    */

    /*
    FILE* file = fopen("Financial2.spc", "r"); // ���� ��θ� ������ ����
    unsigned long long blkno = 0;
    unsigned long long total = 0, blkno_limit = 0, total_check = 0;
    unsigned long long block_size = 4096;
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    struct TraceRecord record;
    while (fscanf(file, "%d,%d,%d,%c,%lf\n", &record.asu, &record.lba, &record.size, &record.opcode, &record.timestamp) == 5)
    {
        //printf("ASU: %d, LBA: %d, Size: %d, Opcode: %c, Timestamp: %lf\n", record.asu, record.lba, record.size, record.opcode, record.timestamp);
        if (record.size == 0) {
            continue;
        }
        blkno = record.lba / block_size;
        total = record.lba + record.size;
        blkno_limit = total / block_size;
        total_check = total % block_size;
        if (total_check != 0) {
            blkno_limit++;
        }
        for (blkno; blkno < blkno_limit; blkno++) {
            //printf("%llu\n", blkno);
            ref(blkno);
            curtime++;
        }
    }
    fclose(fp)'
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
    

    printf("c= %d\n", c);
    printf("hit= %llu\n", hit);
    printf("miss=%llu\n", miss);
    printf("total=%d\n", curtime);
    double rate = (double)hit * 100 / curtime;
    printf("rate=%.3f\n", rate);
    double avg_e_time = total_time / curtime;
    printf("��� ���� �ð�: %.2f ����ũ����\n", avg_e_time);
    return 0;
}

node* getnextnode(node* ptr) {
    node* cur;
    cur = ptr->next;
    return cur;
}

node* getLastNode(node* ptr) // ���� �Ϸ�
{
    node* cur;
    cur = ptr->prev;
    return cur;
}

node* h_getLastNode(node* ptr) // �߰� �콬�� ������ ��� �ޱ� �Լ�
{
    node* cur;
    cur = ptr->h_prev;
    return cur;
}

void init_head(node* head) { // ���� �Ϸ�
    head->next = head;
    head->prev = head;
    head->blkno = NULL;
}

int find_check(node* head, unsigned long long blkno) { // �̰� ������� �콬 ���� ������ �Ⱦ�
    node* ptr = head;
    int done = 0; // find üũ ����

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
        head->prev = newNode; // ���� �ذ� ÷�� �� head ������ �κ��� �ۼ��� ���ؼ� ��� ���� ��尡 ���� �ʾƼ� miss�� ī��Ʈ ��
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

void hash_head_init(node* h_head) { // ����κθ� �ʱ�ȭ �Լ�
    h_head->h_next = h_head;
    h_head->h_prev = h_head;
}

void move_mru(node* head, node* node) {
    node->next->prev = node->prev;
    node->prev->next = node->next;
    // lasthead ����Ʈ���� next prev ������ �� �ڷ� ����
    node->next = head->next;
    node->prev = head;
    head->next->prev = node;
    head->next = node;
}

/*�ؽ� function*/
void hash_init(node* h_head) {
    for (int i = 0; i < H_MAX; i++) {
        hash_head_init(&h_head[i]);
        h_head[i].blkno = NULL;
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
    } 
    return h_done;
}



//�ܼ��ϰ� h_head �ڿ� ��� ����
void hash_add(node* h_head, node* newNode,int h_Index, unsigned long long blkno) {
    node* ptr = &h_head[h_Index];
    if (ptr->h_next == &h_head[h_Index]) { // �ؽ��� ó������ �����Ҷ�  ���� 03 31 �콬 ���Խ� �ϳ� �̻� ���� �ȵǴ� ���� �߻� if ������ =���� ������ �־��� ==���� ����
        newNode->h_prev = ptr;
        newNode->h_next = ptr;
        ptr->h_next = newNode;
        ptr->h_prev = newNode;
    }
    else  // �����ΰ� �־� ����  *** mru=head->next �� 
    {
        newNode->h_next = ptr->h_next;
        newNode->h_prev = ptr;
        ptr->h_next->h_prev = newNode; // 04 01 ERROR �� �������� �� �κ��� �����ΰŰ���
        // ���� ��ũ ����Ʈ�϶� NEXT�����͸� ���� ������
        ptr->h_next = newNode;
    }
}

void hash_del(node* node) {
    if (node->h_next == &h_head) {  // ������ ����϶� ==> ����node->next==NULL ==> node->next==head �� ����
        node->h_prev->h_next = &h_head;
        node->h_prev = node->h_next->h_prev;
        node->h_next = NULL;
        node->h_prev = NULL;
    }
    else { // ������ ��尡 �ƴ� ��
        node->h_prev->h_next = node->h_next;
        node->h_next->h_prev = node->h_prev;
        node->h_next = NULL;
        node->h_prev = NULL;
    }
    free(node);
}

int get_hash_index(node* node) {
    int index = node->blkno;
    int hash = index % H_MAX;
    return hash;
}

int get_hash_blkno(node* node) { // full�ϋ� ���� �Լ��� blkno�� ����
    int re_blkno = node->blkno;
    return re_blkno;
}
node* h_find(node* h_head, int h_index, unsigned long long blkno) { // full�϶� lru������ ������ ��带 �콬 ���̺��� ã�� �Լ�
    node* ptr = &h_head[h_index];
    ptr = ptr->h_next;
    while (ptr != &h_head[h_index])
    {
        if (ptr->blkno == blkno) {
            return ptr;
            break;
        }
        ptr = ptr->h_next;
    } 
}

void hash_print() {
    node* h_ptr;
    for (int i = 0; i < H_MAX; i++) {
        printf("%d=> ", i);
        do {
            h_ptr = &h_head[i];
            printf("%llu->", h_ptr->blkno);
            h_ptr = h_ptr->h_next;
        } while (h_ptr != &h_head[i]);
        printf("\n\n");
    }
}

void print() {
    node* ptr = &head;
    ptr = ptr->next;
    printf("head=>");
    do {
        printf("%llu->", ptr->blkno);
        ptr = ptr->next;
    } while (ptr != &head);
    printf("\n");
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
            head->next = newNode;;
        }
        else {
            newNode->next = head->next;
            newNode->prev = head;
            head->next->prev = newNode;
            head->next = newNode;
        }
    }
}

int s_getlastnode(node* head) {
    node* ptr;
    ptr = head->prev;
    return ptr;
} // save node �� ������ ��� ��������

void returnlastnode(node* head,node* ptr) {
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

void s_insert_node(node* head,node* ptr, unsigned long long blkno) {
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
}

int ref(unsigned long long blkno) {
    if_count++;
    unsigned long long h_index = blkno % H_MAX; // �ؽ� �� function 
    int h_done = hash_find(h_head, h_index, blkno);
    node* ptr = NULL; // find ���� ������ ����

    if (h_done == 1) {// 2. hash�� blkno�� �����Ҷ�
        hit++;
        ptr = h_find(h_head, h_index, blkno);
        move_mru(&head, ptr);
    }

    else { // 3. hash �� �������� �ʴ� ���
        // lru �� ����(mru)  and hash ���� 
        miss++;
        if (c == MAX) { // lru�� full �ΰ� ��� 
            /*ptr = getLastNode(&head);
            del_node(ptr);
            insert_node(&head, blkno);
            ptr = getnextnode(&head);
            hash_add(h_head, ptr, h_index, blkno);
            */
            ptr = getLastNode(&head);
            s_del_node(ptr);
            returnlastnode(&save_node, ptr);
            ptr = s_getnode(&save_node);
            s_insert_node(&head, ptr, blkno);
            ptr = getnextnode(&head);
            hash_add(h_head, ptr, h_index, blkno);
        }
        else { // lru �� full�� �ƴ� 
            /*
            insert_node(&head, blkno);
            ptr = getnextnode(&head);
            hash_add(h_head,ptr, h_index, blkno);
            c++;
            */
            ptr=s_getnode(&save_node);
            s_insert_node(&head, ptr, blkno);
            ptr = getnextnode(&head);
            hash_add(h_head, ptr, h_index, blkno);
            c++;

        }
    }
    return 0;
}