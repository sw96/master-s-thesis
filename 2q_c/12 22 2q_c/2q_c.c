#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<stddef.h>
#include<string.h>
#include <time.h>
#include <windows.h>
// ���Ƿ� ���� recliam �ϱ�����
#define Am_MAX 179298
#define A1in_MAX 56350
#define A1out_MAX 894784
#define Am_h_max 179298
#define A1in_h_max 56350
#define A1out_h_max 894784


typedef struct node {
    unsigned long long blkno; //8
    struct node* next;//8
    struct node* prev;//8
    
    int* data;//4096

    struct node* h_next;//8
    struct node* h_prev;//8
}node;

node Am_head;
node Am_h_head[Am_h_max];
node A1in_head;
node A1in_h_head[A1in_h_max];
node A1out_head;
node A1out_h_head[A1out_MAX];

node s_am;
node s_a1in;
node s_a1out;
/*
node *Am_head node* A1out_head node* A1in_head �� �����ؼ�
ref�ʹݿ� �Լ��� ���ϴ� �κ��� ����� ������ �Ǿ��ٰ� �ȵǾ��ٰ� �ϸ鼭 ������ �߻���
*/


unsigned long long Am_hit = 0; 
unsigned long long A1in_hit = 0;
unsigned long long A1out_hit = 0;
unsigned long long miss = 0;

int am_c = 0;
int a1in_c = 0;
int a1out_c = 0;

int curtime = 0;
unsigned long long blkno = 0;

void init_head(node* head);// ��� ������ �ʱ�ȭ
int find_check(node* head, unsigned long long blkno); //blkno�� ����Ʈ�� �����ϴ� �˻��ϴ� �Լ�
void insert_node(node* head, unsigned long long blkno); // 
void del_node(node* node);// �ش� ��� ����
node* find(node* head, unsigned long long blkno);// �ش� blkno ��� �˻��ϴ� �Լ�
node* getLastNode(node* ptr); // ������ ��� �ֱ�
node* getnextnode(node* ptr); //���Ե� ��� ��������
int get_blkno(node* node);
void print();
void move_mru(node* head, node* node); // ���� ����Ʈ���� mru �� �̵�

// node �ʱ�ȭ 
void list_node_save(node* head, int max);
int s_getlastnode(node* head); // save node �� ������ ��� ��������
void returnlastnode(node* head, node* ptr); //svae node �� ��� �ݳ�
void s_insert_node(node* head, node* ptr, unsigned long long blkno);
void s_del_node(node* node);
node* s_getnode(node* head);

/*�ؽ� ��� ������ �Լ�*/ // ���� �Լ����� ���� �Լ��� h�� �߰��ؼ� ����
void hash_init(node* h_head,int max); // �ؽ� �ش� �ʱ�ȭ
void hash_head_init(node* h_head); // ����κθ� �ʱ�ȭ �Լ�
int hash_find(node* h_head, unsigned long long h_Index, unsigned long long blkno); // �ؽ� ���̺��� blkno ��ã�� ==> �ؽ� function�� ���
void hash_add(node* h_head, node*newnode, unsigned long long h_Index, unsigned long long blkno); //�ؽ� ���̺� blkno�� �߰�   ==> �ؽ� ���̺��� mru�� ����
void hash_del(node* node);
int get_hash_index(node* node,int max); // full�϶� ���� �Լ��� h_index�� �޴� �Լ�
int get_hash_blkno(node* node); // full�ϋ� ���� �Լ��� blkno�� ����
node* h_find(node* h_head, unsigned long long h_index, unsigned long long blkno); // full�϶� lru������ ������ ��带 �콬 ���̺��� ã�� �Լ� 
node* h_getLastNode(node* ptr); // �ؽ��� ������ ��� �޴� �Լ�
node* getnextnode(node* ptr);

int ref(unsigned long long blkno);

int main() {
    printf("2q start\n");
    LARGE_INTEGER start_time, end_time, frequency;
    double elapsed_time = 0, total_time = 0;

    init_head(&Am_head); // lru ����
    init_head(&A1in_head); // FIFO ����
    init_head(&A1out_head); // FIFO ����
    hash_init(Am_h_head,Am_h_max);
    hash_init(A1in_h_head,A1in_h_max);
    hash_init(A1out_h_head,A1out_h_max);
    //node keep node �ʱ�ȭ
    list_node_save(&s_am,Am_MAX);
    list_node_save(&s_a1in, A1in_MAX);
    list_node_save(&s_a1out, A1out_MAX);
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
    
    double rate = (double)(Am_hit + A1in_hit) * 100 / curtime;
    printf("Am_hit=%llu \n", Am_hit);
    printf("A1in_hit=%llu \n", A1in_hit);
    printf("A1out_hit=%llu\n", A1out_hit);
    printf("hit=%llu\n", Am_hit + A1in_hit);
    printf("miss=%llu\n", miss);
    printf("curtime=%llu\n", curtime);
    printf("am_c=%d a1in=%d a1out=%d\n", am_c, a1in_c, a1out_c);
    printf("rate= %.3f\n",rate);
    double avg_e_time = total_time / curtime;
    printf("��� ���� �ð�: %.2f ����ũ����\n", avg_e_time);

    FILE* out;
    out = fopen("2q ref_trace.txt", "w");
    fprintf(out, "2q\n");
    fprintf(out, "ref_number=%d\n", curtime);
    fprintf(out, "Am_hit=%d\n", Am_hit);
    fprintf(out, "A1in_hit=%d \n", A1in_hit);
    fprintf(out, "A1out_hit=%d \n", A1out_hit);
    fprintf(out, "miss=%d\n", miss);
    fclose(out);

    return 0;
}

void init_head(node* head) { // ���� �Ϸ�
    head->next = head;
    head->prev = head;
    head->blkno = NULL;
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
    ptr = ptr->h_next;
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

void print() {
    node* Am_ptr = &Am_head;
    Am_ptr = Am_ptr->next;
    printf("\nAm_head=>");
    do {
        printf("%d->", Am_ptr->blkno);
        Am_ptr = Am_ptr->next;
    } while (Am_ptr != &Am_head);
    printf("\n");
    printf("\n");
    node* A1out_ptr = &A1out_head;
    A1out_ptr = A1out_ptr->next;

    printf("A1out_head=>");
    do {
        printf("%d->", A1out_ptr->blkno);
        A1out_ptr = A1out_ptr->next;
    } while (A1out_ptr != &A1out_head);
    printf("\n");
    printf("\n");
    node* A1in_ptr = &A1in_head;
    A1in_ptr = A1in_ptr->next;

    printf("A1in_head=>");
    do {
        printf("%llu->", A1in_ptr->blkno);
        A1in_ptr = A1in_ptr->next;
    } while (A1in_ptr != &A1in_head);
    printf("\n");
    printf("\n");
    printf("am_c=%d a1in=%d a1out=%d\n", am_c, a1in_c, a1out_c);
    printf("--------------------------\n");
    printf("Am_hit=%d ", Am_hit);
    printf("A1in_hit=%d ", A1in_hit);
    printf("A1out_hit=%d ", A1out_hit);
    printf("miss=%d\n", miss);
    printf("curtime=%d\n", curtime);
}

void hash_head_init(node* h_head) { // ����κθ� �ʱ�ȭ �Լ�
    h_head->h_next = h_head;
    h_head->h_prev = h_head;
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

int s_getlastnode(node* head) {
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
}

/*�ؽ� function*/
void hash_init(node* h_head,int max) {
    for (int i = 0; i < max; i++) {
        hash_head_init(&h_head[i]);
        h_head[i].blkno = NULL;
    }
}

int hash_find(node* h_head, unsigned long long h_Index, unsigned long long blkno) {
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
    } ;
    return h_done;
}

void hash_add(node* h_head, node*newnode, unsigned long long h_Index, unsigned long long blkno) {
    node *ptr = &h_head[h_Index];
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

node* h_find(node* h_head, unsigned long long h_index, unsigned long long blkno) { // full�϶� lru������ ������ ��带 �콬 ���̺��� ã�� �Լ�
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
    unsigned long long am_hash = blkno % Am_h_max;
    unsigned long long a1in_hash = blkno % A1in_h_max;
    unsigned long long a1out_hash = blkno % A1out_h_max;
    unsigned long long Am_done = 0, A1in_done = 0, A1out_done = 0; // blkno �� �����ϴ� üũ�ϴ� ����
    A1in_done = hash_find(A1in_h_head,a1in_hash, blkno);
    Am_done = hash_find(Am_h_head, am_hash, blkno);
    A1out_done = hash_find(A1out_h_head, a1out_hash, blkno);
    node* ptr; // ���� ��� ������ ����
    //printf("%llu ", blkno);
    if (Am_done == 1) // blkno�� Am�� ����
    {  
        ptr = h_find(Am_h_head,am_hash, blkno);  // �ؽ��� ã�� �ɷ� �ٲپ����
        move_mru(&Am_head, ptr);
        Am_hit++;
    }
    else if (A1out_done == 1)// blkno�� A1_out ����
    {
        if (am_c == Am_MAX) { 
            ptr = h_find(A1out_h_head, a1out_hash, blkno);
            s_del_node(ptr);
            returnlastnode(&s_a1out,ptr);
            a1out_c--;
            //a1out tail ���� (max)
            ptr = getLastNode(&Am_head);
            s_del_node(ptr);
            returnlastnode(&s_am, ptr);
            ptr = s_getnode(&s_am);
            s_insert_node(&Am_head,ptr, blkno);
            ptr = getnextnode(&Am_head);
            hash_add(Am_h_head, ptr, am_hash, blkno);
        }
        else {
            ptr = h_find(&A1out_h_head, a1out_hash, blkno);
            s_del_node(ptr);
            returnlastnode(&s_a1out, ptr);
            a1out_c--;
            // a1out tail del 
            ptr = s_getnode(&s_am);
            s_insert_node(&Am_head,ptr, blkno);
            ptr = getnextnode(&Am_head);
            hash_add(Am_h_head, ptr, am_hash, blkno);
            am_c++;
        }
        A1out_hit++;
    }
    else if (A1in_done == 1) // blkno�� A1in ����
    {
        A1in_hit++;
        return NULL;
    }
    else // blkno�� ���� �� == ó������ ���Եɶ�  ==> ���⼭�� A1in A1out ����Ʈ�� �ٷ�  ==> A1in �� ����
    {
        if (a1in_c == A1in_MAX) { // A1in �� �� ä������ ��
            if (a1out_c == A1out_MAX) {// A1out�� �� ä���� ���� ��
                ptr = getLastNode(&A1out_head);
                s_del_node(ptr);
                returnlastnode(&s_a1out, ptr);
                a1out_c--;
                //a1out tail ����
                ptr = getLastNode(&A1in_head);
                int re_blkno = get_blkno(ptr);
                int re_hash = re_blkno % A1out_h_max;
                s_del_node(ptr);
                returnlastnode(&s_a1in,ptr);
                a1in_c--;
                //a1in's tail delete
                ptr=s_getnode(&s_a1out);
                s_insert_node(&A1out_head,ptr, re_blkno);
                ptr = getnextnode(&A1out_head);
                hash_add(A1out_h_head, ptr, re_hash,re_blkno);
                a1out_c++;
                //a1out �� a1in's tail insert
            }
            else {
                ptr = getLastNode(&A1in_head);
                int re_blkno = get_blkno(ptr);
                int re_hash = re_blkno % A1out_h_max;
                s_del_node(ptr);
                returnlastnode(&s_a1in, ptr);
                a1in_c--;
                // a1in tail del
                ptr=s_getnode(&s_a1out);
                s_insert_node(&A1out_head, ptr,re_blkno);
                ptr = getnextnode(&A1out_head);
                hash_add(A1out_h_head, ptr,re_hash, re_blkno);
                a1out_c++;
            }
        }
        ptr = s_getnode(&s_a1in);
        s_insert_node(&A1in_head, ptr, blkno); // s node ���� �Ϸ�
        ptr = getnextnode(&A1in_head);
        hash_add(A1in_h_head,ptr, a1in_hash, blkno);
        a1in_c++;
        miss++;
    }

}

