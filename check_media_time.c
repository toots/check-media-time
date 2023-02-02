#include <libavformat/avformat.h>
#include <time.h>

#define NSEC_PER_SEC 1000000000

struct timespec timespec_normalise(struct timespec ts) {
  while (ts.tv_nsec >= NSEC_PER_SEC) {
    ++(ts.tv_sec);
    ts.tv_nsec -= NSEC_PER_SEC;
  }

  while (ts.tv_nsec <= -NSEC_PER_SEC) {
    --(ts.tv_sec);
    ts.tv_nsec += NSEC_PER_SEC;
  }

  if (ts.tv_nsec < 0) {
    --(ts.tv_sec);
    ts.tv_nsec = (NSEC_PER_SEC + ts.tv_nsec);
  }

  return ts;
}

struct timespec timespec_sub(struct timespec ts1, struct timespec ts2) {
  ts1 = timespec_normalise(ts1);
  ts2 = timespec_normalise(ts2);

  ts1.tv_sec -= ts2.tv_sec;
  ts1.tv_nsec -= ts2.tv_nsec;

  return timespec_normalise(ts1);
}

int64_t timespec_to_ms(struct timespec ts) {
  return (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
}

#define TIME_MS(ts, time_base) (time_base.num * ((1000 * ts) / time_base.den))

int main(int argc, char **argv) {
  const char *filename;
  AVFormatContext *format_context = NULL;
  AVStream *stream;
  AVPacket *pkt = NULL;
  struct timespec start_time, current_time;
  int64_t current_time_ms, start_media_time_ms = 0, current_media_time_ms;
  int ret;

  if (argc <= 1) {
    fprintf(stderr, "Usage: %s <input url>\n", argv[0]);
    exit(0);
  }

  filename = argv[1];
  ret = avformat_open_input(&format_context, filename, NULL, NULL);
  if (ret < 0) {
    fprintf(stderr, "Could not open %s: %s\n", filename, av_err2str(ret));
    exit(1);
  }

  ret = avformat_find_stream_info(format_context, 0);
  if (ret < 0) {
    fprintf(stderr, "Failed to retrieve input stream information");
    exit(1);
  }

  pkt = av_packet_alloc();
  if (!pkt) {
    fprintf(stderr, "Could not allocate packet\n");
    exit(1);
  }

  ret = clock_gettime(CLOCK_MONOTONIC, &start_time);
  if (ret < 0) {
    fprintf(stderr, "Error while getting current clock time");
    exit(1);
  }

  printf("Reading from: %s\n", filename);
  fflush(stdout);
  while (av_read_frame(format_context, pkt) >= 0) {
    ret = clock_gettime(CLOCK_MONOTONIC, &current_time);
    if (ret < 0) {
      fprintf(stderr, "Error while getting current clock time");
      exit(1);
    }

    current_time_ms = timespec_to_ms(timespec_sub(current_time, start_time));

    stream = format_context->streams[pkt->stream_index];
    current_media_time_ms = TIME_MS(pkt->pts, stream->time_base);

    if (start_media_time_ms == 0) {
      start_media_time_ms = current_media_time_ms - current_time_ms;
    }

    current_media_time_ms = current_media_time_ms - start_media_time_ms;

    printf("\33[2K\r");
    printf("status: media time: %" PRId64 "ms, clock time: %" PRId64
           "ms, diff: %" PRId64 "ms",
           current_media_time_ms, current_time_ms,
           current_media_time_ms - current_time_ms);
    fflush(stdout);

    av_packet_unref(pkt);
  }
end:
  avformat_close_input(&format_context);
  av_packet_free(&pkt);

  return 0;
}
