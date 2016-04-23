/*

TODO:
- Draw edges
- Draw axis
  - Or simpler: use different colors for cube vertices

*/

#include <windows.h>
#include <math.h>

#define PI 3.14159265f

#define VERTEX_COUNT 8
#define EDGE_COUNT 12

struct BackBuffer {
    BITMAPINFO info;
    unsigned int *memory;
    int width, height;
};

BackBuffer makeBackBuffer(int width, int height) {
    BackBuffer bb = {};

    bb.width = width;
    bb.height = height;

    bb.info.bmiHeader.biSize = sizeof(bb.info.bmiHeader);
    bb.info.bmiHeader.biWidth = width;
    bb.info.bmiHeader.biHeight = -height;
    bb.info.bmiHeader.biPlanes = 1;
    bb.info.bmiHeader.biBitCount = 32;
    bb.info.bmiHeader.biCompression = BI_RGB;

    bb.memory = (unsigned int*)malloc(width * height * sizeof(unsigned int));

    return bb;
}

void setPixel(BackBuffer *bb, int x, int y, unsigned int c) {
    bb->memory[y*bb->width + x] = c;
}

float square(float x) {
    return x * x;
}

void drawRectangle(BackBuffer *bb, int x1, int y1, int x2, int y2, unsigned int color) {
    if (x1 < 0) x1 = 0;
    if (y1 < 0) y1 = 0;
    if (x2 >= bb->width) x2 = bb->width - 1;
    if (y2 >= bb->height) y2 = bb->height - 1;
    for (int y = y1; y < y2; ++y) {
        for (int x = x1; x < x2; ++x) {
            setPixel(bb, x, y, color);
        }
    }
}

void drawLine(BackBuffer *bb, int x1, int y1, int x2, int y2, unsigned int color) {
    int xStart, xEnd, yStart, yEnd;
    int dx = x2 - x1;
    int dy = y2 - y1;

    if (abs(dx) > abs(dy)) {
        float m = float(dy) / float(dx);
        if (x1 < x2) {
            xStart = x1;
            yStart = y1;
            xEnd = x2;
            yEnd = y2;
        } else {
            xStart = x2;
            yStart = y2;
            xEnd = x1;
            yEnd = y1;
        }
        for (int x = xStart; x <= xEnd; ++x) {
            int y = int(m * (x - xStart) + yStart);
            setPixel(bb, x, y, color);
        }
    } else {
        float m = float(dx) / float(dy);
        if (y1 < y2) {
            xStart = x1;
            yStart = y1;
            xEnd = x2;
            yEnd = y2;
        } else {
            xStart = x2;
            yStart = y2;
            xEnd = x1;
            yEnd = y1;
        }
        for (int y = yStart; y <= yEnd; ++y) {
            int x = int(m * (y - yStart) + xStart);
            setPixel(bb, x, y, color);
        }
    }
}

void clearScreen(BackBuffer *bb) {
    drawRectangle(bb, 0, 0, bb->width - 1, bb->height - 1, 0);
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

    BackBuffer bb = makeBackBuffer(windowWidth, windowHeight);

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

        float vertices[VERTEX_COUNT][3] = {
            { 1, -1, -1 },
            { 1, -1, 1 },
            { 1,  1, -1 },
            { 1,  1, 1 },
            { -1, -1, -1 },
            { -1, -1, 1 },
            { -1,  1, -1 },
            { -1,  1, 1 }
        };

        int projectedVertices[VERTEX_COUNT][2] = {};

        int edges[EDGE_COUNT][2] = {
            {0, 1}, {2, 3}, {0, 2}, {1, 3},
            {4, 5}, {6, 7}, {4, 6}, {5, 7},
            {4, 0}, {6, 2}, {5, 1}, {7, 3}
        };

        unsigned int colors[] = {
            0xFFFF0000, // 0 red
            0xFF00FF00, // 1 green
            0xFF0000FF, // 2 blue
            0xFFFFFF00, // 3 yellow
            0xFF00FFFF, // 4 cyan
            0xFFFF00FF, // 5 magenta
            0xFFFFFFFF, // 6 white
            0xFF808080, // 7 gray
        };

        theta += 0.01f;

        clearScreen(&bb);

        for (int i = 0; i < VERTEX_COUNT; ++i) {
            float x = vertices[i][0];
            float y = vertices[i][1];
            float z = vertices[i][2];

            float x_rotated = x * cosf(theta) + z * sinf(theta);
            float y_rotated = y;
            float z_rotated = z * cosf(theta) - x * sinf(theta) - 3.0f;

            float x_proj = x_rotated / -z_rotated;
            float y_proj = y_rotated / -z_rotated;

            float x_proj_remap = (1.0f + x_proj) / 2.0f;
            float y_proj_remap = (1.0f + y_proj) / 2.0f;

            int x_proj_pix = (int) (x_proj_remap * bb.width);
            int y_proj_pix = (int) (y_proj_remap * bb.height);

            projectedVertices[i][0] = x_proj_pix;
            projectedVertices[i][1] = y_proj_pix;

            int sz = 5;
            drawRectangle(&bb, x_proj_pix - sz, y_proj_pix - sz, x_proj_pix + sz, y_proj_pix + sz, colors[i]);
        }

        for (int edge = 0; edge < EDGE_COUNT; ++edge) {
            int x1 = projectedVertices[edges[edge][0]][0];
            int y1 = projectedVertices[edges[edge][0]][1];
            int x2 = projectedVertices[edges[edge][1]][0];
            int y2 = projectedVertices[edges[edge][1]][1];
            drawLine(&bb, x1, y1, x2, y2, 0xFF00FF00);
        }

        // clearScreen(&bb);
        // drawLine(&bb, 0, 0, 100, 100, 0xFFFFFF00);
        // drawLine(&bb, 100, 0, 100, 100, 0xFFFFFF00);
        // drawLine(&bb, 200, 200, 100, 0, 0xFFFF0000);
        // drawLine(&bb, 200, 450, 299, 375, 0xFFFF00FF);
        // drawLine(&bb, 0, 300, 100, 300, 0xFFFF0000);
        // drawLine(&bb, 100, 305, 0, 305, 0xFFFF0000);

        HDC hDC = GetDC(hWnd);

        StretchDIBits(hDC, 0, 0, bb.width, bb.height,
                      0, 0, bb.width, bb.height, bb.memory,
                      &bb.info, DIB_RGB_COLORS, SRCCOPY);
        ReleaseDC(hWnd, hDC);
    }

    return 0;
}
