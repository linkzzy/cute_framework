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

#ifdef _MSC_VER
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <cute_c_runtime.h>
#include <cute_file_system.h>
#include <test_harness.h>
#include <internal/cute_file_system_internal.h>

#include <test_handle.h>
#include <test_circular_buffer.h>
#include <test_doubly_list.h>
#include <test_base64.h>
#include <test_kv.h>
#include <test_audio.h>
#include <test_ecs.h>
#include <test_lru_cache.h>
#include <test_array.h>
#include <test_aseprite.h>
#include <test_png_cache.h>
#include <test_sprite.h>
#include <test_coroutine.h>

int main(int argc, const char** argv)
{
	file_system_init(argv[0]);
	printf("Tests are running from \"%s\"\n\n", file_system_get_base_dir());
	file_system_destroy();

#ifdef _MSC_VER
	_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG | _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
	_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
	windows_turn_on_console_color();
#endif

	test_t tests[] = {
		CUTE_TEST_CASE_ENTRY(test_handle_basic),
		CUTE_TEST_CASE_ENTRY(test_handle_large_loop),
		CUTE_TEST_CASE_ENTRY(test_handle_large_loop_and_free),
		CUTE_TEST_CASE_ENTRY(test_handle_alloc_too_many),
		CUTE_TEST_CASE_ENTRY(test_circular_buffer_basic),
		CUTE_TEST_CASE_ENTRY(test_circular_buffer_fill_up_and_empty),
		CUTE_TEST_CASE_ENTRY(test_circular_buffer_overflow),
		CUTE_TEST_CASE_ENTRY(test_circular_buffer_underflow),
		CUTE_TEST_CASE_ENTRY(test_circular_buffer_two_threads),
		CUTE_TEST_CASE_ENTRY(test_doubly_list),
		CUTE_TEST_CASE_ENTRY(test_base64_encode),
		CUTE_TEST_CASE_ENTRY(test_kv_basic),
		CUTE_TEST_CASE_ENTRY(test_kv_std_string_to_disk),
		CUTE_TEST_CASE_ENTRY(test_kv_std_string_from_disk),
		CUTE_TEST_CASE_ENTRY(test_kv_std_vector),
		CUTE_TEST_CASE_ENTRY(test_kv_write_delta_basic),
		CUTE_TEST_CASE_ENTRY(test_kv_read_delta_basic),
		CUTE_TEST_CASE_ENTRY(test_kv_write_delta_deep),
		CUTE_TEST_CASE_ENTRY(test_kv_read_delta_deep),
		CUTE_TEST_CASE_ENTRY(test_kv_read_delta_array),
		CUTE_TEST_CASE_ENTRY(test_kv_read_and_write_delta_blob),
		CUTE_TEST_CASE_ENTRY(test_kv_read_delta_string),
		CUTE_TEST_CASE_ENTRY(test_kv_read_delta_object),
		CUTE_TEST_CASE_ENTRY(test_audio_load_synchronous),
		CUTE_TEST_CASE_ENTRY(test_audio_load_asynchronous),
		CUTE_TEST_CASE_ENTRY(test_ecs_octorok),
		CUTE_TEST_CASE_ENTRY(test_ecs_no_kv),
		CUTE_TEST_CASE_ENTRY(test_lru_cache),
		CUTE_TEST_CASE_ENTRY(test_array_list_init),
		CUTE_TEST_CASE_ENTRY(test_aseprite_make_destroy),
		CUTE_TEST_CASE_ENTRY(test_png_cache),
		CUTE_TEST_CASE_ENTRY(test_sprite_make),
		CUTE_TEST_CASE_ENTRY(test_coroutine),
	};
	int test_count = sizeof(tests) / sizeof(*tests);
	int fail_count = 0;

	// Run all tests.
	if (argc == 1) {
		for (int i = 0; i < test_count; ++i)
		{
			test_t* test = tests + i;
			if (do_test(test, i + 1)) fail_count++;
		}
		if (fail_count) {
			fprintf(CUTE_TEST_IO_STREAM, "\033[31mFAILED\033[0m %d test case%s.\n\n", fail_count, fail_count > 1 ? "s" : "");
		} else {
			fprintf(CUTE_TEST_IO_STREAM, "All %d tests \033[32mPASSED\033[0m.\n\n", test_count);
		}
	} else if (argc == 2) {
		const char* soak = argv[1];
		if (CUTE_STRCMP(soak, "soak") == 0) {
			while (1)
			{
				for (int i = 0; i < test_count; ++i)
				{
					test_t* test = tests + i;
					int result = do_test(test, i + 1);
					if (result) goto break_soak;
				}
			}
		}

		// Run a specific test function.
		const char* test_name = argv[1];
		int found = 0;
		for (int i = 0; i < test_count; ++i)
		{
			test_t* test = tests + i;
			if (CUTE_STRCMP(test_name, test->test_name) == 0) {
				do_test(test, 1);
				found = 1;
				break;
			}
		}

		if (!found) {
			fprintf(CUTE_TEST_IO_STREAM, "Unable to find test %s.\n", test_name);
			return -1;
		}
	} else {
		fprintf(CUTE_TEST_IO_STREAM, "Invalid number of parameters. Please pass in either no parameters to run all tests, or just the test function name.\nYou may also pass in \"soak\" to run tests in an infinite loop.\n");
		return -1;
	}

	return fail_count ? -1 : 0;

break_soak:
	return -1;
}
