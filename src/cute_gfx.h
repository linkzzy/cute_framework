/*
	Cute Framework
	Copyright (C) 2019 Randy Gaul https://randygaul.net

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
*/

#include <cute_defines.h>
#include <sokol/sokol_gfx.h>

#ifndef CUTE_GFX_H
#define CUTE_GFX_H

namespace cute
{

union pixel_t
{
	struct
	{
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
	} colors;
	uint32_t val;
};

using texture_t = uint64_t;

CUTE_API texture_t CUTE_CALL texture_make(pixel_t* pixels, int w, int h, sg_wrap mode = SG_WRAP_CLAMP_TO_EDGE);
CUTE_API void CUTE_CALL texture_destroy(texture_t texture);

struct matrix_t
{
	float data[16];
};

extern CUTE_API matrix_t CUTE_CALL matrix_identity();
extern CUTE_API matrix_t CUTE_CALL matrix_ortho_2d(float w, float h, float x, float y);

struct triple_buffer_t
{
	struct buffer_t
	{
		int stride = 0;
		int buffer_number = 0;
		int offset = 0;
		sg_buffer buffer[3];
	};

	buffer_t vbuf;
	buffer_t ibuf;

	CUTE_INLINE void advance() {
		++vbuf.buffer_number; vbuf.buffer_number %= 3;
		++ibuf.buffer_number; ibuf.buffer_number %= 3;
	}
	CUTE_INLINE sg_buffer get_vbuf() { return vbuf.buffer[vbuf.buffer_number]; }
	CUTE_INLINE sg_buffer get_ibuf() { return ibuf.buffer[ibuf.buffer_number]; }
};

CUTE_API triple_buffer_t CUTE_CALL triple_buffer_make(int vertex_data_size, int vertex_stride, int index_count);
CUTE_API void CUTE_CALL triple_buffer_append(triple_buffer_t* buffer, int vertex_count, const void* vertices, int index_count, const void* indices);

}

#endif // CUTE_GFX_H
