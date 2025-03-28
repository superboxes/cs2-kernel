#pragma once
#include <wtypes.h>

namespace render
{
    void DrawLine(HDC hdc, int x1, int y1, int x2, int y2, COLORREF color);
    void DrawCircle(HDC hdc, int x, int y, int radius, COLORREF color);
    void DrawBorderBox(HDC hdc, int x, int y, int w, int h, COLORREF borderColor);
    void DrawFilledBox(HDC hdc, int x, int y, int width, int height, COLORREF color);
    void SetTextSize(HDC hdc, int textSize);
    void RenderText(HDC hdc, int x, int y, const char* text, COLORREF textColor, int textSize);
}