.PHONY: all clean format

CC := cc

LIBS := $(shell pkg-config --libs libavformat)

C_FILES := $(wildcard *.c)
C_OBJS := $(C_FILES:.c=.o)

all: check_media_time

check_media_time: $(C_OBJS)
	$(CC) $(LIBS) -o $@ $(C_OBJS)

%.o: %.c
	$(CC) -g -c $<

clean:
	rm -rf check_media_time *.o

format:
	clang-format -i *.c
