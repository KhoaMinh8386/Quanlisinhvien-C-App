#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "student.h"
#include "academic.h"
#include "management.h"
#include "dlg_student.h"
#include "dlg_detail.h"

/* --- COLOR PALETTE --- */
#define CLR_SIDEBAR_BG    RGB(15,  23,  42)   
#define CLR_SIDEBAR_HOVER RGB(30,  41,  59)   
#define CLR_ACCENT        RGB(99, 102, 241)   
#define CLR_SUCCESS       RGB(34, 197,  94)   
#define CLR_WARNING       RGB(245, 158,  11)   
#define CLR_DANGER        RGB(239, 68,  68)   
#define CLR_WHITE         RGB(255,255,255)
#define CLR_TEXT_MUTED    RGB(148,163,184)    
#define CLR_BORDER        RGB(226,232,240)    

/* --- IDs --- */
#define ID_BTN_LIST       5001
#define ID_BTN_REPORT     5002
#define ID_BTN_ADD        5003
#define ID_BTN_DELETE     5004
#define ID_BTN_EXPORT     5005
#define ID_SEARCH_EDIT    106

/* --- GLOBALS --- */
Student students[MAX_STUDENTS];
int studentCount = 0;
HWND hListView, hStatusBar, hPanelList, hSearchEdit;
HFONT hFontViet, hFontTitle, hFontBold;
int activePage = 0; 
int sidebarWidth = 240;

COLORREF BAR_COLORS[] = {
    RGB(99, 102, 241), RGB(34, 197, 94), RGB(59, 130, 246), RGB(245, 158, 11), RGB(239, 68, 68)
};
const wchar_t *RANK_LABELS[] = { L"Xuất Sắc", L"Giỏi", L"Khá", L"Trung Bình", L"Yếu" };

/* --- HELPERS --- */
void AToW(const char* a, wchar_t* w, int n) { MultiByteToWideChar(CP_UTF8,0,a,-1,w,n); }
BOOL WcsContains(const wchar_t* str, const wchar_t* key) {
    if(!str || !key) return FALSE;
    wchar_t *s = _wcsdup(str), *k = _wcsdup(key);
    CharLowerW(s); CharLowerW(k);
    BOOL f = (wcsstr(s,k)!=NULL); free(s); free(k); return f;
}
static void LVSet(int row, int sub, const wchar_t* text) {
    LVITEMW si = {0}; si.mask = LVIF_TEXT; si.iItem = row; si.iSubItem = sub; si.pszText = (wchar_t*)text;
    SendMessageW(hListView, LVM_SETITEMTEXTW, row, (LPARAM)&si);
}
wchar_t* FormatNumber(long n) {
    static wchar_t buf[32]; wchar_t tmp[32]; swprintf(tmp, 32, L"%ld", n);
    int len = wcslen(tmp), out = 0;
    for (int i=0; i<len; i++) {
        if (i > 0 && (len-i) % 3 == 0) buf[out++] = L'.';
        buf[out++] = tmp[i];
    }
    buf[out] = L'\0'; return buf;
}

/* --- DRAWING --- */
void DrawSidebarButton(HDC hdc, int x, int y, int w, int h, const wchar_t *label, BOOL isActive) {
    RECT rc = {x+12, y+2, x+w-12, y+h-2};
    if (isActive) {
        HBRUSH br = CreateSolidBrush(CLR_ACCENT); FillRect(hdc, &rc, br); DeleteObject(br);
        SetTextColor(hdc, CLR_WHITE);
    } else { SetTextColor(hdc, CLR_TEXT_MUTED); }
    SelectObject(hdc, hFontViet); SetBkMode(hdc, TRANSPARENT);
    RECT txRc = {rc.left+15, rc.top, rc.right-5, rc.bottom};
    DrawTextW(hdc, label, -1, &txRc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
}

void DrawSidebar(HDC hdc, int h) {
    RECT rcS = {0,0,sidebarWidth,h}; HBRUSH bg = CreateSolidBrush(CLR_SIDEBAR_BG); FillRect(hdc,&rcS,bg); DeleteObject(bg);
    SelectObject(hdc, hFontTitle); SetTextColor(hdc, CLR_WHITE);
    RECT rcL = {0,25,sidebarWidth,75}; DrawTextW(hdc, L"🎓 SV Manager", -1, &rcL, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    HPEN p = CreatePen(PS_SOLID, 1, RGB(30,41,59)); SelectObject(hdc, p); MoveToEx(hdc,20,85,NULL); LineTo(hdc,sidebarWidth-20,85); DeleteObject(p);
    DrawSidebarButton(hdc, 0, 100, sidebarWidth, 45, L"📋  Danh Sách SV", activePage==0);
    DrawSidebarButton(hdc, 0, 150, sidebarWidth, 45, L"📊  Báo Cáo",       activePage==1);
    DrawSidebarButton(hdc, 0, 230, sidebarWidth, 45, L"➕  Thêm Mới",      FALSE);
    DrawSidebarButton(hdc, 0, 280, sidebarWidth, 45, L"👁️  Xem Chi Tiết",   FALSE);
    DrawSidebarButton(hdc, 0, 330, sidebarWidth, 45, L"🗑️  Xóa Chọn",      FALSE);
    DrawSidebarButton(hdc, 0, 380, sidebarWidth, 45, L"📥  Xuất CSV",      FALSE);
}

void DrawInfoCard(HDC hdc, int x, int y, int w, int h, const wchar_t *title, const wchar_t* val, const wchar_t *unit, COLORREF accent, COLORREF bg) {
    HBRUSH bgBr = CreateSolidBrush(bg); RECT rcC = {x, y, x+w, y+h}; FillRect(hdc, &rcC, bgBr); DeleteObject(bgBr);
    HBRUSH acBr = CreateSolidBrush(accent); RECT rcB = {x, y, x+4, y+h}; FillRect(hdc, &rcB, acBr); DeleteObject(acBr);
    HPEN bo = CreatePen(PS_SOLID, 1, CLR_BORDER); SelectObject(hdc, bo); SelectObject(hdc, GetStockObject(NULL_BRUSH)); Rectangle(hdc,x,y,x+w,y+h); DeleteObject(bo);
    SetTextColor(hdc, CLR_TEXT_MUTED); SelectObject(hdc, hFontViet); SetBkMode(hdc, TRANSPARENT);
    RECT rt = {x+15, y+15, x+w-5, y+35}; DrawTextW(hdc, title, -1, &rt, DT_LEFT);
    SetTextColor(hdc, accent); SelectObject(hdc, hFontTitle);
    RECT rv = {x+15, y+40, x+w-5, y+75}; DrawTextW(hdc, val, -1, &rv, DT_LEFT);
    SetTextColor(hdc, CLR_TEXT_MUTED); SelectObject(hdc, hFontViet);
    RECT ru = {x+15, y+75, x+w-5, y+95}; DrawTextW(hdc, unit, -1, &ru, DT_LEFT);
}

void DrawBarChart(HDC hdc, RECT rc) {
    int ctr[5] = {0}; for(int i=0; i<studentCount; i++) {
        if(wcscmp(students[i].rank, L"Xuất Sắc")==0) ctr[0]++; else if(wcscmp(students[i].rank, L"Giỏi")==0) ctr[1]++;
        else if(wcscmp(students[i].rank, L"Khá")==0) ctr[2]++; else if(wcscmp(students[i].rank, L"Trung Bình")==0) ctr[3]++; else ctr[4]++;
    }
    int maxV = 1; for(int i=0; i<5; i++) if(ctr[i]>maxV) maxV=ctr[i];
    int cX = rc.left + 70, cY = rc.top + 40, cW = rc.right-rc.left-100, cH = rc.bottom-rc.top-100;
    HBRUSH wh = CreateSolidBrush(CLR_WHITE); RECT rcB={cX,cY,cX+cW,cY+cH}; FillRect(hdc,&rcB,wh); DeleteObject(wh);
    HPEN gP = CreatePen(PS_DOT, 1, CLR_BORDER); SelectObject(hdc, gP);
    for(int i=0; i<=4; i++) { int gy = cY+cH-(cH*i/4); MoveToEx(hdc,cX,gy,NULL); LineTo(hdc,cX+cW,gy); } DeleteObject(gP);

    int bW = cW / 8, bG = bW;
    for(int i=0; i<5; i++) {
        int bh = (ctr[i]*cH*0.8)/maxV; int bx = cX + bG + i*(bW+bG/2); int by = cY+cH-bh;
        HBRUSH br = CreateSolidBrush(BAR_COLORS[i]); RECT r={bx,by,bx+bW,cY+cH}; FillRect(hdc,&r,br); DeleteObject(br);
        SetTextColor(hdc, BAR_COLORS[i]); SelectObject(hdc, hFontBold); 
        wchar_t v[10]; swprintf(v,10,L"%d",ctr[i]); RECT rn = {bx, by-22, bx+bW, by-2}; DrawTextW(hdc,v,-1,&rn,DT_CENTER);
        SetTextColor(hdc, RGB(71,85,105)); SelectObject(hdc, hFontViet);
        RECT rl = {bx-20, cY+cH+10, bx+bW+20, cY+cH+30}; DrawTextW(hdc,RANK_LABELS[i],-1,&rl,DT_CENTER);
    }
    /* Legend */
    int lx = cX, ly = cY + cH + 50;
    for(int i=0; i<5; i++) {
        HBRUSH br = CreateSolidBrush(BAR_COLORS[i]); RECT rd = {lx + i*130, ly, lx + i*130 + 12, ly + 12}; FillRect(hdc, &rd, br); DeleteObject(br);
        RECT rl = {lx + i*130 + 16, ly - 2, lx + i*130 + 130, ly + 14}; DrawTextW(hdc, RANK_LABELS[i], -1, &rl, DT_LEFT);
    }
}

void DrawReport(HDC hdc, int w, int h) {
    RECT rcMain = {sidebarWidth, 0, w, h}; HBRUSH bg = CreateSolidBrush(RGB(241,245,249)); FillRect(hdc,&rcMain,bg); DeleteObject(bg);
    SelectObject(hdc, hFontTitle); SetTextColor(hdc, CLR_SIDEBAR_BG); SetBkMode(hdc,TRANSPARENT);
    TextOutW(hdc, sidebarWidth+30, 30, L"📊 Thống Kê & Phân Tích Học Lực", 31);
    SYSTEMTIME st; GetLocalTime(&st); wchar_t ds[100]; swprintf(ds,100,L"Cập nhật lúc %02d:%02d — %02d/%02d/%04d", st.wHour, st.wMinute, st.wDay, st.wMonth, st.wYear);
    SelectObject(hdc, hFontViet); SetTextColor(hdc, CLR_TEXT_MUTED); TextOutW(hdc, sidebarWidth+30, 65, ds, wcslen(ds));
    int cW = (w - sidebarWidth - 80) / 4; long rev = 0; float gpa = 0; int sch = 0;
    for(int i=0; i<studentCount; i++) { rev += students[i].tuition_total; gpa += students[i].gpa_4_avg; if(students[i].tuition_scholarship>0) sch++; }
    if(studentCount>0) gpa /= studentCount;
    DrawInfoCard(hdc, sidebarWidth+30, 100, cW, 110, L"👥 Tổng Sinh Viên", FormatNumber(studentCount), L"sinh viên", CLR_ACCENT, RGB(239,246,255));
    DrawInfoCard(hdc, sidebarWidth+30+(cW+20), 100, cW, 110, L"💰 Tổng Học Phí", FormatNumber(rev), L"VND", CLR_SUCCESS, RGB(240,253,244));
    wchar_t gS[20]; swprintf(gS,20,L"%.2f", gpa);
    DrawInfoCard(hdc, sidebarWidth+30+(cW+20)*2, 100, cW, 110, L"🎓 GPA Trung Bình", gS, L"thang điểm 4.0", CLR_WARNING, RGB(255,251,235));
    DrawInfoCard(hdc, sidebarWidth+30+(cW+20)*3, 100, cW, 110, L"🏆 Học Bổng", FormatNumber(sch), L"sinh viên", RGB(139,92,246), RGB(245,243,255));
    RECT rcChart = {sidebarWidth+30, 240, w-30, h-80}; DrawBarChart(hdc, rcChart);
}

void UpdateStatusBar(void) {
    long rev = 0; int sch = 0;
    for (int i=0; i<studentCount; i++) { rev += students[i].tuition_total; if(students[i].tuition_scholarship > 0) sch++; }
    wchar_t p1[80], p2[80], p3[80];
    swprintf(p1, 80, L"  👥 Tổng: %d sinh viên", studentCount);
    swprintf(p2, 80, L"  💰 Tổng thu: %ls VNĐ", FormatNumber(rev));
    swprintf(p3, 80, L"  🏆 Học bổng: %d SV", sch);
    SendMessageW(hStatusBar, SB_SETTEXTW, 0, (LPARAM)p1);
    SendMessageW(hStatusBar, SB_SETTEXTW, 1, (LPARAM)p2);
    SendMessageW(hStatusBar, SB_SETTEXTW, 2, (LPARAM)p3);
}

void RefreshListView(const wchar_t* filter) {
    SendMessage(hListView, LVM_DELETEALLITEMS, 0, 0);
    for (int i = 0; i < studentCount; i++) {
        if (filter && !WcsContains(students[i].name, filter)) {
            wchar_t idb[20]; swprintf(idb,20,L"%d",students[i].id); if (!WcsContains(idb, filter)) continue;
        }
        LVITEMW lvI = {0}; lvI.mask = LVIF_TEXT|LVIF_PARAM; lvI.iItem=0; lvI.lParam=i; lvI.pszText=(wchar_t*)L"";
        int idx = SendMessageW(hListView, LVM_INSERTITEMW, 0, (LPARAM)&lvI);
        wchar_t b[200]; swprintf(b,20,L"%d",students[i].id); LVSet(idx,0,b); LVSet(idx,1,students[i].name); LVSet(idx,2,students[i].class_name);
        swprintf(b,10,L"HK%d",students[i].semester); LVSet(idx,3,b); swprintf(b,10,L"%d",students[i].total_credits); LVSet(idx,4,b);
        swprintf(b,20,L"%.2f",students[i].gpa_4_avg); LVSet(idx,5,b); LVSet(idx,6,students[i].rank); swprintf(b,10,L"%d",students[i].fail_count); LVSet(idx,7,b);
        LVSet(idx,8,FormatNumber(students[i].tuition_base)); LVSet(idx,9,FormatNumber(students[i].tuition_scholarship));
        LVSet(idx,10,FormatNumber(students[i].tuition_total)); LVSet(idx,11,students[i].tuition_status?L"Đã đóng":L"Chưa đóng");
    }
    UpdateStatusBar();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {
        case WM_CREATE: {
            hFontViet = CreateFontW(17,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,0,0,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_SWISS,L"Segoe UI");
            hFontTitle = CreateFontW(24,0,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,0,0,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_SWISS,L"Segoe UI");
            hFontBold = CreateFontW(16,0,0,0,FW_SEMIBOLD,0,0,0,DEFAULT_CHARSET,0,0,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_SWISS,L"Segoe UI");
            hPanelList = CreateWindowW(L"STATIC", L"", WS_CHILD|WS_VISIBLE, sidebarWidth+20, 70, 1020, 650, hWnd, NULL, NULL, NULL);
            hListView = CreateWindowExW(0, WC_LISTVIEWW, L"", WS_CHILD|WS_VISIBLE|LVS_REPORT|LVS_SINGLESEL, 0, 0, 1020, 650, hPanelList, NULL, NULL, NULL);
            SendMessage(hListView, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_DOUBLEBUFFER);
            SendMessage(hListView, WM_SETFONT, (WPARAM)hFontViet, TRUE);
            const wchar_t* head[] = {L"ID", L"Họ Tên", L"Lớp", L"HK", L"TC", L"GPA", L"Loại", L"Nợ", L"Gốc", L"Giảm", L"Tổng", L"Phí"};
            int ws[] = {50, 160, 90, 50, 40, 50, 90, 40, 90, 90, 100, 90};
            for(int i=0; i<12; i++) { LVCOLUMNW lc={LVCF_TEXT|LVCF_WIDTH|LVCF_SUBITEM, 0, ws[i], (wchar_t*)head[i], 0, i}; SendMessageW(hListView, LVM_INSERTCOLUMNW, i, (LPARAM)&lc); }
            hStatusBar = CreateWindowExW(0, STATUSCLASSNAMEW, NULL, WS_CHILD|WS_VISIBLE, 0,0,0,0, hWnd, NULL, NULL, NULL);
            int pts[] = {250, 650, -1}; SendMessage(hStatusBar, SB_SETPARTS, 3, (LPARAM)pts); SendMessage(hStatusBar, WM_SETFONT, (WPARAM)hFontBold, TRUE);
            hSearchEdit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD|WS_VISIBLE, sidebarWidth+20, 25, 350, 32, hWnd, (HMENU)ID_SEARCH_EDIT, NULL, NULL);
            SendMessage(hSearchEdit, WM_SETFONT, (WPARAM)hFontViet, TRUE); SendMessageW(hSearchEdit, EM_SETCUEBANNER, FALSE, (LPARAM)L" 🔍 Tìm kiếm sinh viên...");
            load_from_file(students, &studentCount); RefreshListView(NULL); break;
        }
        case WM_ERASEBKGND: return 1;
        case WM_PAINT: {
            PAINTSTRUCT ps; HDC hdc = BeginPaint(hWnd, &ps); RECT rc; GetClientRect(hWnd, &rc);
            HDC mDC = CreateCompatibleDC(hdc); HBITMAP mBM = CreateCompatibleBitmap(hdc, rc.right, rc.bottom); SelectObject(mDC, mBM);
            HBRUSH wh = CreateSolidBrush(CLR_WHITE); FillRect(mDC, &rc, wh); DeleteObject(wh);
            DrawSidebar(mDC, rc.bottom); if (activePage == 1) DrawReport(mDC, rc.right, rc.bottom);
            BitBlt(hdc, 0, 0, rc.right, rc.bottom, mDC, 0, 0, SRCCOPY); DeleteObject(mBM); DeleteDC(mDC); EndPaint(hWnd, &ps); break;
        }
        case WM_LBUTTONDOWN: {
            int x = LOWORD(lParam), y = HIWORD(lParam);
            if (x < sidebarWidth) {
                if (y >= 100 && y < 145) { activePage = 0; ShowWindow(hPanelList, SW_SHOW); ShowWindow(hSearchEdit, SW_SHOW); }
                else if (y >= 150 && y < 195) { activePage = 1; ShowWindow(hPanelList, SW_HIDE); ShowWindow(hSearchEdit, SW_HIDE); }
                else if (y >= 230 && y < 275) { Student ns; if (ShowStudentDialog(hWnd, &ns, FALSE, GetModuleHandle(NULL))) { if (studentCount < MAX_STUDENTS) { students[studentCount++] = ns; RefreshListView(NULL); save_to_file(students, studentCount); } } }
                else if (y >= 280 && y < 325) { int i = SendMessage(hListView, LVM_GETNEXTITEM, -1, LVNI_SELECTED); if (i != -1) { LVITEMW lv={0}; lv.iItem=i; lv.mask=LVIF_PARAM; SendMessageW(hListView, LVM_GETITEMW, i, (LPARAM)&lv); ShowStudentDetailDialog(hWnd, &students[lv.lParam], GetModuleHandle(NULL)); RefreshListView(NULL); save_to_file(students, studentCount); } }
                else if (y >= 330 && y < 375) { int i = SendMessage(hListView, LVM_GETNEXTITEM, -1, LVNI_SELECTED); if (i != -1) { LVITEMW lv={0}; lv.iItem=i; lv.mask=LVIF_PARAM; SendMessageW(hListView, LVM_GETITEMW, i, (LPARAM)&lv); for(int j=lv.lParam; j<studentCount-1; j++) students[j] = students[j+1]; studentCount--; RefreshListView(NULL); save_to_file(students, studentCount); } }
                else if (y >= 380 && y < 425) { 
                    OPENFILENAMEW ofn = {0}; wchar_t szF[260] = L"bao_cao.csv"; ofn.lStructSize=sizeof(ofn); ofn.hwndOwner=hWnd; ofn.lpstrFile=szF; ofn.nMaxFile=260; ofn.lpstrFilter=L"CSV\0*.csv\0"; ofn.Flags=OFN_OVERWRITEPROMPT;
                    if (GetSaveFileNameW(&ofn)) { FILE *f = _wfopen(szF, L"w"); if(f) { fwprintf(f, L"\xFEFFID,Họ Tên,Lớp,HK,Tín Chỉ,GPA,Loại,Cần Đóng,Trạng Thái\n"); for (int i=0; i<studentCount; i++) fwprintf(f, L"%d,%ls,%ls,%d,%d,%.2f,%ls,%.0f,%ls\n", students[i].id, students[i].name, students[i].class_name, students[i].semester, students[i].total_credits, students[i].gpa_4_avg, students[i].rank, students[i].tuition_total, students[i].tuition_status?L"Đã đóng":L"Chưa đóng"); fclose(f); MessageBoxW(hWnd, L"Xuất CSV thành công!", L"Thông báo", MB_OK); } }
                }
                InvalidateRect(hWnd, NULL, TRUE);
            }
            break;
        }
        case WM_MOUSEMOVE: if (LOWORD(lParam) < sidebarWidth) SetCursor(LoadCursor(NULL, IDC_HAND)); break;
        case WM_NOTIFY: {
            NMHDR* nm = (NMHDR*)lParam;
            if (nm->hwndFrom == hListView && nm->code == NM_DBLCLK) {
                int i = SendMessage(hListView, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
                if (i != -1) {
                    LVITEMW lv={0}; lv.iItem=i; lv.mask=LVIF_PARAM; SendMessageW(hListView, LVM_GETITEMW, i, (LPARAM)&lv);
                    ShowStudentDetailDialog(hWnd, &students[lv.lParam], GetModuleHandle(NULL));
                    RefreshListView(NULL); save_to_file(students, studentCount);
                }
            }
            if (nm->hwndFrom == hListView && nm->code == NM_CUSTOMDRAW) {
                LPNMLVCUSTOMDRAW cd = (LPNMLVCUSTOMDRAW)lParam;
                if (cd->nmcd.dwDrawStage == CDDS_PREPAINT) return CDRF_NOTIFYITEMDRAW;
                if (cd->nmcd.dwDrawStage == CDDS_ITEMPREPAINT) {
                    int r = (int)cd->nmcd.lItemlParam;
                    if (students[r].gpa_4_avg < 1.0f) cd->clrTextBk = RGB(255,230,230);
                    else if (students[r].gpa_4_avg >= 3.6f) cd->clrTextBk = RGB(230,255,230);
                    return CDRF_DODEFAULT;
                }
            }
            break;
        }
        case WM_COMMAND: if (HIWORD(wParam) == EN_CHANGE && LOWORD(wParam) == ID_SEARCH_EDIT) { wchar_t b[100]; GetWindowTextW(hSearchEdit, b, 100); RefreshListView(b); } break;
        case WM_DESTROY: PostQuitMessage(0); break;
        default: return DefWindowProcW(hWnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow) {
    (void)hPrev; (void)lpCmd; (void)nShow;
    SetConsoleOutputCP(65001); SetConsoleCP(65001);
    InitCommonControls();
    WNDCLASSW wc = {0}; wc.lpfnWndProc = WndProc; wc.hInstance = hInst; wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1); wc.lpszClassName = L"MainCls";
    RegisterClassW(&wc);
    HWND hWnd = CreateWindowW(L"MainCls", L"🎓 Hệ Thống Quản Lý Sinh Viên & Tài Chính  •  Pro v2.5", WS_OVERLAPPEDWINDOW|WS_VISIBLE, (GetSystemMetrics(SM_CXSCREEN)-1280)/2, (GetSystemMetrics(SM_CYSCREEN)-800)/2, 1280, 800, NULL, NULL, hInst, NULL);
    MSG msg; while(GetMessageW(&msg, NULL, 0, 0)) { TranslateMessage(&msg); DispatchMessageW(&msg); }
    return (int)msg.wParam;
}
