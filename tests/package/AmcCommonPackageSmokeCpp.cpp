/* AmcCommonPackageSmokeCpp.cpp — the C++ twin of AmcCommonPackageSmoke.c: proves
 * the installed umbrella header compiles and links from a C++ downstream target. */

#include "AmcCommon.h"

#include <cstdio>

int main()
{
    std::printf("{\"via\":\"find_package\",\"lang\":\"c++\",\"version\":\"%s\"}\n",
                AMC_COMMON_VERSION);
    return 0;
}
