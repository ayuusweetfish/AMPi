#include <stdint.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

unsigned synth(int16_t **o_buf, unsigned chunk_size)
{
	static int16_t buf[8192];
	*o_buf = &buf[0];

	static uint8_t phase = 0;
	static uint32_t count = 0;
	if (count >= 131072) { count = 0; return 0; }
	for (unsigned i = 0; i < chunk_size; i += 2) {
		int16_t sample = (int16_t)(32767 * sin(phase / 255.0 * M_PI * 2));
		buf[i] = buf[i + 1] = sample;
		phase += 2; // Folds over to 0 ~ 255, generates 344.5 Hz (F4 - ~1/4 semitone)
	}
	count += (chunk_size >> 1);
	return chunk_size;
}
