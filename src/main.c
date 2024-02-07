#include <rebound.h>

#include "iris.h"

#include <dlfcn.h>

i32_t main(i32_t argc, char **argv) {
    re_init();

    void *core = dlopen("bin/modules/core.imod", RTLD_NOW | RTLD_GLOBAL);
    ((imod_entry_func_t) dlsym(core, "iris_module_entry"))();
    dlclose(core);

    re_terminate();
    return 0;
}
