#include <windows.h>
#include <iostream>

int main() {
    std::cout << "[+] Dynamic Link Library Loader" << std::endl;
    std::cout << "[+] Loading emuvgc.dll..." << std::endl;

    // Load the DLL into the process address space
    HMODULE hModule = LoadLibraryW(L"C:\\Users\\gex20\\OneDrive\\Escritorio\\emuvgc.dll");

    if (!hModule) {
        DWORD error = GetLastError();
        std::cout << "[-] Failed to load DLL. Error code: " << error << std::endl;
        std::cout << "[!] Press Enter to exit..." << std::endl;
        std::cin.get();
        return 1;
    }

    std::cout << "[+] emuvgc.dll loaded successfully at base address: 0x" 
              << std::hex << (uintptr_t)hModule << std::endl;
    std::cout << "[+] DllMain initialized." << std::endl;
    std::cout << "[+] Press Enter to unload DLL and exit..." << std::endl;

    std::cin.get();

    // Free the loaded module upon exit
    FreeLibrary(hModule);
    std::cout << "[+] Module unloaded. Exiting." << std::endl;

    return 0;
}
