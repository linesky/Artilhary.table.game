#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//./gcc artelharytablewindows.c -o artillery -lgdi32 -luser32 -mwindows

#define GRID_SIZE 16
#define CELL_SIZE 40
#define WINDOW_SIZE (GRID_SIZE * CELL_SIZE)
#define CANNON_X (GRID_SIZE / 2)
#define CANNON_Y (GRID_SIZE - 1)
#define BALL_SIZE 10

HINSTANCE hInst;
HWND hWndMain, hAngleInput, hAngleButton, hChargeInput, hChargeButton;
int cannon_angle = 90;
POINT ball;
RECT target;

void InitializeGame();
void DrawGrid(HDC hdc);
void PlaceCannon(HDC hdc);
void PlaceTarget();
void RotateCannon();
void FireCannon();
void AnimateBall(int charge);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)  {
    hInst = hInstance;
    WNDCLASS wc = { 0 };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "ArtilleryGameClass";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);

    hWndMain = CreateWindowEx(
        0,
        wc.lpszClassName,
        "Artillery Game",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_SIZE + 16, WINDOW_SIZE + 100,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hWndMain == NULL) {
        return 0;
    }

    ShowWindow(hWndMain, nCmdShow);
    UpdateWindow(hWndMain);

    InitializeGame();

    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

void InitializeGame() {
    srand((unsigned int)time(NULL));

    CreateWindow(
        "STATIC", "Angle (0-180):",
        WS_CHILD | WS_VISIBLE,
        10, 10, 100, 20,
        hWndMain, NULL, hInst, NULL
    );

    hAngleInput = CreateWindow(
        "EDIT", "",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
        110, 10, 50, 20,
        hWndMain, NULL, hInst, NULL
    );

    hAngleButton = CreateWindow(
        "BUTTON", "Rotate",
        WS_CHILD | WS_VISIBLE,
        170, 10, 70, 20,
        hWndMain, (HMENU)1, hInst, NULL
    );

    CreateWindow(
        "STATIC", "Charge:",
        WS_CHILD | WS_VISIBLE,
        250, 10, 50, 20,
        hWndMain, NULL, hInst, NULL
    );

    hChargeInput = CreateWindow(
        "EDIT", "",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
        300, 10, 50, 20,
        hWndMain, NULL, hInst, NULL
    );

    hChargeButton = CreateWindow(
        "BUTTON", "Fire",
        WS_CHILD | WS_VISIBLE,
        360, 10, 70, 20,
        hWndMain, (HMENU)2, hInst, NULL
    );

    PlaceTarget();
}

void DrawGrid(HDC hdc) {
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    SelectObject(hdc, hPen);

    for (int i = 0; i <= GRID_SIZE; ++i) {
        MoveToEx(hdc, i * CELL_SIZE, 0, NULL);
        LineTo(hdc, i * CELL_SIZE, WINDOW_SIZE);
        MoveToEx(hdc, 0, i * CELL_SIZE, NULL);
        LineTo(hdc, WINDOW_SIZE, i * CELL_SIZE);
    }

    DeleteObject(hPen);
}

void PlaceCannon(HDC hdc) {
    HPEN hPen = CreatePen(PS_SOLID, 5, RGB(0, 0, 0));
    SelectObject(hdc, hPen);

    double angle_rad = cannon_angle * M_PI / 180.0;
    int x1 = CANNON_X * CELL_SIZE + CELL_SIZE / 2;
    int y1 = CANNON_Y * CELL_SIZE + CELL_SIZE / 2;
    int x2 = (int)(x1 + 20 * cos(angle_rad));
    int y2 = (int)(y1 - 20 * sin(angle_rad));

    MoveToEx(hdc, x1, y1, NULL);
    LineTo(hdc, x2, y2);

    DeleteObject(hPen);
}

void PlaceTarget() {
    target.left = (rand() % GRID_SIZE) * CELL_SIZE;
    target.top = 0;
    target.right = target.left + CELL_SIZE;
    target.bottom = target.top + CELL_SIZE;
}

void RotateCannon() {
    char buffer[4];
    GetWindowText(hAngleInput, buffer, 4);
    cannon_angle = atoi(buffer);

    if (cannon_angle < 0) cannon_angle = 0;
    if (cannon_angle > 180) cannon_angle = 180;

    InvalidateRect(hWndMain, NULL, TRUE);
}

void FireCannon() {
    char buffer[4];
    GetWindowText(hChargeInput, buffer, 4);
    int charge = atoi(buffer);

    if (charge > 0) {
        AnimateBall(charge);
    }
}

void AnimateBall(int charge) {
    HDC hdc = GetDC(hWndMain);

    double angle_rad = cannon_angle * M_PI / 180.0;
    double dx = cos(angle_rad);
    double dy = -sin(angle_rad);

    ball.x = CANNON_X * CELL_SIZE + CELL_SIZE / 2;
    ball.y = CANNON_Y * CELL_SIZE + CELL_SIZE / 2;
    int distance = 0;

    while (distance < charge && ball.x >= 0 && ball.x < WINDOW_SIZE && ball.y >= 0 && ball.y < WINDOW_SIZE) {
        ball.x = (int)(ball.x + dx * CELL_SIZE);
        ball.y = (int)(ball.y + dy * CELL_SIZE);
        distance++;
        Ellipse(hdc, ball.x - BALL_SIZE / 2, ball.y - BALL_SIZE / 2, ball.x + BALL_SIZE / 2, ball.y + BALL_SIZE / 2);
        Sleep(100);
    }

    while (ball.y < WINDOW_SIZE - CELL_SIZE) {
        ball.y += CELL_SIZE;
        Ellipse(hdc, ball.x - BALL_SIZE / 2, ball.y - BALL_SIZE / 2, ball.x + BALL_SIZE / 2, ball.y + BALL_SIZE / 2);
        Sleep(100);
    }

    if (ball.x >= target.left && ball.x < target.right && ball.y >= target.top && ball.y < target.bottom) {
        PlaceTarget();
    }

    ReleaseDC(hWndMain, hdc);
    InvalidateRect(hWndMain, NULL, TRUE);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            DrawGrid(hdc);
            PlaceCannon(hdc);

            HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));
            FillRect(hdc, &target, hBrush);
            DeleteObject(hBrush);

            EndPaint(hwnd, &ps);
        } break;

        case WM_COMMAND:
            if (LOWORD(wParam) == 1) {
                RotateCannon();
            } else if (LOWORD(wParam) == 2) {
                FireCannon();
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

