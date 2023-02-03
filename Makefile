.PHONY: all clean format

CC := cc

CC := cc

LIBS := $(shell pkg-config --libs libavformat,libavcodec,libavutil)
CFLAGS := -g $(shell pkg-config --cflags libavformat,libavcodec,libavutil)

all: check_media_time

check_media_time: check_media_time.c
	$(CC) $< $(CFLAGS) $(LIBS) -o $@

clean:
	rm -rf check_media_time

format:
	clang-format -i *.c
