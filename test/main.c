#include "common.h"

struct __a {
    int dummy[100];
};

static bool success(void)
{
    DCHECK;

    return true;
}

static bool failure(void)
{
    DCHECK;

    return false;
}

static bool check_assert(void)
{
    DCHECK;

    MASSERT(false, NOP, "massert test %d\n", __LINE__);
    ASSERT(false, NOP);
    MEXPECT(false, NOP, "mexpect test %d\n", __LINE__);
    EXPECT(false, NOP);
    DASSERT(false,NOP);
    EXPECT(success(), return false);
    EXPECT(failure(), NOP);

    return true;
}

static bool check_alloc(void)
{
    DCHECK;

    bool ret = false;
    struct __a *p = NULL;

    TALLOC(p, goto error);
    ASSERT(p != NULL, goto error);

    int i;
    for(i = 0; i < 100; ++i) {
        p->dummy[i] = -1;
    }

    CLEAR(*p);

    for(i = 0; i < 100; ++i) {
        ASSERT(p->dummy[i] == 0, goto error);
    }

    ret = true;

error:
    FREE(p);
    ASSERT(p == NULL, return false);

    return ret;
}

static bool check_allocs(void)
{
    DCHECK;

    bool ret = false;
    struct __a *p = NULL;
    int const n = 4;

    TALLOCS(p, n, goto error);

    ASSERT(p != NULL, goto error);

    int i,j;

    for(i = 0; i < n; ++i) {
        for(j = 0; j < 100; ++j) {
            p[i].dummy[j] = -1;
        }
    }

    CLEARS(p, n);

    DUMPZ(sizeof(*p) * n);

    for(i = 0; i < n; ++i) {
        for(j = 0; j < 100; ++j) {
            MASSERT(p[i].dummy[j] == 0, goto error,
                    "(%d,%d) = %d\n", i, j, p[i].dummy[j]);
        }
    }

    REALLOC(p,1, goto error);
    REALLOC(p,2, goto error);

    ret = true;
error:

    FREE(p);
    ASSERT(p == NULL, return false);

    return ret;
}

static bool check_trivial(void)
{
    DCHECK;

    ASSERT(max(1,2) == 2, return false);
    ASSERT(max(2,1) == 2, return false);
    ASSERT(min(1,2) == 1, return false);
    ASSERT(min(2,1) == 1, return false);
    ASSERT(range(-1,0,1) == 0, return false);
    ASSERT(range(-1,-2,1) == -1, return false);
    ASSERT(range(-1,2,1) == 1, return false);
    ASSERT(range(1,2,3) == 2, return false);
    ASSERT(range(1,0,3) == 1, return false);
    ASSERT(range(1,4,3) == 3, return false);
    ASSERT(abs(-1) == 1, return false);
    ASSERT(abs(1) == 1, return false);

    int x = 1, y = 2;

    swap(x,y);

    ASSERT(x == 2, return false);
    ASSERT(y == 1, return false);

    return true;
}

static bool check_new(void)
{
    DCHECK;

    int ret = false;
    struct __a *p = NULL;

    ASSERT(p = NEW(struct __a, .dummy = {1,2,3}),
            return false);

    ASSERT(p->dummy[0] == 1, goto error);
    ASSERT(p->dummy[1] == 2, goto error);
    ASSERT(p->dummy[2] == 3, goto error);

    ret = true;
error:

    FREE(p);
    return true;
}

static bool check_dump(void)
{
    DCHECK;

    DUMPC('a');
    DUMPP(&check_dump);
    DUMPS(__FUNCTION__);
    DUMPD((uint64_t)(-1));
    DUMPZ((uint64_t)(-1));

    DUMP8((uint64_t)(-1));
    DUMP16((uint64_t)(-1));
    DUMP32((uint64_t)(-1));
    DUMP64((uint64_t)(-1));

    return true;
}

int main(int argc, char **argv)
{
    INFO("INFO is blue\n");
    WARN("WARN is purple\n");
    ERR("ERR is red\n");
    DBG("DBG is default color\n");

    ICHECK;
    ECHECK;
    DCHECK;

    ASSERT(check_assert(), return 1);
    ASSERT(check_alloc(), return 1);
    ASSERT(check_allocs(), return 1);
    ASSERT(check_dump(), return 1);
    ASSERT(check_trivial(), return 1);
    ASSERT(check_new(), return 1);

    return 0;
}
