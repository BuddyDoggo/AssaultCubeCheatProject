#include "HelperFunctions.h"

DWORD GetProcId(const wchar_t* procName) {
	DWORD procId = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if (hSnap != INVALID_HANDLE_VALUE) {
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(procEntry);

		if (Process32First(hSnap, &procEntry)) {
			
			do {
				
				if (!_wcsicmp(procEntry.szExeFile, procName)) {
					procId = procEntry.th32ProcessID;
					break;
				}
			} while (Process32Next(hSnap,&procEntry));
		}
	}
	CloseHandle(hSnap);
	return procId;
}

uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName) {
	uintptr_t modBaseAddr = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
	if (hSnap != INVALID_HANDLE_VALUE) {
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);
		
		if (Module32First(hSnap, &modEntry)) {
			
			do {
				if (!_wcsicmp(modEntry.szModule, modName)) {
					modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
					break;
				}
			} while (Module32Next(hSnap, &modEntry));
		}
	}
	CloseHandle(hSnap);
	return modBaseAddr;
}

float SquaredDistanceBetweenEntities(HANDLE hProc, uintptr_t playerBase, uintptr_t botBase) {
	float playerPos[2] = {};
	float botPos[2] = {};
	ReadProcessMemory(hProc, (BYTE*)playerBase + 0x4, &playerPos, sizeof(playerPos), nullptr);
	ReadProcessMemory(hProc, (BYTE*)botBase + 0x4, &botPos, sizeof(botPos), nullptr);
	return (pow((playerPos[0] - botPos[0]),2) + pow((playerPos[1] - botPos[1]), 2));
}

std::vector<float> CalculateAngles(HANDLE hProc, uintptr_t playerBase, uintptr_t botBase) {
	float playerPos[3] = {0};
	float botPos[3] = {0};
	ReadProcessMemory(hProc, (BYTE*)playerBase + 0x4, &playerPos, sizeof(playerPos), nullptr);
	ReadProcessMemory(hProc, (BYTE*)botBase + 0x4, &botPos, sizeof(botPos), nullptr);
	float flatVectorX = playerPos[0] - botPos[0];		// X Coords
	float flatVectorY = playerPos[1] - botPos[1];		// Y Coords
	float flatVectorZ = playerPos[2] - botPos[2];		// Z Coords
	float calcVectorX = 0;
	float calcVectorY = 1;
	float horizontalAngle = acos((flatVectorX * calcVectorX) + (flatVectorY * calcVectorY) / ( sqrt(flatVectorX * flatVectorX + flatVectorY * flatVectorY) * sqrt( 0 + 1 ) )) * (180/3.141592);
	if (playerPos[0] - botPos[0] > 0) {
		horizontalAngle = 360-horizontalAngle;
	}
	float distance = sqrt(flatVectorX * flatVectorX + flatVectorY * flatVectorY);
	float verticalAngle = acos(distance / sqrt(flatVectorX * flatVectorX + flatVectorY * flatVectorY + flatVectorZ * flatVectorZ)) * (180 / 3.141592);
	if (playerPos[2] - botPos[2] > 0) {
		verticalAngle = -verticalAngle;
	}
	return std::vector<float>{horizontalAngle,verticalAngle};
}