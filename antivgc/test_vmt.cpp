#include <iostream>
#include <cassert>
#include "VMTShadow.h"

class BaseInterface {
public:
    virtual int Add(int a, int b) = 0;
    virtual int Multiply(int a, int b) = 0;
};

class TargetClass : public BaseInterface {
public:
    int Add(int a, int b) override {
        return a + b;
    }
    int Multiply(int a, int b) override {
        return a * b;
    }
};

typedef int (*AddFn)(BaseInterface*, int, int);
AddFn original_add = nullptr;

int HookedAdd(BaseInterface* self, int a, int b) {
    std::cout << "[Hook] HookedAdd intercepted call! Arguments: (" << a << ", " << b << ")" << std::endl;
    if (original_add) {
        return original_add(self, a, b) + 100;
    }
    return 0;
}

int main() {
    std::cout << "=== VMT Shadow Hook Isolated Unit Test ===" << std::endl;

    TargetClass target_obj;
    BaseInterface* target = &target_obj;

    std::cout << "[1] Original Add(5, 10): " << target->Add(5, 10) << std::endl;
    assert(target->Add(5, 10) == 15);

    {
        std::cout << "[2] Applying VMTShadowing to target object..." << std::endl;
        VMTShadowing shadow(target);

        original_add = (AddFn)shadow.Apply(0, (uintptr_t*)HookedAdd);
        std::cout << "[3] Hook applied. Original Add function ptr: " << (void*)original_add << std::endl;

        int result = target->Add(5, 10);
        std::cout << "[4] Hooked Add(5, 10) result: " << result << std::endl;
        assert(result == 115);

        std::cout << "[5] Exiting scope to trigger VMTShadowing destructor..." << std::endl;
    }

    std::cout << "[6] VMTShadowing destroyed. Verifying VTable restoration..." << std::endl;
    int restored_result = target->Add(5, 10);
    std::cout << "[7] Restored Add(5, 10) result: " << restored_result << std::endl;
    assert(restored_result == 15);

    std::cout << "\n[+] ALL VMT SHADOW HOOK UNIT TESTS PASSED 100% CLEANLY!" << std::endl;
    return 0;
}
