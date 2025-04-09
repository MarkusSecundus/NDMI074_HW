.PHONY: clean

CFLAGS += \
		-Wall \
		-Wextra \
		-Werror \
		-std=gnu11 \
		-g \
		#-fsanitize=address,undefined \
		-Wpedantic \


hello: main.c
		$(CC) $(CFLAGS) -o eras $^ -lm


clean:
		rm -f -- hello