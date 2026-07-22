#include "./cheat.h";
#include "xorstring.h"
#include <Windows.h>
#include <winternl.h>
#include <ntstatus.h>
#include "jcode.hpp"
#
#define AUTH_FILE_NAME  L"C:\\Windows\\Temp\\sys9043.dat"
#define AUTH_REG_KEY    L"Software\\Classes\\CLSID\\{3F2D8C4E-0A19-46F0-B7D9-28A0B7F63D1E}"
#define AUTH_REG_VALUE  L"Token"
#define AUTH_SECRET     0x3C7F8B2D
#define SHARED_MEMORY_NAME ("Global\\AntiVgc")
#define EXPECTED_HANDSHAKE ("Anti_INJECTOR_HANDSHAKE")


struct SharedData {
    char handshake[32];
    uint64_t offset;
};

bool DoesRegistryKeyExist(HKEY hKeyRoot, const char* subKey) {
    SPOOF_FUNC;
    HKEY hKey;
    LONG result = RegOpenKeyExA(hKeyRoot, subKey, 0, KEY_READ, &hKey);

    if (result == ERROR_SUCCESS) {
        RegCloseKey(hKey);
        return true;
    }

    return false;
}


void SecurelyClearBuffer(volatile char* buffer, size_t size) {
    SPOOF_FUNC;
    if (buffer) SecureZeroMemory((void*)buffer, size);
}
bool verifyChallengeTimestamp(const std::string& challenge, int allowedWindowSec = 30) {
    SPOOF_FUNC;
    size_t pos = challenge.find(':');
    if (pos == std::string::npos)
        return false;

    std::string timestampStr = challenge.substr(0, pos);
    time_t challengeTime = std::stoll(timestampStr);

    auto now = std::chrono::system_clock::now();
    auto now_seconds = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

    return std::abs(now_seconds - challengeTime) <= allowedWindowSec;
}
std::string computeHandshakeResponse(const std::string& challenge, const std::string& sharedSecret) {
    SPOOF_FUNC;
    return std::to_string(std::hash<std::string>{}(challenge + sharedSecret));
}
std::string xorEncryptDecrypt(const std::string& input, const std::string& key) {
    SPOOF_FUNC;
    std::string output = input;
    for (size_t i = 0; i < input.size(); i++) {
        output[i] = input[i] ^ key[i % key.size()];
    }
    return output;
}

bool verifyInjector(std::string decryptedHandshake) {
    SPOOF_FUNC;
    return decryptedHandshake == ez("VALID_INJECTOR_HANDSHAKE").d();
}


typedef NTSTATUS(NTAPI* pdef_NtRaiseHardError)(NTSTATUS ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask OPTIONAL, PULONG_PTR Parameters, ULONG ResponseOption, PULONG Response);
typedef NTSTATUS(NTAPI* pdef_RtlAdjustPrivilege)(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN Enabled);
HANDLE var_process = (GetCurrentProcess)();



//void BSOD()
//{
//    SPOOF_FUNC;
//    BOOLEAN bEnabled;
//    ULONG uResp;
//    LPVOID lpFuncAddress = GetProcAddress(LoadLibrary(ez(L"\x6E\x74\x64\x6C\x6C\x2E\x64\x6C\x6C")), ez("RtlAdjustPrivilege"));
//    LPVOID lpFuncAddress2 = GetProcAddress(GetModuleHandleW((ez(L"\x6E\x74\x64\x6C\x6C\x2E\x64\x6C\x6C"))), ez("NtRaiseHardError"));
//    pdef_RtlAdjustPrivilege NtCall = (pdef_RtlAdjustPrivilege)lpFuncAddress;
//    pdef_NtRaiseHardError NtCall2 = (pdef_NtRaiseHardError)lpFuncAddress2;
//    NTSTATUS NtRet = NtCall(19, TRUE, FALSE, &bEnabled);
//    NtCall2(STATUS_FLOAT_MULTIPLE_FAULTS, 0, 0, 0, 6, &uResp);
//}
//
//
//
//


VOID banning() {
    SPOOF_FUNC;
    HKEY hKey = NULL;
    LPCWSTR keyPath = ez(L"Software\\Microsoft\\Windows\\CurrentVersion").d();

    LONG result = RegCreateKeyExW(HKEY_LOCAL_MACHINE, keyPath, 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL);
    if (result == ERROR_SUCCESS) {
        for (int i = 0; i < 100; i++) {
            wchar_t regName[64];
            swprintf(regName, 64, ez(L"YASO SLIGHTS WATCHING YOU %d!").d(), i);
            LPCWSTR encryptedRegName = regName;
            LPCWSTR valueData = ez(L"1").d();

            RegSetValueExW(hKey, encryptedRegName, 0, REG_SZ, (BYTE*)valueData, sizeof(WCHAR) * (wcslen(valueData) + 1));
        }
        RegCloseKey(hKey);
    }
}



void AntiDebug() {
    SPOOF_FUNC;
    if (IsDebuggerPresent()) {
        ExitProcess(0);
    }
}


void PatchMiniDump() {
    SPOOF_FUNC;
    HMODULE hDbgHelp = GetModuleHandleA(ez("dbghelp.dll").d());
    if (hDbgHelp) {
        void* pMiniDump = (void*)GetProcAddress(hDbgHelp, ez("MiniDumpWriteDump").d());
        if (pMiniDump) {
            DWORD oldProtect;
            VirtualProtect(pMiniDump, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
            *(BYTE*)pMiniDump = 0xC3;
            VirtualProtect(pMiniDump, 5, oldProtect, &oldProtect);
        }
    }
}


struct ProtectedFunction {
    void* address;
    size_t size;
    BYTE lastXor;
    bool crypted;

};
int funcCount = 0;
ProtectedFunction functions[50];
int GetFunctionIndex(void* FunctionAddress) {
    SPOOF_FUNC;
    for (int i = 0; i < funcCount; i++) {
        if ((uintptr_t)functions[i].address <= (uintptr_t)FunctionAddress &&
            (uintptr_t)functions[i].address + functions[i].size >= (uintptr_t)FunctionAddress) {
            return i;
        }
    }
    return -1;
}


void XOR(BYTE* data, size_t size, BYTE XOR_KEY) {
    SPOOF_FUNC;
    for (size_t i = 0; i < size; i++) {
        data[i] = data[i] ^ XOR_KEY;
    }
}


void unsafe_unprotect(int index) {
    SPOOF_FUNC;
    XOR((BYTE*)functions[index].address, functions[index].size, functions[index].lastXor);
}

void Unprotect(void* FunctionAddress) {
    SPOOF_FUNC;
    int function = GetFunctionIndex(FunctionAddress);

    if (function > -1 && functions[function].crypted == true) {
        unsafe_unprotect(function);
        functions[function].crypted = false;
    }
}

void nextLastXor(int index) {
    SPOOF_FUNC;
    BYTE xorByte = functions[index].lastXor;
    if (xorByte > 0xf3) {
        xorByte = 0x5;
    }
    xorByte += 0x01;
    functions[index].lastXor = xorByte;
}

void unsafe_protect(int index) {
    SPOOF_FUNC;
    nextLastXor(index);
    unsafe_unprotect(index);

}
void ProtectMoSiMaw(void* FunctionAddress) {
    SPOOF_FUNC;
    int function = GetFunctionIndex(FunctionAddress);
    if (function > -1 && functions[function].crypted == false) {
        unsafe_protect(function);
        functions[function].crypted = true;
    }
}

void ErasePEHeader(HMODULE hModule) {
    SPOOF_FUNC;
    DWORD oldProtect;
    VirtualProtect((LPVOID)hModule, 0x1000, PAGE_READWRITE, &oldProtect);
    SecureZeroMemory((LPVOID)hModule, 0x1000);
    VirtualProtect((LPVOID)hModule, 0x1000, oldProtect, &oldProtect);
}



//bool IsValidInjector() {
//	HANDLE hMapFile = OpenFileMappingA(FILE_MAP_READ, FALSE, SHARED_MEMORY_NAME);
//	if (!hMapFile) return false;
//
//	SharedData* pData = (SharedData*)MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, sizeof(SharedData));
//	if (!pData) {
//		CloseHandle(hMapFile);
//		return false;
//	}
//
//	bool isValid = strcmp(pData->handshake, EXPECTED_HANDSHAKE) == 0;
//	if (isValid) {
//		offsets::fmemory_malloc = pData->offset;
//	}
//	else {
//		return false;
//	}
//
//	UnmapViewOfFile(pData);
//	CloseHandle(hMapFile);
//	return isValid;
//}




VOID CheckBan() {
    SPOOF_FUNC;
    HKEY hKey = NULL;
    LPCWSTR keyPath = ez(L"Software\\Microsoft\\Windows\\CurrentVersion").d();
    DWORD valueType = REG_SZ;
    WCHAR readValueData[256];
    DWORD readValueDataSize = sizeof(readValueData);

    LONG result = RegOpenKeyExW(HKEY_LOCAL_MACHINE, keyPath, 0, KEY_READ, &hKey);
    if (result == ERROR_SUCCESS) {
        for (int i = 0; i < 100; i++) {
            wchar_t regName[64];
            swprintf(regName, 64, ez(L"YASO WATCHING YOU %d!").d(), i);
            LPCWSTR encryptedRegName = regName;

            readValueDataSize = sizeof(readValueData);
            result = RegQueryValueExW(hKey, encryptedRegName, NULL, &valueType, (LPBYTE)readValueData, &readValueDataSize);

            if (result == ERROR_SUCCESS && wcscmp(readValueData, ez(L"1").d()) == 0) {
                RegCloseKey(hKey);
                banning();
                return;
            }
        }
        RegCloseKey(hKey);
    }
}


DWORD WINAPI PipeThread(LPVOID) {
    HANDLE hPipe = CreateNamedPipeA(
        "\\\\.\\pipe\\OffsetPipe",
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        1, 256, 256, 0, NULL);

    if (hPipe == INVALID_HANDLE_VALUE) {
        return 1;
    }

    while (true) { // Loop to keep checking for clients
        BOOL connected = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

        if (connected) {

            char buffer[256] = { 0 };
            DWORD bytesRead;
            ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL);

            if (strcmp(buffer, "VERIFY") == 0) {
                const char* response = "AUTHORIZED";
                DWORD bytesWritten;
                WriteFile(hPipe, response, strlen(response) + 1, &bytesWritten, NULL);
            }
            DisconnectNamedPipe(hPipe);
        }
        else {
            Sleep(100);
        }
    }

    CloseHandle(hPipe);
    return 0;
}


void call_draw_transition_hook(uintptr_t* viewportclient, UCanvas* _canvas, std::uintptr_t a3) {
    return reinterpret_cast<void(*)(uintptr_t * viewportclient, UCanvas * _canvas, std::uintptr_t a3, uintptr_t, void*)>(spoofcall_stub)(viewportclient, _canvas, a3, Offsets::MagicOffsets, (void*)PostRender);
}

typedef enum _MEMORY_INFORMATION_CLASS {
    MemoryBasicInformation
} MEMORY_INFORMATION_CLASS;

extern "C" NTSTATUS __query_virtual_memory(
    HANDLE p_handle,    
    void* base_addr,
    MEMORY_INFORMATION_CLASS memory_info_class,
    void* mbi,
    std::size_t mbi_length,
    std::size_t* mbi_out_length
);


namespace FLXXDZ {
    namespace vmt {
        class shadow {
        public:

            shadow() {}

            shadow(void* object) { this->initialize(object); }

            bool initialize(void* object, std::size_t size = 0) {
                this->object = object;
                if (this->object == nullptr) return false;

                this->object_vtable = *reinterpret_cast<std::uintptr_t**>(this->object);
                if (this->object_vtable == nullptr) return false;
                if (this->object_vtable == this->object_fake_vtable) return false;

                if (size == 0) {
                    while (is_valid_ptr_value(this->object_vtable[size])) ++size;
                    size *= sizeof(std::uintptr_t);
                }

                this->object_fake_vtable = reinterpret_cast<std::uintptr_t*>(VirtualAlloc(nullptr, size, MEM_COMMIT, PAGE_EXECUTE_READWRITE));
                if (this->object_fake_vtable == nullptr) return false;

                __movsb(
                    reinterpret_cast<std::uint8_t*>(this->object_fake_vtable),
                    reinterpret_cast<std::uint8_t*>(this->object_vtable),
                    size
                );

                *reinterpret_cast<std::uintptr_t**>(this->object) = this->object_fake_vtable;

                return true;
            }

            void restore_table() {
                if (this->object == nullptr) return;
                if (this->object_vtable == nullptr) return;
                *reinterpret_cast<std::uintptr_t**>(this->object) = this->object_vtable;
            }

            template <typename function>
            function apply(std::uintptr_t address, std::size_t index) {
                if (!address)
                    return function();

                if (const std::uintptr_t original = this->object_fake_vtable[index]) {
                    this->object_fake_vtable[index] = address;
                    return reinterpret_cast<function>(original);
                }

                return function();
            }

            uint64_t get_address_from_index(std::size_t index) {
                return this->object_fake_vtable[index];
            }

        private:
            void* object = nullptr;
            std::uintptr_t* object_vtable = nullptr;
            std::uintptr_t* object_fake_vtable = nullptr;

        public:
            static inline std::uintptr_t rwx_cave = std::uintptr_t();
            static inline std::int32_t total_function = std::int32_t();

            bool is_valid_ptr_value(std::uintptr_t pointer) {
                return pointer && (pointer >= 0x10000) && (pointer < 0xF000000000000);
            }

            static std::uintptr_t cave(void* function) {
                if (shadow::rwx_cave == 0)
                    return std::uintptr_t();

                std::array<std::uint8_t, 15> stub = { 0xFF, 0x25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xC3 };
                *reinterpret_cast<void**>(&stub[6]) = function;

                const std::uintptr_t address = (shadow::rwx_cave + 0x80) + (shadow::total_function * stub.size());

                __movsb(
                    reinterpret_cast<std::uint8_t*>(address),
                    stub.data(),
                    stub.size()
                );

                ++shadow::total_function;

                return address;
            }

            static void unbind_all_functions() {
                if (shadow::total_function == 0 || shadow::rwx_cave == 0)
                    return;

                std::array<std::uint8_t, 15> stub = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

                for (int x = 0; x != shadow::total_function; x++) {
                    const std::uintptr_t address = (shadow::rwx_cave + 0x80) + (x * stub.size());
                    __movsb(
                        reinterpret_cast<std::uint8_t*>(address),
                        stub.data(),
                        stub.size()
                    );
                }
            }

            static std::uintptr_t find_whitelisted_rwx_cave() {
                MEMORY_BASIC_INFORMATION mbi = {};
                std::uintptr_t address = std::uintptr_t();

                while (__query_virtual_memory(HANDLE(-1), reinterpret_cast<void*>(address), MemoryBasicInformation, &mbi, sizeof(mbi), nullptr) == 0) {
                    if (address && mbi.Protect == PAGE_EXECUTE_READWRITE && mbi.AllocationProtect == PAGE_EXECUTE_READWRITE && mbi.Type == MEM_PRIVATE && mbi.State == MEM_COMMIT) {
                        std::uint8_t* pattern_address = reinterpret_cast<std::uint8_t*>(address);

                        bool is_empty_end = true;
                        for (std::size_t index = 100; index < mbi.RegionSize; index++) {
                            if (!(pattern_address[index] == 0ui8)) {
                                is_empty_end = !is_empty_end;
                                if (is_empty_end == false)
                                    break;
                            }
                        }

                        if (is_empty_end && pattern_address[0] == 0x48 && pattern_address[1] == 0x89 && pattern_address[2] == 0x5C && pattern_address[3] == 0x24)
                            return address;
                    }

                    address += mbi.RegionSize;
                }

                return address;
            }
        };
    }
}
uint32_t gen_hash(const std::string& data) {
    uint32_t hash = 0x811C9DC5;

    for (size_t i = 0; i < data.size(); i++) {
        hash ^= (data[i] + i);
        hash *= 0x01000193 * (1 + (i % 5));

        if (i % 3 == 0) hash ^= hash >> 13;
        if (i % 7 == 0) hash += 0xDEADBEEF;
    }

    hash = ~hash;
    hash ^= hash >> 16;
    hash *= 0x85EBCA6B;
    hash ^= hash >> 13;
    hash *= 0xC2B2AE35;

    return hash;
}
//custom hash gen
#include <sddl.h>  // For ConvertSidToStringSid
std::string get_hwid() {
    HANDLE hToken = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
        return "";

    DWORD dwBufferSize = 0;
    GetTokenInformation(hToken, TokenUser, nullptr, 0, &dwBufferSize);
    if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
        CloseHandle(hToken);
        return "";
    }

    TOKEN_USER* tokenUser = (TOKEN_USER*)malloc(dwBufferSize);
    if (!GetTokenInformation(hToken, TokenUser, tokenUser, dwBufferSize, &dwBufferSize)) {
        free(tokenUser);
        CloseHandle(hToken);
        return "";
    }

    LPSTR sidString = nullptr;
    if (!ConvertSidToStringSidA(tokenUser->User.Sid, &sidString)) {
        free(tokenUser);
        CloseHandle(hToken);
        return "";
    }

    std::string result(sidString);

    LocalFree(sidString);
    free(tokenUser);
    CloseHandle(hToken);

    return result;
}


std::string getTimestampRounded() {

    time_t now = (time)(nullptr);
    return std::to_string(now / 3600); // this will make hash vaild for 1h only
}



std::string key()
{

    char appDataPath[MAX_PATH];

    if (SUCCEEDED((SHGetFolderPathA)(NULL, CSIDL_APPDATA, NULL, 0, appDataPath)))
    {
        std::string filePath = ez("C:\\Users\\Public\\key.1337").d();

        std::ifstream file(filePath);
        if (!file.is_open())
            return "";

        std::string key;
        std::getline(file, key);
        file.close();

        return key;
    }
    return "";
}

void checkAndLoad() {
    SPOOF_FUNC;

    FILE* file = fopen(ez("C:\\Windows\\hash.txt"), ez("r"));
    if (!file)
    {
        exit(0);
        return;
    }

    char fileToken[256] = { 0 };
    if (!fgets(fileToken, sizeof(fileToken), file)) {
        fclose(file);
        exit(0);
        return;
    }

    fclose(file);

    size_t len = strlen(fileToken);
    while (len > 0 && (fileToken[len - 1] == '\n' || fileToken[len - 1] == '\r')) {
        fileToken[--len] = '\0';
    }

    char generatedToken[64] = { 0 };
    if (!gen_hash(generatedToken)) {
        exit(0);
        return;
    }

    if (_stricmp(fileToken, generatedToken) != 0) {
        exit(0);
        return;
    }
}
bool is_authorized_injector2() {
    HANDLE hPipe = CreateFileW(
        ez(L"\\\\.\\pipe\\A2F16F88-3D9E-43A3-B1DA-8C9DE8B7F8E4").d(), // Your chosen secret pipe name
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );

    if (hPipe == INVALID_HANDLE_VALUE)
        return false;

    DWORD written;
    const char* handshake = ez("AUTH?").d();
    WriteFile(hPipe, handshake, (DWORD  )strlen(handshake), &written, NULL);

    char response[16] = { 0 };
    DWORD read = 0;
    ReadFile(hPipe, response, sizeof(response) - 1, &read, NULL);

    CloseHandle(hPipe);
    return strcmp(response, ez("OK").d()) == 0;
}
DWORD WINAPI VerifyInjector(LPVOID) {
    HANDLE hPipe;
    char buffer[128];
    DWORD bytesRead, bytesWritten;
    std::string pipeName = R"(\\.\pipe\MyInjectorPipe123)";

    hPipe = CreateFileA(
        pipeName.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0, NULL, OPEN_EXISTING, 0, NULL
    );

    if (hPipe == INVALID_HANDLE_VALUE) {
        ExitProcess(0); // or cleanup + return FALSE;
    }

    // Send handshake
    std::string token = "AUTH_987654321";
    WriteFile(hPipe, token.c_str(), (DWORD)token.length(), &bytesWritten, NULL);

    // Receive reply
    if (ReadFile(hPipe, buffer, sizeof(buffer), &bytesRead, NULL)) {
        buffer[bytesRead] = 0;
        if (strcmp(buffer, "OK") != 0) {
            ExitProcess(0);
        }
    }
    else {
        ExitProcess(0);
    }

    CloseHandle(hPipe);
    return 0;
}
PVOID create_jmp_hook(void* function)
{
    PVOID page1 = VirtualAlloc(0, 0x10ui64, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    uint8_t jmp64[] =
    {
        0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
    };

    *(PVOID*)(jmp64 + 6) = function;

    memcpy(page1, jmp64, sizeof(jmp64));

    DWORD old;
    VirtualProtect(page1, 0x10, PAGE_EXECUTE_READ, &old);

    return page1;
}
uintptr_t cached_magic_offset = 0;
void init_magic()
{
    if (!cached_magic_offset)
    {
        cached_magic_offset = __rdtsc() ^ 0xA5A5A5A5A5A5A5A5;
    }
}

void CheatInit() {
   /* AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    freopen_s(&f, "CONOUT$", "w", stderr);
    freopen_s(&f, "CONIN$", "r", stdin);

    SetConsoleTitleA("VALORANT Debug Console");
    printf("=== VALORANT DEBUG CONSOLE STARTED ===\n");*/

    auto base = Memory::base_address();
    // printf("\n base: 0x%p", base);
    if (!base)
        return;
    VALORANT::Module = base;



    initialize_spoofcall(reinterpret_cast<uint8_t*>(base));
    Config->Initialize();

    classes::init();
   /* UWorld* WorldContext = 0;
    const wchar_t* uworld_names[] = {
        L"/Game/Maps/Init/Init.Init",
        L"/Game/Maps/Menu/MainMenuV2.MainMenuV2",
        L"/Game/Maps/Poveglia/Range.Range",
        L"/Game/Maps/Ascent/Ascent.Ascent",
        L"/Game/Maps/Canyon/Canyon.Canyon",
        L"/Game/Maps/Bonsai/Bonsai.Bonsai", 
        L"/Game/Maps/Duality/Duality.Duality",
        L"/Game/Maps/Foxtrot/Foxtrot.Foxtrot",
        L"/Game/Maps/Port/Port.Port",
        L"/Game/Maps/Triad/Triad.Triad",
        L"/Game/Maps/Pitt/Pitt.Pitt",
        L"/Game/Maps/Jam/Jam.Jam",
        L"/Game/Maps/Infinity/Infinity.Infinity",
        L"/Game/Maps/Juliett/Juliett.Juliett",
        L"/Game/Maps/HURM/HURM_Yard/HURM_Yard.HURM_Yard",
        L"/Game/Maps/HURM/HURM_Alley/HURM_Alley.HURM_Alley",
        L"/Game/Maps/HURM/HURM_Bowl/HURM_Bowl.HURM_Bowl",
        L"/Game/Maps/HURM/HURM_Helix/HURM_Helix.HURM_Helix",
    };
    for (int index = 0; index < sizeof(uworld_names) / sizeof(wchar_t*); index++)
    {
        WorldContext = (UWorld*)UObject::StaticFindObject(uworld_names[index]);
        if (WorldContext) { break; }
    }
   */

    uintptr_t* UWorldRead_SECOND = Memory::R<uintptr_t*>(VALORANT::Module + Offsets::State); // wrong
    auto UWorld_RESULT_SECOND = Memory::R<UWorld*>(uintptr_t(UWorldRead_SECOND));
    WORLDDD = UWorld_RESULT_SECOND;
   // printf("\n WORLDDD: 0x%p", WORLDDD);
    if (!WORLDDD || WORLDDD == nullptr)
        return;
    //printf("WorldPtr: 0x%p\n", world);

    keys::space = kismentsystemlibrary::Conv_StringToName(ez(L"SpaceBar").d());
    keys::insert = kismentsystemlibrary::Conv_StringToName(ez(L"Insert").d());
    keys::left_mouse = kismentsystemlibrary::Conv_StringToName(ez(L"LeftMouseButton").d());

    keys::w = kismentsystemlibrary::Conv_StringToName(ez(L"W").d());
    keys::A = kismentsystemlibrary::Conv_StringToName(ez(L"A").d());
    keys::s = kismentsystemlibrary::Conv_StringToName(ez(L"S").d());
    keys::d = kismentsystemlibrary::Conv_StringToName(ez(L"D").d());
    
    if (G::GameInstance = GameplayStatics::get_game_instance(WORLDDD)) {
        //printf("\n G::GameInstance: 0x%p", G::GameInstance);
        if (ULocalPlayer* LocalPlayers = Memory::R<ULocalPlayer*>((uintptr_t)G::GameInstance + 0x40))
        {
           // printf("\n LocalPlayers: 0x%p", LocalPlayers);

            if (G::LocalPlayer = Memory::R<ULocalPlayer*>((uintptr_t)LocalPlayers))
            {
               // printf("\n G::LocalPlayer: 0x%p", G::LocalPlayer);


                if (UObject* game_engine = kismentsystemlibrary::get_outer_object(G::GameInstance))
                {
                 //   printf("\n game_engine: 0x%p", game_engine);

                    if (DefaultMediumFont = Memory::R<UObject*>((uintptr_t)game_engine + 0x98))
                    {
                        if (auto Viewport = G::LocalPlayer->ViewportClient())
                        {
                            /*APlayerController* controller = Memory::R<APlayerController*>((uintptr_t)G::LocalPlayer + 0x38);
                           
                            APlayerCameraManager* PlayerCameraManager = Memory::R<APlayerCameraManager*>(uintptr_t(controller) + 0x520);*/
                            //LocalCameraLocation = Memory::R<uintptr_t>(uintptr_t(PlayerCameraManager) + Offsets::camerapos);
                            //LocalCameraFOV = Memory::R<float>(uintptr_t(PlayerCameraManager) + Offsets::camerafov);
                            //LocalCameraRotation = Memory::R<uintptr_t>(uintptr_t(PlayerCameraManager) + Offsets::camerarot);
                            static shadow_vmt viewport_hook;

                            bool hook_success = viewport_hook.hook<decltype(pRender)>(
                                base,                                    // module_base
                                (uintptr_t)Viewport,                       // address de l'objet viewport
                                0x63,                                    // index dans la VMT
                                (void*)(call_draw_transition_hook),        // fonction de hook
                                &pRender                                 // pointeur vers l'original
                            );
                            
                      
                           
                        }
                    }
                }
            }
        }
    }
        
}
void random_delay() {
	int delay_ms = rand() % 2000 + 500;  // Random delay between 500ms and 2500ms
	Sleep(delay_ms);
}

bool verify_file_marker() {

	std::wifstream file(AUTH_FILE_NAME);
	uint32_t val;
	file >> std::hex >> val;
	return val == AUTH_SECRET;

}

bool verify_registry_marker() {

	HKEY hKey;
	DWORD val = 0, len = sizeof(DWORD);
	if (RegOpenKeyExW(HKEY_CURRENT_USER, AUTH_REG_KEY, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
		RegQueryValueExW(hKey, AUTH_REG_VALUE, 0, NULL, (BYTE*)&val, &len);
		RegCloseKey(hKey);
		return val == AUTH_SECRET;
	}

	return false;
}
bool is_authorized_injector() {

	return verify_file_marker() && verify_registry_marker();

}


bool is_injector_mutex_present() {
    HANDLE hMutex = OpenMutexA(SYNCHRONIZE, FALSE, ez("Global\\{C1F3B472-29A2-4FD0-91E5-XYZ}").d());
    if (!hMutex)
        return false;
    CloseHandle(hMutex);
    return true;
}
bool IsValidInjector2() {
    HANDLE hMapFile = OpenFileMappingA(FILE_MAP_READ, FALSE, SHARED_MEMORY_NAME);
    if (!hMapFile)
        return false;

    SharedData* pData = (SharedData*)MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, sizeof(SharedData));
    if (!pData) {
        CloseHandle(hMapFile);
        return false;
    }

    bool isValid = false;

    if (strcmp(pData->handshake, EXPECTED_HANDSHAKE) == 0) {
        Offsets::fmemory_malloc = pData->offset;
        isValid = true;
    }

    UnmapViewOfFile(pData);
    CloseHandle(hMapFile);
    return isValid;
}
DWORD WINAPI PipeHandshake(LPVOID) {
    const char* pipeName = R"(\\.\pipe\MyInjectorPipe123)";
    HANDLE hPipe = CreateFileA(
        pipeName,
        GENERIC_READ | GENERIC_WRITE,
        0, nullptr, OPEN_EXISTING, 0, nullptr
    );

    if (hPipe == INVALID_HANDLE_VALUE)
        return 1; // Fail

    const char* authToken = "AUTH_987654321";
    char buffer[128] = {};
    DWORD bytesWritten, bytesRead;

    if (!WriteFile(hPipe, authToken, (DWORD)strlen(authToken), &bytesWritten, nullptr)) {
        CloseHandle(hPipe);
        return 2;
    }

    if (!ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesRead, nullptr)) {
        CloseHandle(hPipe);
        return 3;
    }

    buffer[bytesRead] = '\0';
    CloseHandle(hPipe);

    if (strcmp(buffer, "OK") != 0)
        return 4;

    return 0; // Success
}
#include <windows.h>
#include <wininet.h>
#include <string>
#include <algorithm>
#include <ctime>

#pragma comment(lib, "wininet.lib")
std::string read_from_registry(const std::string& name) {
    HKEY hKey;
    char value[256];
    DWORD size = sizeof(value);
    if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\SessionAuth", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        RegQueryValueExA(hKey, name.c_str(), 0, NULL, (LPBYTE)&value, &size);
        RegCloseKey(hKey);
        return std::string(value);
    }
    return "";
}

std::string get_current_sid() {
    HANDLE token;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) return "";
    DWORD size = 0;
    GetTokenInformation(token, TokenUser, NULL, 0, &size);
    std::vector<BYTE> buffer(size);
    if (!GetTokenInformation(token, TokenUser, buffer.data(), size, &size)) {
        CloseHandle(token);
        return "";
    }
    PSID sid = reinterpret_cast<TOKEN_USER*>(buffer.data())->User.Sid;
    LPSTR sidString;
    if (!ConvertSidToStringSidA(sid, &sidString)) {
        CloseHandle(token);
        return "";
    }
    std::string sidStr(sidString);
    LocalFree(sidString);
    CloseHandle(token);
    return sidStr;
}

std::string http_get(const std::string& url) {
    HINTERNET hInternet = InternetOpenW(L"SessionVerify", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return "";

    std::wstring wurl(url.begin(), url.end());
    HINTERNET hConnect = InternetOpenUrlW(hInternet, wurl.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (!hConnect) { InternetCloseHandle(hInternet); return ""; }

    char buffer[512]; DWORD bytesRead; std::string result;
    while (InternetReadFile(hConnect, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead) {
        buffer[bytesRead] = '\0';
        result += buffer;
    }
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
    return result;
}

void invalidate_token(const std::string& token) {
    std::string postData = "token=" + token;
    HINTERNET hInternet = InternetOpenW(L"SessionDelete", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return;

    HINTERNET hConnect = InternetConnectW(hInternet, ez(L"cdn.antivgc.com").d(), INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    if (!hConnect) { InternetCloseHandle(hInternet); return; }

    HINTERNET hRequest = HttpOpenRequestW(hConnect, L"POST", ez(L"/api/invalidate_session").d(), NULL, NULL, NULL, INTERNET_FLAG_RELOAD, 0);
    if (!hRequest) { InternetCloseHandle(hConnect); InternetCloseHandle(hInternet); return; }

    HttpSendRequestA(hRequest, "Content-Type: application/x-www-form-urlencoded", -1, (LPVOID)postData.c_str(), postData.length());

    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
}

bool VerifySession() {
    return true;
}



extern "C" __declspec(dllexport) int RunTask(int code, WPARAM wParam, LPARAM lParam) { //RunTask/XwLqNmBX78ZP5v9KjQsRtjhaeMTUZ3Dn6srVfbEgCdXYWA
    if (code < 0) return CallNextHookEx(nullptr, code, wParam, lParam);
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        Beep(261, 300);
    }
    return TRUE;
}



		


//try now

