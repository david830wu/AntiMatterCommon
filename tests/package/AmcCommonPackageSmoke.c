/* AmcCommonPackageSmoke.c — a minimal downstream consumer built against an
 * INSTALLED AntiMatterCommon package (see CMakeLists.txt in this directory).
 * Exercises the installed umbrella header through AntiMatter::AntiMatterCommon and
 * proves the umbrella target links every component's library. */

#include "AmcCommon.h"

#include <stdio.h>

int main(void)
{
    struct amc_logger_stats stats;
    amc_logger_get_stats(&stats);   /* links through the umbrella */
    printf("{\"via\":\"find_package\",\"version\":\"%s\"}\n", AMC_COMMON_VERSION);
    return 0;
}
