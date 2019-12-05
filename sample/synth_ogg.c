#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#ifdef DEBUG
#include <stdio.h>
#endif

#define STB_VORBIS_NO_PUSHDATA_API
#define STB_VORBIS_NO_CRT
#define assert(x)
int errno;
#include "stb_vorbis.c"

#include "music.h"

static bool initialized = false;
static int len = 0;

static char tmpbuf[1 << 18];    // 256 KiB
static short music[16 << 20];

// NOTE: Define music_ogg as a byte array and music_ogg_len as its size here

static inline void initialize()
{
	int ch;

	// int sr;
	// short *music;
	// len = stb_vorbis_decode_memory(music_ogg, music_ogg_len, &ch, &sr, &music);

	int err;
	stb_vorbis_alloc alloc = {
		.alloc_buffer = tmpbuf,
		.alloc_buffer_length_in_bytes = sizeof tmpbuf,
	};
	stb_vorbis *v = stb_vorbis_open_memory(music_ogg, music_ogg_len, &err, &alloc);
	ch = v->channels;

	int total = sizeof music / sizeof music[0];
	int offset = 0;
	len = 0;
	while (1) {
		int n = stb_vorbis_get_frame_short_interleaved(v, ch, music + offset, total - offset);
		if (n == 0) break;
		len += n;
		offset += n * ch;
	}
	stb_vorbis_close(v);

#ifdef DEBUG
	printf("len = %d\n", len);
	for (int i = 0; i < len / 10; i += 2)
		printf("%d\t%.4f\t%hd %hd\n", i, (float)i / 88200, music[i], music[i + 1]);
#endif

	initialized = true;
}

unsigned synth(int16_t *buf, unsigned chunk_size)
{
	if (!initialized) initialize();

	static uint16_t phase = 0;
	for (unsigned i = 0; i < chunk_size; i += 2) {
		buf[i] = music[phase];
		buf[i + 1] = music[phase + 1];
		if ((phase += 2) >= len * 2) phase = 0;
	}

	return chunk_size;
}

#ifdef DEBUG
int main()
{
	synth(0, 0);
	return 0;
}
#endif