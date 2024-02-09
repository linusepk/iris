#include <iris.h>

imod_desc_t iris_module_entry(void) {
    return (imod_desc_t) {
        .name = "Test"
    };
}

