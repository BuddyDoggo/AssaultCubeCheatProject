#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <math.h>
#include <vector>

DWORD GetProcId(const wchar_t* procName);

uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName);

float SquaredDistanceBetweenEntities(HANDLE hProc, uintptr_t playerBase, uintptr_t botBase);

std::vector<float> CalculateAngles(HANDLE hProc, uintptr_t playerBase, uintptr_t botBase);