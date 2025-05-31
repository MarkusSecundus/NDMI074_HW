#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

#define debug(...) fprintf(stderr, __VA_ARGS__)

typedef unsigned int num_t;

static void load_file(FILE *f, num_t **out_data, size_t *out_count){
    size_t allocated_size = 16;
    num_t *data = malloc(allocated_size * sizeof(num_t));
    size_t count = 0;
    for(num_t num; fscanf(f, "%u", &num), !feof(f); ){
        if(count >= allocated_size - 1){
            allocated_size *= 2;
            data = realloc(data, allocated_size * sizeof(num_t));
        }
        data[count] = num;
        ++count;
    }
    *out_data = data;
    *out_count = count;
}
static void dump_to_file(FILE *f, num_t *data, size_t count){
    for(size_t t=0;t<count;++t) fprintf(f, "%u\n", data[t]);
}

static num_t * generate_random_data(size_t count){
    num_t *buffer = malloc(count * sizeof(num_t));
    for(size_t t=0; t<count;++t){
        buffer[t] = rand()&0xFF;
    }
    return buffer;
}

static int qsort_comparer(const void *const a_void, const void *const b_void){
    const num_t *const a = (const num_t *)a_void;
    const num_t *const b = (const num_t *)b_void;
    return *a == *b ? 0 : (*a<*b ? -1 : 1);
}

void task_qsort(num_t*buffer, size_t count)
{
    qsort(buffer, count, sizeof(num_t), qsort_comparer);
}

void task(num_t*buffer, size_t count)
{
    task_qsort(buffer, count);
}

int main(int argc, const char *argv[])
{
	struct timespec start, end;
	uint64_t diff_ns, diff_ms;

	// TODO: Read input here (outside of the benchmark).
	if(argc <= 2){
		debug("Not enough arguments - expected `in.pbm out.pbm` but only %d arguments were provided!\n", argc);
		return 1;
	}
    FILE *in = fopen(argv[1], "rb");
    FILE *out = fopen(argv[2], "wb");
    size_t count;
    num_t *buffer;
    load_file(in, &buffer, &count);

    srand(time(NULL));

	// Benchmark code.
	clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    (void)generate_random_data;
    (void)task_qsort;
    (void)task;
	task(buffer, count);
    //task(buffer, count);
	clock_gettime(CLOCK_MONOTONIC_RAW, &end);
	diff_ns = 1000000000 * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
	diff_ms = diff_ns / 1000 / 1000;

    dump_to_file(out, buffer, count);
	// TODO: Write output here (outside of the benchmark).

    fclose(in);
    fclose(out);

	fprintf(stderr, "# Net processing time: %lu ms\n", diff_ms);

	return 0;
}
