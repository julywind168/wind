#include "runtime.h"


extern "C" DLLEXPORT void* malloc(size_t size) {
    return GC_malloc(size);
}

extern "C" DLLEXPORT void* realloc(void *ptr, size_t size) {
    return GC_realloc(ptr, size);
}