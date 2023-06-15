#include <stdio.h>
#include <stdlib.h>

// 제약조건
// 1. input 으로 주어지는 파일의 한줄은 100자를 넘지 않음.
// 2. input 으로 주어지는 파일의 길이는 100000줄을 넘지 않음. 즉, address trace 길이는 100000줄을 넘지 않음.
#define MAX_ROW     100000
#define MAX_COL     2
#define MAX_INPUT   100

// 구현해야하는 함수
void solution(int cache_size, int block_size, int assoc);
void read_op(int addr, int cache_size, int block_size, int assoc);
void write_op(int addr, int cache_size, int block_size, int assoc);
void fetch_inst(int addr, int cache_size, int block_size, int assoc);

// 전역변수
// 문제를 풀기 위한 힌트로써 제공된 것이며, 마음대로 변환 가능합니다.
enum COLS {
    MODE,
    ADDR
};

int i_total, i_miss;            /* instructino cache 총 접근 횟수, miss 횟수*/
int d_total, d_miss, d_write;   /* data cache 접근 횟수 및 miss 횟수, memory write 횟수 */
int trace[MAX_ROW][MAX_COL] = {{0,0},};
int trace_length = 0;

int main(){
	// DO NOT MODIFY -- START --  //
    // cache size
    int cache[5] = {1024, 2048, 4096, 8192, 16384}; 
    // block size
    int block[2] = {16, 64};
    // associatvity e.g., 1-way, 2-way, ... , 8-way
    int associative[4] = {1, 2, 4, 8};
    int i=0,j=0,k=0; 

    /* 입력 받아오기 */
    char input[MAX_INPUT];
    while (fgets(input, sizeof(input), stdin)) {  
        if(sscanf(input, "%d %x\n", &trace[trace_length][MODE], &trace[trace_length][ADDR]) != 2) {
            fprintf(stderr, "error!\n");
        }
        trace_length++;
    }


    /* 캐시 시뮬레이션 */
    printf("cache size || block size || associative || d-miss rate || i-miss rate || mem write\n");
    for(i=0; i<5; i++){
      	for(j=0; j<2; j++){
            for(k=0; k<4; k++){
                solution(cache[i], block[j], associative[k]);
            }
        }
    }
	// DO NOT MODIFY -- END --  //
    return 0;
}

void solution(int cache_size, int block_size, int assoc) {
	
	// DO NOT MODIFY -- START --  //
    int mode, addr;
    double i_miss_rate, d_miss_rate;    /* miss rate을 저장하는 변수 */

    int index = 0;
    while(index != trace_length) {
        mode = trace[index][MODE];
        addr = trace[index][ADDR];

        switch(mode) {
            case 0 :
                read_op(addr, cache_size, block_size, assoc);
                d_total++;	
                break;
            case 1 :
                write_op(addr, cache_size, block_size, assoc);
                d_total++;	
                break;
            case 2 :
                fetch_inst(addr, cache_size, block_size, assoc);
                i_total++;	
                break;
        }
        index++;
    }
   	// DO NOT MODIFY -- END --  //
	
    // hint. data cache miss rate 와 intruction cache miss rate를 계산하시오. 
    // ? 에는 알맞는 변수를 넣으면 됩니다.
    /*
        i_miss_rate = ? / ? 
        d_miss_rate = ? / ? 
    */
	
	// DO NOT MODIFY -- START --  //
    printf("%8d\t%8d\t%8d\t%.4lf\t%.4lf\t%8d\n", cache_size, block_size, assoc, d_miss_rate, i_miss_rate, d_write);
	// DO NOT MODIFY -- END --  //
}

// 아래 함수를 직접 구현하시오, 차례로 읽기, 쓰기, 그리고 인스트럭션 fetch 동작입니다.
void read_op(int addr, int cache_size, int block_size, int assoc){
	return;
}


void write_op(int addr, int cache_size, int block_size, int assoc){
	return;
}

void fetch_inst(int addr, int cache_size, int block_size, int assoc){
	return;
}

// hint. LRU 알고리즘 교체 정책을 구현하기 위한 함수도 작성하셔서 적용하면 됩니다.