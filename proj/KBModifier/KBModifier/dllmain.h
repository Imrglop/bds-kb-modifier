#pragma once
#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>
#include <tchar.h>
#include "Config.h"
#include "hook.h"

// exportable to other programs/scripts

#ifndef KBMOD_EXPORTS
#define KBMOD_API __declspec(dllexport)
#else
#define KBMOD_API __declspec(dllimport)
#endif

#define log std::cout<<"[KBMod] "

extern "C" KBMOD_API void SetKnockbackStatus(int status); // sets whether u take knockback or not
extern "C" KBMOD_API void SetKnockbackMode(int mode); // sets how much knockback u take & w-tapping
extern "C" KBMOD_API void SetKnockbackModeStr(string mode); // pretty much setknockbackmode but its "default","decreased","far",etc
//entity collision control coming soon