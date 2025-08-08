#include <Windows.h>
#include <thread>

void Autostrafe(HINSTANCE hinstDll)
{
	srand(time(0));

	unsigned long long client = (unsigned long long)GetModuleHandle("client.dll");
	unsigned long long engine = (unsigned long long)GetModuleHandle("engine.dll");
	unsigned long long forceLeft = 0xA34908;
	unsigned long long forceRight = 0xA34918;
	unsigned long long m_angAbsRotation = 0x6571FC;
	float oldYaw = 0.0f;
	bool lastDir = false, runOnce = true; //lastDir false = left, true = right

	while (!GetAsyncKeyState(VK_END))
	{
		if (GetAsyncKeyState(VK_XBUTTON2))
		{
			float yaw = *(float*)(engine + m_angAbsRotation + 0x4); //base is pitch, + 0x4 is yaw because a float is 4 bytes
			int delay = rand() % 8 + 4;

			if (yaw > oldYaw || yaw == oldYaw && !lastDir) //we want to ensure A or D is still being pressed when the mouse is not being moved
			{
				Sleep(delay); //random delay to appear human

				*(int*)(client + forceRight) = 0;
				*(int*)(client + forceLeft) = 1;
				lastDir = false;
			}
			else if (yaw < oldYaw || yaw == oldYaw && lastDir)
			{
				Sleep(delay);

				*(int*)(client + forceLeft) = 0;
				*(int*)(client + forceRight) = 1;
				lastDir = true;
			}

			oldYaw = yaw;
			runOnce = true;
		}
		else if (runOnce) //we release the keys when mouse5 is not held, but only once
		{
			*(int*)(client + forceLeft) = 0;
			*(int*)(client + forceRight) = 0;
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
