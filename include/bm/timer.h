#ifndef _CINCLUDE_BM_TIMER_H
#define _CINCLUDE_BM_TIMER_H
#ifdef __MACH__
#    include <mach/mach.h>
#    include <mach/clock.h>
#else
#    include <sys/time.h>
#    include <time.h>
#endif

#include "common.h"

EXTERN_C_BEGIN

#ifdef __MACH__
typedef struct mach_timespec __timespec_t;
#else
typedef struct timespec __timespec_t;
#endif

static inline void
bmtimer_gettime(__timespec_t *ts)
{
#ifdef __MACH__
    /* 
     * http://boredzo.org/blog/archives/2006-11-26/how-to-use-mach-clocks
     */
    clock_serv_t host_clock;

    /* XXX: waste a few micro seconds */
    host_get_clock_service(mach_host_self(), REALTIME_CLOCK, &host_clock);

    clock_get_time(host_clock, ts);
#else
    clock_gettime(CLOCK_MONOTONIC, ts);
#endif
}

struct bmtimer_obj {
    __timespec_t tv1;
    __timespec_t tv2;
    long         sum;
    long         max;
    long         min;
    double       tick;
    unsigned int cnt;
    bool         active;
};

typedef struct bmtimer_obj *bmtimer_handle;

/******************************************************************************
 *                          INLINE FUNCTIONS
 ******************************************************************************/
static inline void 
bmtimer_delete(bmtimer_handle h)
{
    FREE(h);
}

static inline void 
bmtimer_print(bmtimer_handle h,const char *reporter)
{
    double avg = 0;
    
    ASSERT(h, return);
   
    avg = (double)h->sum / (double)h->cnt / 1000.0;

    INFO("%s spends: min[%ldus], max[%ldus], avg[%.3fms], in %u times\n", 
            reporter, h->min, h->max, avg, h->cnt);
}

static inline void
bmtimer_countdown_usec(bmtimer_handle h, double time_usec)
{
    bmtimer_gettime(&h->tv1);

    h->tick = (time_usec * 1e3);
}

static inline  void
bmtimer_countdown_sec(bmtimer_handle h, double time)
{
    bmtimer_gettime(&h->tv1);

    h->tick = (time * 1e9);
}

static inline bool
bmtimer_countdown_is_finish(bmtimer_handle h)
{
    __timespec_t tv;
    double diff;

    if(h->tick < 0) return false;

    bmtimer_gettime(&tv);

    diff = (tv.tv_sec - h->tv1.tv_sec)  * 1e9 + 
        (tv.tv_nsec - h->tv1.tv_nsec);

    return diff > h->tick;
}

static inline void 
bmtimer_lap_start(bmtimer_handle h)
{
    ASSERT(h, return);

    bmtimer_gettime(&h->tv1);
}

static inline void 
bmtimer_lap_end(bmtimer_handle h)
{
    bmtimer_gettime(&h->tv2);

    long            usec;
    long            avg = 0;

    usec = 
        (h->tv2.tv_nsec - h->tv1.tv_nsec) / 1000 + 
        (h->tv2.tv_sec - h->tv1.tv_sec) * 1000000;

    if(h->max < usec) h->max = usec;

    if(h->active) {
        if(h->min > usec) h->min = usec;
    } else {
        h->min = usec;
    }

    h->sum += usec;
    h->cnt++;
    avg = h->sum / h->cnt;
    h->active = true;
}

EXTERN_C_END

#endif
