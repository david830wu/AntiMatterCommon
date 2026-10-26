/* AmcLoggerPackageSmoke.c — a minimal downstream consumer of the logger component,
 * built against an INSTALLED AntiMatterCommon package (see CMakeLists.txt in this
 * directory). Exercises the installed header (including the AMC_LOGGER_JSON
 * composer) and links the installed static library through AntiMatter::AmcLogger. */

#include "AmcLogger.h"

int main(void)
{
    if (amc_logger_init(NULL) < 0)
        return 1;
    AMC_LOGGER_INFO("AmcLoggerPackageSmoke",
                    AMC_LOGGER_JSON(AMC_LOGGER_KV_STR("via", "find_package"),
                                    AMC_LOGGER_KV_STR_ESC("esc", "with \"quotes\""),
                                    AMC_LOGGER_KV_BOOL("exported", 1)));
    return amc_logger_shutdown();
}
