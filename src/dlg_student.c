#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include "student.h"
#include "academic.h"
#include "dlg_student.h"

/* IDs for Dialog Controls */
#define IDC_TAB             2001
#define IDC_PANEL_INFO      2002
#define IDC_PANEL_SUBJ      2003
#define IDC_PANEL_FIN       2004

#define IDC_TXT_ID          2101
#define IDC_TXT_NAME        2102
#define IDC_TXT_DOB         2103
#define IDC_CMB_GENDER      2104
#define IDC_CMB_CLASS       2105
#define IDC_CMB_YEAR        2106
#define IDC_TXT_PHONE       2107
#define IDC_TXT_EMAIL       2108

#define IDC_BTN_ADD_SUBJ    2201
#define IDC_BTN_DEL_SUBJ    2202
#define IDC_LBL_CREDITS     2203

#define IDC_FIN_TEXT        2301
#define IDC_CMB_PAY_STS     2302
#define IDC_TXT_PAY_DATE    2303
#define IDC_FIN_STATUS_CB   2304

#define IDC_BTN_SAVE        IDOK
#define IDC_BTN_CANCEL      IDCANCEL

/* Global variables for dialog state */
static HWND hPanelInfo, hPanelSubj, hPanelFin, hTab, hFinStatusCb;
static Student dlgStudent;
static BOOL    dlgSaved = FALSE;
static HWND    hSubjEdit[MAX_SUBJECTS][3]; 
static HWND    hGpaLabel[MAX_SUBJECTS];
static HWND    hSubjCredits[MAX_SUBJECTS];
static HINSTANCE hDlgInst;
static WNDPROC OldPanelProc;
static HFONT hFontGlobal = NULL;

/* Forward declarations */
static void CreateInfoPanel(HWND hDlg);
static void CreateSubjPanel(HWND hDlg);
static void CreateFinPanel(HWND hDlg);
static void SwitchTab(int idx);
static BOOL UpdateSubjRow(int row);
static void AddSubjectRow(HWND hDlg, int row);
static void RefreshFinPanel(HWND hDlg);
static BOOL SaveAndValidate(HWND hDlg);
static LRESULT CALLBACK PanelSubjProc(HWND, UINT, WPARAM, LPARAM);

/* Utility to set Unicode font to every control */
static BOOL CALLBACK SetFontProc(HWND hCtrl, LPARAM lParam) {
    SendMessageW(hCtrl, WM_SETFONT, (WPARAM)lParam, TRUE);
    /* For ComboBoxes, ensure item height is sufficient for Unicode */
    wchar_t cls[64]; GetClassNameW(hCtrl, cls, 64);
    if (wcscmp(cls, WC_COMBOBOXW) == 0) {
        SendMessageW(hCtrl, CB_SETITEMHEIGHT, (WPARAM)-1, 22);
        SendMessageW(hCtrl, CB_SETITEMHEIGHT, (WPARAM)0, 20);
    }
    return TRUE;
}

static void CreateInfoPanel(HWND hDlg) {
    (void)hDlg;
    HWND h;
    CreateWindowW(L"STATIC", L"Mã sinh viên:", WS_CHILD|WS_VISIBLE, 15, 18, 100, 22, hPanelInfo, NULL, hDlgInst, NULL);
    h = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD|WS_VISIBLE, 120, 16, 120, 26, hPanelInfo, (HMENU)IDC_TXT_ID, hDlgInst, NULL);
    if (dlgStudent.id != 0) { wchar_t buf[20]; swprintf(buf, 20, L"%d", dlgStudent.id); SetWindowTextW(h, buf); }

    CreateWindowW(L"STATIC", L"Họ và tên:", WS_CHILD|WS_VISIBLE, 15, 53, 100, 22, hPanelInfo, NULL, hDlgInst, NULL);
    h = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", dlgStudent.name, WS_CHILD|WS_VISIBLE, 120, 51, 300, 26, hPanelInfo, (HMENU)IDC_TXT_NAME, hDlgInst, NULL);

    CreateWindowW(L"STATIC", L"Ngày sinh:", WS_CHILD|WS_VISIBLE, 15, 88, 100, 22, hPanelInfo, NULL, hDlgInst, NULL);
    h = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", dlgStudent.birth_date, WS_CHILD|WS_VISIBLE, 120, 86, 120, 26, hPanelInfo, (HMENU)IDC_TXT_DOB, hDlgInst, NULL);

    CreateWindowW(L"STATIC", L"Giới tính:", WS_CHILD|WS_VISIBLE, 255, 88, 70, 22, hPanelInfo, NULL, hDlgInst, NULL);
    h = CreateWindowW(WC_COMBOBOXW, L"", WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST, 330, 86, 90, 200, hPanelInfo, (HMENU)IDC_CMB_GENDER, hDlgInst, NULL);
    SendMessageW(h, CB_ADDSTRING, 0, (LPARAM)L"Nam"); SendMessageW(h, CB_ADDSTRING, 0, (LPARAM)L"Nữ");
    SendMessage(h, CB_SETCURSEL, (wcscmp(dlgStudent.gender, L"Nữ")==0 ? 1 : 0), 0);

    CreateWindowW(L"STATIC", L"Lớp học:", WS_CHILD|WS_VISIBLE, 15, 123, 100, 22, hPanelInfo, NULL, hDlgInst, NULL);
    h = CreateWindowW(WC_COMBOBOXW, L"", WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST, 120, 121, 200, 200, hPanelInfo, (HMENU)IDC_CMB_CLASS, hDlgInst, NULL);
    for (int i=0; i<DEFAULT_CLASS_COUNT; i++) SendMessageW(h, CB_ADDSTRING, 0, (LPARAM)DEFAULT_CLASSES[i].name);
    SendMessage(h, CB_SETCURSEL, 0, 0);

    CreateWindowW(L"STATIC", L"Học kỳ:", WS_CHILD|WS_VISIBLE, 15, 158, 100, 22, hPanelInfo, NULL, hDlgInst, NULL);
    h = CreateWindowW(WC_COMBOBOXW, L"", WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST, 120, 156, 120, 150, hPanelInfo, (HMENU)IDC_CMB_YEAR, hDlgInst, NULL);
    for (int i=1; i<=8; i++) { wchar_t b[10]; swprintf(b,10,L"HK %d",i); SendMessageW(h, CB_ADDSTRING, 0, (LPARAM)b); }
    SendMessage(h, CB_SETCURSEL, (dlgStudent.semester > 0 ? dlgStudent.semester-1 : 0), 0);

    CreateWindowW(L"STATIC", L"Điện thoại:", WS_CHILD|WS_VISIBLE, 15, 193, 100, 22, hPanelInfo, NULL, hDlgInst, NULL);
    h = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", dlgStudent.phone, WS_CHILD|WS_VISIBLE, 120, 191, 140, 26, hPanelInfo, (HMENU)IDC_TXT_PHONE, hDlgInst, NULL);

    CreateWindowW(L"STATIC", L"Email:", WS_CHILD|WS_VISIBLE, 15, 228, 100, 22, hPanelInfo, NULL, hDlgInst, NULL);
    h = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", dlgStudent.email, WS_CHILD|WS_VISIBLE, 120, 226, 300, 26, hPanelInfo, (HMENU)IDC_TXT_EMAIL, hDlgInst, NULL);
}

static void CreateSubjPanel(HWND hDlg) {
    (void)hDlg;
    HWND h;
    h = CreateWindowW(L"BUTTON", L"➕ Thêm môn", WS_CHILD|WS_VISIBLE, 10, 5, 110, 28, hPanelSubj, (HMENU)IDC_BTN_ADD_SUBJ, hDlgInst, NULL);
    h = CreateWindowW(L"BUTTON", L"🗑️ Xóa môn",  WS_CHILD|WS_VISIBLE, 130, 5, 110, 28, hPanelSubj, (HMENU)IDC_BTN_DEL_SUBJ, hDlgInst, NULL);
    h = CreateWindowW(L"STATIC", L"Tổng tín chỉ: 0", WS_CHILD|WS_VISIBLE, 260, 10, 200, 22, hPanelSubj, (HMENU)IDC_LBL_CREDITS, hDlgInst, NULL);

    const wchar_t* hdrs[] = {L"Môn học", L"TC", L"Quá trình", L"Giữa kỳ", L"Cuối kỳ", L"GPA"};
    int x[] = {10, 175, 225, 295, 365, 440};
    for (int i=0; i<6; i++) { CreateWindowW(L"STATIC", hdrs[i], WS_CHILD|WS_VISIBLE, x[i], 42, 145, 22, hPanelSubj, NULL, hDlgInst, NULL); }
}

static void AddSubjectRow(HWND hDlg, int row) {
    (void)hDlg;
    int y = 65 + row * 32;
    HWND hCb = CreateWindowW(WC_COMBOBOXW, L"", WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST, 10, y, 160, 300, hPanelSubj, (HMENU)(UINT_PTR)(4100+row), hDlgInst, NULL);
    for (int i=0; i<DEFAULT_SUBJECT_COUNT; i++) SendMessageW(hCb, CB_ADDSTRING, 0, (LPARAM)DEFAULT_SUBJECTS[i].name);
    
    int selIdx = -1;
    for (int i=0; i<DEFAULT_SUBJECT_COUNT; i++) if (wcscmp(dlgStudent.subjects[row].name, DEFAULT_SUBJECTS[i].name)==0) { selIdx=i; break; }
    SendMessage(hCb, CB_SETCURSEL, (selIdx != -1 ? selIdx : 0), 0);

    wchar_t cr[10]; swprintf(cr, 10, L"%d", dlgStudent.subjects[row].credits);
    hSubjCredits[row] = CreateWindowW(L"STATIC", cr, WS_CHILD|WS_VISIBLE, 175, y+4, 45, 22, hPanelSubj, NULL, hDlgInst, NULL);

    int ex[] = {225, 295, 365};
    for (int c=0; c<3; c++) {
        float val = (c==0?dlgStudent.subjects[row].score_process : (c==1?dlgStudent.subjects[row].score_midterm : dlgStudent.subjects[row].score_final));
        wchar_t sv[10]; swprintf(sv,10,L"%.1f", val);
        hSubjEdit[row][c] = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", sv, WS_CHILD|WS_VISIBLE, ex[c], y, 65, 26, hPanelSubj, (HMENU)(UINT_PTR)(3100+row*10+c), hDlgInst, NULL);
    }
    hGpaLabel[row] = CreateWindowW(L"STATIC", dlgStudent.subjects[row].grade, WS_CHILD|WS_VISIBLE, 440, y+4, 60, 22, hPanelSubj, NULL, hDlgInst, NULL);
    
    /* Apply font and height to new controls */
    EnumChildWindows(hPanelSubj, (WNDENUMPROC)SetFontProc, (LPARAM)hFontGlobal);
}

static BOOL UpdateSubjRow(int row) {
    wchar_t buf[20];
    float sp, sm, sf;
    GetWindowTextW(hSubjEdit[row][0], buf, 20); sp = (float)_wtof(buf);
    GetWindowTextW(hSubjEdit[row][1], buf, 20); sm = (float)_wtof(buf);
    GetWindowTextW(hSubjEdit[row][2], buf, 20); sf = (float)_wtof(buf);
    
    if (sp < 0.0f || sp > 10.0f || sm < 0.0f || sm > 10.0f || sf < 0.0f || sf > 10.0f) {
        MessageBoxW(GetParent(hPanelSubj), L"Lỗi: Điểm phải nằm trong khoảng từ 0 đến 10!", L"Dữ liệu không hợp lệ", MB_OK|MB_ICONERROR);
        return FALSE;
    }

    dlgStudent.subjects[row].score_process = sp;
    dlgStudent.subjects[row].score_midterm = sm;
    dlgStudent.subjects[row].score_final = sf;
    calculate_subject_gpa(&dlgStudent.subjects[row]);
    SetWindowTextW(hGpaLabel[row], dlgStudent.subjects[row].grade);
    return TRUE;
}

static void CreateFinPanel(HWND hDlg) {
    (void)hDlg;
    CreateWindowW(L"STATIC", L"", WS_CHILD|WS_VISIBLE, 10, 10, 500, 200, hPanelFin, (HMENU)IDC_FIN_TEXT, hDlgInst, NULL);
    CreateWindowW(L"STATIC", L"Trạng thái đóng tiền:", WS_CHILD|WS_VISIBLE, 10, 220, 150, 22, hPanelFin, NULL, hDlgInst, NULL);
    hFinStatusCb = CreateWindowW(WC_COMBOBOXW, L"", WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST, 170, 218, 150, 100, hPanelFin, (HMENU)IDC_FIN_STATUS_CB, hDlgInst, NULL);
    SendMessageW(hFinStatusCb, CB_ADDSTRING, 0, (LPARAM)L"Chưa đóng");
    SendMessageW(hFinStatusCb, CB_ADDSTRING, 0, (LPARAM)L"Đã đóng");
    SendMessage(hFinStatusCb, CB_SETCURSEL, dlgStudent.tuition_status, 0);
    RefreshFinPanel(NULL);
}

static void RefreshFinPanel(HWND hDlg) {
    (void)hDlg;
    calculate_student_gpa(&dlgStudent);
    calculate_full_tuition(&dlgStudent);
    wchar_t buf[512];
    swprintf(buf, 512, L"TỔNG HỢP CHI TIẾT TÀI CHÍNH:\n\n- GPA: %.2f (%ls)\n- Học phí gốc: %.0f VND\n- Học bổng giảm trừ: %.0f VND\n- Tiền phạt nợ muộn: %.0f VND\n- TỔNG PHẢI ĐÓNG: %.0f VND", 
        dlgStudent.gpa_4_avg, dlgStudent.rank, dlgStudent.tuition_base, dlgStudent.tuition_scholarship, dlgStudent.tuition_penalty, dlgStudent.tuition_total);
    SetWindowTextW(GetDlgItem(hPanelFin, IDC_FIN_TEXT), buf);
}

static LRESULT CALLBACK PanelSubjProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_COMMAND) {
        if (HIWORD(wParam) == CBN_SELCHANGE) {
            int row = LOWORD(wParam) - 4100;
            if (row >=0 && row < dlgStudent.subject_count) {
                int sel = SendMessage((HWND)lParam, CB_GETCURSEL,0,0);
                if (sel != CB_ERR) {
                    wcscpy(dlgStudent.subjects[row].name, DEFAULT_SUBJECTS[sel].name);
                    dlgStudent.subjects[row].credits = DEFAULT_SUBJECTS[sel].credits;
                    wchar_t cr[10]; swprintf(cr, 10, L"%d", dlgStudent.subjects[row].credits);
                    SetWindowTextW(hSubjCredits[row], cr);
                }
            }
        }
        SendMessage(GetParent(hWnd), msg, wParam, lParam);
    }
    return CallWindowProc(OldPanelProc, hWnd, msg, wParam, lParam);
}

static void SwitchTab(int idx) {
    ShowWindow(hPanelInfo, idx==0?SW_SHOW:SW_HIDE);
    ShowWindow(hPanelSubj, idx==1?SW_SHOW:SW_HIDE);
    ShowWindow(hPanelFin, idx==2?SW_SHOW:SW_HIDE);
    if (idx==2) RefreshFinPanel(NULL);
}

static BOOL SaveAndValidate(HWND hDlg) {
    wchar_t id_s[20], name_s[100];
    GetWindowTextW(GetDlgItem(hPanelInfo, IDC_TXT_ID), id_s, 20); dlgStudent.id = _wtoi(id_s);
    GetWindowTextW(GetDlgItem(hPanelInfo, IDC_TXT_NAME), name_s, 100); wcscpy(dlgStudent.name, name_s);
    GetWindowTextW(GetDlgItem(hPanelInfo, IDC_TXT_DOB), dlgStudent.birth_date, 20);
    GetWindowTextW(GetDlgItem(hPanelInfo, IDC_TXT_PHONE), dlgStudent.phone, 20);
    GetWindowTextW(GetDlgItem(hPanelInfo, IDC_TXT_EMAIL), dlgStudent.email, 100);
    
    int cls_idx = SendMessage(GetDlgItem(hPanelInfo, IDC_CMB_CLASS), CB_GETCURSEL, 0, 0);
    dlgStudent.class_id = DEFAULT_CLASSES[cls_idx].id;
    wcscpy(dlgStudent.class_name, DEFAULT_CLASSES[cls_idx].name);

    dlgStudent.semester = SendMessage(GetDlgItem(hPanelInfo, IDC_CMB_YEAR), CB_GETCURSEL, 0, 0) + 1;
    dlgStudent.tuition_status = SendMessage(hFinStatusCb, CB_GETCURSEL, 0, 0);

    for (int i=0; i<dlgStudent.subject_count; i++) {
        if (!UpdateSubjRow(i)) return FALSE;
    }
    
    calculate_student_gpa(&dlgStudent);
    calculate_full_tuition(&dlgStudent);
    
    dlgSaved = TRUE;
    return TRUE;
}

LRESULT CALLBACK StudentDlgWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            hFontGlobal = CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
            int W=660; 
            hTab = CreateWindowW(WC_TABCONTROLW, L"", WS_CHILD|WS_VISIBLE, 5, 5, W-10, 440, hWnd, (HMENU)IDC_TAB, hDlgInst, NULL);
            TCITEMW ti = {TCIF_TEXT, 0,0, L"📋 Thông Tin", 0,0}; SendMessageW(hTab, TCM_INSERTITEMW, 0, (LPARAM)&ti);
            ti.pszText = L"📚 Môn Học"; SendMessageW(hTab, TCM_INSERTITEMW, 1, (LPARAM)&ti);
            ti.pszText = L"💰 Tài Chính"; SendMessageW(hTab, TCM_INSERTITEMW, 2, (LPARAM)&ti);

            hPanelInfo = CreateWindowW(L"STATIC", L"", WS_CHILD|WS_VISIBLE, 12, 30, W-25, 405, hWnd, (HMENU)IDC_PANEL_INFO, hDlgInst, NULL);
            hPanelSubj = CreateWindowW(L"STATIC", L"", WS_CHILD, 12, 30, W-25, 405, hWnd, (HMENU)IDC_PANEL_SUBJ, hDlgInst, NULL);
            hPanelFin  = CreateWindowW(L"STATIC", L"", WS_CHILD, 12, 30, W-25, 405, hWnd, (HMENU)IDC_PANEL_FIN, hDlgInst, NULL);
            
            OldPanelProc = (WNDPROC)SetWindowLongPtrW(hPanelSubj, GWLP_WNDPROC, (LONG_PTR)PanelSubjProc);

            CreateInfoPanel(hWnd); CreateSubjPanel(hWnd); CreateFinPanel(hWnd);
            for (int i=0; i<dlgStudent.subject_count; i++) AddSubjectRow(hWnd, i);

            CreateWindowW(L"BUTTON", L"Hủy bỏ", WS_CHILD|WS_VISIBLE, W-180, 455, 80, 28, hWnd, (HMENU)IDC_BTN_CANCEL, hDlgInst, NULL);
            CreateWindowW(L"BUTTON", L"Lưu Sinh Viên", WS_CHILD|WS_VISIBLE, W-90, 455, 120, 28, hWnd, (HMENU)IDC_BTN_SAVE, hDlgInst, NULL);
            
            /* Apply Global Font to everything */
            EnumChildWindows(hWnd, (WNDENUMPROC)SetFontProc, (LPARAM)hFontGlobal);
            EnumChildWindows(hPanelInfo, (WNDENUMPROC)SetFontProc, (LPARAM)hFontGlobal);
            EnumChildWindows(hPanelSubj, (WNDENUMPROC)SetFontProc, (LPARAM)hFontGlobal);
            EnumChildWindows(hPanelFin, (WNDENUMPROC)SetFontProc, (LPARAM)hFontGlobal);
            break;
        }
        case WM_NOTIFY: {
            NMHDR* nm = (NMHDR*)lParam;
            if (nm->idFrom == IDC_TAB && nm->code == TCN_SELCHANGE) SwitchTab(SendMessage(hTab, TCM_GETCURSEL, 0, 0));
            break;
        }
        case WM_COMMAND: {
            switch (LOWORD(wParam)) {
                case IDC_BTN_ADD_SUBJ: {
                    if (dlgStudent.subject_count < MAX_SUBJECTS) {
                        int idx = dlgStudent.subject_count++;
                        dlgStudent.subjects[idx].credits = 3;
                        wcscpy(dlgStudent.subjects[idx].name, DEFAULT_SUBJECTS[0].name);
                        AddSubjectRow(hWnd, idx);
                        InvalidateRect(hPanelSubj, NULL, TRUE);
                    }
                    break;
                }
                case IDC_BTN_SAVE: if (SaveAndValidate(hWnd)) DestroyWindow(hWnd); break;
                case IDC_BTN_CANCEL: DestroyWindow(hWnd); break;
            }
            break;
        }
        case WM_DESTROY: if (hFontGlobal) DeleteObject(hFontGlobal); break;
        default: return DefWindowProcW(hWnd, msg, wParam, lParam);
    }
    return 0;
}

BOOL ShowStudentDialog(HWND hParent, Student* s, BOOL isEdit, HINSTANCE hInst) {
    hDlgInst = hInst; dlgSaved = FALSE;
    if (isEdit && s) dlgStudent = *s; else memset(&dlgStudent, 0, sizeof(Student));

    WNDCLASSW wc = {0}; wc.lpfnWndProc = StudentDlgWndProc; wc.hInstance = hInst; wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1); wc.lpszClassName = L"SvDlgClass";
    RegisterClassW(&wc);

    HWND hDlg = CreateWindowExW(WS_EX_DLGMODALFRAME, L"SvDlgClass", isEdit?L"Sửa Sinh Viên":L"Thêm Sinh Viên", WS_VISIBLE|WS_CAPTION|WS_SYSMENU, 100, 100, 680, 530, hParent, NULL, hInst, NULL);
    EnableWindow(hParent, FALSE);
    MSG m; while (IsWindow(hDlg) && GetMessageW(&m, NULL, 0, 0)) { TranslateMessage(&m); DispatchMessageW(&m); }
    EnableWindow(hParent, TRUE); SetForegroundWindow(hParent);
    if (dlgSaved && s) *s = dlgStudent;
    return dlgSaved;
}
