/*******************************************************************************
 *   Ledger App Coti
 *   (c) 2017 Ledger
 *   (c) 2020 Tchain Ltd. adaptation for COTI
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ********************************************************************************/

#include <stdint.h>

static const unsigned char hex_digits[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

void arrayHexstr(char *strBuf, const void *bin, uint32_t len)
{
    while (len--)
    {
        *strBuf++ = hex_digits[((*((char *)bin)) >> 4) & 0xF];
        *strBuf++ = hex_digits[(*((char *)bin)) & 0xF];
        bin = (const void *)((unsigned int)bin + 1);
    }
    *strBuf = 0;
}
