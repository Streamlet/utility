#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include <memory.h>

#include "../external/xlUnitTest/UnitTest.h"
#include "../scope_exit.h"

using namespace std;
using namespace stdex;

void function_set(int *variable, int value) {
    *variable = value;
}

class functor_set
{
public:
    void operator ()(int *variable, int value) {
        *variable = value;
    }
};

class member_set
{
public:
    void set(int *variable, int value) {
        *variable = value;
    }
};

auto lambda_set = [](int *variable, int value) {
    *variable = value;
};


XL_NAMED_TEST_CASE(scope_exit)
{
    int i = 0;
    XL_TEST_ASSERT(i == 0);
    function_set(&i, 1);
    XL_TEST_ASSERT(i == 1);
    functor_set fs;
    fs(&i, 2);
    XL_TEST_ASSERT(i == 2);
    member_set ms;
    ms.set(&i, 3);
    XL_TEST_ASSERT(i == 3);
    lambda_set(&i, 4);
    XL_TEST_ASSERT(i == 4);

    i = 0;
    XL_TEST_ASSERT(i == 0);
    {
        i = -1;
        XL_TEST_ASSERT(i == -1);
        STDEX_ON_BLOCK_EXIT(function_set, &i, 1);
    }
    XL_TEST_ASSERT(i == 1);
    {
        i = -1;
        XL_TEST_ASSERT(i == -1);
        STDEX_ON_BLOCK_EXIT(fs, &i, 2);
    }
    XL_TEST_ASSERT(i == 2);
    {
        i = -1;
        XL_TEST_ASSERT(i == -1);
        STDEX_ON_BLOCK_EXIT(&member_set::set, &ms, &i, 3);
    }
    XL_TEST_ASSERT(i == 3);
    {
        i = -1;
        XL_TEST_ASSERT(i == -1);
        STDEX_ON_BLOCK_EXIT(lambda_set, &i, 4);
    }
    XL_TEST_ASSERT(i == 4);
}
