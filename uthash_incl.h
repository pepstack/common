/***********************************************************************
* Copyright (c) 2008-2080 cheungmine, pepstack.com, 350137278@qq.com
*
* ALL RIGHTS RESERVED.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
*   Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
* A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
* OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***********************************************************************/

/**
 uthash.h
   http://troydhanson.github.com/uthash/
   http://troydhanson.github.io/uthash/

Copyright (c) 2003-2020, Troy D. Hanson
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef UTHASH_INCL_H
#define UTHASH_INCL_H

#ifdef MEMAPI_USE_LIBJEMALLOC
#define uthash_malloc(sz) je_malloc(sz)
#define uthash_free(ptr,sz) je_free(ptr)
#endif

#include "uthash/uthash.h"
#include "uthash/utarray.h"


/**
 * uthash extension
 */
#define HASH_FIND_STR_LEN(head,findstr,findstrlen,out)                           \
do {                                                                             \
    unsigned _uthash_hfstr_keylen = (unsigned)(findstrlen);                      \
    HASH_FIND(hh, head, findstr, _uthash_hfstr_keylen, out);                     \
} while (0)

#define HASH_ADD_STR_LEN(head,strfield,strfieldlen,add)                          \
do {                                                                             \
    unsigned _uthash_hastr_keylen = (unsigned)(strfieldlen);                     \
    HASH_ADD(hh, head, strfield[0], _uthash_hastr_keylen, add);                  \
} while (0)

#define HASH_REPLACE_STR_LEN(head,strfield,strfieldlen,add,replaced)             \
do {                                                                             \
    unsigned _uthash_hrstr_keylen = (unsigned)(strfieldlen);                     \
    HASH_REPLACE(hh, head, strfield[0], _uthash_hrstr_keylen, add, replaced);    \
} while (0)


/**
 * utarray extension
 */

/**
 * utarray_find_no_sort()
 *   lookup elements in array by sequence and as a replacement of utarray_find().
 *
 * DO NOT use utarray_find() since array must be sorted by calling utarray_sort()
 *   before calling utarray_find().
 */
static void* utarray_find_no_sort(UT_array *objects, void *eltpos, int(*cmpfn)(const void *elt, const char *findob), const void *findob)
{
    void *elt = eltpos;

    while ((elt = utarray_next(objects, elt)) != NULL) {
        if (!cmpfn(elt, findob)) {
            break;
        }
    }

    return elt == eltpos? NULL : elt;
}

#endif /* UTHASH_INCL_H */
