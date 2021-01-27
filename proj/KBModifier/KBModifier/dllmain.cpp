//
//
//    KBModifier for BDS
//        By Imrglop
//
//

#include "dllmain.h"

Config config;
Hook hook;
HANDLE hConsole;

namespace address
{
    //DWORD_PTR sprint;
    DWORD_PTR code;
    DWORD_PTR yKnockback;
    DWORD_PTR normal;
    DWORD_PTR moduleBase;
}

namespace kb // offsets
{ 
    std::vector<string> kbKeys = { "decreased", "default", "extended", "double", "quad", "far", "extreme", "default-normal" };
    std::vector<std::vector<byte>> kbValues = {
        { /*0xF3, 0x0F, 0x10, 0x0D, */0xC6, 0x53, 0xCA, 0x00 }, // dec
        { 0x72, 0x54, 0xCA, 0x00 }, // def
        { 0xF2, 0x54, 0xCA, 0x00 }, // extended
        { 0x6A, 0x55, 0xCA, 0x00 }, // double
        { 0x4A, 0x56, 0xCA, 0x00 }, // quad
        { 0x12, 0x59, 0xCA, 0x00 }, // far
        { 0x72, 0x59, 0xCA, 0x00 }, // extreme
        { /*0xF3, 0x0F, 0x59, 0x25*/ 0xCD, 0x54, 0xCA, 0x00 } // default-normal
    }; // edited asm codes
    std::vector<byte> get(std::string key) {
        return kbValues[config.findInList(kbKeys, key)];
    }
}// decreased: 1bd4cf4        default: 1bd4da0      extended: 1bd4e20   double: 1bd4e98
// quad: 1bd4f78   far: 1bd5240   extreme: 1bd52a0

DWORD_PTR GetModuleBase(DWORD pID, wchar_t* moduleName) 
{
    DWORD_PTR moduleBase = 0;
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
                    moduleBase = (DWORD_PTR)moduleEntry32.modBaseAddr;
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
    return moduleBase;
}

void SetSprintKnockback(int mode)
{
    auto vec = kb::kbValues[mode];
    hook.WriteBytes(address::code,
        { 0xF3, 0x0F, 0x10, 0x0D, vec[0], vec[1], vec[2], vec[3] });
}

void SetSprintKnockbackStr(string mode)
{
    SetConsoleTextAttribute(hConsole, 0xE); // yellow
    log << "Warning: Set Sprint Knockback String is outdated/depreacted. Use SetSprintKnockbackFloat instead.\n";
    SetConsoleTextAttribute(hConsole, 0xF);
    auto vec = kb::get(mode);
    hook.WriteBytes(address::code,
        { 0xF3, 0x0F, 0x10, 0x0D, vec[0], vec[1], vec[2], vec[3] });
}

void SetSprintKnockbackFloat(float kb) {
    float* overrideFloat = (float*)(address::moduleBase + 0x1bd579b);
    DWORD oldProtect;
    VirtualProtect((void*)overrideFloat, sizeof(float), PAGE_EXECUTE_READWRITE, &oldProtect);
    *overrideFloat = kb;
    VirtualProtect((void*)overrideFloat, sizeof(float), oldProtect, &oldProtect);
    hook.WriteBytes(address::code,
        { 0xF3, 0x0F, 0x10, 0x0D, 0x6D, 0x5E, 0xCA, 0x00 }); //movss xmm1,[bedrock_server.exe+0x1bd579b]
}

void SetKnockback(int mode)
{
    auto vec = kb::kbValues[mode];
    hook.WriteBytes(address::normal,
        { 0xF3, 0x0F, 0x59, 0x25, vec[0], vec[1], vec[2], vec[3] });
}

void SetYKnockback(float yKnockback)
{
    hook.WriteBytes(address::yKnockback,
        {});
}


// old
void SetKnockbackStr(string mode)
{
    SetConsoleTextAttribute(hConsole, 0xE); // yellow
    log << "Warning: Set Knockback String is outdated/depreacted. Use SetKnockbackFloat instead.\n";
    SetConsoleTextAttribute(hConsole, 0xF);
    auto vec = kb::get(mode);
    hook.WriteBytes(address::normal,
        { 0xF3, 0x0f, 0x59, 0x25, vec[0], vec[1], vec[2], vec[3] });
}

void SetKnockbackFloat(float kb)
{
    float* overrideFloat = (float*)(address::moduleBase + 0x1bd576b);
    DWORD oldProtect;
    VirtualProtect((void*)overrideFloat, sizeof(float), PAGE_EXECUTE_READWRITE, &oldProtect);
    *overrideFloat = kb;
    VirtualProtect((void*)overrideFloat, sizeof(float), oldProtect, &oldProtect);
    hook.WriteBytes(address::normal,
        { 0xF3, 0x0F, 0x59, 0x25, 0xCC, 0x5E, 0xCA, 0x00 });
}

void ReloadConfig()
{
    config.load();
}

std::vector<byte> intToBytes(int val)
{
    std::vector<byte> bytes = { 0, 0, 0, 0 };

    unsigned char const* plist = reinterpret_cast<unsigned char const*>(&val);
    for (UINT i = 0; i < 4; i++)
    {
        bytes[i] = plist[i];
    }
    return bytes;
}

void SetHurtTime(int ticks)
{
    std::vector<byte> bytes = intToBytes(ticks);
    // -- -- -- -- -- -- (the value) 
    // C7 87 98 02 00 00 0A 00 00 00 = mov [rdi+00000298],0000000A
    // F2EEF0
    hook.WriteBytes((DWORD_PTR)(address::moduleBase + 0xF2EEF0),
        { 0xC7, 0x87, 0x98, 0x02, 0x00, 0x00, bytes[0], bytes[1], bytes[2], bytes[3] });
}

void SetKnockbackStatus(int status) // 1: knockback ON, 0: knockback OFF
{
    if (status == 1) {
        // turn on
        hook.WriteBytes((DWORD_PTR)(address::moduleBase + 0xF2F936), { 0xF3, 0x0F, 0x11, 0x93, 0xD4, 0x04, 0x00, 0x00 }); // (x) NOP -> movss [rbx+000004D4],xmm2
        hook.WriteBytes((DWORD_PTR)(address::yKnockback), { 0xF3, 0x0F, 0x11, 0x93, 0xD8, 0x04, 0x00, 0x00 }); // (x) NOP -> movss [rbx+000004D8],xmm2
        hook.WriteBytes((DWORD_PTR)(address::moduleBase + 0xF2F966), { 0xF3, 0x0F, 0x11, 0x83, 0xDC, 0x04, 0x00, 0x00 }); // (x) NOP -> movss [rbx+000004DC],xmm0
    }
    else {
        // turn off
        hook.NOPBytes((DWORD_PTR)(address::moduleBase + 0xF2F936), 8); // (x) movss [rbx+000004D4],xmm2 -> NOP
        hook.NOPBytes((DWORD_PTR)(address::yKnockback), 8); // (y) movss [rbx+000004D8],xmm2 -> NOP
        hook.NOPBytes((DWORD_PTR)(address::moduleBase + 0xF2F966), 8); // (z) movss [rbx+000004DC],xmm0 -> NOP
    }
}

void SetMobCollision(int status) // 1/0
{
    if (status) 
    {
        // turn on
        hook.WriteBytes(address::moduleBase + 0xa51b52,
            // F3 0F 11 83 D4 04 00 00
            { 0xF3, 0x0F, 0x11, 0x83, 0xD4, 0x04, 0x00, 0x00 }); // x velocity 
        hook.WriteBytes(address::moduleBase + 0xa51b7c,
            // F3 0F 11 83 DC 04 00 00
            { 0xF3, 0x0F, 0x11, 0x83, 0xDC, 0x04, 0x00, 0x00 }); // Z velocity 
    }
    else
    {
        // turn off
        hook.NOPBytes(address::moduleBase + 0xa51b52, 8); // x velocity (movss [rbx+000004D4],xmm0 -> NOP)
        hook.NOPBytes(address::moduleBase + 0xa51b7c, 8); // z velocity
    }
}

bool CompareVersions(string ver1, string ver2)
{
    try {
        std::vector<string> sl1;
        auto is = std::istringstream(ver1);
        for (std::string each; std::getline(is, each, '.');) {
            sl1.push_back(each);
        };

        std::vector<string> sl2;
        auto is2 = std::istringstream(ver2);
        for (std::string each; std::getline(is2, each, '.');) {
            sl2.push_back(each);
        };
        return (sl1[0] >= sl2[0]
            && sl1[1] >= sl2[1]
            && sl1[2] >= sl2[2]);
    }
    catch (std::exception e)
    {
        log << "Couldn't compare versions! error: " << e.what();
        return 0;
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
        try {
            string formatVersion = "1.2.0";
            string currentFormatVersion = config.getString("format_version");
            hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            bool isNoKnockback = config.getBool("no_knockback");
            bool isYKnockback = config.getBool("edit_y_knockback");
            int newHurtTime = config.getInteger("hurt_time");
            //      bool isAdvancedKnockback = config.getBool("advanced_knockback");

            DWORD pID = GetCurrentProcessId();
            address::moduleBase = GetModuleBase(pID, (WCHAR*)_T("bedrock_server.exe"));
            VirtualAlloc((void*)(0x7FF6F51E0000), 0x100, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
            address::code = address::moduleBase + 0xF2F926;
            address::yKnockback = address::moduleBase + 0xf2f94e;
            address::normal = address::moduleBase + 0xf2f897;
            string newSprintKnockback = "";
            if (CompareVersions(currentFormatVersion, "1.1.0")/*true*/)
            {
                if (CompareVersions(currentFormatVersion, "1.2.0")/*true*/)
                {
                    if (isYKnockback)
                    {
                        float newYKnockback = config.getNumber("y_knockback");
                        log << "Y Knockback: " << newYKnockback << '\n';
                        // TODO make customizable
                        /*hook.WriteBytes(0x7FF6F51E0000, //     1   2   3   4
                            { 0xC7, 0x83, 0xD8, 0x04, 00, 00, 00, 00, 00, 0x3F, 0xE9, 0x47, 0xF9, 0xF3, 0x00 });*/
                        /*
                        * C7 83 D8 04 00 00 00 00 00 3F E9 47 F9 F3 00
                        * disassembled:
                        * mov [rbx+000004D8],3F000000
                        * jmp bedrock_server.exe+F2F956
                        */

                        /*hook.WriteBytes(address::yKnockback,
                            { 0xE9, 0xAD, 0x06, 0x0C, 0xFF, 0x0F, 0x1F, 00 });*/
                        /*
                        * old:
                        * F3 0F 11 93 D8 04 00 00
                        * new:
                        * E9 AD 06 0C FF 0F 1F 00
                        * disassembled:
                        * jmp 7FF6F611F94E
                        * nop dword ptr [rax]
                        */
                    }
                }
                newSprintKnockback = config.getString("sprint_knockback");
                string newKnockback = config.getString("knockback");
                if (!CompareVersions(currentFormatVersion, "1.2.0"))
                    SetKnockbackStr(newKnockback); 
                else SetKnockbackFloat(config.getNumber("knockback"));

                log << "Knockback: " << newKnockback << '\n';
                SetHurtTime(newHurtTime);
                log << "Hurt Time: " << newHurtTime << '\n';
                bool isCollisionDisabled = config.getBool("mob_collision_off");
                if (isCollisionDisabled) {
                    log << "Disabling mob collision\n";
                    SetMobCollision(FALSE);
                }
            }
            else if (!CompareVersions(currentFormatVersion, formatVersion))
            {
                if (!CompareVersions("1.1.0", formatVersion))
                {
                    newSprintKnockback = config.getString("new_knockback");
                }
                SetConsoleTextAttribute(hConsole, 0xE); // yellow
                log << "Outdated format version " << currentFormatVersion << ", new version: " << formatVersion << '\n';
                log << "If you want to update, go to github.com/Imrglop/bds-kb-modifier\n";
                SetConsoleTextAttribute(hConsole, 0x7); // light gray idk
            }
            if (isNoKnockback)
            {
                // 1.16.200
                log << "Turning off knockback...\n";
                SetKnockbackFloat(0.0);
            }
            else
            {
                if (!CompareVersions(currentFormatVersion, "1.2.0"))
                    SetSprintKnockbackStr(newSprintKnockback);
                else {
                    SetSprintKnockbackFloat(config.getNumber("sprint_knockback"));
                    //log << "using 1.2.0 kb method\n";
                }
                log << "Sprint-knockback: " << newSprintKnockback << '\n';
            }
        }
        catch (std::exception e) {
            log << "Couldn't load KB Mod! Error: " << e.what() << '\n';
        }
    }
    return TRUE;
}