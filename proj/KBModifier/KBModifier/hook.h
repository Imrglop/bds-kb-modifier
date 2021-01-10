#pragma once
#ifndef HOOK_H
#define HOOK_H
#include <Windows.h>
#include <vector>
class Hook {
public:
	void NOPBytes(DWORD_PTR staticAddy, UINT amount);
	void WriteBytes(DWORD_PTR staticAddy, std::vector<unsigned char> bytes);
};
#endif