#include "common.h"

struct __a {
    int dummy[100];
};

static int success(void)
{
    DCHECK;

    return SUCCESS;
}

static int failure(void)
{
    DCHECK;

    return FAILURE;
}

static int check_assert(void)
{
    DCHECK;

    MASSERT(FALSE, NOP, "massert test %d\n", __LINE__);
    ASSERT(FALSE, NOP);
    MEXPECT(FALSE, NOP, "mexpect test %d\n", __LINE__);
    EXPECT(FALSE, NOP);
    DASSERT(FALSE,NOP);
    EXPECT(success() == SUCCESS, return FAILURE);
    EXPECT(failure() == SUCCESS, NOP);

    return SUCCESS;
}

static int check_alloc(void)
{
    DCHECK;

    int ret = FAILURE;
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

    ret = SUCCESS;

error:
    FREE(p);
    ASSERT(p == NULL, return FAILURE);

    return ret;
}

static int check_allocs(void)
{
    DCHECK;

    int ret = FAILURE;
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

    ret = SUCCESS;
error:

    FREE(p);
    ASSERT(p == NULL, return FAILURE);

    return ret;
}

static int check_trivial(void)
{
    DCHECK;

    ASSERT(max(1,2) == 2, return FAILURE);
    ASSERT(max(2,1) == 2, return FAILURE);
    ASSERT(min(1,2) == 1, return FAILURE);
    ASSERT(min(2,1) == 1, return FAILURE);
    ASSERT(range(-1,0,1) == 0, return FAILURE);
    ASSERT(range(-1,-2,1) == -1, return FAILURE);
    ASSERT(range(-1,2,1) == 1, return FAILURE);
    ASSERT(range(1,2,3) == 2, return FAILURE);
    ASSERT(range(1,0,3) == 1, return FAILURE);
    ASSERT(range(1,4,3) == 3, return FAILURE);
    ASSERT(abs(-1) == 1, return FAILURE);
    ASSERT(abs(1) == 1, return FAILURE);

    int x = 1, y = 2;

    swap(x,y);

    ASSERT(x == 2, return FAILURE);
    ASSERT(y == 1, return FAILURE);

    return SUCCESS;
}

static int check_new(void)
{
    DCHECK;

    int ret = FAILURE;
    struct __a *p = NULL;

    ASSERT(p = NEW(struct __a, .dummy = {1,2,3}),
            return FAILURE);

    ASSERT(p->dummy[0] == 1, goto error);
    ASSERT(p->dummy[1] == 2, goto error);
    ASSERT(p->dummy[2] == 3, goto error);

    ret = SUCCESS;
error:

    FREE(p);
    return SUCCESS;
}

static int check_dump(void)
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

    return SUCCESS;
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

    ASSERT(check_assert() == SUCCESS, return 1);
    ASSERT(check_alloc() == SUCCESS, return 1);
    ASSERT(check_allocs() == SUCCESS, return 1);
    ASSERT(check_dump() == SUCCESS, return 1);
    ASSERT(check_trivial() == SUCCESS, return 1);
    ASSERT(check_new() == SUCCESS, return 1);

    return 0;
}
