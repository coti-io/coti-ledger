# COTI Ledger App: Common Technical Specifications

## About

This application describes the APDU messages interface to communicate with the COTI application.

## General purpose APDUs

### 1. Get COTI Public Key

#### Description

This command returns the public key for the given BIP 32 path.

#### Coding

##### Command

| **CLA** | **INS**  | **P1**               | **P2**       | **Lc**     | **Le**
| ------- | -------- | ---------------------| ------------ | ---------- | ------ |
|   E0  |   02   |  <p>`00`: return public key<br/>`01`: display public key and confirm before returning</p> | var | var | var

##### Input data

| **Description**                                     | **Length**
| ------------- | ------------- |
| Number of BIP 32 derivations to perform (max 10)  | 1
| First derivation index (big endian)               | 4
| ...                                               | 4
| Last derivation index (big endian)                | 4

##### Output data

| **Description**                                     | **Length**
| ------------- | ------------- |
| Public Key length                                 | 1
| Uncompressed Public Key                           | var

### Sign Message

#### Description

This command signs a non-hashed message following. The input data is the message to sign, streamed to the device in 255
bytes maximum data chunks

#### Coding

##### Command

| **CLA** | **INS**  | **P1**               | **P2**       | **Lc**     | **Le**
| ------- | -------- | ---------------------| ------------ | ---------- | ------ |
|   E0  |   08   |  <p>`00`: first message data block<br/>`80`: subsequent message data block</p>|   00       | var | var|

##### Input Data (first message)

| **Description**                                    | **Length**
| ------------- | ------------- |
| Number of BIP 32 derivations to perform (max 10) | 1
| First derivation index (big endian)              | 4
| ...                                              | 4
| Last derivation index (big endian)               | 4
| Message length                                   | 4
| Message chunk                                    | var

##### Input Data (following messages)

| **Description**                                    | **Length**
| ------------- | ------------- |
| Message chunk                                    | var

##### Output Data

| **Description**                                    | **Length**
| ------------- | ------------- |
| v                                                | 1
| r                                                | 32
| s                                                | 32

## Transport Protocol

### General Transport Description

Ledger APDUs requests and responses are encapsulated using a flexible protocol allowing to fragment large payloads over
different underlying transport mechanisms.

The common transport header is defined as follows:

| **Description**                          | **Length**
| ------------- | ------------- |
| Communication channel ID (big endian)  | 2
| Command tag                            | 1
| Packet sequence index (big endian)     | 2
| Payload                                | var

The Communication channel ID allows commands multiplexing over the same physical link. It is not used for the time
being, and should be set to 0101 to avoid compatibility issues with implementations ignoring a leading 00 byte.

The Command tag describes the message content. Use TAG_APDU (0x05) for standard APDU payloads, or TAG_PING (0x02) for a
simple link test.

The Packet sequence index describes the current sequence for fragmented payloads. The first fragment index is 0x00.

### APDU Command payload encoding

APDU Command payloads are encoded as follows :

| **Description**             | **Length**
| ------------- | ------------- |
| APDU length (big endian)  | 2
| APDU CLA                  | 1
| APDU INS                  | 1
| APDU P1                   | 1
| APDU P2                   | 1
| APDU length               | 1
| Optional APDU data        | var

APDU payload is encoded according to the APDU case

| Case Number  | **Lc** | **Le** | Case description |
| ------------ | ------ | -------| ---------------- |
|   1          |  0   |  0   | No data in either direction - L is set to 00
|   2          |  0   |  !0  | Input Data present, no Output Data - L is set to Lc
|   3          |  !0  |  0   | Output Data present, no Input Data - L is set to Le
|   4          |  !0  |  !0  | Both Input and Output Data are present - L is set to Lc

### APDU Response payload encoding

APDU Response payloads are encoded as follows :

| **Description**                      | **Length**
| ------------- | ------------- |
| APDU response length (big endian)  | 2
| APDU response data and Status Word | var

### USB mapping

Messages are exchanged with the dongle over HID endpoints over interrupt transfers, with each chunk being 64 bytes long.
The HID Report ID is ignored.

### BLE mapping

A similar encoding is used over BLE, without the Communication channel ID.

The application acts as a GATT server defining service UUID D973F2E0-B19E-11E2-9E96-0800200C9A66

When using this service, the client sends requests to the characteristic D973F2E2-B19E-11E2-9E96-0800200C9A66, and gets
notified on the characteristic D973F2E1-B19E-11E2-9E96-0800200C9A66 after registering for it.

Requests are encoded using the standard BLE 20 bytes MTU size

## Status Words

The following standard Status Words are returned for all APDUs - some specific Status Words can be used for specific
commands and are mentioned in the command description.

| **SW**   | **Description**
| -------- | ------------- |
|   6700   | Incorrect length
|   6982   | Security status not satisfied
|   6983   | Device locked
|   6985   | Instruction rejected by user
|   6A80   | Invalid data
|   6A81   | Invalid path
|   6A82   | Instruction not initiated
|   6B00   | Incorrect parameter P1 or P2
|   6D00   | Unknown instruction
|   6E00   | Unknown class
|   6Fxx   | Technical problem (Internal error, please report)
|   9000   | Normal ending of the command
