.PHONY: clean

CFLAGS += \
		-Wall \
		-Wextra \
		-Wno-unused-result \
		-std=gnu11 \
		-g \
		-O3 \
		-march=native \
		-fno-strict-aliasing \
		#-fsanitize=address,undefined \
		-Wpedantic \


hello: main.c
		$(CC) $(CFLAGS) -o rot90 $^ -lm


clean:
		rm -f -- rot90