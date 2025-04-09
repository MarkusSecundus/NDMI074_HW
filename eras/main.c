#include<stdio.h>
#include<time.h>
#include<limits.h>
#include<stdint.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<math.h>

#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof(*(arr)))

typedef unsigned eint_t;

bool get_bit(const char *const bitarray, const size_t idx){
    const char byte = bitarray[idx>>3];
    return !!(byte & (1<< (idx&0x7)));
}
void set_bit(char *const bitarray, const size_t idx, const bool value){
    char *const byte = &bitarray[idx>>3];
    char mask = (1<< (idx&0x7));
    if(value){
        *byte |= mask;
    }else{
        *byte &= ~mask;
    }
}

void eratosthenes_bitmap(size_t count, eint_t *result_destination, size_t *result_size){
    const size_t flags_size = (count + 2)/8;
    char *const flags = malloc(flags_size * sizeof(char));
    memset(flags, 0, flags_size*sizeof(char));
    for(eint_t p = 2; p < count; ++p){
        if(get_bit(flags, p)) continue;
        for(eint_t t = p * 2; t < count; t += p){
            set_bit(flags, t, 1);
        }
    }
    
    size_t res_size = 0;
    for(eint_t p = 2; p < count; ++p){
        if(! get_bit(flags, p)){
            if(result_destination)
                result_destination[res_size] = p;
            ++res_size;
        }
    }
    *result_size = res_size;
}


void eratosthenes_bitmap_sqrt(size_t count, eint_t *result_destination, size_t *result_size){
    const size_t flags_size = (count + 2)/8;
    char *const flags = malloc(flags_size * sizeof(char));
    memset(flags, 0, flags_size*sizeof(char));
    eint_t sq = (eint_t)(sqrt(count)) + 1;
    for(eint_t p = 2; p < sq; ++p){
        if(get_bit(flags, p)) continue;
        for(eint_t t = p * 2; t < count; t += p){
            set_bit(flags, t, 1);
        }
    }
    
    size_t res_size = 0;
    for(eint_t p = 2; p < count; ++p){
        if(! get_bit(flags, p)){
            if(result_destination)
                result_destination[res_size] = p;
            ++res_size;
        }
    }
    *result_size = res_size;
}

void eratosthenes_ref(size_t count, eint_t *result_destination, size_t *result_size){
    char *const flags = malloc((count + 2)*sizeof(char));
    memset(flags, 0, (count + 2)*sizeof(char));
    for(eint_t p = 2; p < count; ++p){
        if(flags[p]) continue;
        for(eint_t t = p * 2; t < count; t += p){
            flags[t] = 1;
        }
    }
    
    size_t res_size = 0;
    for(eint_t p = 2; p < count; ++p){
        if(! flags[p]){
            if(result_destination)
                result_destination[res_size] = p;
            ++res_size;
        }
    }
    *result_size = res_size;
}

static const size_t TEST_COUNT = 10000000;
static const size_t BENCH_COUNT = 1000000000;

void test(void){
#define eratosthenes_test eratosthenes_bitmap_sqrt

    eint_t  *result_ref = malloc(TEST_COUNT * sizeof(eint_t)),
            *result     = malloc(TEST_COUNT * sizeof(eint_t));

    size_t result_count_ref = 0, result_count = 0;

    eratosthenes_ref(TEST_COUNT, result_ref, &result_count_ref);
    printf("Ref did run\n");
    eratosthenes_test(TEST_COUNT, result, &result_count);
    printf("Test did run\n");

    if(result_count != result_count_ref){
        printf("Result count %lu != %lu\n", result_count, result_count_ref);
        return;
    }
    for(size_t t = 0;t<result_count;++t){
        if(result[t] != result_ref[t]){
            printf("%lu| %u != %u\n", t, result[t], result_ref[t]);
        }
    }
    printf("TEST FINISHED!\n");


    result = malloc(BENCH_COUNT * sizeof(eint_t));
    struct timespec start, finish;
    clock_gettime(CLOCK_REALTIME, &start);
    const size_t ITERATIONS_COUNT = 1;
    for(size_t it = 0; it < ITERATIONS_COUNT ; ++it){
        eratosthenes_test(BENCH_COUNT, result, &result_count);
    }
    clock_gettime( CLOCK_REALTIME, &finish);
    long long unsigned duration_ns = (finish.tv_sec*1000000000 + finish.tv_nsec) - (start.tv_sec*1000000000 + start.tv_nsec)/ITERATIONS_COUNT;
    printf("benchmark time: %llu ns = %.9f s\n", duration_ns, duration_ns * (1.0/ 1000000000.0));
}

void bitarray_test(void){
    char buff[500] = {};
    for(size_t t=0;t<500;++t){
        printf("%u,", get_bit(buff, t));
        set_bit(buff, t, 1);
        printf("%u|%2x|", get_bit(buff, t), buff[t>>3]&0xFF);
        set_bit(buff, t, 0);
        printf("%u\n", get_bit(buff, t));
    }
}


int main(void){
    size_t in, ret;
    scanf("%lu", &in);
    eratosthenes_bitmap_sqrt(in, NULL, &ret);
    printf("%lu", ret);

    return 0;
}