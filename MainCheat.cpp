#include "MainCheat.h"

int main() {
	DWORD processID = GetProcId(L"ac_client.exe");
	while (processID == 0) {
		printf("Assaultcube Process not found. Retrying in 5s\n");
		Sleep(5000);
		processID = GetProcId(L"ac_client.exe");
	}
	printf("Process ID: %d\n", processID);

	uintptr_t moduleBaseAddress = GetModuleBaseAddress(processID, L"ac_client.exe");
	if (moduleBaseAddress == 0) {
		printf("Could not locate ModBaseAddr\n");
		return 1;
	}
	printf("Module Base Address: %p\n", (int*)moduleBaseAddress);

	HANDLE hProcess = 0;
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, processID);

	uintptr_t playerBaseAddress = 0;
	ReadProcessMemory(hProcess, (BYTE*)(moduleBaseAddress + 0x18AC14), &playerBaseAddress, sizeof(playerBaseAddress), nullptr);
	printf("Player Base Address: %p\n",(int*)playerBaseAddress);

	int playerCount = 0;
	uintptr_t playerListPosition = 0;
	std::vector<BYTE*> playerList;
	//std::vector<const char*> playerNames;

	//uintptr_t xCoordAddr = playerBaseAddress + 0x04;
	//uintptr_t yCoordAddr = playerBaseAddress + 0x08;
	//uintptr_t zCoordAddr = playerBaseAddress + 0x0C;
	uintptr_t rotHorizontalAddr = playerBaseAddress + 0x34;
	uintptr_t rotVerticalAddr = playerBaseAddress + 0x38;

	uintptr_t healthAddr = playerBaseAddress + 0xEC;
	uintptr_t armorAddr = playerBaseAddress + 0xF0;

	BYTE* clipAddr[] = {
		(BYTE*)0x012C,	// Pistol Clip Ammo
		(BYTE*)0x0130,	// Carbine Clip Ammo
		(BYTE*)0x0134,	// Shotgun Clip Ammo
		(BYTE*)0x0138,	// MP Clip Ammo
		(BYTE*)0x013C,	// Sniper Clip Ammo
		(BYTE*)0x0140,	// AR Clip Ammo
		(BYTE*)0x0144,	// Grenade Ammo
		(BYTE*)0x0148,	// Akimbo Clip Ammo
	};

	BYTE* reserveAddr[] = {
		(BYTE*)0x0108,	// Pistol Reserve Ammo
		(BYTE*)0x010C,	// Carbine Reserve Ammo
		(BYTE*)0x0110,	// Shotgun Reserve Ammo
		(BYTE*)0x0114,	// MP Reserve Ammo
		(BYTE*)0x0118,	// Sniper Reserve Ammo
		(BYTE*)0x011C,	// AR Reserve Ammo
		(BYTE*)0x0124,	// Akimbo Reserve Ammo
	};

	uintptr_t pistolReserveAddr = playerBaseAddress + 0x108;
	uintptr_t mpReserveAddr = playerBaseAddress + 0x11C;
	uintptr_t pistolClipAddr = playerBaseAddress + 0x12C;
	uintptr_t mpClipAddr = playerBaseAddress + 0x140;
	uintptr_t grenadeAddr = playerBaseAddress + 0x144;

	uintptr_t recoilAddr = moduleBaseAddress + 0xC3511;
	char recoilReplace[] = { 0x90, 0x90, 0x90, 0x90, 0x90 };
	char recoilOriginal[] = { 0xF3, 0x0F, 0x11, 0x4E, 0x38 };

	int desiredHealth = 200;
	int desiredArmor = 100;
	int desiredClipAmmo = 20;

	int desiredBotHP = 1;
	int desiredBotArmor = 0;
	int desiredBotAmmo = 0;

	bool lastInfiniteAmmoCheatState = false;
	bool lastBotHealthCheatState = false;

	BYTE* lookingAtPlayerPointer = 0;
	char lookingAtPlayerName[16] = "";
	bool lookAtPlayer = false;
	uintptr_t codeLocation = moduleBaseAddress + 0x0001B15D;
	uintptr_t myCodeLocation = moduleBaseAddress + 0x00124D5E;
	uintptr_t placeDown = moduleBaseAddress + 0x0016D2FE;

	char jumpToMyCode[] = { 0xE9, 0xFC, 0x9B, 0x10, 0x00 };
	char myCode[] = { 0x50, 0x8B, 0x44, 0x24, 0x08, 0xA3, 0xFE, 0xD2, 0x56, 0x00, 0x58, 0x0F, 0xBE, 0x0F, 0x85, 0xC9, 0xE9, 0xEF, 0x63, 0xEF, 0xFF };
	
	WriteProcessMemory(hProcess, (BYTE*)codeLocation, &jumpToMyCode, sizeof(jumpToMyCode), nullptr);
	WriteProcessMemory(hProcess, (BYTE*)myCodeLocation, &myCode, sizeof(myCode), nullptr);

	INPUT inputDown[1] = {};
	inputDown[0].type = INPUT_MOUSE;
	inputDown[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

	INPUT inputUp[1] = {};
	inputUp[0].type = INPUT_MOUSE;
	inputUp[0].mi.dwFlags = MOUSEEVENTF_LEFTUP;

	Sleep(500);
	system("cls");

	while (true) {

		ReadProcessMemory(hProcess, (BYTE*)placeDown, &lookingAtPlayerPointer, sizeof(lookingAtPlayerPointer), nullptr);
		lookingAtPlayerName[0] = 0;
		lookAtPlayer = false;
		for (BYTE* b : playerList) {
			if (b == lookingAtPlayerPointer) {
				ReadProcessMemory(hProcess, b + 0x205, &lookingAtPlayerName, sizeof(lookingAtPlayerName), nullptr);
				lookAtPlayer = true;
				break;
			}
		}

		ClearScreen();
		printf("__Welcome to the Assault Cube Cheat__\n"
			   "F2 : Godmode        %s\n"
			   "F3 : Easy Bots      %s\n"
			   "F4 : Triggerbot     %s\n"
			   "F7 : Update Player List. Currently %d Players\n"
			   "F8 : Exit Cheat\n"
			   "MOUSE 4 : Aimbot\n"
			   "Aiming at: %-16s\n",
				(GetKeyState(VK_F2) ? "[ON] " : "[OFF]"),
				(GetKeyState(VK_F3) ? "[ON] " : "[OFF]"),
				(GetKeyState(VK_F4) ? "[ON] " : "[OFF]"),
				playerCount,
				lookingAtPlayerName);
		
		Sleep(10);

		if (GetAsyncKeyState(VK_F8)) {
			break;
		}

		if (playerCount != ReadProcessMemory(hProcess, (BYTE*)(moduleBaseAddress + 0x18AC20), &playerCount, sizeof(playerCount), nullptr) || GetAsyncKeyState(VK_F7)) {
			ReadProcessMemory(hProcess, (BYTE*)(moduleBaseAddress + 0x18AC20), &playerCount, sizeof(playerCount), nullptr);
			ReadProcessMemory(hProcess, (BYTE*)(moduleBaseAddress + 0x191FE4), &playerListPosition, sizeof(playerListPosition), nullptr);

			playerList.clear();
			uintptr_t tmp = 0;
			for (int i = 0; i < playerCount - 1; i++) {
				ReadProcessMemory(hProcess, (BYTE*)(playerListPosition + (sizeof(int) * i)), &tmp, sizeof(tmp), nullptr);
				playerList.push_back((BYTE*)tmp);
			}
		}

		// Get Good stats
		if (GetKeyState(VK_F2)) {
			if (lastInfiniteAmmoCheatState == false) {
				lastInfiniteAmmoCheatState = true;
				WriteProcessMemory(hProcess, (BYTE*)recoilAddr, &recoilReplace, sizeof(recoilReplace), nullptr);
			}
			WriteProcessMemory(hProcess, (BYTE*)healthAddr, &desiredHealth, sizeof(desiredHealth), nullptr);
			WriteProcessMemory(hProcess, (BYTE*)armorAddr, &desiredArmor, sizeof(desiredArmor),nullptr);
			for (BYTE* p : clipAddr) {
				WriteProcessMemory(hProcess, playerBaseAddress + p, &desiredClipAmmo, sizeof(desiredClipAmmo), nullptr);
			}
		}
		else {
			if (lastInfiniteAmmoCheatState == true) {
				lastInfiniteAmmoCheatState = false;
				WriteProcessMemory(hProcess, (BYTE*)healthAddr, &desiredArmor, sizeof(desiredArmor), nullptr);
				WriteProcessMemory(hProcess, (BYTE*)armorAddr, &desiredArmor, sizeof(desiredArmor), nullptr);
				WriteProcessMemory(hProcess, (BYTE*)recoilAddr, &recoilOriginal, sizeof(recoilOriginal), nullptr);
			}
		}


		// Bots only low HP, no armor and no Ammo
		if (GetKeyState(VK_F3)) {
			int tempHealthValue = 0;
			for (int i = 0; i < playerCount - 1; i++) {
				ReadProcessMemory(hProcess, (playerList.at(i) + 0xEC), &tempHealthValue, sizeof(tempHealthValue), nullptr);
				if (tempHealthValue > desiredBotHP) {
					WriteProcessMemory(hProcess, (playerList.at(i) + 0xEC), &desiredBotHP, sizeof(desiredBotHP), nullptr);
				}
				WriteProcessMemory(hProcess, (playerList.at(i) + 0xF0), &desiredBotArmor, sizeof(desiredBotArmor), nullptr);
				for (BYTE* p : clipAddr) {
					WriteProcessMemory(hProcess, (BYTE*)((int)playerList.at(i) + (int)p), &desiredClipAmmo, sizeof(desiredClipAmmo), nullptr);
				}
			}
		}

		// Aimbot
		if (GetAsyncKeyState(VK_XBUTTON1)) {
			float shortestDist = FLT_MAX;
			//char tempName[20] = "";
			float tempDist = 0;
			int tempBotHealth = -1;
			int closestBotIndex = 0;
			for (int i = 0; i < playerList.size(); i++) {
				tempDist = SquaredDistanceBetweenEntities(hProcess, playerBaseAddress, (uintptr_t)playerList.at(i));
				ReadProcessMemory(hProcess, (playerList.at(i) + 0xEC), &tempBotHealth, sizeof(tempBotHealth), nullptr);
				if (tempDist < shortestDist && tempBotHealth > 0) {
					shortestDist = tempDist;
					closestBotIndex = i;
					//ReadProcessMemory(hProcess, (playerList.at(i) + 0x205), &tempName, sizeof(tempName), nullptr);
				}
			}
			if (shortestDist != FLT_MAX) {
				std::vector<float> angles = CalculateAngles(hProcess, playerBaseAddress, (uintptr_t)playerList.at(closestBotIndex));
				if (angles.at(1) != angles.at(1)) {
					angles.at(1) = 0.0;
				}
				WriteProcessMemory(hProcess, (BYTE*)rotHorizontalAddr, &angles.at(0), sizeof(angles.at(0)), nullptr);
				WriteProcessMemory(hProcess, (BYTE*)rotVerticalAddr, &angles.at(1), sizeof(angles.at(1)), nullptr);
				//printf("Closest Enemy is % s at % f // Horizontal angle: %f  // Vertical Angle: %f\n", tempName, sqrt(shortestDist), angles.at(0), angles.at(1));
			}
			else {
				//printf("No enemy nearby\n");
			}
		}

		//Triggerbot
		if (GetKeyState(VK_F4) || GetKeyState(VK_F5)) {
			if (lookAtPlayer) {
				SendInput(1, inputDown, sizeof(INPUT));
			}
			else {
				SendInput(1, inputUp, sizeof(INPUT));
			}
		}
	}
	printf("Cheat closed\n");

}

void ClearScreen()
{
	COORD cursorPosition;
	cursorPosition.X = 0;
	cursorPosition.Y = 0;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPosition);
}