#include "Main.h"

static TCHAR szWindowClass[] = _T("Thumb");
static TCHAR szTitle[] = _T("Thumb");
static HWND hWnd;
static HWND hWndSource;
static HTHUMBNAIL hThumbnail;
static RECT rtSource;
static float zoom = 1.0f;
static POINT center; // relative to your own window

void UpdateThumbnailPosition()
{
	RECT rDest, rSrc{};
	DWM_THUMBNAIL_PROPERTIES prop{};

	GetClientRect(hWndSource, &rSrc);
	GetClientRect(hWnd, &rDest);

	// origin is 0,0, so right/bottom = width/height
	POINT scaledCenter = { 
		(center.x * rSrc.right) / rDest.right, 
		(center.y * rSrc.bottom) / rDest.bottom };

	POINT area = {
		rSrc.right / zoom,
		rSrc.bottom / zoom
	};
	RECT r{
		scaledCenter.x - (area.x / 2),
		scaledCenter.y - (area.y / 2),
		scaledCenter.x + (area.x / 2),
		scaledCenter.y + (area.y / 2)
	};

	if (r.left < 0) {
		r.right -= r.left;
		r.left = 0;
	}
	if (r.right > rSrc.right)
	{
		r.left -= r.right - rSrc.right;
		r.right = rSrc.right;
	}

	if (r.top < 0) {
		r.bottom -= r.top;
		r.top = 0;
	}
	if (r.bottom > rSrc.bottom)
	{
		r.top -= r.bottom - rSrc.bottom;
		r.bottom = rSrc.bottom;
	}

	if (zoom == 1.0f)
	{
		r = rSrc;
	}

	prop.dwFlags = DWM_TNP_RECTSOURCE;
	prop.fSourceClientAreaOnly = true;
	prop.fVisible = true;
	prop.opacity = 255;
	prop.rcSource = r;
	DwmUpdateThumbnailProperties(hThumbnail, &prop);
}

LRESULT CALLBACK WndProc(
	_In_ HWND   hWnd,
	_In_ UINT   message,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
) {
	short zDelta, xPos, yPos;
	RECT dimensions{};
	DWM_THUMBNAIL_PROPERTIES prop{};
	switch (message) {
	case WM_SHOWWINDOW: 
		DwmRegisterThumbnail(hWnd, GetAncestor(hWndSource, GA_ROOTOWNER), &hThumbnail);
		break;
	case WM_SIZE:
		GetClientRect(hWnd, &dimensions);

		prop.dwFlags = DWM_TNP_RECTDESTINATION;
		prop.fSourceClientAreaOnly = true;
		prop.fVisible = true;
		prop.opacity = 255;
		prop.rcDestination = dimensions;
		DwmUpdateThumbnailProperties(hThumbnail, &prop);
		break;
	case WM_DESTROY:
		DwmUnregisterThumbnail(&hThumbnail);
		PostQuitMessage(0);
		break;
	case WM_MOUSEWHEEL:
		zoom = max(1, zoom + 0.1f * GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA);
		center.x = GET_X_LPARAM(lParam);
		center.y = GET_Y_LPARAM(lParam);
		ScreenToClient(hWnd, &center);
		UpdateThumbnailPosition();
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
}

int CALLBACK wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow
) {
	hWndSource = FindWindow(nullptr, lpCmdLine);
	if (hWndSource == nullptr || !IsWindow(hWndSource)) {
		MessageBox(NULL,
			_T("No window matching!"),
			szTitle,
			NULL);

		return 1;
	}
	
	WNDCLASSEX wcex{};

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			szTitle,
			NULL);

		return 1;
	}

	RECT r{};
	GetClientRect(hWndSource, &r);
	hWnd = CreateWindowEx(
		WS_EX_TOPMOST,
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		max(256, r.right / 2), max(256, r.bottom / 2),
		NULL,
		NULL,
		hInstance,
		NULL
	);
	if (!hWnd)
	{
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			szTitle,
			NULL);

		return 1;
	}

	MARGINS margins{};
	DwmExtendFrameIntoClientArea(0, &margins);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}