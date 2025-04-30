#include<stdio.h>
#include<time.h>
#include<limits.h>
#include<stdint.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<math.h>

#define min(a,b) (((a) < (b)) ? (a) : (b))

#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof(*(arr)))

typedef size_t eint_t;

bool get_bit(const char *const bitarray, const size_t idx){
    const char byte = bitarray[idx>>3];
    return !!(byte & (1<< (7 - (idx&0x7))));
}
void set_bit(char *const bitarray, const size_t idx, const bool value){
    char *const byte = &bitarray[idx>>3];
    char mask = (1<< (7 - (idx&0x7)));
    if(value){
        *byte |= mask;
    }else{
        *byte &= ~mask;
    }
}

void eratosthenes_bitmap_sqrt_halved(size_t count, eint_t *result_destination, size_t *result_size){
    const size_t flags_size = (count + 2)/(8 * 2);
    char *const flags = malloc(flags_size * sizeof(char));
    memset(flags, 0, flags_size*sizeof(char));

    eint_t sq = (eint_t)(sqrt(count)) + 1;

    for(eint_t p = 3; p < sq; p += 2){
        //printf("p: %lu\n", p);
        if(get_bit(flags, p>>1)) continue;
        for(eint_t t = p * p; t < count; t += 2*p){
            set_bit(flags, t>>1, 1);
        }
    }
    
    size_t res_size = 0;

    if(count >= 2){
        if(result_destination)
            result_destination[res_size] = 2;
        ++res_size;
    }
    for(eint_t p = 3; p < count; p += 2){
        if(! get_bit(flags, p>>1)){
            if(result_destination)
                result_destination[res_size] = p;
            ++res_size;
        }
    }
    *result_size = res_size;
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
        printf("p: %lu\n", p);
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




char* bits_tos(uint64_t num){
    char* ret = malloc(1024);
    char* it = ret;
    for(uint64_t t = 0; t < 64;++t){
        if(num & (1<<t)){
            it += sprintf(it, "%lu,", t);
        }
    }
    return ret;
}

void eratosthenes_bitmap_sqrt_halved_pseudovec(size_t count, eint_t *result_destination, size_t *result_size){
    const size_t flags_size = (count + 2)/(8 * 2);
    char *const flags = malloc((flags_size+8) * sizeof(char));
    const size_t PREFILLED_SIZE = 256*128 + 512;
    char *const prefilled = malloc(PREFILLED_SIZE);
    memset(flags, 0, flags_size*sizeof(char));

    eint_t sq = (eint_t)(sqrt(count)) + 1;

    const eint_t FAST_MAX = 20;

    for(eint_t p = 3; p < FAST_MAX && p < sq; p+=2){
        if(get_bit(flags, p>>1)) continue;

        uint64_t *const flags_wide = (uint64_t*)flags;
        uint64_t *const prefilled_wide = (uint64_t*)prefilled;
        memset(prefilled, 0, (p+10)*8);
        for(eint_t t = p; t <= (p+10)*128; t += 2*p){
            set_bit(prefilled, t>>1, 1);
        }
        for(eint_t a = 0; a < PREFILLED_SIZE/8; ++a){
            for(eint_t b = a + 1; b < PREFILLED_SIZE/8; ++b){
                if(prefilled_wide[a] == prefilled_wide[b]){
                    printf("%lu: found cycle %lu -> %lu (%lx)\n", p, a, b, prefilled_wide[a]);
                    goto endcycle;
                }
            }
        }
        endcycle:

        printf("%lu: [0] %lx vs [%lu] %lx\n", p, prefilled_wide[0], p, prefilled_wide[p]);
        for(size_t t=0;t<=p + 10;++t){
            char* bits = bits_tos(prefilled_wide[t]);
            printf("\t%lu: %lx -- %s\n", t, prefilled_wide[t], bits);
            free(bits);
        }


        for(size_t t=0, pr = 0; t<(flags_size/8 + 1); ++t, pr = min(pr + 1, pr + 1 - p)){
            //printf("\t%lu\n", pr);
            flags_wide[t] |= prefilled_wide[pr];
        }
        set_bit(flags, p>>1, 0);
    }

    for(eint_t p = FAST_MAX; p < sq; p += 2){
        if(get_bit(flags, p>>1)) continue;
        for(eint_t t = p * 3; t < count; t += 2*p){
            set_bit(flags, t>>1, 1);
        }
    }
    
    size_t res_size = 0;

    if(count >= 2){
        if(result_destination)
            result_destination[res_size] = 2;
        ++res_size;
    }
    for(eint_t p = 3; p < count; p += 2){
        if(! get_bit(flags, p>>1)){
            if(result_destination)
                result_destination[res_size] = p;
            ++res_size;
        }
    }
    *result_size = res_size;
    free(flags);
    free(prefilled);
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


#define eratosthenes_solution eratosthenes_bitmap_sqrt_halved

static const size_t TEST_COUNT = 100000000;
static const size_t BENCH_COUNT = 1000000000;

void test(void){
#define eratosthenes_test eratosthenes_solution

    eint_t  *result_ref = malloc(TEST_COUNT * sizeof(eint_t)),
            *result     = malloc(TEST_COUNT * sizeof(eint_t));

    size_t result_count_ref = 0, result_count = 0;

    eratosthenes_ref(TEST_COUNT, result_ref, &result_count_ref);
    printf("Ref did run\n");
    eratosthenes_test(TEST_COUNT, result, &result_count);
    printf("Test did run\n");

    printf("Result | Reference\n");
    if(result_count != result_count_ref){
        printf("Result count %lu != %lu\n", result_count, result_count_ref);
    }
    unsigned errors_count = 0;
    (void)errors_count;
    for(size_t t = 0;t<result_count;++t){
        if(result[t] != result_ref[t]){
            printf("%lu| %lu != %lu\n", t, result[t], result_ref[t]);
            if(++errors_count > 20) return;
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
    //test(); return 0;
    size_t in;
    size_t ret;
    scanf("%lu", &in);
    //eint_t result[in];
    eratosthenes_solution(in, NULL, &ret);
    printf("%lu", ret);
    //for(size_t t=0;t<ret;++t){
    //    printf("%lu\n", result[t]);
    //}

    return 0;
}