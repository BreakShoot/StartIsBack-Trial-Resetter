#include <stdio.h>
#include <windows.h>
#include <intrin.h>
#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

#define BYTEn(x, n)   (*((unsigned char*)&(x)+n))
#define WORDn(x, n)   (*((unsigned short*)&(x)+n))
#define BYTE1(x)   BYTEn(x,  1)
#define BYTE2(x)   BYTEn(x,  2)
#define BYTE3(x)   BYTEn(x,  3)
#define BYTE4(x)   BYTEn(x,  4)
#define BYTE5(x)   BYTEn(x,  5)
#define BYTE6(x)   BYTEn(x,  6)
#define WORD2(x)   WORDn(x,  2)

UINT __fastcall RetrieveUniqueHWID(LPSTR a1)
{
	UINT result; // eax
	DWORD v3; // ebx
	unsigned __int8* v4; // r14
	UINT v5; // eax
	__int64 v6; // rbx
	unsigned int v7; // edi
	UINT v8; // esi
	int v9; // edx
	int v10; // ebx
	int i; // eax
	__int64 v12; // rbx
	unsigned __int64 v13; // r8
	unsigned __int64 v14; // xmm0_8
	int v15; // [rsp+20h] [rbp-68h]
	int v16; // [rsp+28h] [rbp-60h]
	int v17; // [rsp+30h] [rbp-58h]
	int v18; // [rsp+38h] [rbp-50h]
	int v19; // [rsp+40h] [rbp-48h]
	int v20; // [rsp+48h] [rbp-40h]
	int v21; // [rsp+50h] [rbp-38h]
	int v22; // [rsp+58h] [rbp-30h]
	int v23; // [rsp+60h] [rbp-28h]

	result = GetSystemFirmwareTable(0x52534D42u, 0, 0i64, 0);
	v3 = result;
	if (result)
	{
		v4 = (unsigned __int8*)GlobalAlloc(0x40u, result + 8);
		v5 = GetSystemFirmwareTable(0x52534D42u, 0, v4, v3);
		v6 = 0i64;
		v7 = v5;
		v8 = 0;
		if (v5)
		{
			while (v4[v6 + 8] != 1)
			{
				v9 = v4[v6 + 9];
				if ((BYTE)v9)
				{
					v10 = v9 + v6;
					for (i = lstrlenA((LPCSTR)&v4[v10 + 8]); i; i = lstrlenA((LPCSTR)&v4[v10 + 8]))
						v10 += i + 1;
					v6 = (unsigned int)(v10 + 1);
					if ((unsigned int)v6 < v7)
						continue;
				}
				goto LABEL_11;
			}
			if ((unsigned int)v6)
			{
				v12 = (unsigned int)(v6 + 8);
				v13 = *(__int64*)&v4[v12 + 8];
				v14 = _mm_srli_si128(*(__m128i*)&v4[v12 + 8], 8).m128i_u64[0];
				v23 = BYTE4(v14);
				v22 = BYTE5(v14);
				v21 = BYTE6(v14);
				v20 = HIBYTE(v14);
				v19 = (unsigned __int8)v14;
				v18 = BYTE1(v14);
				v17 = BYTE2(v14);
				v16 = BYTE3(v14);
				v15 = WORD2(v13);
				wsprintfA(
					a1,
					"%08x-%04x%04x-%02x%02x%02x%02x-%02x%02x%02x%02x",
					v13,
					HIWORD(v13),
					v15,
					v16,
					v17,
					v18,
					v19,
					v20,
					v21,
					v22,
					v23);
				v8 = 1;
			}
		}
	LABEL_11:
		GlobalFree(v4);
		result = v8;
	}
	return result;
}

BOOL IsElevated()
{
	BOOL fRet = FALSE;
	HANDLE hToken = NULL;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
	{
		TOKEN_ELEVATION Elevation;
		DWORD cbSize = sizeof(TOKEN_ELEVATION);
		if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize))
		{
			fRet = Elevation.TokenIsElevated;
		}
	}
	if (hToken)
	{
		CloseHandle(hToken);
	}
	return fRet;
}


int main()
{
	CHAR cRegEntry[256];
	CHAR cHWID[30];
	HKEY hKey;
	LSTATUS lStatus;

	SetConsoleTitle(L"StartIsBack Crack");

	if (!IsElevated())
	{
		printf(
			"This cannot complete its function without admin rights! Please rerun this application as administrator!\n");
		printf("Press any key to exit!");
		getchar();
		exit(-1);
	}

	lstrcpyA(cHWID, "yyyy yyyy");
	RetrieveUniqueHWID(cHWID);
	cHWID[13] = 45;
	lstrcpyA(cRegEntry, "SOFTWARE\\Classes\\CLSID\\{");
	lstrcatA(cRegEntry, cHWID);
	lstrcatA(cRegEntry, "}");
	printf("Deleting the key: %s\n", cRegEntry);

	lStatus = RegOpenKeyExA(HKEY_CURRENT_USER, cRegEntry, 0, KEY_READ, &hKey);
	
	if (lStatus == ERROR_SUCCESS)
	{
		lStatus = SHDeleteKeyA(HKEY_CURRENT_USER, cRegEntry);

		if (lStatus == ERROR_SUCCESS)
			printf("Successfully deleted key!\n");
		else
			printf(
				"There was an exception when attempting to delete the registry key! ERROR CODE: %04x\n",
				lStatus);
	}
	else
		printf("Could not find the CLSID key, perhaps it was already deleted! error code: %d\n", lStatus);


	printf("Press any key to exit!");
	getchar();
	exit(0);
}
