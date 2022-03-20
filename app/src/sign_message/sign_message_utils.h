#pragma once

#include <stdint.h>

void setSignMessageDisplayData(void);
void uxSignFlowInit(void);
void setBip32Path(const uint8_t **workBufferPtr, uint16_t *unreadDataLength);
void setSigningType(const uint8_t **workBufferPtr, uint16_t *unreadDataLength);
void setMessageLength(const uint8_t **workBufferPtr, uint16_t *unreadDataLength);
void setPathLength(const uint8_t **workBufferPtr, uint16_t *unreadDataLength);
