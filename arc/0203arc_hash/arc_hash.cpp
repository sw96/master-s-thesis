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
// 임의로 정함 recliT2 하기전에
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

void init_head(node* head);// 헤드 변수만 초기화
int find_check(node* head, unsigned long long blkno); //blkno가 리스트에 존재하는 검사하는 함수
void insert_node(node* head, unsigned long long blkno); // 
void del_node(node* node);// 해당 노드 삭제
node* find(node* head, unsigned long long blkno);// 해당 blkno 노드 검색하는 함수
node* getLastNode(node* ptr); // 마지막 노드 넣기
int get_blkno(node* node);
void print();
void move_mru(node* head, node* node); // 원래 리스트에서 mru 로 이동
void move_list_mru(node* head, node* node); // hot cache 로 이동

// node 초기화 
void list_node_save(node* head, int max);
node* s_getlastnode(node* head); // save node 의 마지막 노드 가져오기
void returnlastnode(node* head, node* ptr); //svae node 에 노드 반납
void s_insert_node(node* head, node* ptr, unsigned long long blkno);
void s_del_node(node* node);
node* s_getnode(node* head);

//hash function
void hash_init(node* h_head, int max); // 해쉬 해더 초기화
void hash_head_init(node* h_head); // 헤더부부만 초기화 함수
int hash_find(node* h_head, int h_Index, unsigned long long blkno); // 해쉬 테이블에서 blkno 값찾기 ==> 해쉬 function을 사용
void hash_add(node* h_head, node* newnode, int h_Index, unsigned long long blkno); //해쉬 테이블에 blkno값 추가   ==> 해쉬 테이블의 mru에 삽입
void hash_del(node* node);
int get_hash_index(node* node, int max); // full일때 지울 함수의 h_index를 받는 함수
int get_hash_blkno(node* node); // full일떄 지울 함수의 blkno를 받음
node* h_find(node* h_head, int h_index, unsigned long long blkno); // full일때 lru에서의 마지막 노드를 헤쉬 테이블에서 찾는 함수 
node* h_getLastNode(node* ptr); // 해쉬의 마지막 노드 받는 함수
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
    for (int i = 0; i < 100; i++) { // 10번 반복

        random = rand() % 9; // 난수 생성
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
    ref(5); print(); // T2=>B2 이동 but 0이 2번 b2로 insert 됨 수정필요
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
        QueryPerformanceFrequency(&frequency); // 타이머의 주파수 얻기
        QueryPerformanceCounter(&start_time);  // 코드 실행 전 시간 측정 시작
        ref(blkno);
        QueryPerformanceCounter(&end_time); // 코드 실행 후 시간 측정 종료
        elapsed_time = (double)(end_time.QuadPart - start_time.QuadPart) / frequency.QuadPart * 1000000.0;
        //printf("코드 실행 시간: %.2f 마이크로초\n", elapsed_time);

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
    printf("평균 실행 시간: %.2f 마이크로초\n", avg_e_time);

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

void init_head(node* head) { // 수정 완료
    head->next = head;
    head->prev = head;
    head->blkno = 9999999;
}

int find_check(node* head, unsigned long long blkno) { // 이거 상관없음 헤쉬 쓰면 어차피 안씀
    node* ptr = head;
    int done = 0; // find 체크 
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

    if (head->next == head) { // 비어있을때 ( head의 next가 head를 가르침) == 처음 insert일때 
        newNode->prev = head;
        newNode->next = head;
        head->prev = newNode;
        head->next = newNode;
    }
    else { // 처음 insert 아닐 때 
        newNode->next = head->next;
        newNode->prev = head;
        head->next->prev = newNode;
        head->next = newNode;
    }
}

void del_node(node* node) {  // 수정 완료
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

node* find(node* head, unsigned long long blkno) { // 수정 완료
    node* ptr = head;
    do {
        if (ptr->blkno == blkno) {
            return ptr;
            break;
        }
        ptr = ptr->next;
    } while (ptr != head);
    return NULL;// 없을때는 null리턴
}

node* getLastNode(node* ptr) // 수정 완료
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
    // lasthead 리스트에서 next prev 포인터 앞 뒤로 연결
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
    // hash 노드 list 노드 next prev 포인터 앞 뒤로 연결
    if (head->next == head) {  // 새 노드 isnert시 비어있을 경우
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
} // save node 의 마지막 노드 가져오기

void returnlastnode(node* head, node* ptr) {
    if (head->next == head) { // save node에 아무것도 없는 경우
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
} //svae node 에 노드 반납

void s_insert_node(node* head, node* ptr, unsigned long long blkno) {
    ptr->blkno = blkno;// blknok save node의 blkno에 삽입
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
void hash_head_init(node* h_head) { // 헤더부부만 초기화 함수
    h_head->h_next = h_head;
    h_head->h_prev = h_head;
}

/*해쉬 function*/
void hash_init(node* h_head, int max) {
    for (int i = 0; i < max; i++) {
        hash_head_init(&h_head[i]);
        h_head[i].blkno = 999999999;
    }
}

int hash_find(node* h_head, int h_Index, unsigned long long blkno) {
    int h_done = 0; // 헤쉬 히트 변수
    node* ptr = &h_head[h_Index];
    ptr = ptr->h_next;
    while (ptr != &h_head[h_Index])
    {
        if (ptr->blkno == blkno) {
            h_done = 1;  // 해당 헤쉬 노드에 존재할때 done으로 1을 리턴함
            break;
        }
        ptr = ptr->h_next; // 03 31 에러 ptr=ptr->next를 ptr=ptr->h_next로 변경 
    };
    return h_done;
}

void hash_add(node* h_head, node* newnode, int h_Index, unsigned long long blkno) {
    node* ptr = &h_head[h_Index];
    if (ptr->h_next == ptr) {//해쉬 테이블에 처음 삽입할때
        newnode->h_next = ptr;
        newnode->h_prev = ptr;
        ptr->h_next = newnode;
        ptr->h_prev = newnode;
    }
    else { // 처음 삽입이 아닌 경우 
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

int get_hash_blkno(node* node) { // full일떄 지울 함수의 blkno를 받음
    int re_blkno = node->blkno;
    return re_blkno;
}

node* h_find(node* h_head, int h_index, unsigned long long blkno) { // full일때 lru에서의 마지막 노드를 헤쉬 테이블에서 찾는 함수
    node* ptr = &h_head[h_index];
    do {
        if (ptr->blkno == blkno) {
            return ptr;
            break;
        }
        ptr = ptr->h_next;
    } while (ptr != &h_head[h_index]);
}

node* h_getLastNode(node* ptr) // 추가 헤쉬의 마지막 노드 받기 함수
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
        int return_hash = return_blkno % b1_h_max; //t1_h_max에서 b1_h_max로 수정 8.30
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
            int return_hash = return_blkno % b2_h_max; //t1_h_max에서 b1_h_max로 수정 8.30
            s_del_node(ptr); 
            returnlastnode(&s_T_head,ptr);
            t2_c--;
            ptr = s_getnode(&s_B_head); // 이 부분에서 s_B_head에 B2_head 데이터가 삽입됨 이유잘 모르곘음
            s_insert_node(&b2_head,ptr, return_blkno); // s_b_head 에 삽입됨 check필요
            ptr = getnextnode(&b2_head);
            hash_add(b2_h_head, ptr, return_hash, return_blkno);
            b2_c++;
    }
}

int ref(unsigned long long blkno) {
    int T2_done = 0, T1_done = 0, b2_done = 0, b1_done = 0; // blkno 가 존재하는 체크하는 변수
    int T1_hash = blkno % T1_h_max;
    int T2_hash = blkno % T2_h_max;
    int b1_hash = blkno % b1_h_max;
    int b2_hash = blkno % b2_h_max;
    T2_done = hash_find(T2_h_head, T2_hash, blkno);
    T1_done = hash_find(T1_h_head, T1_hash, blkno);
    b2_done = hash_find(b2_h_head, b2_hash, blkno);
    b1_done = hash_find(b1_h_head, b1_hash, blkno);
    //printf("T2_done=%d T1_done=%d B2_done=%d B1_done=%d miss=%llu blkno=%llu ratio=%d \n", T2_done, T1_done, b2_done, b1_done, miss,blkno,ratio);

    node* ptr; // 제거 노드 포인터 변수
    if (T2_done == 1) // blkno가 T2에 존재
    {
        T2_hit++;
        ptr = h_find(T2_h_head, T1_hash, blkno);
        move_mru(&T2_head, ptr);
    }
    else if (T1_done == 1) // blkno가 t1 존재
    {
        T1_hit++;
        ptr = h_find(T1_h_head, T1_hash, blkno);
        t1_c--;
        hash_del(ptr);
        move_mru(&T2_head, ptr); // 해결 완료
        ptr = getnextnode(&T2_head);
        hash_add(T2_h_head, ptr, T2_hash, blkno);
        t2_c++;
    }
    else if (b1_done == 1)// blkno가 b1 존재
    {
        B1_hit++;
        ratio = min(capacity, ratio + max(1, (b2_c / b1_c)));
        replace(blkno, b2_done, &T1_head, &T2_head, ratio); //t2_insert
        //b1에서 삭제 
        ptr = h_find(b1_h_head, b1_hash, blkno);
        s_del_node(ptr);
        returnlastnode(&s_B_head, ptr); //이 포인트 지나면 009됨
        b1_c--;
        //b1 list 리턴

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

    else // blkno가 없을 때 == 처음으로 삽입될때  ==> 여기서는 A1in b1 리스트만 다룸  ==> A1in 에 삽입
    {
        miss++;
        if ((t1_c + b1_c) == capacity) {// t1 & b1 의 사이즈가 다 채워졌을 경우
            if (t1_c < capacity) {// t1 이 용량을 모두 채운 경우가 아닌 경우
                ptr = getLastNode(&b1_head);
                s_del_node(ptr);//b1'tail remove // error point 
                returnlastnode(&s_B_head, ptr);
                b1_c--;
                replace(blkno, b2_done, &T1_head, &T2_head, ratio);
            }
            else { // t1이 다 채운 경우 
                ptr = getLastNode(&T1_head);
                s_del_node(ptr);
                returnlastnode(&s_T_head, ptr);
                t1_c--;
            }
        }
        else {
            //sm==> t1 t2 b1 b2 의 모든 용량
            int sm = (t1_c + t2_c + b1_c + b2_c);
            if (sm >= capacity) {// 총 용량이 원래의 용량보다 큰 경우
                if (sm == (2 * capacity)) {// sm 이 원래 용량보다 2배가 되는 경우
                    ptr = getLastNode(&b2_head);
                    s_del_node(ptr);//b2 ' tail remove
                    returnlastnode(&s_B_head, ptr);
                    b2_c--;
                }
                replace(blkno, b2_done, &T1_head, &T2_head, ratio);
            }
        }
        ptr = s_getnode(&s_T_head);
        s_insert_node(&T1_head, ptr, blkno); // s node 수정 완료
        ptr = getnextnode(&T1_head);
        hash_add(T1_h_head, ptr, T1_hash, blkno);
        t1_c++;
    }
    //printf("T2_done=%d T1_done=%d B2_done=%d B1_done=%d miss=%llu blkno=%llu ratio=%d \n", T2_done, T1_done, b2_done, b1_done, miss, blkno, ratio);

    //printf("T2_done=%d T1_done=%d B2_done=%d B1_done=%d miss=%llu", T2_done, T1_done, b2_done, b1_done, miss);
    return 0;
}
