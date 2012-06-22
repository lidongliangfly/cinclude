/* This is Public Domain Software under CC BY 3.0 */
#ifndef _CINCLUDE_COMMON_H
#define _CINCLUDE_COMMON_H

#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#ifndef likely
#   ifdef __builtin_expect
#       define likely(x)   __builtin_expect((x), 1)
#   else
#       define likely(x)   (x)
#   endif
#endif

#ifndef unlikely
#   ifdef __builtin_expect
#       define unlikely(x)   __builtin_expect((x), 0)
#   else
#       define unlikely(x)   (x)
#   endif
#endif

#define WARN(fmt, ...) do { \
		fprintf(stderr, "\x1b[35mWARN :%s:%s:%d: " fmt "\x1b[0m",  __BASE_FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
	} while (0)

#define INFO(fmt, ...) do { \
		fprintf(stderr, "\x1b[36mINFO :%s:%s:%d: " fmt "\x1b[0m",  __BASE_FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__ ); \
	} while (0)

#define ERR(fmt, ...) do { \
		fprintf(stderr, "\x1b[1;31mERR  :%s:%s:%d: " fmt "\x1b[0m",  __BASE_FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
	} while (0)

#define MASSERT(b,action,fmt,...) ({bool __b = (bool)(b); if(unlikely(!(__b))){ ERR("failed (%s): " fmt, #b, ##__VA_ARGS__);action;} __b;})
#define MEXPECT(b,action,fmt,...) ({bool __b = (bool)(b); if(unlikely(!(__b))){ WARN("failed (%s): " fmt, #b, ##__VA_ARGS__);action;} __b;})
#define MCHECK(b,action,fmt,...) ({bool __b = (bool)(b);if(unlikely(!(__b))){ DBG("failed (%s): " fmt, #b, ##__VA_ARGS__);action;} __b;})

#define ASSERT(b,action) MASSERT(b,action,"\n")
#define EXPECT(b,action) MEXPECT(b,action,"\n")
#define CHECK(b,action) MCHECK(b,action,"\n")

#ifdef __DEBUG
#   define DBG(fmt, args...) do { \
		fprintf(stderr, "DBG  :%s:%s:%d: " fmt,  __BASE_FILE__, __FUNCTION__, __LINE__, ## args); \
	} while (0)
#   define DASSERT(b,action) MASSERT(b,action,"(debug)\n")
#else
#   define DBG( fmt, args...)
#   define DASSERT(b,action) 
#endif

#define NOP ({;})

#define ECHECK do {ERR("(#-_-)p\n");} while (0)
#define DCHECK do {DBG("(*^-^)v\n");} while (0)
#define ICHECK do {INFO("(*^-^)b\n");} while (0)

#define DUMPD(x) printf("%s = %d\n", #x, (int) (x))
#define DUMPZ(x) printf("%s = %zu\n", #x, (size_t) (x))
#define DUMPP(x) printf("%s = %p\n", #x, (void *) (x))
#define DUMPS(x) printf("%s = %s\n", #x, (char *) (x))
#define DUMPC(x) printf("%s = %c\n", #x, (char) (x))
#define DUMP8(x) printf("%s = 0x%02x\n", #x, (uint8_t) (x))
#define DUMP16(x) printf("%s = 0x%04x\n", #x, (uint16_t) (x))
#define DUMP32(x) printf("%s = 0x%08x\n", #x, (uint32_t) (x))
#define DUMP64(x) printf("%s = 0x%016llx\n", #x, (uint64_t) (x))

//#define TRUE            (0==0)
//#define FALSE           (!TRUE)

#ifdef __cplusplus
#   define EXTERN_C_BEGIN extern "C" {
#   define EXTERN_C_END }
#else
#   define EXTERN_C_BEGIN 
#   define EXTERN_C_END 
#endif

#define CLEAR(x) memset(&(x), 0, sizeof(x))
#define CLEARS(a,sz) ({typeof(sz) __sz = (sz);memset(a,0x00,(sizeof(*(a))) * __sz);})

#define ALLOC(t) (t = (typeof(t))calloc(1,sizeof(typeof(*(t)))))
#define TALLOC(h,action) do {h=NULL;if(!(ALLOC(h))){ERR("Cannot alloc %zuB\n",sizeof(*(h)));action;}} while (0)
#define TALLOCS(h,num,action) ({typeof(num) __num = (num); if(!((h) = (typeof (h))(calloc((sizeof (*(h))), __num)))){ERR("Cannot alloc %zuB\n",(size_t)(sizeof(*(h))*__num));action;} })

#define REALLOC(t,num,action) ({ \
        typeof(t) __t = (t); \
        typeof(num) __num = (num); \
        size_t __sz = (__num) * sizeof(*(t)); \
        if(unlikely(!(__t = (typeof(t))realloc((t),__sz)))) { \
            ERR("Cannot realloc %zuB\n", __sz); \
            action; \
        } \
        (t) = __t;})

#define NEW(type,...) ({type __x = {__VA_ARGS__};type *__p = malloc(sizeof(type));__p ? (memcpy(__p,&__x,sizeof(type)), __p) : NULL;}) 

#define FREE(h) do {if(h){free(h);(h)=NULL;}} while (0);


#ifndef swap    
#   define swap(l,r) ({typeof(l) __tmp = (l); (l) = (r); (r) = __tmp;})
#endif

#ifndef max
#   define max(a,b) ({typeof(a) __ma = (a); typeof(b) __mb = (b);\
                  __ma > __mb ? __ma : __mb; })
#endif

#ifndef min
#   define min(a,b) ({typeof(a) __ma = (a); typeof(b) __mb = (b);\
                  __ma < __mb ? __ma: __mb; })
#endif

#ifndef abs
#define abs(a) ({__typeof(a) __a = (a);__a > 0 ? __a : -(__a);})
#endif

#ifndef range
#   define range(l,x,r) (min(max(l,x),r))
#endif

#endif				
