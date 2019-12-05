#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include "common.h"

#define STB_VORBIS_NO_PUSHDATA_API
#define STB_VORBIS_NO_CRT
#define assert(x)
int errno;
#include <stdlib.h>
#include <string.h>
#include "stb_vorbis.c"

extern unsigned char music_ogg[];
extern unsigned int music_ogg_len;

static bool initialized = false;
static int len = 0;

static char tmpbuf[1 << 18];    // 256 KiB
static short music[64 << 20];   // 64 MiB, stores ~6 minutes of stereo audio

static inline void initialize()
{
	initialized = true;

	// Turn on LED
    DSB();
    *GPFSEL4 |= (1 << 21);
    *GPCLR1 = (1 << 15);

	int ch;

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

    *GPSET1 = (1 << 15);
	DMB();
}

unsigned synth(int16_t *buf, unsigned chunk_size)
{
	if (!initialized) initialize();

	static uint32_t phase = 0;
	for (unsigned i = 0; i < chunk_size; i += 2) {
		buf[i] = music[phase];
		buf[i + 1] = music[phase + 1];
		if ((phase += 2) >= len * 2) phase = 0;
	}

	return chunk_size;
}
