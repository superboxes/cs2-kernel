#include "render.hpp"
#include <windows.h>

namespace render
{
    void DrawLine(HDC hdc, int x1, int y1, int x2, int y2, COLORREF color)
    {
        HPEN hPen = CreatePen(PS_SOLID, 2, color);
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

        MoveToEx(hdc, x1, y1, NULL);
        LineTo(hdc, x2, y2);

        SelectObject(hdc, hOldPen);
        DeleteObject(hPen);
    }

    void DrawCircle(HDC hdc, int x, int y, int radius, COLORREF color)
    {
        HPEN hPen = CreatePen(PS_SOLID, 2, color);
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

        Arc(hdc, x - radius, y - radius, x + radius, y + radius * 1.5, 0, 0, 0, 0);

        SelectObject(hdc, hOldPen);
        DeleteObject(hPen);
    }

    void DrawBorderBox(HDC hdc, int x, int y, int w, int h, COLORREF borderColor)
    {
        HBRUSH hBorderBrush = CreateSolidBrush(borderColor);
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBorderBrush);

        RECT rect = { x, y, x + w, y + h };
        FrameRect(hdc, &rect, hBorderBrush);

        SelectObject(hdc, hOldBrush);
        DeleteObject(hBorderBrush);
    }

    void DrawFilledBox(HDC hdc, int x, int y, int width, int height, COLORREF color)
    {
        HBRUSH hBrush = CreateSolidBrush(color);
        RECT rect = { x, y, x + width, y + height };
        FillRect(hdc, &rect, hBrush);
        DeleteObject(hBrush);
    }

    void SetTextSize(HDC hdc, int textSize)
    {
        LOGFONT lf;
        HFONT hFont, hOldFont;

        ZeroMemory(&lf, sizeof(LOGFONT));
        lf.lfHeight = -textSize;
        lf.lfWeight = FW_NORMAL;
        lf.lfQuality = ANTIALIASED_QUALITY;

        hFont = CreateFontIndirect(&lf);
        hOldFont = (HFONT)SelectObject(hdc, hFont);
        DeleteObject(hOldFont);
    }

    void RenderText(HDC hdc, int x, int y, const char* text, COLORREF textColor, int textSize)
    {
        SetTextSize(hdc, textSize);
        SetTextColor(hdc, textColor);

        int len = MultiByteToWideChar(CP_UTF8, 0, text, -1, NULL, 0);
        wchar_t* wide_text = new wchar_t[len];
        MultiByteToWideChar(CP_UTF8, 0, text, -1, wide_text, len);

        TextOutW(hdc, x, y, wide_text, len - 1);

        delete[] wide_text;
    }
}