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

typedef struct{
    float x;
    float y;
} vec2;

// (a + bi) * (c + di) = a*c + a*di + bi*c + b*d*i^2 = a*c - b*d + a*di + bi*c
// (a + bi) * (a * bi) = a*a + a*bi + bi*a - b*b = a*a - b*b + 2*a*bi 
static vec2 add2(vec2 a, vec2 b){
    return (vec2){.x = a.x + b.x, .y= a.y + b.y};
}
static vec2 sqr2(vec2 a){
    return (vec2){a.x*a.x - a.y*a.y, + 2.0f*a.x*a.y};
}
//static vec2 mul2(vec2 a, vec2 b){
//    return (vec2){a.x*b.x - a.y*b.y, a.x*b.y + a.y*b.x};
//}
static float abs2sqr(vec2 a){
    return a.x*a.x + a.y*a.y;
}


bool is_mandelbrot(const vec2 c, unsigned N){
    vec2 z ={0,0};
    for(unsigned i = 0; i < N; ++i){
        z = add2(sqr2(z), c);
        if(abs2sqr(z) >= (2*2)){
            return true;
        }
    }
    return false;
}


typedef char pixel_t;

typedef struct{
	unsigned width;
	unsigned height;
	pixel_t *data;
} bitmap_t;

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
	const size_t buffer_size = (bitmap->width * bitmap->height)/8 + 1;
	char *buffer = malloc(buffer_size);
	for(unsigned y = 0; y < bitmap->height;++y)
	for(unsigned x = 0; x < bitmap->width;++x){
		set_bit(buffer, y * bitmap->width + x, get_pixel(bitmap, x, y));
	}
    fprintf(f, "P4\n%u %u\n", bitmap->width, bitmap->height);
	for(size_t t=0;t<buffer_size;++t){
		fputc(buffer[t], f);
	}
}



bool is_mandelbrot_simple(float cx, float cy, unsigned N){
	float zx = 0, zy = 0;
	for(unsigned i = 0; i < N;++i){
		float sqr_x = (zx*zx - zy*zy) + cx, sqr_y = (2.0f*zx*zy) + cy;
		zx = sqr_x, zy = sqr_y;
		if((zx * zx + zy * zy) >= 4){
			return true;
		}
	}
	return false;
}

#define VECSIZE 4

typedef double __attribute__((vector_size(VECSIZE *sizeof(double)))) floatvec;
typedef long long int __attribute__((vector_size(VECSIZE * sizeof(long long)))) intvec;
#define vecof(i) {i, i, i, i} //{ i, i, i, i, i, i, i, i }
#define VEC_FOREACH(XX) XX(0) XX(1) XX(2) XX(3) // XX(4) XX(5) XX(6) XX(7)

intvec is_mandelbrot_vec(floatvec cx, floatvec cy, unsigned N){
	floatvec zx = vecof(0), zy = vecof(0);
	for(unsigned i = 0; i < N; ++i){
		floatvec sqr_x = (zx*zx - zy*zy) + cx, sqr_y = (2.0f*zx*zy) + cy;
		zx = sqr_x, zy = sqr_y;

		floatvec distances = (zx * zx + zy * zy);
		intvec magnitudes = distances <= 4.0;
		//fprintf(stderr, "iteration %u\n", i);
#		define AND(i) && (magnitudes[i] == 0)
		if(1 VEC_FOREACH(AND)){
			return (intvec)vecof(0);
		}
#		undef AND
	}
	floatvec magnitudes = (zx * zx + zy * zy);
	intvec distances = magnitudes <= 4.0;
	return distances;
}


static void task_simple(const float begin_x, const float begin_y, const float step, bitmap_t bitmap, const unsigned iterations_count)
{
	float my = begin_y;
	for(unsigned y = 0 ; y < bitmap.height; ++y, my -= step){
		float mx = begin_x;
		for(unsigned x = 0;x < bitmap.width; ++x, mx += step){
			pixel_t mandel_value = is_mandelbrot_simple(mx, my, iterations_count);
			set_pixel(&bitmap, x, y, ! mandel_value);
		}
	}
}
// task is where you perform the actual task.
// Please perform all processing in this function and its descendants.
static void task(const float begin_x, const float begin_y, const float step, bitmap_t bitmap, const unsigned iterations_count)
{
	float my = begin_y;
	for(unsigned y = 0 ; y < bitmap.height; ++y, my -= step){
#		define INITVEC(i) (begin_x + i*step) ,
		floatvec mx = (floatvec){VEC_FOREACH(INITVEC)};
#		undef INITVEC
		for(unsigned x = 0;x < bitmap.width; x+=VECSIZE, mx += (step*VECSIZE)){
			intvec mandel_value = is_mandelbrot_vec(mx, (floatvec)vecof(my), iterations_count);
#		define DUMPVEC(i) set_pixel(&bitmap, x + i, y, mandel_value[i]);
			VEC_FOREACH(DUMPVEC)
#		undef DEMPVEC
		}
	}
}

int main(int argc, const char *argv[])
{
	struct timespec start, end;
	uint64_t diff_ns, diff_ms;

	// TODO: Read input here (outside of the benchmark).
	if(argc <= 6){
		fprintf(stderr, "Not enough arguments - expected `r0 i0 w h step iter` but only %d arguments were provided!", argc);
		return 1;
	}
	const float begin_x = atof(argv[1]);
	const float begin_y = atof(argv[2]);
	const unsigned width = (unsigned)atoi(argv[3]);
	const unsigned height = (unsigned)atoi(argv[4]);
	const float step = atof(argv[5]);
	const unsigned iterations_count = (unsigned)atoi(argv[6]);
	bitmap_t bitmap = make_bitmap(width, height);

	//printf("%f %f %u %u %f %u\n", begin_x, begin_y, width, height, step, iterations_count);

	// Benchmark code.
	clock_gettime(CLOCK_MONOTONIC_RAW, &start);
	(void)task_simple;
	(void)task;
	task(begin_x, begin_y, step, bitmap, iterations_count);
	clock_gettime(CLOCK_MONOTONIC_RAW, &end);
	diff_ns = 1000000000 * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
	diff_ms = diff_ns / 1000 / 1000;

	// TODO: Write output here (outside of the benchmark).
	
	//FILE *f = fopen("mandel.pbm", "wb");
	dump_to_file(&bitmap, stdout);
	//fclose(f);
	destroy_bitmap(&bitmap);

	fprintf(stderr, "# Net processing time: %lu ms\n", diff_ms);

	return 0;
}
