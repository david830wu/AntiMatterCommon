/* AmcCommonExample — the smallest AntiMatterCommon program: include the umbrella
 * header, link AntiMatter::AntiMatterCommon, and report the package version. */

#include "AmcCommon.h"

#include <stdio.h>

int main(void)
{
    printf("AntiMatterCommon %s\n", AMC_COMMON_VERSION);
    return 0;
}
