#pragma once

#include <stdint.h>
#include <stdbool.h>

void validateParameters(uint8_t p1, uint8_t p2);
void setMessage(uint8_t p2, const uint8_t **workBufferPtr, uint16_t *unreadDataLength);
void setMessageHash(uint8_t p2, const uint8_t *workBufferPtr);
uint32_t getRemainingMessageLength(void);
uint32_t getProcessedAmountLength(void);
uint32_t getRemainingAmountLength(void);
void setAmount(const uint8_t **workBufferPtr, uint16_t *unreadDataLength);
uint32_t getProcessedAddressLength(void);
uint32_t getRemainingAddressLength(void);
void setAddress(const uint8_t *workBufferPtr, uint16_t unreadDataLength);
bool isFinalRequest(void);
void setSignMessageDisplayData(void);
void uxSignFlowInit(void);
void setBip32Path(const uint8_t **workBufferPtr, uint16_t *unreadDataLength);
void setSigningType(const uint8_t **workBufferPtr, uint16_t *unreadDataLength);
void setAmountLength(const uint8_t **workBufferPtr, uint16_t *unreadDataLength);
void setMessageLength(const uint8_t **workBufferPtr, uint16_t *unreadDataLength);
void setPathLength(const uint8_t **workBufferPtr, uint16_t *unreadDataLength);
