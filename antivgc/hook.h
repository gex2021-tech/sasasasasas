#pragma once


#include <cstdint>
#define MAX_VMT_METHODS 512

void* FindWritableRDataSpace(size_t sizeRequired) {
    HMODULE hModule = GetModuleHandle(nullptr);
    MODULEINFO modInfo = {};
    GetModuleInformation(GetCurrentProcess(), hModule, &modInfo, sizeof(modInfo));

    uint8_t* base = (uint8_t*)modInfo.lpBaseOfDll;
    uint8_t* end = base + modInfo.SizeOfImage;

    for (uint8_t* ptr = base; ptr < end; ptr += 0x1000) {
        MEMORY_BASIC_INFORMATION mbi;
        VirtualQuery(ptr, &mbi, sizeof(mbi));

        if ((mbi.Protect & (PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READWRITE)) && mbi.State == MEM_COMMIT) {
            size_t space = mbi.RegionSize;
            for (size_t offset = 0; offset + sizeRequired <= space; offset += sizeof(void*)) {
                bool found = true;
                for (size_t i = 0; i < sizeRequired; i++) {
                    if (ptr[offset + i] != 0x00) {
                        found = false;
                        break;
                    }
                }
                if (found)
                    return &ptr[offset];
            }
        }
    }
    return nullptr;

}

void** originalVMT = nullptr;
    void** customVMT = nullptr;
    int methodCount = 0;
    int hookIndex = -1;
    void* originalFn = nullptr;

    void reinstract(void* instance, void* hookFunction, int index) {
        originalVMT = *(void***)instance;
        hookIndex = index;

        // Count methods
        while (originalVMT[methodCount] && methodCount < MAX_VMT_METHODS) {
            methodCount++;
        }

        // Allocate stealth space in module's writable memory
        customVMT = (void**)FindWritableRDataSpace(methodCount * sizeof(void*)); // Stealthy way u fucking gian nigga
        if (!customVMT) return;

        // Copy original VMT because Shadow is our legit mememory :P
        for (int i = 0; i < methodCount; ++i)
            customVMT[i] = originalVMT[i];

        // Store original function and hook
        originalFn = originalVMT[index];
        customVMT[index] = hookFunction;

        // Replace instance's vtable
        DWORD old;
        VirtualProtect(instance, sizeof(void*), PAGE_READWRITE, &old);
        *(void***)instance = customVMT;
        VirtualProtect(instance, sizeof(void*), old, &old);
    }
    class shadow_vmt
    {
    public:
        template <typename type>
        bool hook(uint64_t module_base, uint64_t address, int index, void* function, type* original)
        {
            if (is_hooked || !address || !function)
                return false;

            target_object = address;
            hook_index = index;

            virtual_table = *(uintptr_t**)(address);
            if (!virtual_table)
                return false;

            if (reinterpret_cast<void*>(virtual_table[index]) == function)
                return false;

            // Get vtable size safely
            while (IsValidPointer(&virtual_table[vtable_size]) && virtual_table[vtable_size])
                vtable_size++;

            if (index >= vtable_size || vtable_size <= 0)
                return false;

            allocated_vtable = reinterpret_cast<uintptr_t*>(
                spoofed_malloc(vtable_size * sizeof(uintptr_t), PAGE_READWRITE)
                );
            if (!allocated_vtable)
                return false;

            for (int i = 0; i < vtable_size; ++i)
                allocated_vtable[i] = virtual_table[i];

            *original = reinterpret_cast<type>(virtual_table[index]);

            allocated_vtable[index] = reinterpret_cast<uintptr_t>(function);
            *(uintptr_t**)(address) = allocated_vtable;

            is_hooked = true;
            return true;
        }
        template <typename type2>
        bool hook2(uint64_t module_base, uint64_t address, int index, void* function, type2* original)
        {
            this->virtual_table = *(uintptr_t**)(address);
            if (reinterpret_cast<void*>(this->virtual_table[index]) != function)
            {
                while (this->virtual_table[this->vtable_size])
                {
                    this->vtable_size++;
                }
                if (index < this->vtable_size)
                {

                    this->allocated_vtable = reinterpret_cast<uintptr_t*>(
                        spoofed_malloc(this->vtable_size * sizeof(uintptr_t), PAGE_EXECUTE_READWRITE)
                        );

                    for (int i = 0; i < this->vtable_size; i++)
                    {
                        this->allocated_vtable[i] = this->virtual_table[i];
                    }
                    *original = reinterpret_cast<type2>(this->virtual_table[index]);
                    this->allocated_vtable[index] = reinterpret_cast<uintptr_t>(function);
                    *(uintptr_t**)(address) = this->allocated_vtable;
                    return true;
                }
            }
            return false;
        }
           
        bool unhook()
        {
            if (!is_hooked)
                return false;

            if (!IsValidPointer((void*)target_object) || !IsValidPointer(virtual_table))
                return false;

            auto current_vtable = *(uintptr_t**)(target_object);
            if (current_vtable != allocated_vtable)
                return false; // Already replaced externally, skip

            *(uintptr_t**)(target_object) = virtual_table;

            allocated_vtable = nullptr;
            is_hooked = false;
            return true;
        }

        void unload(HMODULE module_handle)
        {
            unhook();
            FreeLibraryAndExitThread(module_handle, 0);
        }

    private:
        uintptr_t* virtual_table = nullptr;
        uintptr_t* allocated_vtable = nullptr;
        uint64_t target_object = 0;
        int vtable_size = 0;
        int hook_index = -1;
        bool is_hooked = false;
        void* build_trampoline_stub(void* encrypted_fn, uint64_t xor_key)
        {
            constexpr size_t stub_size = 32;
            uint8_t* stub = reinterpret_cast<uint8_t*>(spoofed_malloc(stub_size, PAGE_EXECUTE_READWRITE));
            if (!stub) return nullptr;

            // mov rax, <encrypted_fn XOR key>
            *(uint8_t*)(stub + 0) = 0x48;
            *(uint8_t*)(stub + 1) = 0xB8;
            *(uint64_t*)(stub + 2) = (uint64_t)encrypted_fn ^ xor_key;

            // xor rax, <key>
            *(uint8_t*)(stub + 10) = 0x48;
            *(uint8_t*)(stub + 11) = 0x35;
            *(uint64_t*)(stub + 12) = xor_key;

            // jmp rax
            *(uint8_t*)(stub + 20) = 0xFF;
            *(uint8_t*)(stub + 21) = 0xE0;

            return stub;
        }

        uint64_t spoofed_malloc(int32_t size, uint32_t protection)
        {
            return reinterpret_cast<uint64_t(__cdecl*)(int32_t, uint32_t, uintptr_t, void*)>(spoofcall_stub)(
                size,
                protection,
                Offsets::MagicOffsets,
                (void*)&fmemory::malloc
                );
        }
        uint64_t spoofed_malloc_near(void* preferred, int32_t size, uint32_t protection)
        {
            return reinterpret_cast<uint64_t(__cdecl*)(void*, int32_t, uint32_t, uintptr_t, void*)>(spoofcall_stub)(
                preferred,
                size,
                protection,
                Offsets::MagicOffsets,
                (void*)&fmemory::malloc
                );
        }

        bool IsValidPointer(void* ptr)
        {
            MEMORY_BASIC_INFORMATION mbi{};
            if (!ptr)
                return false;

            if (VirtualQuery(ptr, &mbi, sizeof(mbi)))
            {
                DWORD protect = mbi.Protect;
                return (
                    !(protect & PAGE_NOACCESS) &&
                    !(protect & PAGE_GUARD)
                    );
            }
            return false;
        }
    };


   /* template <typename hook_type>
    _declspec(noinline) hook_type vmt(std::uintptr_t address, std::uintptr_t hook_function, int index) {
        auto vtable = *(std::uintptr_t**)address;

        int vtable_size = 0;
        do vtable_size += 1;
        while (*(std::uintptr_t*)(std::uintptr_t(vtable) + (vtable_size * 8)));

        auto original_virtual_function = (void*)vtable[index];

        std::uintptr_t* fake_vtable = new std::uintptr_t[vtable_size * 8];

        for (int i = 0; i < vtable_size; i++) {
            if (i == index) continue;
            fake_vtable[i] = *(std::uintptr_t*)(std::uintptr_t(vtable) + (i * 8));
        } fake_vtable[index] = hook_function;

        *(std::uintptr_t**)address = fake_vtable;

        return hook_type(original_virtual_function);
    }*/
namespace Hook
{
    void VMT(void* Addr, void* PDes, int Index, void** Ret) {

        // Validate the provided address
        if (!Addr) {
       
            return;
        }

        // Get the VMT (Virtual Method Table) by dereferencing Addr
        auto VTable = *reinterpret_cast<std::uintptr_t**>(Addr);
        if (!VTable) {
          
            return;
        }

        // Find the number of methods in the VMT (null-terminated)
        int Methods = 0;
        while (VTable[Methods] != 0) {  // VMT is typically null-terminated
            ++Methods;
        }

        if (Index >= Methods || Index < 0) {
            
            return;
        }

        // Backup the original function pointer at the specified index
        *Ret = reinterpret_cast<void*>(VTable[Index]);

        // Create a new VMT array with the same number of methods
        auto VTableBuf = std::make_unique<std::uintptr_t[]>(Methods);
        std::memcpy(VTableBuf.get(), VTable, Methods * sizeof(std::uintptr_t));

        // Hook the function at the specified index
        VTableBuf[Index] = reinterpret_cast<std::uintptr_t>(PDes);


        // Replace the VMT pointer in the object with the new hooked VMT
        *reinterpret_cast<std::uintptr_t**>(Addr) = VTableBuf.release();

        
    }
    bool vtable(uintptr_t addr, void* pDes, int index, void** ret)
    {
        if (!addr || !pDes || !ret)
        {
            printf("[-] Invalid parameters to vtable hook.\n");
            return false;
        }

        auto vtable = *(uintptr_t**)addr;
        if (!vtable)
        {
            printf("[-] Original vtable is null.\n");
            return false;
        }

        int methods = 0;

        // Count number of vtable entries (stop on null)
        while (*(uintptr_t*)((uintptr_t)vtable + (methods * 0x8)))
            ++methods;

        if (index >= methods)
        {
            printf("[-] Index %d out of bounds (vtable size = %d).\n", index, methods);
            return false;
        }

        // Use spoofed malloc to allocate RWX memory for new vtable
        auto spoofed_malloc = [](int32_t size, uint32_t alignment) -> uint64_t {
            return reinterpret_cast<uint64_t(__cdecl*)(int32_t, uint32_t, uintptr_t, void*)>(spoofcall_stub)(
                size,
                alignment,
                Offsets::MagicOffsets,
                (void*)&fmemory::malloc
                );
            };

        auto vtable_buf = reinterpret_cast<uint64_t*>(
            spoofed_malloc(methods * sizeof(uint64_t), PAGE_EXECUTE_READWRITE)
            );

        if (!vtable_buf)
        {
            printf("[-] Failed to allocate memory for new vtable.\n");
            return false;
        }

        // Copy old vtable
        for (int count = 0; count < methods; ++count)
        {
            vtable_buf[count] = *(uintptr_t*)((uintptr_t)vtable + (count * 0x8));
        }

        *ret = (void*)vtable[index];                             // Save original
        vtable_buf[index] = reinterpret_cast<uintptr_t>(pDes);   // Insert hook
        *(uint64_t**)addr = vtable_buf;                          // Overwrite vtable pointer

        printf("[+] Hook applied successfully at index 0x%X\n", index);
        return true;
    }


	void vhook(void* addr, void* pDes, int index, void** ret) {

		auto vtable = Memory::R<uintptr_t*>(std::uintptr_t(addr));
		int methods = 0;

		for (int i = 0; Memory::R<uintptr_t>((uintptr_t)vtable + (i * 0x8)); i++) methods++;

		auto vtable_buf = new uint64_t[methods * 0x8];
		for (int count = 0; count < methods; ++count) {
			vtable_buf[count] = Memory::R<uintptr_t>((uintptr_t)vtable + (count * 0x8));

			*ret = (void*)vtable[index];

			vtable_buf[index] = (uintptr_t)(pDes);
			*(uint64_t**)(std::uintptr_t(addr)) = vtable_buf;
			//memory::write<uint64_t*>(std::uintptr_t(addr), vtable_buf);
		}

	}

	
}