#include <windows.h>
#include <stdio.h>
#include "student.h"
#include "academic.h"
#include "dlg_student.h"
#include "dlg_detail.h"

#define ID_BTN_EDIT_DETAIL 3001
#define ID_BTN_CLOSE_DETAIL 3002

static Student *pCurSV = NULL;
static HFONT hFDetail, hFTitleDetail, hFBoldDetail;
static HINSTANCE hInstDetail;
static BOOL detailModified = FALSE;

/* Helper để format số tiền có dấu chấm */
static wchar_t* FormatVND(float n) {
    static wchar_t buf[64]; wchar_t tmp[64];
    swprintf(tmp, 64, L"%.0f", n);
    int len = wcslen(tmp), out = 0;
    for (int i=0; i<len; i++) {
        if (i > 0 && (len-i)%3 == 0) buf[out++] = L'.';
        buf[out++] = tmp[i];
    }
    buf[out] = L'\0'; wcscat(buf, L" VND"); return buf;
}

static void DrawDetailGDI(HWND hWnd, HDC hdc) {
    RECT rc; GetClientRect(hWnd, &rc);
    HDC mDC = CreateCompatibleDC(hdc);
    HBITMAP mBM = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
    SelectObject(mDC, mBM);
    FillRect(mDC, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));

    /* --- PANEL TRÁI: Thông tin cá nhân --- */
    SetBkMode(mDC, TRANSPARENT);
    /* Avatar Circle */
    HBRUSH aBr = CreateSolidBrush(RGB(99,102,241)); SelectObject(mDC, aBr);
    Ellipse(mDC, 100, 30, 200, 130); DeleteObject(aBr);
    SelectObject(mDC, hFTitleDetail); SetTextColor(mDC, RGB(255,255,255));
    wchar_t init[2] = {pCurSV->name[0], L'\0'}; RECT rI={100,30,200,130}; DrawTextW(mDC, init, -1, &rI, DT_CENTER|DT_VCENTER|DT_SINGLELINE);

    SetTextColor(mDC, RGB(15,23,42)); RECT rN = {10, 140, 290, 175}; DrawTextW(mDC, pCurSV->name, -1, &rN, DT_CENTER|DT_SINGLELINE);
    
    /* Thông tin dòng */
    SelectObject(mDC, hFDetail); SetTextColor(mDC, RGB(100,116,139));
    int y=200; wchar_t b[256];
    TextOutW(mDC, 30, y, L"Mã SV:", 6); SetTextColor(mDC, RGB(15,23,42)); swprintf(b,20,L"%d",pCurSV->id); TextOutW(mDC, 120, y, b, wcslen(b)); 
    y+=30; SetTextColor(mDC, RGB(100,116,139)); TextOutW(mDC, 30, y, L"Lớp:", 4); SetTextColor(mDC, RGB(15,23,42)); TextOutW(mDC, 120, y, pCurSV->class_name, wcslen(pCurSV->class_name));
    y+=30; SetTextColor(mDC, RGB(100,116,139)); TextOutW(mDC, 30, y, L"Giới tính:", 10); SetTextColor(mDC, RGB(15,23,42)); TextOutW(mDC, 120, y, pCurSV->gender, wcslen(pCurSV->gender));
    y+=30; SetTextColor(mDC, RGB(100,116,139)); TextOutW(mDC, 30, y, L"Ngày sinh:", 10); SetTextColor(mDC, RGB(15,23,42)); TextOutW(mDC, 120, y, pCurSV->birth_date, wcslen(pCurSV->birth_date));
    
    /* Tài chính */
    y+=50; SelectObject(mDC, hFBoldDetail); SetTextColor(mDC, RGB(15,23,42)); TextOutW(mDC, 30, y, L"💰 TÀI CHÍNH", 12);
    SelectObject(mDC, hFDetail); y+=30;
    SetTextColor(mDC, RGB(100,116,139)); TextOutW(mDC, 30, y, L"Phải đóng:", 10); SetTextColor(mDC, RGB(185,28,28)); TextOutW(mDC, 120, y, FormatVND(pCurSV->tuition_total), wcslen(FormatVND(pCurSV->tuition_total)));
    y+=25; SetTextColor(mDC, RGB(100,116,139)); TextOutW(mDC, 30, y, L"Trạng thái:", 11); SetTextColor(mDC, pCurSV->tuition_status?RGB(34,197,94):RGB(245,158,11)); TextOutW(mDC, 120, y, pCurSV->tuition_status?L"Đã đóng ✅":L"Chưa đóng ⚠️", pCurSV->tuition_status?10:12);

    /* --- PANEL PHẢI: Bảng điểm --- */
    HPEN p = CreatePen(PS_SOLID, 1, RGB(226,232,240)); SelectObject(mDC, p); MoveToEx(mDC, 300, 20, NULL); LineTo(mDC, 300, 550); DeleteObject(p);
    SelectObject(mDC, hFBoldDetail); SetTextColor(mDC, RGB(15,23,42)); TextOutW(mDC, 320, 30, L"📚 BẢNG ĐIỂM CHI TIẾT", 21);
    
    int tx=320, ty=70;
    SelectObject(mDC, hFBoldDetail); RECT rH = {tx, ty, rc.right-20, ty+30}; DrawTextW(mDC, L"Môn Học                     TC   QT   GK   CK   GPA   Loại", -1, &rH, DT_LEFT);
    ty += 35; SelectObject(mDC, hFDetail);
    for(int i=0; i<pCurSV->subject_count; i++) {
        if(i%2==0) { HBRUSH b=CreateSolidBrush(RGB(248,250,252)); RECT r={tx-5, ty, rc.right-20, ty+30}; FillRect(mDC,&r,b); DeleteObject(b); }
        SetTextColor(mDC, RGB(15,23,42));
        TextOutW(mDC, tx, ty+5, pCurSV->subjects[i].name, wcslen(pCurSV->subjects[i].name));
        swprintf(b,20,L"%d", pCurSV->subjects[i].credits); TextOutW(mDC, tx+170, ty+5, b, wcslen(b));
        swprintf(b,20,L"%.1f", pCurSV->subjects[i].score_process); TextOutW(mDC, tx+205, ty+5, b, wcslen(b));
        swprintf(b,20,L"%.1f", pCurSV->subjects[i].score_midterm); TextOutW(mDC, tx+240, ty+5, b, wcslen(b));
        swprintf(b,20,L"%.1f", pCurSV->subjects[i].score_final); TextOutW(mDC, tx+275, ty+5, b, wcslen(b));
        swprintf(b,20,L"%.2f", pCurSV->subjects[i].gpa_10); TextOutW(mDC, tx+315, ty+5, b, wcslen(b));
        /* Grade Badge */
        COLORREF gC = (pCurSV->subjects[i].gpa_4 >= 3.0f)?RGB(34,197,94):((pCurSV->subjects[i].gpa_4 >= 1.0f)?RGB(59,130,246):RGB(239,68,68));
        HBRUSH bBr = CreateSolidBrush(gC); RECT rb={tx+365, ty+4, tx+395, ty+24}; FillRect(mDC,&rb,bBr); DeleteObject(bBr);
        SetTextColor(mDC, RGB(255,255,255)); DrawTextW(mDC, pCurSV->subjects[i].grade, -1, &rb, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        ty += 30;
    }

    /* Kết quả tổng hợp */
    ty += 20; SelectObject(mDC, hFBoldDetail); SetTextColor(mDC, RGB(15,23,42));
    swprintf(b, 100, L"TỔNG KẾT: GPA %.2f — Xếp loại: %ls", pCurSV->gpa_4_avg, pCurSV->rank);
    TextOutW(mDC, tx, ty, b, wcslen(b));

    BitBlt(hdc, 0, 0, rc.right, rc.bottom, mDC, 0, 0, SRCCOPY);
    DeleteObject(mBM); DeleteDC(mDC);
}

LRESULT CALLBACK StudentDetailWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            hFDetail = CreateFontW(16,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,0,0,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_SWISS,L"Segoe UI");
            hFTitleDetail = CreateFontW(22,0,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,0,0,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_SWISS,L"Segoe UI");
            hFBoldDetail = CreateFontW(16,0,0,0,FW_SEMIBOLD,0,0,0,DEFAULT_CHARSET,0,0,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_SWISS,L"Segoe UI");
            CreateWindowW(L"BUTTON", L"✏️ Chỉnh Sửa", WS_CHILD|WS_VISIBLE, 650, 555, 120, 35, hWnd, (HMENU)ID_BTN_EDIT_DETAIL, hInstDetail, NULL);
            CreateWindowW(L"BUTTON", L"✖️ Đóng", WS_CHILD|WS_VISIBLE, 780, 555, 80, 35, hWnd, (HMENU)ID_BTN_CLOSE_DETAIL, hInstDetail, NULL);
            break;
        }
        case WM_PAINT: { PAINTSTRUCT ps; HDC hdc = BeginPaint(hWnd, &ps); DrawDetailGDI(hWnd, hdc); EndPaint(hWnd, &ps); break; }
        case WM_COMMAND: {
            if (LOWORD(wParam) == ID_BTN_CLOSE_DETAIL) DestroyWindow(hWnd);
            if (LOWORD(wParam) == ID_BTN_EDIT_DETAIL) {
                if (ShowStudentDialog(hWnd, pCurSV, TRUE, hInstDetail)) { detailModified = TRUE; InvalidateRect(hWnd, NULL, TRUE); }
            }
            break;
        }
        case WM_DESTROY: break;
        default: return DefWindowProcW(hWnd, msg, wParam, lParam);
    }
    return 0;
}

void ShowStudentDetailDialog(HWND hParent, Student *s, HINSTANCE hInst) {
    pCurSV = s; hInstDetail = hInst; detailModified = FALSE;
    WNDCLASSW wc = {0}; wc.lpfnWndProc = StudentDetailWndProc; wc.hInstance = hInst; wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1); wc.lpszClassName = L"DetailSvClass";
    RegisterClassW(&wc);
    HWND hDlg = CreateWindowExW(WS_EX_DLGMODALFRAME, L"DetailSvClass", L"Chi Tiết Sinh Viên", WS_VISIBLE|WS_CAPTION|WS_SYSMENU, 150, 150, 880, 640, hParent, NULL, hInst, NULL);
    EnableWindow(hParent, FALSE);
    MSG m; while (IsWindow(hDlg) && GetMessageW(&m, NULL, 0, 0)) { TranslateMessage(&m); DispatchMessageW(&m); }
    EnableWindow(hParent, TRUE); SetForegroundWindow(hParent);
}
