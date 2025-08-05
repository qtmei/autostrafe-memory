#include <Windows.h>
#include <thread>
#include <iostream>

using namespace std;

void Autostrafe(HINSTANCE hinstDll)
{
	AllocConsole();
	SetConsoleTitleA("Meiware Autostrafe");
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);

	cout << "© 2021 Meiware.net\nhold mouse5 to autostrafe" << endl;

	DWORD client = (DWORD)GetModuleHandle("client.dll");
	DWORD engine = (DWORD)GetModuleHandle("engine.dll");
	DWORD dwForceLeft = 0x799708; //32-bit: 0x799708, 64-bit: 0xA33908
	DWORD dwForceRight = 0x799714; //32-bit: 0x799714, 64-bit: 0xA33918
	DWORD dwm_angAbsRotation = 0x4FC574; //32-bit: 0x4FC574, 64-bit: 0x6571FC
	float oldYaw = 0.0f;
	bool lastDir = false, runOnce = true; //lastDir false=left, true=right

	while (!GetAsyncKeyState(VK_END))
	{
		if (GetAsyncKeyState(VK_XBUTTON2))
		{
			float yaw = *(float*)(engine + dwm_angAbsRotation + 0x4); //base is pitch, + 0x4 is yaw because a float is 4 bytes

			if (yaw > oldYaw || yaw == oldYaw && !lastDir) //we want to ensure A or D is still being pressed when the mouse is not being moved
			{
				*(int*)(client + dwForceRight) = 0;
				*(int*)(client + dwForceLeft) = 1;

				lastDir = false;
			}
			else if (yaw < oldYaw || yaw == oldYaw && lastDir)
			{
				*(int*)(client + dwForceLeft) = 0;
				*(int*)(client + dwForceRight) = 1;

				lastDir = true;
			}

			oldYaw = yaw;
			runOnce = true;
		}
		else
			if (runOnce) //we release the keys when mouse5 is not held, but only once
			{
				*(int*)(client + dwForceLeft) = 0;
				*(int*)(client + dwForceRight) = 0;

				runOnce = false;
			}

		Sleep(1);
	}

	FreeLibraryAndExitThread(hinstDll, EXIT_SUCCESS);
}

BOOL WINAPI DllMain(HINSTANCE hinstDll, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hinstDll);
		const HANDLE thread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)Autostrafe, hinstDll, 0, nullptr);

		if (thread)
			CloseHandle(thread);
	}

	return TRUE;
}
