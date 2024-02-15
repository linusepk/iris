#include "internal.h"
#include "iris.h"

#include <dirent.h>
#include <dlfcn.h>

static module_t module_load(const char *file) {
    module_t mod = {0};

    re_arena_temp_t scratch = re_arena_scratch_get(NULL, 0);

    re_str_t path = re_str_concat(re_str_lit("modules/"), re_str_cstr(file), scratch.arena);
    char *cstr_path = re_arena_push(scratch.arena, path.len + 1);
    for (u32_t i = 0; i < path.len; i++) {
        cstr_path[i] = path.str[i];
    }
    cstr_path[path.len] = '\0';

    mod.dl_handle = dlopen(cstr_path, RTLD_LOCAL | RTLD_NOW);
    if (mod.dl_handle == NULL) {
        re_log_error("Failed to load module %s.", file);
        return mod;
    }

    re_arena_scratch_release(&scratch);

    mod.init = dlsym(mod.dl_handle, "init");
    if (mod.init == NULL) {
        re_log_error("No 'init()' function found in module %s.", file);
        dlclose(mod.dl_handle);
        mod.dl_handle = NULL;
        return mod;
    }

    mod.terminate = dlsym(mod.dl_handle, "terminate");
    if (mod.terminate == NULL) {
        re_log_error("No 'terminate()' function found in module %s.", file);
        dlclose(mod.dl_handle);
        mod.dl_handle = NULL;
        return mod;
    }

    mod.update = dlsym(mod.dl_handle, "update");
    if (mod.update == NULL) {
        re_log_error("No 'update()' function found in module %s.", file);
        dlclose(mod.dl_handle);
        mod.dl_handle = NULL;
        return mod;
    }

    mod.fixed_update = dlsym(mod.dl_handle, "fixed_update");
    if (mod.fixed_update == NULL) {
        re_log_error("No 'fixed_update()' function found in module %s.", file);
        dlclose(mod.dl_handle);
        mod.dl_handle = NULL;
        return mod;
    }

    return mod;
}

static void module_unload(module_t *module) {
    if (module == NULL) {
        return;
    }

    dlclose(module->dl_handle);
    module->dl_handle = NULL;

    module_unload(module->next);
}

void modules_load(state_t *state) {
    DIR *dir = opendir("modules");
    struct dirent *d = NULL;
    while ((d = readdir(dir)) != NULL) {
        if (d->d_type == DT_DIR) {
            continue;
        }

        module_t module = module_load(d->d_name);

        module_t *ptr = re_arena_push(state->permanent_arena, sizeof(module_t));
        *ptr = module;
        ptr->next = state->module;
        state->module = ptr;
    }
}

void modules_unload(state_t *state) {
    module_unload(state->module);
    state->module = NULL;
}

void modules_init(state_t *state) {
    for (module_t *curr = state->module; curr != NULL; curr = curr->next) {
        curr->desc = curr->init(&state->iris);

        re_log_info("Init module: %s", curr->desc.name);
    }
}

void modules_terminate(state_t *state) {
    for (module_t *curr = state->module; curr != NULL; curr = curr->next) {
        re_log_info("Terminate module: %s", curr->desc.name);

        curr->terminate(&state->iris);
    }
}

void modules_update(state_t *state) {
    for (module_t *curr = state->module; curr != NULL; curr = curr->next) {
        curr->update(&state->iris);
    }
}

void modules_fixed_update(state_t *state) {
    for (module_t *curr = state->module; curr != NULL; curr = curr->next) {
        curr->fixed_update(&state->iris);
    }
}
