#pragma once
#include <cstdint>
#include "offsets.h"
#include "unrealengine.h"
#include <Psapi.h>
#include <vector>
#include "definitions.hpp"
#include "importher.hpp"
#include <Psapi.h>
#include <vector>
#ifdef _MSC_VER
#include <softintrin.h>
#endif
#include "Hookign/spoofer.h"
#include "Hookign/ret_spoofing.h"

namespace Memory
{
    inline bool is_bad_write_ptr(LPVOID ptr, uint64_t size)
    {

        return ptr ? false : true;
    }

    inline bool valid_pointer(uintptr_t address)
    {
        return address > 0x1000 && address < 0x7FFFFFFFFFFF;
    }


    //template<typename T>
    //inline T R(uintptr_t address)
    //{
    //    // You can insert your real valid_pointer logic here
    //    if (address > 0x1000 && address < 0x7FFFFFFFFFFF)
    //        return *reinterpret_cast<T*>(address);
    //    return T{};
    //}

    //template<typename T>
    //inline bool W(uintptr_t address, const T& value)
    //{
    //    if (address > 0x1000 && address < 0x7FFFFFFFFFFF)
    //    {
    //        *reinterpret_cast<T*>(address) = value;
    //        return true;
    //    }
    //    return false;
    //}
    template <typename T>
    __forceinline T Read(std::uintptr_t address) {
        if (!valid_pointer(address)) return T{};
        return *reinterpret_cast<T*>(address);
    }

    //template <typename T>
    //__forceinline void W(std::uintptr_t address, T value) {
    //    
    //    *reinterpret_cast<T*>(address) = value;
    //}


    template<typename t>
    inline t R(DWORD_PTR address, const t& def = t())
    {

        if (valid_pointer(address)) {

            return *(t*)(address);
        }
        return def;
    }

    template<typename t>
    inline bool W(DWORD_PTR address, const t& value)
    {

        if (valid_pointer(address)) {

            *(t*)(address) = value;
            return true;
        }
        return false;
    }

    inline bool IsValidPointer(uintptr_t address, bool requireWrite = false) {
        MEMORY_BASIC_INFORMATION mbi;
        if (!VirtualQuery(reinterpret_cast<LPCVOID>(address), &mbi, sizeof(mbi))) {
            return false;
        }
        if (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS)) {
            return false;
        }
        if (requireWrite) {
            // List of writable page protections (for usermode)
            if (!(mbi.Protect & PAGE_READWRITE) &&
                !(mbi.Protect & PAGE_WRITECOPY) &&
                !(mbi.Protect & PAGE_EXECUTE_READWRITE) &&
                !(mbi.Protect & PAGE_EXECUTE_WRITECOPY)) {
                return false;
            }
        }
        return true;
    }
    bool IsValidPointer2(void* ptr) {
        uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
        return addr > 0x10000 && addr < 0x00007FFFFFFFFFFF;  // avoid kernel and null
    }
    inline uintptr_t resolve_api(const char* module, const char* func) {
        auto hMod = LoadLibraryA(module);
        return reinterpret_cast<uintptr_t>(GetProcAddress(hMod, func));
    }
    void EraseIATAndRelocs(HMODULE hModule) {
        if (!hModule) return;

        auto* dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(hModule);
        auto* ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>((uintptr_t)hModule + dosHeader->e_lfanew);
        auto& optionalHeader = ntHeaders->OptionalHeader;

        // 1. Erase Import Directory (IAT)
        if (optionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size) {
            DWORD oldProtect = 0;
            auto* importDir = reinterpret_cast<void*>(
                (uintptr_t)hModule + optionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
            SIZE_T size = optionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size;

            if (VirtualProtect(importDir, size, PAGE_READWRITE, &oldProtect)) {
                SecureZeroMemory(importDir, size);
                VirtualProtect(importDir, size, oldProtect, &oldProtect);
            }
            optionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress = 0;
            optionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size = 0;
        }

        // 2. Erase Relocation Table
        if (optionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size) {
            DWORD oldProtect = 0;
            auto* relocDir = reinterpret_cast<void*>(
                (uintptr_t)hModule + optionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);
            SIZE_T size = optionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size;

            if (VirtualProtect(relocDir, size, PAGE_READWRITE, &oldProtect)) {
                SecureZeroMemory(relocDir, size);
                VirtualProtect(relocDir, size, oldProtect, &oldProtect);
            }
            optionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress = 0;
            optionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size = 0;
        }
    }
    inline void clear_pe_header(HMODULE hModule) {
        DWORD old;
        VirtualProtect(hModule, 0x1000, PAGE_EXECUTE_READWRITE, &old);
        memset(hModule, 0, 0x1000);
        VirtualProtect(hModule, 0x1000, old, &old);
    }


    uintptr_t base_address() {
        static uintptr_t address = 0;
        if (!address)
            address = R<uintptr_t>(__readgsqword(0x60) + 0x10);
        return address;
    }


    template<typename T>
    class simple_data_locator {
        T* _data;
        size_t capacity;
        size_t length;

    public:
        simple_data_locator() : _data(nullptr), capacity(0), length(0) {}

        ~simple_data_locator() {

            LI_FN(free)(_data);
        }

        void push_back(T value) {

            if (length == capacity) {
                capacity = capacity == 0 ? 1 : capacity * 2;
                _data = (T*)LI_FN(realloc)(_data, capacity * sizeof(T));
            }
            _data[length++] = value;
        }

        T operator[](size_t index) const {
            if (index >= length) return T{};
            return _data[index];
        }

        size_t size() const {

            return length;
        }

        T* data() {

            return _data;
        }

        const T* data() const {

            return _data;
        }
    };

    uint64_t ida_scanner(uint64_t module_base, const char* signature, int p_index, bool relativeAddress)
    {

        auto pattern_to_byte = [](const char* pattern) -> simple_data_locator<int>
            {
                simple_data_locator<int> bytes;
                auto start = const_cast<char*>(pattern);
                auto end = const_cast<char*>(pattern) + LI_FN(strlen)(pattern);

                for (auto current = start; current < end; ++current)
                {
                    if (*current == '?')
                    {
                        ++current;
                        if (*current == '?') ++current;
                        bytes.push_back(-1);
                    }
                    else
                    {
                        bytes.push_back(LI_FN(strtoul)(current, &current, 16));
                    }
                }
                return bytes;
            };

        auto dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(module_base);
        auto nt_headers = reinterpret_cast<PIMAGE_NT_HEADERS>((reinterpret_cast<uint8_t*>(module_base) + dos_header->e_lfanew));
        auto text_section = reinterpret_cast<PIMAGE_SECTION_HEADER>(IMAGE_FIRST_SECTION(nt_headers));

        auto size_of_image = text_section->SizeOfRawData;
        auto pattern_bytes = pattern_to_byte(signature);
        auto scan_bytes = reinterpret_cast<uint8_t*>(module_base) + text_section->VirtualAddress;

        auto s = pattern_bytes.size();
        auto d = pattern_bytes.data();

        auto mbi = MEMORY_BASIC_INFORMATION{ 0 };
        uint8_t* next_address = 0;

        for (auto i = 0ul; i < size_of_image - s; ++i)
        {
            bool found = true;
            for (auto j = 0ul; j < s; ++j)
            {
                auto current_address = scan_bytes + i + j;
                if (current_address >= next_address)
                {
                    if (!LI_FN(VirtualQuery)(reinterpret_cast<void*>(current_address), &mbi, sizeof(mbi)))
                    {
                        break;
                    }

                    if (mbi.Protect == PAGE_NOACCESS)
                    {
                        i += ((reinterpret_cast<uintptr_t>(mbi.BaseAddress) + mbi.RegionSize) - (reinterpret_cast<uintptr_t>(scan_bytes) + i));
                        i--;
                        found = false;
                        break;
                    }
                    else
                    {
                        next_address = reinterpret_cast<uint8_t*>(mbi.BaseAddress) + mbi.RegionSize;
                    }
                }

                if (scan_bytes[i + j] != d[j] && d[j] != -1)
                {
                    found = false;
                    break;
                }
            }

            if (found)
            {
                if (relativeAddress)
                {
                    return static_cast<uintptr_t>(reinterpret_cast<uintptr_t>(&scan_bytes[i]) + *reinterpret_cast<const int*>(reinterpret_cast<uintptr_t>(&scan_bytes[i]) + (p_index - sizeof(int))) + p_index);
                }
                else
                {
                    return reinterpret_cast<uintptr_t>(&scan_bytes[i]);
                }
            }
        }

        return NULL;
    }

}
