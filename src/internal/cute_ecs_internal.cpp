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

#include <internal/cute_ecs_internal.h>
#include <internal/cute_app_internal.h>

namespace cute
{

error_t kv_val_entity(kv_t* kv, entity_t* entity)
{
	kv_state_t state = kv_get_state(kv);
	CUTE_ASSERT(state != KV_STATE_UNITIALIZED);

	if (state == KV_STATE_READ) {
		int index;
		error_t err = kv_val(kv, &index);
		if (err.is_error()) return err;
		*entity = app->load_id_table->operator[](index);
		return error_success();
	} else {
		int* index_ptr = app->save_id_table->find(*entity);
		CUTE_ASSERT(index_ptr);
		return kv_val(kv, index_ptr);
	}
}

}
