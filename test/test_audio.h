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

#include <cute_audio.h>
#include <cute_app.h>
#include <cute_concurrency.h>

using namespace cute;

CUTE_TEST_CASE(test_audio_load_synchronous, "Load and free wav/ogg files synchronously.");
int test_audio_load_synchronous()
{
	file_system_init(NULL);
	file_system_mount(file_system_get_base_dir(), "");

	audio_t* audio = audio_load_ogg("test_data/3-6-19-blue-suit-jam.ogg");
	CUTE_TEST_CHECK_POINTER(audio);
	CUTE_TEST_ASSERT(!audio_destroy(audio).is_error());

	audio = audio_load_wav("test_data/jump.wav");
	CUTE_TEST_CHECK_POINTER(audio);
	CUTE_TEST_ASSERT(!audio_destroy(audio).is_error());

	file_system_destroy();

	return 0;
}

static cute::error_t s_audio_error;
static audio_t* s_audio;

static void s_audio_promise(cute::error_t status, void* param, void* udata)
{
	s_audio_error = status;
	atomic_ptr_set((void**)&s_audio, param);
	CUTE_ASSERT(udata == NULL);
}

CUTE_TEST_CASE(test_audio_load_asynchronous, "Load and free wav/ogg files asynchronously.");
int test_audio_load_asynchronous()
{
	CUTE_TEST_ASSERT(!app_make("audio test", 0, 0, 0, 0, CUTE_APP_OPTIONS_HIDDEN).is_error());

	promise_t promise;
	promise.callback = s_audio_promise;

	s_audio_error = error_success();
	s_audio = NULL;
	audio_stream_ogg("test_data/3-6-19-blue-suit-jam.ogg", promise);

	while (!atomic_ptr_get((void**)&s_audio))
		;

	CUTE_TEST_ASSERT(!audio_destroy(s_audio).is_error());

	s_audio_error = error_success();
	s_audio = NULL;
	audio_stream_wav("test_data/jump.wav", promise);

	while (!atomic_ptr_get((void**)&s_audio))
		;

	CUTE_TEST_ASSERT(!audio_destroy(s_audio).is_error());

	app_destroy();

	return 0;
}
