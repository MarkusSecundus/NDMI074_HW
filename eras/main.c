#include<stdio.h>
#include<time.h>
#include<limits.h>
#include<stdint.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<math.h>
#include<threads.h>

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) >= (b)) ? (a) : (b))

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

void eratosthenes_bitmap_sqrt_halved_u32(size_t count, uint32_t *result_destination, size_t *result_size){
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
                result_destination[res_size] = (uint32_t)p;
            ++res_size;
        }
    }
    *result_size = res_size;
}

void eratosthenes_bitmap_sqrt_halved_indirect(size_t count, eint_t *result_destination, size_t *result_size){
    const size_t flags_size = (count + 2)/(8 * 2);
    char *const flags = malloc(flags_size * sizeof(char));
    memset(flags, 0, flags_size*sizeof(char));

    eint_t sq = (eint_t)(sqrt(count)) + 1;

    uint32_t *small_primes = malloc(sq * sizeof(uint32_t));
    size_t small_primes_count;
    eratosthenes_bitmap_sqrt_halved_u32(sq, small_primes, &small_primes_count);

    for(size_t i=1; i < small_primes_count; ++i){
        const uint32_t p = small_primes[i];
        for(eint_t step = p * p; step < count; step += 2*p){
            set_bit(flags, step>>1, 1);
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

void eratosthenes_bitmap_sqrt_halved_blocked(size_t count, eint_t *result_destination, size_t *result_size){
    const size_t flags_size = (count + 2)/(8 * 2);
    char *const flags = malloc(flags_size * sizeof(char));
    memset(flags, 0, flags_size*sizeof(char));

    eint_t sq = (eint_t)(sqrt(count)) + 1;

    uint32_t *small_primes = malloc(sq * sizeof(uint32_t));
    size_t small_primes_count;
    eratosthenes_bitmap_sqrt_halved_u32(sq, small_primes, &small_primes_count);

    const size_t BLOCK_SIZE = (1<<22);

    for(eint_t block_start = 0; block_start < count; block_start += BLOCK_SIZE){
        const eint_t block_end = block_start + BLOCK_SIZE;
        for(size_t i=1; i < small_primes_count; ++i){
            const uint32_t p = small_primes[i];
            eint_t step = p*p;
            if(block_start > step){
                step = block_start - (block_start %p);
                if(!(step&1)) step += p;
                if(step < block_start) step += 2*p;
                //static int cnt = 0;
                //if(block_start > 0 && (step % p) == 0 && (++cnt < 20)) printf("!%lu (%u): %lu!\n", block_start, p, step);
            }
            for(; step < block_end; step += 2*p){
                //if(step < block_start) continue;
                set_bit(flags, step>>1, 1);
            }
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



static const size_t THREADED_BLOCK_SIZE = (1<<22);

struct chunk_args{
    eint_t block_start;
    eint_t count;
    char *flags;
    uint32_t  *small_primes;
    size_t  small_primes_count;
};

int chunk(void *arg_void){
    struct chunk_args *args = (struct chunk_args*)arg_void;
    eint_t block_start = args->block_start;
    const eint_t count = args->count;
    char *const flags = args->flags;
    uint32_t *const small_primes = args->small_primes;
    const size_t small_primes_count = args->small_primes_count;

    for(; block_start < count; block_start += THREADED_BLOCK_SIZE){
        const eint_t block_end = block_start + THREADED_BLOCK_SIZE;
        for(size_t i=1; i < small_primes_count; ++i){
            const uint32_t p = small_primes[i];
            eint_t step = p*p;
            if(block_start > step){
                step = block_start - (block_start %p);
                if(!(step&1)) step += p;
                if(step < block_start) step += 2*p;
                //static int cnt = 0;
                //if(block_start > 0 && (step % p) == 0 && (++cnt < 20)) printf("!%lu (%u): %lu!\n", block_start, p, step);
            }
            for(; step < block_end; step += 2*p){
                //if(step < block_start) continue;
                set_bit(flags, step>>1, 1);
            }
        }
    }
    return 0;
}


void eratosthenes_bitmap_sqrt_halved_blocked_threaded(size_t count, eint_t *result_destination, size_t *result_size){
    const size_t flags_size = (count + 2)/(8 * 2);
    char *const flags = malloc(flags_size * sizeof(char));
    memset(flags, 0, flags_size*sizeof(char));

    eint_t sq = (eint_t)(sqrt(count)) + 1;

    uint32_t *small_primes = malloc(sq * sizeof(uint32_t));
    size_t small_primes_count;
    eratosthenes_bitmap_sqrt_halved_u32(sq, small_primes, &small_primes_count);


    static const unsigned THREADS_COUNT = 16;
    thrd_t threads[THREADS_COUNT];
    uint32_t chunk_per_thread = count / THREADS_COUNT;
    chunk_per_thread += THREADED_BLOCK_SIZE - (chunk_per_thread % THREADED_BLOCK_SIZE);
    struct chunk_args thread_args[THREADS_COUNT];
    for(unsigned i = 0; i <THREADS_COUNT;++i){
        thread_args[i].block_start = i * chunk_per_thread;
        thread_args[i].count = min(count, thread_args[i].block_start + chunk_per_thread);
        thread_args[i].flags = flags;
        thread_args[i].small_primes = small_primes;
        thread_args[i].small_primes_count = small_primes_count;
        thrd_create(&threads[i], chunk, &thread_args[i]);
    }

    for(unsigned i = 0; i < THREADS_COUNT;++i){
        thrd_join(threads[i], NULL);
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

void eratosthenes_sqrt_halved_blocked(size_t count, eint_t *result_destination, size_t *result_size){
    const size_t flags_size = (count + 2)/2;
    char *const flags = malloc(flags_size * sizeof(char));
    memset(flags, 0, flags_size*sizeof(char));

    eint_t sq = (eint_t)(sqrt(count)) + 1;

    uint32_t *small_primes = malloc(sq * sizeof(uint32_t));
    size_t small_primes_count;
    eratosthenes_bitmap_sqrt_halved_u32(sq, small_primes, &small_primes_count);

    const size_t BLOCK_SIZE = (1<<20);

    for(eint_t block_start = 0; block_start < count; block_start += BLOCK_SIZE){
        const eint_t block_end = block_start + BLOCK_SIZE;
        for(size_t i=1; i < small_primes_count; ++i){
            const uint32_t p = small_primes[i];
            eint_t step = p*p;
            if(block_start > step){
                step = block_start - (block_start %p);
                if(!(step&1)) step += p;
                if(step < block_start) step += 2*p;
                //static int cnt = 0;
                //if(block_start > 0 && (step % p) == 0 && (++cnt < 20)) printf("!%lu (%u): %lu!\n", block_start, p, step);
            }
            for(; step < block_end; step += 2*p){
                //if(step < block_start) continue;
                flags[step>>1] = 1;
            }
        }
    }
    
    size_t res_size = 0;

    if(count >= 2){
        if(result_destination)
            result_destination[res_size] = 2;
        ++res_size;
    }
    for(eint_t p = 3; p < count; p += 2){
        if(! flags[p>>1]){
            if(result_destination)
                result_destination[res_size] = p;
            ++res_size;
        }
    }
    *result_size = res_size;
}


void eratosthenes_bitmap_sqrt_halved_blocked2_broken(size_t count, eint_t *result_destination, size_t *result_size){
    const size_t BLOCK_SIZE = 8192;
    const size_t flags_size = (count + 2)/(8 * 2);
    char *const flags = malloc(flags_size * sizeof(char));
    memset(flags, 0, flags_size*sizeof(char));

    eint_t sq = (eint_t)(sqrt(count)) + 1;


    for(size_t block_start = 0; block_start < count; block_start += BLOCK_SIZE){
        const size_t outer_end = min(block_start + BLOCK_SIZE, sq);
        const size_t inner_end = min(block_start + BLOCK_SIZE, count);
        for(eint_t p = 3; p < outer_end; p += 2){
            //printf("p: %lu\n", p);
            if(get_bit(flags, p>>1)) continue;
            for(eint_t t = 3*p; t < inner_end; t += 2*p){
                if(t < block_start) continue;
                set_bit(flags, t>>1, 1);
            }
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


int count_primes(size_t n, eint_t *result_destination, size_t *result_size) {
    (void) result_destination;
    const size_t S = 10000;

    eint_t nsqrt = (eint_t)(sqrt(n) + 1);
    char * is_prime = malloc(nsqrt + 2);
    memset(is_prime, ~0, nsqrt + 2);
    eint_t* primes = malloc((nsqrt + 2)*sizeof(eint_t));
    size_t primes_length = 0;
    for (size_t i = 2; i <= nsqrt; i++) {
        if (is_prime[i]) {
            primes[primes_length++] = i;
            for (size_t j = i * i; j <= nsqrt; j += i)
                is_prime[j] = false;
        }
    }

    size_t result = 0;
    char* block = malloc(S);
    
    for (int k = 0; k * S <= n; k++) {
        memset(block, ~0, S);
        int start = k * S;
        for (size_t t = 0; t<primes_length;++t) {
            eint_t p = primes[t];
            size_t start_idx = (start + p - 1) / p;
            size_t j = max(start_idx, p) * p - start;
            for (; j < S; j += p)
                block[j] = false;
        }
        if (k == 0)
            block[0] = block[1] = false;
        for (size_t i = 0; i < S && start + i <= n; i++) {
            if (block[i])
                result++;
        }
    }

    *result_size = result; 

    //size_t res_size = 0;
    //for(eint_t p = 3; p < n; p += 2){
    //    if(! get_bit(flags, p>>1)){
    //        if(result_destination)
    //            result_destination[res_size] = p;
    //        ++res_size;
    //    }
    //}
    //*result_size = res_size;

    free(is_prime);
    free(primes);
    free(block);
    return result;
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


#define eratosthenes_solution eratosthenes_bitmap_sqrt_halved_blocked_threaded

static const size_t TEST_COUNT  = 100000000;
static const size_t BENCH_COUNT = 20000000000;//1000000000;

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