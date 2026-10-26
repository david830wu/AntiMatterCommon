/* AmcCommonPackageSmoke.c — a minimal downstream consumer built against an
 * INSTALLED AntiMatterCommon package (see CMakeLists.txt in this directory).
 * Exercises the installed umbrella header through AntiMatter::AntiMatterCommon. */

#include "AmcCommon.h"

#include <stdio.h>

int main(void)
{
    printf("{\"via\":\"find_package\",\"version\":\"%s\"}\n", AMC_COMMON_VERSION);
    return 0;
}
