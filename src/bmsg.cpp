#include <assert.h>
#include <string.h>
#include <nlohmann/json.hpp>
#include "bonfire-internal.h"

using json = nlohmann::json;

void bmsg_init(struct bmsg *bm)
{
	memset(bm, 0, sizeof(*bm));

	// request & response
	spdnet_msg_init(&bm->request);
	spdnet_msg_init(&bm->response);

	// user arg
	bm->user_arg = NULL;

	// lifetime state
	bm->state = BM_RAW;

	// snode which received the request
	bm->snode = NULL;
}

void bmsg_close(struct bmsg *bm)
{
	spdnet_msg_close(&bm->request);
	spdnet_msg_close(&bm->response);
}

void bmsg_pending(struct bmsg *bm)
{
	assert(bm->state == BM_RAW);
	bm->state = BM_PENDING;
}

void bmsg_filtered(struct bmsg *bm)
{
	assert(bm->state <= BM_PENDING);
	bm->state = BM_FILTERED;
}

void bmsg_handled(struct bmsg *bm)
{
	assert(bm->state <= BM_PENDING);
	bm->state = BM_HANDLED;
}

void *bmsg_get_user_arg(struct bmsg *bm)
{
	return bm->user_arg;
}

void bmsg_set_user_arg(struct bmsg *bm, void *arg)
{
	bm->user_arg = arg;
}

void bmsg_get_request(struct bmsg *bm, void **data, size_t *size)
{
	*data = MSG_CONTENT_DATA(&bm->request);
	*size = MSG_CONTENT_SIZE(&bm->request);
}

void bmsg_write_response(struct bmsg *bm, const char *data)
{
	bmsg_write_response_size(bm, data, strlen(data));
}

void bmsg_write_response_size(struct bmsg *bm, const void *data, size_t size)
{
	spdnet_frame_close(MSG_CONTENT(&bm->response));
	spdnet_frame_init_size(MSG_CONTENT(&bm->response), size);
	memcpy(MSG_CONTENT_DATA(&bm->response), data, size);
}