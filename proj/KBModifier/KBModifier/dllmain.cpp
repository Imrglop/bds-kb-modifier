//
//
//    KBModifier for BDS
//        By Imrglop
//
//

#include "dllmain.h"

Config config;
Hook hook;
DWORD_PTR moduleBase;
DWORD_PTR codeAddy;

namespace kb // offsets
{ 
    std::vector<string> kbKeys = { "decreased", "default", "extended", "double", "quad", "far", "extreme" };
    std::vector<std::vector<byte>> kbValues = {
        { 0xF3, 0x0F, 0x10, 0x0D, 0xC6, 0x53, 0xCA, 0x00 }, // dec
        { 0xF3, 0x0F, 0x10, 0x0D, 0x72, 0x54, 0xCA, 0x00 }, // def
        { 0xF3, 0x0F, 0x10, 0x0D, 0xF2, 0x54, 0xCA, 0x00 }, // extended
        { 0xF3, 0x0F, 0x10, 0x0D, 0x6A, 0x55, 0xCA, 0x00 }, // double
        { 0xF3, 0x0F, 0x10, 0x0D, 0x4A, 0x56, 0xCA, 0x00 }, // quad
        { 0xF3, 0x0F, 0x10, 0x0D, 0x12, 0x59, 0xCA, 0x00 }, // far
        { 0xF3, 0x0F, 0x10, 0x0D, 0x72, 0x59, 0xCA, 0x00 }, // extreme
    }; // edited asm codes
    std::vector<byte> get(std::string key) {
        return kbValues[config.findInList(kbKeys, key)];
    }
}// decreased: 1bd4cf4        default: 1bd4da0      extended: 1bd4e20   double: 1bd4e98
// quad: 1bd4f78   far: 1bd5240   extreme: 1bd52a0

DWORD_PTR GetModuleBase(DWORD pID, wchar_t* moduleName) 
{
    DWORD_PTR _moduleBase = 0;
    DWORD _err;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pID);
    if (hSnapshot != INVALID_HANDLE_VALUE) 
    {
        MODULEENTRY32W moduleEntry32;
        moduleEntry32.dwSize = sizeof(MODULEENTRY32W);
        if (Module32FirstW(hSnapshot, &moduleEntry32)) {
            do {
                if (_tcsicmp(moduleEntry32.szModule, moduleName) == NULL) {
                    // found match
                    _moduleBase = (DWORD_PTR)moduleEntry32.modBaseAddr;
                    break;
                }
            } while (Module32NextW(hSnapshot, &moduleEntry32));
        }
        CloseHandle(hSnapshot);
    }
    else {
        _err = GetLastError();
        log << "CreateTH32Snapshot failed! error: " << _err << '\n';
    }
    return _moduleBase;
}

void SetKnockbackMode(int mode)
{
    hook.WriteBytes(codeAddy,
        kb::kbValues[mode]);
}

void SetKnockbackModeStr(string mode)
{
    hook.WriteBytes(codeAddy,
        kb::get(mode));
}

void ReloadConfig()
{
    config.load();
}

void SetKnockbackStatus(int status) // 1: knockback ON, 0: knockback OFF
{
    if (status == 1) {
        // turn on
        hook.WriteBytes((DWORD_PTR)(moduleBase + 0xF2F936), { 0xF3, 0x0F, 0x11, 0x93, 0xD4, 0x04, 0x00, 0x00 }); // (x) NOP -> movss [rbx+000004D4],xmm2
        hook.WriteBytes((DWORD_PTR)(moduleBase + 0xF2F94E), { 0xF3, 0x0F, 0x11, 0x93, 0xD8, 0x04, 0x00, 0x00 }); // (x) NOP -> movss [rbx+000004D8],xmm2
        hook.WriteBytes((DWORD_PTR)(moduleBase + 0xF2F966), { 0xF3, 0x0F, 0x11, 0x83, 0xDC, 0x04, 0x00, 0x00 }); // (x) NOP -> movss [rbx+000004DC],xmm0
    }
    else {
        // turn off
        hook.NOPBytes((DWORD_PTR)(moduleBase + 0xF2F936), 8); // (x) movss [rbx+000004D4],xmm2 -> NOP
        hook.NOPBytes((DWORD_PTR)(moduleBase + 0xF2F94E), 8); // (y) movss [rbx+000004D8],xmm2 -> NOP
        hook.NOPBytes((DWORD_PTR)(moduleBase + 0xF2F966), 8); // (z) movss [rbx+000004DC],xmm0 -> NOP
    }
}

void SetMobCollision(int status) // 1/0
{
    if (status) 
    {
        // turn on
        hook.WriteBytes(moduleBase + 0xa51b52,
            // F3 0F 11 83 D4 04 00 00
            { 0xF3, 0x0F, 0x11, 0x83, 0xD4, 0x04, 0x00, 0x00 }); // x velocity 
        hook.WriteBytes(moduleBase + 0xa51b7c,
            // F3 0F 11 83 DC 04 00 00
            { 0xF3, 0x0F, 0x11, 0x83, 0xDC, 0x04, 0x00, 0x00 }); // Z velocity 
    }
    else
    {
        // turn off
        hook.NOPBytes(moduleBase + 0xa51b52, 8); // x velocity (movss [rbx+000004D4],xmm0 -> NOP)
        hook.NOPBytes(moduleBase + 0xa51b7c, 8); // z velocity
    }
}

void printVector(std::vector<byte> vector) {
    std::cout << "vector contents: ";
    for (UINT i = 0; i < vector.size(); i++)
    {
        std::cout << std::hex << (int)vector[i] << " ";
    }
    std::cout << std::endl;
}

std::vector<byte> uintToBytes(UINT val)
{
    std::vector<byte> bytes = { 0, 0, 0, 0 };

    unsigned char const* plist = reinterpret_cast<unsigned char const*>(&val);
    for (UINT i = 0; i < 4; i++)
    {
        bytes[i] = plist[i];
    }
    return bytes;
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        // attach to bedrock_server.exe
        log << "Initializing..." << '\n';
        if (!config.load()) {
            log << "Couldn't load config! Check your file.\n";
            return FALSE;
        };
        string formatVersion = "1.0.0";
        bool isNoKnockback = config.getBool("no_knockback");
//      bool isAdvancedKnockback = config.getBool("advanced_knockback");
        DWORD pID = GetCurrentProcessId();
        moduleBase = GetModuleBase(pID, (WCHAR*)_T("bedrock_server.exe"));
        codeAddy = moduleBase + 0xF2F926;
        if (isNoKnockback) 
        {
            // 1.16.200
            log << "Turning off knockback...\n";
            try {
                SetKnockbackStatus(FALSE); // turn off
            }
            catch (std::exception e)
            {
                log << "Error patching bytes! e: " << e.what() << '\n';
            }
        }
        else 
        {
            string newKnockback = config.getString("new_knockback");
            SetKnockbackModeStr(newKnockback);
            log << "Set knockback to: " << newKnockback << '\n';
        }
        if (formatVersion.compare("1.0.0") != 0)
        {
            bool isCollisionDisabled = config.getBool("mob_collision_off");
            if (isCollisionDisabled) {
                log << "Disabling mob collision\n";
                SetMobCollision(TRUE);
            }
        }
    }
    return TRUE;
}