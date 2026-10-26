/* AmcCommonPackageSmokeCpp.cpp — the C++ twin of AmcCommonPackageSmoke.c: proves
 * the installed umbrella header compiles and links from a C++ downstream target. */

#include "AmcCommon.h"

#include <cstdio>

int main()
{
    struct amc_logger_stats stats;
    amc_logger_get_stats(&stats);   /* links through the umbrella */
    std::printf("{\"via\":\"find_package\",\"lang\":\"c++\",\"version\":\"%s\"}\n",
                AMC_COMMON_VERSION);
    return 0;
}
