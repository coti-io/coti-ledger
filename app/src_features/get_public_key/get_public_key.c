#include "shared_context.h"

uint32_t set_result_get_public_key() {
    uint32_t tx = 0;
    G_io_apdu_buffer[tx++] = 65;
    os_memmove(G_io_apdu_buffer + tx, tmp_ctx.public_key_context.public_key.W, 65);
    tx += 65;

    return tx;
}
