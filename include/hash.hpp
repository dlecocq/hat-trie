/**
 * Some hash functions. All hash functions provided here must:
 *
 *    - Be implemented as a struct
 *    - Implement uint32_t operator()(const char* data)
 *    - Implement uint32_t operator()(const char* data, size_t length)
 */

#ifndef HT__HASH_H
#define HT__HASH_H

#include <stdint.h>

/* Taken from http://www.azillionmonkeys.com/qed/hash.html */
#undef get16bits
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) \
  || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
#define get16bits(d) (*((const uint16_t *) (d)))
#endif

#if !defined (get16bits)
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
                       +(uint32_t)(((const uint8_t *)(d))[0]) )
#endif

namespace ht {
    struct superfast {
        uint32_t operator()(const char* data, size_t len) const {
            uint32_t hash = len, tmp;
            int rem;
            
            if (len <= 0 || data == NULL) return 0;
            
            rem = len & 3;
            len >>= 2;
            
            /* Main loop */
            for (;len > 0; len--) {
                hash  += get16bits (data);
                tmp    = (get16bits (data+2) << 11) ^ hash;
                hash   = (hash << 16) ^ tmp;
                data  += 2*sizeof (uint16_t);
                hash  += hash >> 11;
            }
            
            /* Handle end cases */
            switch (rem) {
                case 3: hash += get16bits (data);
                        hash ^= hash << 16;
                        hash ^= ((signed char)data[sizeof (uint16_t)]) << 18;
                        hash += hash >> 11;
                        break;
                case 2: hash += get16bits (data);
                        hash ^= hash << 11;
                        hash += hash >> 17;
                        break;
                case 1: hash += (signed char)*data;
                        hash ^= hash << 10;
                        hash += hash >> 1;
            }
            
            /* Force "avalanching" of final 127 bits */
            hash ^= hash << 3;
            hash += hash >> 5;
            hash ^= hash << 4;
            hash += hash >> 17;
            hash ^= hash << 25;
            hash += hash >> 6;
            
            return hash;
        }
    };

    struct crapwow {
        inline uint32_t operator()(const char* key, uint32_t len, uint32_t seed=0) const {
            //finline uint32_t fastcall CrapWow( const u8 *key, uint32_t len, uint32_t seed ) {
#if !defined(__LP64__) && !defined(_MSC_VER) && ( defined(__i386__) || defined(__i486__) || defined(__i586__) || defined(__i686__) )
    // esi = k, ebx = h
    uint32_t hash;
    asm(
        "leal 0x5052acdb(%%ecx,%%esi), %%esi\n"
        "movl %%ecx, %%ebx\n"
        "cmpl $8, %%ecx\n"
        "jb DW%=\n"
    "QW%=:\n"
        "movl $0x5052acdb, %%eax\n"
        "mull (%%edi)\n"
        "addl $-8, %%ecx\n"
        "xorl %%eax, %%ebx\n"
        "xorl %%edx, %%esi\n"
        "movl $0x57559429, %%eax\n"
        "mull 4(%%edi)\n"
        "xorl %%eax, %%esi\n"
        "xorl %%edx, %%ebx\n"
        "addl $8, %%edi\n"
        "cmpl $8, %%ecx\n"
        "jae QW%=\n"
    "DW%=:\n"
        "cmpl $4, %%ecx\n"
        "jb B%=\n"
        "movl $0x5052acdb, %%eax\n"
        "mull (%%edi)\n"
        "addl $4, %%edi\n"
        "xorl %%eax, %%ebx\n"
        "addl $-4, %%ecx\n"
        "xorl %%edx, %%esi\n"
    "B%=:\n"
        "testl %%ecx, %%ecx\n"
        "jz F%=\n"
        "shll $3, %%ecx\n"
        "movl $1, %%edx\n"
        "movl $0x57559429, %%eax\n"
        "shll %%cl, %%edx\n"
        "addl $-1, %%edx\n"
        "andl (%%edi), %%edx\n"
        "mull %%edx\n"
        "xorl %%eax, %%esi\n"
        "xorl %%edx, %%ebx\n"
    "F%=:\n"
        "leal 0x5052acdb(%%esi), %%edx\n"
        "xorl %%ebx, %%edx\n"
        "movl $0x5052acdb, %%eax\n"
        "mull %%edx\n"
        "xorl %%ebx, %%eax\n"
        "xorl %%edx, %%esi\n"
        "xorl %%esi, %%eax\n"
        : "=a"(hash), "=c"(len), "=S"(len), "=D"(key)
        : "c"(len), "S"(seed), "D"(key)
        : "%ebx", "%edx", "cc" 
    );
    return hash;
#else
    #define cwfold( a, b, lo, hi ) { p = (uint32_t)(a) * (uint64_t)(b); lo ^= (uint32_t)p; hi ^= (uint32_t)(p >> 32); }
    #define cwmixa( in ) { cwfold( in, m, k, h ); }
    #define cwmixb( in ) { cwfold( in, n, h, k ); }

    const uint32_t m = 0x57559429, n = 0x5052acdb, *key4 = (const uint32_t *)key;
    uint32_t h = len, k = len + seed + n;
    uint64_t p;

    while ( len >= 8 ) { cwmixb(key4[0]) cwmixa(key4[1]) key4 += 2; len -= 8; }
    if ( len >= 4 ) { cwmixb(key4[0]) key4 += 1; len -= 4; }
    if ( len ) { cwmixa( key4[0] & ( ( 1 << ( len * 8 ) ) - 1 ) ) }
    cwmixb( h ^ (k + n) )
    return k ^ h;
#endif
}
    };
}

#endif
