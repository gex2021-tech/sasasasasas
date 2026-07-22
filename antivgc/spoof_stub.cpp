#include <windows.h>
#include <cstdint>

extern "C" {
uintptr_t proxy_call_returns[256] = {0};
size_t proxy_call_fakestack_size = 0;
uintptr_t* proxy_call_fakestack = nullptr;

typedef void* (*GenericFn)(void*, void*, void*, void*, void*, void*, void*, void*);

void* spoofcall_stub(void* a1, void* a2, void* a3, void* a4, void* a5, void* a6, void* a7, void* fn) {
    if (!fn) return nullptr;
    GenericFn func = (GenericFn)fn;
    return func(a1, a2, a3, a4, a5, a6, a7, nullptr);
}
}
