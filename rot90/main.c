#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

bool get_bit(const char *const bitarray, const size_t idx){
    const char byte = bitarray[idx>>3];
    return !!(byte & (1<< (7- (idx&0x7))));
}
void set_bit(char *const bitarray, const size_t idx, const bool value){
    char *const byte = &bitarray[idx>>3];
    char mask = (1<< (7- (idx&0x7)));
    if(value){
        *byte |= mask;
    }else{
        *byte &= ~mask;
    }
}



typedef char pixel_t;

typedef struct{
	unsigned width;
	unsigned height;
	pixel_t *data;
} bitmap_t;

#define NULL_BITMAP ((bitmap_t){0, 0, NULL})

bitmap_t make_bitmap(unsigned width, unsigned height){
	pixel_t *const data = malloc((width * height) * sizeof(pixel_t));
	return (bitmap_t){width, height, data};
}
void destroy_bitmap(bitmap_t *bitmap){
	free(bitmap->data);
	bitmap->data = NULL;
	bitmap->width = bitmap->height = 0;
}

bitmap_t get_row(bitmap_t *bitmap, unsigned y){
	return (bitmap_t){bitmap->width, 1, &bitmap->data[bitmap->width * y]};
}
pixel_t get_pixel(bitmap_t *bitmap, unsigned x, unsigned y){
	return bitmap->data[bitmap->width * y + x];
}

void set_pixel(bitmap_t *bitmap, unsigned x, unsigned y, pixel_t value){
	const unsigned offset = bitmap->width * y + x;
	bitmap->data[offset] = value;
}

void dump_to_file(bitmap_t *bitmap, FILE *f){
	const size_t buffer_size = (bitmap->width * bitmap->height)/8;
	char *buffer = malloc(buffer_size);
	for(unsigned y = 0; y < bitmap->height;++y)
	for(unsigned x = 0; x < bitmap->width;++x){
		set_bit(buffer, y * bitmap->width + x, get_pixel(bitmap, x, y));
	}
    fprintf(f, "P4\n%u %u\n", bitmap->width, bitmap->height);
	for(size_t t=0;t<buffer_size;++t){
		fputc(buffer[t], f);
	}
    free(buffer);
}

#define debug(...) fprintf(stderr, __VA_ARGS__)

bitmap_t load_from_file(FILE *f){
    unsigned width, height;
    if(fscanf(f, "P4\n%u %u", &width, &height) != 2){
        debug("INVALID PBM FILE!");
        exit(2);
    }
    fgetc(f); // skip newline
    bitmap_t ret = make_bitmap(width, height);

    for(unsigned y = 0; y< height; ++y){
        for(unsigned x = 0; x < width; x += 8){
            char next_word = fgetc(f);
            for(unsigned i = 0; i < 8;++i){
                pixel_t p= get_bit(&next_word, i);
                //if(!p){
                //    debug("[%u, %u]: %u", x + i, y, p);
                //}
                set_pixel(&ret, x + i, y, p);
            }
        }
    }
    return ret;
}







// task is where you perform the actual task.
// Please perform all processing in this function and its descendants.
static void task(bitmap_t in, bitmap_t out)
{
    for(unsigned y = 0; y < in.height; ++y){
        for(unsigned x = 0; x < in.width; ++x){
            pixel_t p = get_pixel(&in, x, y);
            set_pixel(&out, out.width - y - 1, x, p);
        }
    }
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

    bitmap_t in_bitmap = load_from_file(in);
    bitmap_t out_bitmap = make_bitmap(in_bitmap.width, in_bitmap.height);

	// Benchmark code.
	clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    (void)task;
	task(in_bitmap, out_bitmap);
	clock_gettime(CLOCK_MONOTONIC_RAW, &end);
	diff_ns = 1000000000 * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
	diff_ms = diff_ns / 1000 / 1000;

	// TODO: Write output here (outside of the benchmark).

    dump_to_file(&out_bitmap, out);
    fclose(in);
    fclose(out);
	fprintf(stderr, "# Net processing time: %lu ms\n", diff_ms);

	return 0;
}
