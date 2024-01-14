#define _CRT_SECURE_NO_WARNINGS
#include<time.h>

#include<stdio.h>
#include<string.h>
#include <stdlib.h>
#include<stddef.h>
#include <string.h>
#include <windows.h> // 마이크로 시간 측정 헤더 파일
#include <time.h>

#define MAX   256106
#define H_MAX 256106

unsigned long long hit = 0, miss = 0;
int done = 0; // find 함수 변수 hit=>1 // miss=>0
int c = 0; // lru총 갯수 카운트
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
node* getLastNode(node* ptr);// 마지막 노드 얻기
void init_head(node* head);// 헤드 변수만 초기화k
int find_check(node* head, unsigned long long blkno); //blkno가 리스트에 존재하는 검사하는 함수
void insert_node(node* head, unsigned long long blkno); // 
void del_node(node* node);// 해당 노드 삭제
int ref(unsigned long long blkno);

void move_mru(node* head, node* node); // 원래 리스트에서 mru 로 이동


// node 초기화 
void list_node_save(node* head, int max);
int s_getlastnode(node* head); // save node 의 마지막 노드 가져오기
void returnlastnode(node* head, node* ptr); //svae node 에 노드 반납
void s_insert_node(node* head, node* ptr, unsigned long long blkno);
void s_del_node(node* node);
node* s_getnode(node* head);

/*해쉬 노드 포인터 함수*/ // 위의 함수에서 재사용 함수는 h를 추가해서 재사용
void hash_init(node* h_head); // 해쉬 해더 초기화
void hash_head_init(node* h_head); // 헤더부부만 초기화 함수
int hash_find(node* h_head, unsigned long long h_Index, unsigned long long blkno); // 해쉬 테이블에서 blkno 값찾기 ==> 해쉬 function을 사용
void hash_add(node* h_head, unsigned long long h_Index, unsigned long long blkno); //해쉬 테이블에 blkno값 추가   ==> 해쉬 테이블의 mru에 삽입
void hash_del(node* h_head); //해쉬 테이블에 blkno값 제거   ==> 해쉬 테이블의 lru만 제거 

int get_hash_index(node* node); // full일때 지울 함수의 h_index를 받는 함수
int get_hash_blkno(node* node); // full일떄 지울 함수의 blkno를 받음
node* h_find(node* h_head, unsigned long long h_index, unsigned long long blkno); // full일때 lru에서의 마지막 노드를 헤쉬 테이블에서 찾는 함수 
node* h_getLastNode(node* ptr); // 해쉬의 마지막 노드 받는 함수
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
            QueryPerformanceFrequency(&frequency); // 타이머의 주파수 얻기
            QueryPerformanceCounter(&start_time);  // 코드 실행 전 시간 측정 시작

            ref(blkno);

            QueryPerformanceCounter(&end_time); // 코드 실행 후 시간 측정 종료
            elapsed_time = (double)(end_time.QuadPart - start_time.QuadPart) / frequency.QuadPart * 1000000.0;
            //printf("코드 실행 시간: %.2f 마이크로초\n", elapsed_time);

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
            QueryPerformanceFrequency(&frequency); // 타이머의 주파수 얻기
            QueryPerformanceCounter(&start_time);  // 코드 실행 전 시간 측정 시작

            ref(blkno);

            QueryPerformanceCounter(&end_time); // 코드 실행 후 시간 측정 종료
            elapsed_time = (double)(end_time.QuadPart - start_time.QuadPart) / frequency.QuadPart * 1000000.0;
            //printf("코드 실행 시간: %.2f 마이크로초\n", elapsed_time);

            total_time += elapsed_time;
            curtime++;
        }
    }jump1:
    fclose(fp1);
    */

    /*
    FILE* file = fopen("Financial2.spc", "r"); // 파일 경로를 적절히 수정
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
    

    printf("c= %d\n", c);
    printf("hit= %llu\n", hit);
    printf("miss=%llu\n", miss);
    printf("total=%d\n", curtime);
    double rate = (double)hit * 100 / curtime;
    printf("rate=%.3f\n", rate);
    double avg_e_time = total_time / curtime;
    printf("평균 실행 시간: %.2f 마이크로초\n", avg_e_time);
    return 0;
}

node* getnextnode(node* ptr) {
    node* cur;
    cur = ptr->next;
    return cur;
}

node* getLastNode(node* ptr) // 수정 완료
{
    node* cur;
    cur = ptr->prev;
    return cur;
}

node* h_getLastNode(node* ptr) // 추가 헤쉬의 마지막 노드 받기 함수
{
    node* cur;
    cur = ptr->h_prev;
    return cur;
}

void init_head(node* head) { // 수정 완료
    head->next = head;
    head->prev = head;
    head->blkno = NULL;
}

int find_check(node* head, unsigned long long blkno) { // 이거 상관없음 헤쉬 쓰면 어차피 안씀
    node* ptr = head;
    int done = 0; // find 체크 변수

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
        head->prev = newNode; // 에러 해결 첨에 이 head 포인터 부분을 작성을 안해서 헤더 다음 노드가 들어가지 않아서 miss가 카운트 됨
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

void hash_head_init(node* h_head) { // 헤더부부만 초기화 함수
    h_head->h_next = h_head;
    h_head->h_prev = h_head;
}

void move_mru(node* head, node* node) {
    node->next->prev = node->prev;
    node->prev->next = node->next;
    // lasthead 리스트에서 next prev 포인터 앞 뒤로 연결
    node->next = head->next;
    node->prev = head;
    head->next->prev = node;
    head->next = node;
}

/*해쉬 function*/
void hash_init(node* h_head) {
    for (int i = 0; i < H_MAX; i++) {
        hash_head_init(&h_head[i]);
        h_head[i].blkno = NULL;
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
    } 
    return h_done;
}



//단순하게 h_head 뒤에 계속 삽입
void hash_add(node* h_head, node* newNode,int h_Index, unsigned long long blkno) {
    node* ptr = &h_head[h_Index];
    if (ptr->h_next == &h_head[h_Index]) { // 해쉬에 처음으로 삽입할때  에러 03 31 헤쉬 삽입시 하나 이상 삽입 안되는 에러 발생 if 문에서 =으로 조건을 주었음 ==으로 수정
        newNode->h_prev = ptr;
        newNode->h_next = ptr;
        ptr->h_next = newNode;
        ptr->h_prev = newNode;
    }
    else  // 무엇인가 있어 여기  *** mru=head->next 임 
    {
        newNode->h_next = ptr->h_next;
        newNode->h_prev = ptr;
        ptr->h_next->h_prev = newNode; // 04 01 ERROR 내 생각에는 이 부분이 에러인거같음
        // 더블 링크 리스트일때 NEXT포인터를 먼저 설정함
        ptr->h_next = newNode;
    }
}

void hash_del(node* node) {
    if (node->h_next == &h_head) {  // 마지작 노드일때 ==> 원래node->next==NULL ==> node->next==head 로 수정
        node->h_prev->h_next = &h_head;
        node->h_prev = node->h_next->h_prev;
        node->h_next = NULL;
        node->h_prev = NULL;
    }
    else { // 마지막 노드가 아닐 때
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

int get_hash_blkno(node* node) { // full일떄 지울 함수의 blkno를 받음
    int re_blkno = node->blkno;
    return re_blkno;
}
node* h_find(node* h_head, int h_index, unsigned long long blkno) { // full일때 lru에서의 마지막 노드를 헤쉬 테이블에서 찾는 함수
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
} // save node 의 마지막 노드 가져오기

void returnlastnode(node* head,node* ptr) {
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

void s_insert_node(node* head,node* ptr, unsigned long long blkno) {
    ptr->blkno = blkno;// blknok save node의 blkno에 삽입
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
    unsigned long long h_index = blkno % H_MAX; // 해쉬 값 function 
    int h_done = hash_find(h_head, h_index, blkno);
    node* ptr = NULL; // find 노드용 포인터 변수

    if (h_done == 1) {// 2. hash에 blkno가 존재할때
        hit++;
        ptr = h_find(h_head, h_index, blkno);
        move_mru(&head, ptr);
    }

    else { // 3. hash 에 존재하지 않는 경우
        // lru 에 삽입(mru)  and hash 삽입 
        miss++;
        if (c == MAX) { // lru가 full 인가 고려 
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
        else { // lru 가 full이 아님 
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