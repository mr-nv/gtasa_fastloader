#include <windows.h>
#include <stdio.h>

BOOL APIENTRY DllMain(HMODULE, DWORD, LPVOID);

void ShowRaster_Prox();
void StartGame_Prox();
void ChangeMenu_Prox();

#define patch(a, v, s) _patch((void*)(a), (void*)(v), (s))
void _patch(void*, void*, int);
#define nop(a, s) _nop((void*)(a), (s))
void _nop(void*, int);
bool check(void*, unsigned char, const char*, bool);
bool _check(void*, unsigned char);

long pRaseter_Func;
bool bIsDraw = false;
long pStart_Func;
int iAlreadyPatched = 2;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			unsigned long dwValue;

			//if (check((void*)0x747483, 0x89, "Initialize game state", true)) nop(0x747483, 6);
			//else if (check((void*)0x7474D3, 0x89, "Initialize game state", false)) nop(0x7474D3, 6);

			dwValue = 5;
			patch(0xC8D4C0, &dwValue, 4); // Skip ads

			if (check((void*)0x748C2B, 0xE8, "Legal info fade-in", true)) nop(0x748C2B, 5); // Legal info fade-in
			else if (check((void*)0x748C7B, 0xE8, "Legal info fade-in", false)) nop(0x748C7B, 5); // Legal info fade-in
			dwValue = 1;
			if (check((void*)0x5909AA, 0xBE, "Legal info", false)) patch(0x5909AB, &dwValue, 4); // Legal info
			if (check((void*)0x590A1D, 0xBE, "Legal info fade-out", false))
			{
				dwValue = 0xE9;
				patch(0x590A1D, &dwValue, 1); // Legal info fade-out
				dwValue = 0x0000008D;
				patch(0x590A1E, &dwValue, 4); // Legal info fade-out
			}

			if (check((void*)0x748C6B, 0xC6, "Show load game", true)) nop(0x748C6B, 7); // Show load game
			else if (check((void*)0x748CBB, 0xC6, "Show load game", false)) nop(0x748CBB, 7); // Show load game
			dwValue = 0x09;
			if (check((void*)0x5745DD, 0xC6, "Show load game", false)) patch(0x5745E3, &dwValue, 1); // Show load game

			dwValue = 0x75;
			if (check((void*)0x5737E0, 0x74, "Skip confim", false)) patch(0x5737E0, &dwValue, 1); // Skip confim

			if (check((void*)0x590AF0, 0xA1, "Skip loading", false))
			{
				dwValue = 0xE9;
				patch(0x590AF0, &dwValue, 1); // Skip loading
				dwValue = 0x00000140;
				patch(0x590AF1, &dwValue, 4); // Skip loading
			}

/*			if (check((void*)0x748E52, 0x74, "Load world", false))
			{
				*(unsigned char*)0x748E52 = 0x75;
				*(unsigned char*)0x748E53 = 0x12;
			}*/

			if (check((void*)0x619440, 0xE9, "Show raster", false))
			{
				memcpy(&pRaseter_Func, (void*)0x619441, 4);
				pRaseter_Func += 0x619445;
				dwValue = (long)&ShowRaster_Prox - 0x619445;
				patch(0x619441, &dwValue, 4); // Skip splash screen
			}

			if (check((void*)0x748D1A, 0xE9, "Start Game", true))
			{
				memcpy(&pStart_Func, (void*)0x748D1B, 4);
				pStart_Func += 0x748D1F;

				dwValue = (long)&StartGame_Prox - 0x748D1F;
				patch(0x748D1B, &dwValue, 4); // Show menu
			} else
			if (check((void*)0x748D6A, 0xE9, "Start Game", false))
			{
				memcpy(&pStart_Func, (void*)0x748D6B, 4);
				pStart_Func += 0x748D6F;

				dwValue = (long)&StartGame_Prox - 0x748D6F;
				patch(0x748D6B, &dwValue, 4); // Show menu
			}

			if (check((void*)0x573683, 0x8A, "Change Menu", false))
			{
				dwValue = 0xE9;
				patch(0x573683, &dwValue, 1);
				dwValue = (long)&ChangeMenu_Prox - 0x573688;
				patch(0x573684, &dwValue, 4); // Enable confim
			}

			break;
		}
	case DLL_THREAD_ATTACH:
		{
			break;
		}
	case DLL_THREAD_DETACH:
		{
			break;
		}
	case DLL_PROCESS_DETACH:
		{
			break;
		}
	}

	return TRUE;
}

void __declspec(naked) ShowRaster_Prox()
{
	if (*(unsigned char*)0xC8D4C0 != 9 && bIsDraw)
	{
		__asm
		{
			ret;
		}
	}
	bIsDraw = true;
	__asm
	{
		mov eax, pRaseter_Func;
		push eax;
		ret;
	}
}

void __declspec(naked) StartGame_Prox()
{
	if (GetModuleHandle("samp"))
	{
		iAlreadyPatched = 0;
		if (*(unsigned char*)0x5745E3 == 0x09) *(unsigned char*)0x5745E3 = 0x2A;
		if (*(unsigned char*)0x5737E0 == 0x75) *(unsigned char*)0x5737E0 = 0x74;
	} else
	{
		*(char*)0xBA677B = 1;
		*(char*)0xBA6831 = 0;
		*(char*)0xBA68A5 = 9;
	}
	__asm
	{
		mov eax, pStart_Func;
		push eax;
		ret;
	}
}

void __declspec(naked) ChangeMenu_Prox()
{
	if (iAlreadyPatched)
	{
		iAlreadyPatched--;
		if (iAlreadyPatched == 1) if (*(unsigned char*)0x5745E3 == 0x09) *(unsigned char*)0x5745E3 = 0x2A;
		if (iAlreadyPatched == 0)
		{
			if (*(unsigned char*)0x5737E0 == 0x75) *(unsigned char*)0x5737E0 = 0x74;
			if (*(unsigned char*)0x748E02 == 0x74) *(unsigned char*)0x748E02 = 0x75;
			if (*(unsigned char*)0x748E52 == 0x74) *(unsigned char*)0x748E52 = 0x75;
		}
	}
	__asm
	{
		mov al, [esi+0x15D];
		push 0x573689;
		ret;
	}
}

void _patch(void* pAddress, void* pData, int iSize)
{
	unsigned long dwProtect[2];
	VirtualProtect(pAddress, iSize, PAGE_EXECUTE_READWRITE, &dwProtect[0]);
	memcpy(pAddress, pData, iSize);
//	VirtualProtect(pAddress, iSize, dwProtect[0], &dwProtect[1]);
}

void _nop(void* pAddress, int iSize)
{
	unsigned long dwProtect[2];
	VirtualProtect(pAddress, iSize, PAGE_EXECUTE_READWRITE, &dwProtect[0]);
	memset(pAddress, 0x90, iSize);
	VirtualProtect(pAddress, iSize, dwProtect[0], &dwProtect[1]);
}

bool check(void* pAddress, unsigned char cByte, const char* sExp, bool bContinue)
{
	if (_check(pAddress, cByte)) return true;
	if (bContinue) return false;
	char buf[1024];
	sprintf_s(buf, 1024, "An unknown code found at 0x%08X (%s).\nDo you want to continue without this Function?", pAddress, sExp);
	if (MessageBox(0, buf, "Fast Loader", MB_YESNO | MB_ICONERROR | MB_DEFBUTTON2) == IDNO) ExitProcess(0);
	return false;
}

bool _check(void* pAddress, unsigned char cByte)
{
	unsigned long dwProtect[2];
	unsigned char cValue = cByte;
	VirtualProtect(pAddress, 1, PAGE_EXECUTE_READ, &dwProtect[0]);
	memcpy(&cValue, pAddress, 1);
	VirtualProtect(pAddress, 1, dwProtect[0], &dwProtect[1]);
	return (cValue == cByte);
}
