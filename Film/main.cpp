
#include "stdafx.h"
#include "ui/ui.h"
#include "model/data.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	CoInitialize(nullptr);

	MyRegisterClass(hInstance);
	if (!InitInstance(hInstance, nCmdShow)) return FALSE;

	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE)
		{
			EscapeProc();
			continue;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	CoUninitialize();
	return (int)msg.wParam;
}
