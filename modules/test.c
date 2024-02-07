#include <iris.h>

iris_module_desc_t iris_module_entry(void) {
    iris_test();

    return (iris_module_desc_t) {
        .name = "Test module"
    };
}
