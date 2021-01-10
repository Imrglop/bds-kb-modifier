#include "hook.h"
#include <iostream>

void Hook::NOPBytes(DWORD_PTR staticAddy, UINT amount)
{
	try {
		DWORD oldVP;
//		std::cout << "thething= " << std::hex << staticAddy << std::endl;
		VirtualProtect((byte*)staticAddy, (DWORD_PTR)amount, PAGE_EXECUTE_READWRITE, &oldVP);
		for (UINT i = 0; i < amount; i++) {
			byte* writeTo = (byte*)(staticAddy + i);
			*writeTo = (byte)0x90;
		}
		VirtualProtect((byte*)staticAddy, (DWORD_PTR)amount, oldVP, &oldVP);
	}
	catch (std::exception e)
	{
		std::cout << "Error: " << e.what() << '\n';
	}
	
}

void Hook::WriteBytes(DWORD_PTR staticAddy, std::vector<unsigned char> bytes)
{
	DWORD oldVP;
	VirtualProtect((byte*)staticAddy, (DWORD_PTR)bytes.size(), PAGE_EXECUTE_READWRITE, &oldVP);
	for (UINT i = 0; i < bytes.size(); i++) {
		byte* writeTo = (byte*)(staticAddy + i); // the byte pointer to write to..
		//std::cout << "@@ " << std::hex << (int*)writeTo << '\n';
		//std::cout << "Before: " << std::hex << (int)*writeTo << '\n';
		*writeTo = bytes[i];
		//std::cout << "after: " << std::hex << (int)*writeTo << std::endl;
	}
	VirtualProtect((byte*)staticAddy, (DWORD_PTR)bytes.size(), oldVP, &oldVP);
}