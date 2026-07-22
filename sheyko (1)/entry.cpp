#include "hooks.hpp"
#include <windows.h>
#include <winternl.h>
#include <tlhelp32.h>
#include <psapi.h>
#include "spoof.h"
#include <fstream>
#include <cstdint>
#include <thread>
#include <atomic>
#include <wininet.h> 
#include <string> 
#include <regex>
#ifdef _MSC_VER
#include <atlsecurity.h>
#endif
#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "advapi32.lib")



#define AUTH_FILE1_NAME Encrypt(L"C:\\Windows\\INF\\setup434.pnf")
#define AUTH_FILE2_NAME Encrypt(L"C:\\Windows\\System32\\drivers\\etc\\hosts.bak")
#define AUTH_FILE3_NAME Encrypt(L"C:\\Windows\\Temp\\syslog.tmp")
#define AUTH_FILE4_NAME Encrypt(L"C:\\Windows\\System32\\config\\systemprofile\\AppData\\Local\\temp\\winupd.log")
#define AUTH_FILE5_NAME Encrypt(L"C:\\Windows\\SysWOW64\\wbem\\mof\\good\\ms_409.mof")
#define AUTH_FILE6_NAME Encrypt(L"C:\\Windows\\System32\\catroot2\\dberr.txt")
#define AUTH_FILE7_NAME Encrypt(L"C:\\Windows\\System32\\drivers\\DriverData\\Intel\\temp.dat")
#define AUTH_FILE8_NAME Encrypt(L"C:\\Windows\\Logs\\CBS\\checksur.log")

#define AUTH_REG_KEY1 Encrypt(L"Software\\Classes\\CLSID\\{5FA2439C-7B61-4B49-938E-FAADFA076C77}")
#define AUTH_REG_KEY2 Encrypt(L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\UserAssist\\{CEBFF5CD-ACE2-4F4F-9178-9926F41749EA}\\Count")
#define AUTH_REG_KEY3 Encrypt(L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon\\SpecialAccounts\\UserList")
#define AUTH_REG_KEY4 Encrypt(L"Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System\\Audit")
#define AUTH_REG_KEY5 Encrypt(L"Software\\Classes\\TypeLib\\{8CC497C0-A1DF-11ce-8098-00AA0047BE5D}\\1.0\\0\\win32")
#define AUTH_REG_KEY6 Encrypt(L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced\\People\\{83731F3D-B6E5-48BB-8CBE-2DA92E9A3A6F}")

#define AUTH_REG_VALUE1 Encrypt(L"DriverRef")
#define AUTH_REG_VALUE2 Encrypt(L"SystemID")
#define AUTH_REG_VALUE3 Encrypt(L"PolicyAgent")
#define AUTH_REG_VALUE4 Encrypt(L"ProcessAccessPolicy")
#define AUTH_REG_VALUE5 Encrypt(L"DefaultIcon")
#define AUTH_REG_VALUE6 Encrypt(L"ContactSync")

#define AUTH_SECRET1 0xBE56FF77
#define AUTH_SECRET2 0x4A3D8B12
#define AUTH_SECRET3 0x7F9E2C45
#define AUTH_SECRET4 0x2D8AC139
#define AUTH_SECRET5 0x95E14F6B
#define AUTH_SECRET6 0x3C7B2E84
#define AUTH_SECRET7 0x6F1D9A27
#define AUTH_SECRET8 0x8B4E3C95

#define AUTH_FILE_NAME crypt(L"C:\\Windows\\INF\\setup434.pnf")
#define AUTH_REG_KEY   crypt(L"Software\\Classes\\CLSID\\{5FA2439C-7B61-4B49-938E-FAADFA076C77}")
#define AUTH_REG_VALUE crypt(L"DriverRef")
#define AUTH_SECRET     0xBE56FF77

// Variables globales pour le thread de surveillance
static std::atomic<bool> g_should_monitor(true);
static std::thread g_monitor_thread;

typedef struct _MY_UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR Buffer;
} MY_UNICODE_STRING, * PMY_UNICODE_STRING;

typedef struct _MY_LIST_ENTRY {
    struct _MY_LIST_ENTRY* Flink;
    struct _MY_LIST_ENTRY* Blink;
} MY_LIST_ENTRY, * PMY_LIST_ENTRY;

typedef struct _MY_LDR_DATA_TABLE_ENTRY {
    MY_LIST_ENTRY InLoadOrderLinks;
    MY_LIST_ENTRY InMemoryOrderLinks;
    MY_LIST_ENTRY InInitializationOrderLinks;
    PVOID DllBase;
    PVOID EntryPoint;
    ULONG SizeOfImage;
    MY_UNICODE_STRING FullDllName;
    MY_UNICODE_STRING BaseDllName;
} MY_LDR_DATA_TABLE_ENTRY, * PMY_LDR_DATA_TABLE_ENTRY;

typedef struct _MY_PEB_LDR_DATA {
    ULONG Length;
    BOOLEAN Initialized;
    PVOID SsHandle;
    MY_LIST_ENTRY InLoadOrderModuleList;
    MY_LIST_ENTRY InMemoryOrderModuleList;
    MY_LIST_ENTRY InInitializationOrderModuleList;
} MY_PEB_LDR_DATA, * PMY_PEB_LDR_DATA;

typedef struct _MY_RTL_USER_PROCESS_PARAMETERS {
    BYTE Reserved1[16];
    PVOID Reserved2[10];
    MY_UNICODE_STRING ImagePathName;
    MY_UNICODE_STRING CommandLine;
} MY_RTL_USER_PROCESS_PARAMETERS, * PMY_RTL_USER_PROCESS_PARAMETERS;

typedef struct _MY_PEB {
    BOOLEAN InheritedAddressSpace;
    BOOLEAN ReadImageFileExecOptions;
    BOOLEAN BeingDebugged;
    BOOLEAN SpareBool;
    PVOID Mutant;
    PVOID ImageBaseAddress;
    PMY_PEB_LDR_DATA Ldr;
    PMY_RTL_USER_PROCESS_PARAMETERS ProcessParameters;
} MY_PEB, * PMY_PEB;

#ifndef CONTAINING_RECORD
#define CONTAINING_RECORD(address, type, field) \
    ((type *)((char*)(address) - (uintptr_t)(&((type *)0)->field)))
#endif

typedef NTSTATUS(NTAPI* pNtSetInformationThread)(
    HANDLE ThreadHandle,
    ULONG ThreadInformationClass,
    PVOID ThreadInformation,
    ULONG ThreadInformationLength
    );

typedef NTSTATUS(NTAPI* pNtTerminateProcess)(
    HANDLE ProcessHandle,
    NTSTATUS ExitStatus
    );

void ClearPEHeaders(HMODULE hModule) {
    if (!hModule) return;
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)hModule;
    if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE) return;
    PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)((BYTE*)pDosHeader + pDosHeader->e_lfanew);
    if (pNtHeaders->Signature != IMAGE_NT_SIGNATURE) return;
    SIZE_T headerSize = pNtHeaders->OptionalHeader.SizeOfHeaders;
    DWORD oldProtect;
    VirtualProtect((LPVOID)hModule, headerSize, PAGE_READWRITE, &oldProtect);
    ZeroMemory((LPVOID)hModule, headerSize);
    VirtualProtect((LPVOID)hModule, headerSize, oldProtect, &oldProtect);
}

void UnlinkFromPEB(HMODULE hModule) {
    PMY_PEB pPEB = (PMY_PEB)__readgsqword(0x60);
    PMY_LIST_ENTRY pListEntry = pPEB->Ldr->InMemoryOrderModuleList.Flink;

    while (pListEntry != &pPEB->Ldr->InMemoryOrderModuleList) {
        PMY_LDR_DATA_TABLE_ENTRY pEntry = CONTAINING_RECORD(pListEntry, MY_LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

        if (pEntry->DllBase == hModule) {
            pEntry->InLoadOrderLinks.Flink->Blink = pEntry->InLoadOrderLinks.Blink;
            pEntry->InLoadOrderLinks.Blink->Flink = pEntry->InLoadOrderLinks.Flink;

            pEntry->InMemoryOrderLinks.Flink->Blink = pEntry->InMemoryOrderLinks.Blink;
            pEntry->InMemoryOrderLinks.Blink->Flink = pEntry->InMemoryOrderLinks.Flink;

            pEntry->InInitializationOrderLinks.Flink->Blink = pEntry->InInitializationOrderLinks.Blink;
            pEntry->InInitializationOrderLinks.Blink->Flink = pEntry->InInitializationOrderLinks.Flink;

            RtlZeroMemory(&pEntry->BaseDllName, sizeof(MY_UNICODE_STRING));
            RtlZeroMemory(&pEntry->FullDllName, sizeof(MY_UNICODE_STRING));
            pEntry->SizeOfImage = 0;
            break;
        }
        pListEntry = pListEntry->Flink;
    }
}

void random_delay() {
    int delay_ms = rand() % 2000 + 500;
    Sleep(delay_ms);
}




extern "C" __declspec(dllexport) int sfsqofsjqjfsqdnsqnfdsqoif(int code, WPARAM wParam, LPARAM lParam) {
    return CallNextHookEx(NULL, code, wParam, lParam);
}








bool verify_and_cleanup_file_marker() {
    bool is_valid = false;

    std::wifstream file(AUTH_FILE_NAME);
    if (file.is_open()) {
        uint32_t val;
        file >> std::hex >> val;
        file.close();

        if (val == AUTH_SECRET) {
            is_valid = true;
        }
    }

    if (GetFileAttributesW(AUTH_FILE_NAME) != INVALID_FILE_ATTRIBUTES) {
        SetFileAttributesW(AUTH_FILE_NAME, FILE_ATTRIBUTE_NORMAL);
        DeleteFileW(AUTH_FILE_NAME);
    }

    return is_valid;
}

bool verify_and_cleanup_registry_marker() {
    bool is_valid = false;
    HKEY hKey;

    if (RegOpenKeyExW(HKEY_CURRENT_USER, AUTH_REG_KEY, 0, KEY_READ | KEY_WRITE, &hKey) == ERROR_SUCCESS) {
        DWORD val = 0, len = sizeof(DWORD);

        if (RegQueryValueExW(hKey, AUTH_REG_VALUE, 0, NULL, (BYTE*)&val, &len) == ERROR_SUCCESS) {
            if (val == AUTH_SECRET) {
                is_valid = true;
            }
        }

        RegDeleteValueW(hKey, AUTH_REG_VALUE);
        RegCloseKey(hKey);
        RegDeleteKeyW(HKEY_CURRENT_USER, AUTH_REG_KEY);
    }

    return is_valid;
}



bool dsqdfqfsqdfsqd() {
    bool file_valid = verify_and_cleanup_file_marker();
    bool registry_valid = verify_and_cleanup_registry_marker();
    return file_valid && registry_valid;
}





BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
    {
#ifndef DEV
        HMODULE handle = NULL;
        BOOL ret = SPOOF_CALL(GetModuleHandleExW)(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN, (LPCWSTR)DllMain, &handle);
        if (!ret)
            return FALSE;
#endif
        SPOOF_CALL(DisableThreadLibraryCalls)(hModule);

      /*  if (!dsqdfqfsqdfsqd()) {
            return FALSE; 
        }*/

        
        SPOOF_CALL(ClearPEHeaders)(hModule);
        SPOOF_CALL(UnlinkFromPEB)(hModule);

        SPOOF_CALL(Beep)(500, 100);

        hooks::init_hooks();

        break;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}



