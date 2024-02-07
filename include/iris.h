#pragma once

#include <rebound.h>

typedef struct imod_desc_t imod_desc_t;
struct imod_desc_t {
    const char *name;
};

// Module entry point.
// This will be called for every module loaded.
// If the entry point is not found, an error is printed and module is discarded.
typedef imod_desc_t (*imod_entry_func_t)(void);
