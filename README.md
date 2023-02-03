# Check Media Time

Compare current media time on a media stream with the local monotonous clock to detect potential clock drifts.

### Build

You will need the `libavformat`, `libavcodec` and `libavutil` to be detectable by `pkg-config`

```
make
```

### Usage

```
./check_media_time <URL>
Reading from: <URL
status: media time: 4992ms, clock time: 997ms, diff: 3995ms
```

If you hit `r<enter>`, you can reset the offset timer:
```
./check_media_time <URL>
Reading from: <URL>
status: media time: 5760ms, clock time: 2028ms, diff: 3732ms
Resetting offset..
status: media time: 8103ms, clock time: 7786ms, diff: 317ms
```
