#include <iris.h>

#include <stdio.h>
#include <rebound.h>

imod_desc_t iris_module_entry(void) {
    re_log_info("Core");

    return (imod_desc_t) {
        .name = "Core"
    };
}
