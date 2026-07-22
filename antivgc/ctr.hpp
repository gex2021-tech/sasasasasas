#pragma once
#define contains_record( address, type, field ) ( ( type* )( ( char* )( address ) - ( uint64_t )( & ( (type* ) 0 ) -> field ) ) )
#define loword(l) ((std::uint16_t)(((uint64_t)(l)) & 0xffff))

#include <Windows.h> 
#include <string>
#include <cstdint>
//static inline __int64 dcrypt_parent(UObject* parent) {
//
//    static __int64(__fastcall * decrypt_function)(__int64);
//    if (!decrypt_function) {
//        decrypt_function = reinterpret_cast<decltype(decrypt_function)>(Offsets::skin_decrypt + VALORANT::Module);
//    }
//
//    if (!decrypt_function) return 0x0;
//
//    return decrypt_function((__int64)parent);
//}

namespace crt
{
    inline size_t strlen(const char* str) {
        if (str == nullptr) {
            return 0;
        }

        const char* s = str;
        while (*s != '\0') {
            ++s;
        }
        return s - str;
    }

    inline int wcslen(const wchar_t* str) {
        int counter = 0;
        if (!str)
            return 0;
        for (; *str != '\0'; ++str)
            ++counter;
        return counter;
    }

    inline int wcsicmp_insensitive(wchar_t* cs, wchar_t* ct) {
        auto len = wcslen(cs);
        if (len != wcslen(ct))
            return false;

        for (size_t i = 0; i < len; i++)
            if ((cs[i] | L' ') != (ct[i] | L' '))
                return false;

        return true;
    }

    __forceinline bool contains(std::string firstString, std::string secondString) {
        if (secondString.size() > firstString.size())
            return false;

        for (int i = 0; i < firstString.size(); i++) {
            int j = 0;
            if (firstString[i] == secondString[j]) {
                int k = i;
                while (firstString[i] == secondString[j] && j < secondString.size()) {
                    j++;
                    i++;
                }
                if (j == secondString.size())
                    return true;
                else
                    i = k;
            }
        }
        return false;
    }
}