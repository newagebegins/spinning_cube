/*

TODO:
- Draw axis
  - Or simpler: use different colors for cube vertices
- Rotate cube around Z axis
- Indicate depth by the size of the vertices

*/

#include <windows.h>
#include <math.h>

#define PI 3.14159265f

struct GameState {
    BITMAPINFO *bitmapInfo;
    int *bitmapBuffer;
    int bitmapWidth, bitmapHeight;
};

void initGame(GameState *state, int windowWidth, int windowHeight) {
    state->bitmapWidth = windowWidth;
    state->bitmapHeight = windowHeight;

    state->bitmapInfo = (BITMAPINFO *)malloc(sizeof(BITMAPINFO));
    state->bitmapInfo->bmiHeader.biSize = sizeof(state->bitmapInfo->bmiHeader);
    state->bitmapInfo->bmiHeader.biWidth = state->bitmapWidth;
    state->bitmapInfo->bmiHeader.biHeight = -state->bitmapHeight;
    state->bitmapInfo->bmiHeader.biPlanes = 1;
    state->bitmapInfo->bmiHeader.biBitCount = 32;
    state->bitmapInfo->bmiHeader.biCompression = BI_RGB;

    state->bitmapBuffer = (int*)malloc(state->bitmapWidth * state->bitmapHeight * sizeof(int));
}

void drawRectangle(GameState *state, int x1, int y1, int x2, int y2, int color) {
    if (x1 < 0) x1 = 0;
    if (y1 < 0) y1 = 0;
    if (x2 >= state->bitmapWidth) x2 = state->bitmapWidth - 1;
    if (y2 >= state->bitmapHeight) y2 = state->bitmapHeight - 1;
    for (int y = y1; y < y2; ++y) {
        for (int x = x1; x < x2; ++x) {
            state->bitmapBuffer[y*state->bitmapWidth + x] = color;
        }
    }
}

void clearScreen(GameState *state) {
    drawRectangle(state, 0, 0, state->bitmapWidth-1, state->bitmapHeight-1, 0);
}

LRESULT CALLBACK wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = {};
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = wndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = "cg";

    if (!RegisterClass(&wc)) {
        MessageBox(NULL, "RegisterClass failed!", NULL, NULL);
        return 1;
    }

    int windowWidth = 800;
    int windowHeight = 600;

    RECT clientRect = { 0, 0, windowWidth, windowHeight };
    DWORD windowStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
    AdjustWindowRect(&clientRect, windowStyle, NULL);
    HWND hWnd = CreateWindowEx(NULL, wc.lpszClassName, "cg hackery", windowStyle, 300, 0,
        clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, NULL, NULL, hInstance, NULL);

    if (!hWnd) {
        MessageBox(NULL, "CreateWindowEx failed!", NULL, NULL);
        return 1;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    LARGE_INTEGER perfCounterFrequency = { 0 };
    QueryPerformanceFrequency(&perfCounterFrequency);
    LARGE_INTEGER perfCounter = { 0 };
    QueryPerformanceCounter(&perfCounter);
    LARGE_INTEGER prevPerfCounter = { 0 };

    float dt = 0.0f;
    float realDt = dt;
    const float targetFps = 60.0f;
    const float targetDt = 1.0f / targetFps;

    GameState *gameState = (GameState *)malloc(sizeof(GameState));
    memset(gameState, 0, sizeof(GameState));
    initGame(gameState, windowWidth, windowHeight);

    bool gameIsRunning = true;
    float theta = 0;
    bool upIsDown = false;

    while (gameIsRunning) {
        prevPerfCounter = perfCounter;
        QueryPerformanceCounter(&perfCounter);
        dt = (float)(perfCounter.QuadPart - prevPerfCounter.QuadPart) / (float)perfCounterFrequency.QuadPart;
        realDt = dt;
        if (dt > targetDt)
            dt = targetDt;

        MSG msg;
        while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
            switch (msg.message) {
                case WM_QUIT:
                    gameIsRunning = false;
                    break;
                case WM_KEYDOWN:
                case WM_KEYUP: {
                    bool isDown = ((msg.lParam & (1 << 31)) == 0);
                    switch (msg.wParam) {
                        case VK_LEFT: {
                            break;
                        }
                        case VK_RIGHT: {
                            break;
                        }
                        case VK_UP: {
                            upIsDown = isDown;
                            break;
                        }
                        case VK_SPACE: {
                            break;
                        }
                        case VK_ESCAPE: {
                            gameIsRunning = false;
                            break;
                        }
                    }
                    break;
                }
                default:
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                    break;
            }
        }

        float corners[][3] = {
            { 1, -1, -1 },
            { 1, -1, 1 },
            { 1,  1, -1 },
            { 1,  1, 1 },
            { -1, -1, -1 },
            { -1, -1, 1 },
            { -1,  1, -1 },
            { -1,  1, 1 }
        };

        unsigned int colors[] = {
            0xFFFFFF00,
            0xFFFF0000,
            0xFFFFFF00,
            0xFFFF0000,
            0xFFFFFF00,
            0xFFFF0000,
            0xFFFFFF00,
            0xFFFF0000,
        };

        theta += 0.01f;

        clearScreen(gameState);

        for (int i = 0; i < 8; ++i) {
            float x = corners[i][0];
            float y = corners[i][1];
            float z = corners[i][2];

            float x_rotated = x * cosf(theta) + z * sinf(theta);
            float y_rotated = y;
            float z_rotated = z * cosf(theta) - x * sinf(theta) - 3.0f;

            float x_proj = x_rotated / -z_rotated;
            float y_proj = y_rotated / -z_rotated;

            float x_proj_remap = (1.0f + x_proj) / 2.0f;
            float y_proj_remap = (1.0f + y_proj) / 2.0f;

            int x_proj_pix = (int) (x_proj_remap * gameState->bitmapWidth);
            int y_proj_pix = (int) (y_proj_remap * gameState->bitmapHeight);

            int w = (int) fabs(20.0f / z_rotated);
            int h = w;
            drawRectangle(gameState, x_proj_pix - w, y_proj_pix - h, x_proj_pix + w, y_proj_pix + h, colors[i]);
        }

        HDC hDC = GetDC(hWnd);

        StretchDIBits(hDC, 0, 0, gameState->bitmapWidth, gameState->bitmapHeight,
            0, 0, gameState->bitmapWidth, gameState->bitmapHeight,
            gameState->bitmapBuffer, gameState->bitmapInfo, DIB_RGB_COLORS, SRCCOPY);
        ReleaseDC(hWnd, hDC);
    }

    return 0;
}
