#include <windows.h>

#ifndef WS_EX_NOACTIVATE
#define WS_EX_NOACTIVATE 0x08000000L
#endif
#ifndef WM_MOUSEACTIVATE
#define WM_MOUSEACTIVATE 0x0021
#endif
#ifndef MA_NOACTIVATE
#define MA_NOACTIVATE 3
#endif
#ifndef DT_NOPREFIX
#define DT_NOPREFIX 0x00000800
#endif

#define GAP 3
#define N(a) (sizeof(a)/sizeof(a[0]))

/* SDKに無いことがあるので自前定義 */
#define VKX_SEMI   0xBA
#define VKX_PLUS   0xBB
#define VKX_COMMA  0xBC
#define VKX_MINUS  0xBD
#define VKX_PERIOD 0xBE
#define VKX_SLASH  0xBF
#define VKX_TILDE  0xC0
#define VKX_LBRACK 0xDB
#define VKX_BSLASH 0xDC
#define VKX_RBRACK 0xDD
#define VKX_QUOTE  0xDE
#define VKX_TOGGLE 0xFF   /* 内部用: &123 / ABC 切替 */

#define LY_STD  0
#define LY_SYM  1
#define LY_FULL 2

enum { TB_MODE, TB_FLOAT, TB_MINUS, TB_PLUS, TB_MIN, TB_N };

/* label, vk, 幅, Shift必要, フリックで出す数字(0=なし), Shift時の表示 */
typedef struct { const WCHAR *label; BYTE vk; BYTE w; BYTE shift; BYTE flick; const WCHAR *slabel; } KEY;
typedef struct { const KEY *k; int n; } ROW;
typedef struct { const ROW *rows; int nrows; int units; } LAYER;

#define K(l,v,w,s,f)  {l,v,w,s,f,0}
#define FK(l,s,v,w)   {l,v,w,0,0,s}
#define LT(l,v)       K(l,v,4,0,0)
#define SP            K(L"",0,2,0,0)

/* ================= 通常レイヤー ================= */
static const KEY a1[] = {
    K(L"q",'Q',4,0,'1'),K(L"w",'W',4,0,'2'),K(L"e",'E',4,0,'3'),K(L"r",'R',4,0,'4'),K(L"t",'T',4,0,'5'),
    K(L"y",'Y',4,0,'6'),K(L"u",'U',4,0,'7'),K(L"i",'I',4,0,'8'),K(L"o",'O',4,0,'9'),K(L"p",'P',4,0,'0')};
static const KEY a2[] = {
    SP,LT(L"a",'A'),LT(L"s",'S'),LT(L"d",'D'),LT(L"f",'F'),LT(L"g",'G'),
    LT(L"h",'H'),LT(L"j",'J'),LT(L"k",'K'),LT(L"l",'L'),SP};
static const KEY a3[] = {
    K(L"Shift",VK_SHIFT,6,0,0),LT(L"z",'Z'),LT(L"x",'X'),LT(L"c",'C'),LT(L"v",'V'),
    LT(L"b",'B'),LT(L"n",'N'),LT(L"m",'M'),K(L"BS",VK_BACK,6,0,0)};

/* ================= 記号レイヤー ================= */
static const KEY s1[] = {
    LT(L"1",'1'),LT(L"2",'2'),LT(L"3",'3'),LT(L"4",'4'),LT(L"5",'5'),
    LT(L"6",'6'),LT(L"7",'7'),LT(L"8",'8'),LT(L"9",'9'),LT(L"0",'0')};
static const KEY s2[] = {
    LT(L"-",VKX_MINUS),LT(L"/",VKX_SLASH),K(L":",VKX_SEMI,4,1,0),LT(L";",VKX_SEMI),
    K(L"(",'9',4,1,0),K(L")",'0',4,1,0),K(L"$",'4',4,1,0),K(L"&",'7',4,1,0),
    K(L"@",'2',4,1,0),K(L"\"",VKX_QUOTE,4,1,0)};
static const KEY s3[] = {
    K(L"!",'1',4,1,0),K(L"#",'3',4,1,0),K(L"%",'5',4,1,0),K(L"*",'8',4,1,0),
    K(L"+",VKX_PLUS,4,1,0),LT(L"=",VKX_PLUS),K(L"_",VKX_MINUS,4,1,0),K(L"~",VKX_TILDE,4,1,0),
    K(L"BS",VK_BACK,8,0,0)};

/* ---- 通常/記号 共通の最下段 ---- */
static const KEY r4[] = {
    K(L"&123",VKX_TOGGLE,6,0,0),LT(L",",VKX_COMMA),K(L"Space",VK_SPACE,12,0,0),
    LT(L"'",VKX_QUOTE),LT(L".",VKX_PERIOD),K(L"?",VKX_SLASH,4,1,0),K(L"Enter",VK_RETURN,6,0,0)};

/* ================= フルキーボード(US配列, 60ユニット幅) ================= */
static const KEY f0[] = {
    K(L"Esc",VK_ESCAPE,4,0,0),
    LT(L"F1",VK_F1),LT(L"F2",VK_F2),LT(L"F3",VK_F3),LT(L"F4",VK_F4),LT(L"F5",VK_F5),LT(L"F6",VK_F6),
    LT(L"F7",VK_F7),LT(L"F8",VK_F8),LT(L"F9",VK_F9),LT(L"F10",VK_F10),LT(L"F11",VK_F11),LT(L"F12",VK_F12),
    K(L"Del",VK_DELETE,8,0,0)};
static const KEY f1[] = {
    FK(L"`",L"~",VKX_TILDE,4),FK(L"1",L"!",'1',4),FK(L"2",L"@",'2',4),FK(L"3",L"#",'3',4),
    FK(L"4",L"$",'4',4),FK(L"5",L"%",'5',4),FK(L"6",L"^",'6',4),FK(L"7",L"&",'7',4),
    FK(L"8",L"*",'8',4),FK(L"9",L"(",'9',4),FK(L"0",L")",'0',4),
    FK(L"-",L"_",VKX_MINUS,4),FK(L"=",L"+",VKX_PLUS,4),K(L"Bksp",VK_BACK,8,0,0)};
static const KEY f2[] = {
    K(L"Tab",VK_TAB,6,0,0),
    LT(L"q",'Q'),LT(L"w",'W'),LT(L"e",'E'),LT(L"r",'R'),LT(L"t",'T'),
    LT(L"y",'Y'),LT(L"u",'U'),LT(L"i",'I'),LT(L"o",'O'),LT(L"p",'P'),
    FK(L"[",L"{",VKX_LBRACK,4),FK(L"]",L"}",VKX_RBRACK,4),FK(L"\\",L"|",VKX_BSLASH,6)};
static const KEY f3[] = {
    K(L"Caps",VK_CAPITAL,7,0,0),
    LT(L"a",'A'),LT(L"s",'S'),LT(L"d",'D'),LT(L"f",'F'),LT(L"g",'G'),
    LT(L"h",'H'),LT(L"j",'J'),LT(L"k",'K'),LT(L"l",'L'),
    FK(L";",L":",VKX_SEMI,4),FK(L"'",L"\"",VKX_QUOTE,4),K(L"Enter",VK_RETURN,9,0,0)};
static const KEY f4[] = {
    K(L"Shift",VK_SHIFT,9,0,0),
    LT(L"z",'Z'),LT(L"x",'X'),LT(L"c",'C'),LT(L"v",'V'),LT(L"b",'B'),LT(L"n",'N'),LT(L"m",'M'),
    FK(L",",L"<",VKX_COMMA,4),FK(L".",L">",VKX_PERIOD,4),FK(L"/",L"?",VKX_SLASH,4),
    K(L"Shift",VK_SHIFT,11,0,0)};
static const KEY f5[] = {
    K(L"Ctrl",VK_CONTROL,6,0,0),K(L"Alt",VK_MENU,5,0,0),
    K(L"Space",VK_SPACE,28,0,0),K(L"Alt",VK_MENU,5,0,0),
    LT(L"<",VK_LEFT),LT(L"^",VK_UP),LT(L"v",VK_DOWN),LT(L">",VK_RIGHT)};

static const ROW alphaRows[4] = {{a1,N(a1)},{a2,N(a2)},{a3,N(a3)},{r4,N(r4)}};
static const ROW symRows[4]   = {{s1,N(s1)},{s2,N(s2)},{s3,N(s3)},{r4,N(r4)}};
static const ROW fullRows[6]  = {{f0,N(f0)},{f1,N(f1)},{f2,N(f2)},{f3,N(f3)},{f4,N(f4)},{f5,N(f5)}};

static const LAYER layers[3] = {{alphaRows,4,40},{symRows,4,40},{fullRows,6,60}};

/* ================= 状態 ================= */
static HWND  g_hwnd;
static HFONT g_fMain, g_fSmall;
static int   g_fontH = 0;                   /* フォントを作った時の g_rowH */
static int   g_rowH;                        /* キー1段の高さ(px)。サイズ変更の基準 */
static int   g_layer = LY_STD;
static int   g_float = 0;                   /* 0=ドック 1=フロート */
static int   g_min = 0, g_mpress = 0, g_mmoved = 0;  /* 最小化中 / 押下(1=KB 2=X) / ドラッグした */
static int   g_mx = 0, g_my = 0, g_mvalid = 0;       /* ユーザーが動かしたタイル位置 */
static int   g_fx = 0, g_fy = 0, g_fw = 0;  /* フロート時の位置と幅 */
static int   g_shift = 0, g_ctrl = 0, g_alt = 0, g_caps = 0;
static int   g_pr = -1, g_pc = -1;          /* 押下中のキー */
static int   g_ptb = -1;                    /* 押下中のツールバーボタン */
static int   g_sy, g_flick = 0;             /* フリック判定用 */
static int   g_drag = 0;                    /* 1=移動 2=リサイズ */
static POINT g_dp;                          /* ドラッグ開始のスクリーン座標 */
static int   g_dx0, g_dy0, g_dw0, g_dh0;    /* ドラッグ開始時のウィンドウ位置/サイズ */

#define LY() (&layers[g_layer])

static int TbH(void)  { return g_rowH * 3 / 4; }
static int WinH(void) { return TbH() + g_rowH * LY()->nrows; }

static void ClampSize(void)
{
    int sh = GetSystemMetrics(SM_CYSCREEN);
    int maxRow = sh * 4 / (6 * 4 + 3);      /* 6段+ツールバーが画面に収まる上限 */
    if (g_rowH > maxRow) g_rowH = maxRow;
    if (g_rowH < 24)     g_rowH = 24;
}

static void MakeFonts(void)
{
    LOGFONT lf;
    if (g_fMain && g_fontH == g_rowH) return;
    if (g_fMain)  DeleteObject(g_fMain);
    if (g_fSmall) DeleteObject(g_fSmall);
    memset(&lf, 0, sizeof(lf));
    lstrcpy(lf.lfFaceName, L"Tahoma"); lf.lfWeight = FW_NORMAL;
    lf.lfHeight = -(g_rowH * 2 / 5); g_fMain  = CreateFontIndirect(&lf);
    lf.lfHeight = -(g_rowH / 5);     g_fSmall = CreateFontIndirect(&lf);
    g_fontH = g_rowH;
}

/* ウィンドウの位置・サイズを現在のモードに合わせて反映 */
static void Relayout(void)
{
    int sw = GetSystemMetrics(SM_CXSCREEN), sh = GetSystemMetrics(SM_CYSCREEN);
    int hgt = WinH(), w, x, y;

    if (g_min) {                                   /* 最小化: 小さなタイル */
                w = g_rowH * 3; hgt = TbH();                 /* [KB][X] ぶん幅を広げる */
        if (g_mvalid)     { x = g_mx; y = g_my; }
        else if (g_float) { x = g_fx; y = g_fy; }
        else              { x = sw - w; y = sh - hgt; }
        if (x + w > sw)   x = sw - w;
        if (y + hgt > sh) y = sh - hgt;
        if (x < 0) x = 0;
        if (y < 0) y = 0;
    } else if (g_float) {
        if (g_fw < TbH() * 11) g_fw = TbH() * 11;  /* ボタンが1つ増えたので最小幅も拡大 */
        if (g_fw > sw)         g_fw = sw;
        w = g_fw; x = g_fx; y = g_fy;
        if (x + w > sw)   x = sw - w;
        if (y + hgt > sh) y = sh - hgt;
        if (x < 0) x = 0;
        if (y < 0) y = 0;
        g_fx = x; g_fy = y;
    } else {
        w = sw; x = 0; y = sh - hgt;
    }
    SetWindowPos(g_hwnd, HWND_TOPMOST, x, y, w, hgt, SWP_NOACTIVATE);
    InvalidateRect(g_hwnd, NULL, FALSE);
}

static void ApplySize(void) { ClampSize(); MakeFonts(); Relayout(); }

static void SetLayer(int n)
{
    g_layer = n;
    g_shift = g_ctrl = g_alt = 0;
    Relayout();
}

/* ---- ツールバー ---- */
static int TbW(int i) { return (i < 2) ? TbH() * 5 / 2 : TbH() * 3 / 2; }

static void TbRect(int i, RECT *rc)
{
    int k, acc = 0;
    for (k = 0; k < i; k++) acc += TbW(k);
    rc->left = acc; rc->right = acc + TbW(i); rc->top = 0; rc->bottom = TbH();
    InflateRect(rc, -GAP, -GAP);
}

static int TbHit(int x, int y)
{
    RECT rc; POINT pt; int i;
    pt.x = x; pt.y = y;
    for (i = 0; i < TB_N; i++) { TbRect(i, &rc); if (PtInRect(&rc, pt)) return i; }
    return -1;
}

static void GripRect(const RECT *cl, RECT *rc)   /* 右上のリサイズハンドル */
{
    rc->right = cl->right; rc->left = cl->right - TbH();
    rc->top = 0;           rc->bottom = TbH();
}

static const WCHAR *TbLabel(int i)
{
    switch (i) {
    case TB_MODE:  return (g_layer == LY_FULL) ? L"Std" : L"Full";
    case TB_FLOAT: return g_float ? L"Dock" : L"Float";
    case TB_MINUS: return L"-";
    case TB_PLUS:  return L"+";
    default:       return L"_";
    }
}

static void Scale(int up)
{
    int n = up ? g_rowH * 11 / 10 : g_rowH * 9 / 10;
    if (n == g_rowH) n += up ? 1 : -1;
    if (g_float && g_fw) g_fw = g_fw * n / g_rowH;   /* フロート時は幅も同じ比率で */
    g_rowH = n;
    ApplySize();
}

static void OnToolbar(int b)
{
    switch (b) {
    case TB_MIN:
        g_min = 1; Relayout();
        break;
    case TB_MODE:
        SetLayer(g_layer == LY_FULL ? LY_STD : LY_FULL);
        break;
    case TB_FLOAT:
        g_float = !g_float;
        if (g_float && g_fw == 0) {                  /* 初回の配置 */
            int sw = GetSystemMetrics(SM_CXSCREEN), sh = GetSystemMetrics(SM_CYSCREEN);
            g_fw = sw * 2 / 3;
            g_fx = (sw - g_fw) / 2;
            g_fy = sh - WinH() - sh / 8;
        }
        Relayout();
        break;
    case TB_MINUS: Scale(0); break;
    case TB_PLUS:  Scale(1); break;
    }
}

/* ---- キー領域 ---- */
static void KeyRect(int r, int c, const RECT *cl, RECT *rc)
{
    const LAYER *L = LY();
    int W = cl->right, tb = TbH(), H = cl->bottom - tb, i, acc = 0;
    for (i = 0; i < c; i++) acc += L->rows[r].k[i].w;
    rc->left   = acc * W / L->units;
    rc->right  = (acc + L->rows[r].k[c].w) * W / L->units;
    rc->top    = tb + r * H / L->nrows;
    rc->bottom = tb + (r + 1) * H / L->nrows;
    InflateRect(rc, -GAP, -GAP);
}

static int HitTest(int x, int y, int *pr, int *pc)
{
    const LAYER *L = LY();
    RECT cl, rc; POINT pt; int r, c;
    GetClientRect(g_hwnd, &cl);
    pt.x = x; pt.y = y;
    for (r = 0; r < L->nrows; r++)
        for (c = 0; c < L->rows[r].n; c++) {
            if (!L->rows[r].k[c].vk) continue;
            KeyRect(r, c, &cl, &rc);
            if (PtInRect(&rc, pt)) { *pr = r; *pc = c; return 1; }
        }
    return 0;
}

static void Tap(BYTE vk, int shift)
{
    if (shift) keybd_event(VK_SHIFT, 0, 0, 0);
    keybd_event(vk, 0, 0, 0);
    keybd_event(vk, 0, KEYEVENTF_KEYUP, 0);
    if (shift) keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
}

static void OnKey(const KEY *k)
{
    int letter = (k->vk >= 'A' && k->vk <= 'Z');

    switch (k->vk) {
    case VKX_TOGGLE: SetLayer(g_layer == LY_STD ? LY_SYM : LY_STD); return;
    case VK_SHIFT:   g_shift = !g_shift; break;
    case VK_CONTROL: g_ctrl  = !g_ctrl;  break;
    case VK_MENU:    g_alt   = !g_alt;   break;
    case VK_CAPITAL: Tap(VK_CAPITAL, 0); g_caps = !g_caps; break;
    default: {
        int useShift = k->shift || (g_shift && (letter || g_layer == LY_FULL));
        if (g_ctrl) keybd_event(VK_CONTROL, 0, 0, 0);
        if (g_alt)  keybd_event(VK_MENU, 0, 0, 0);
        Tap(k->vk, useShift);
        if (g_alt)  keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
        if (g_ctrl) keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
        g_shift = g_ctrl = g_alt = 0;      /* ワンショット */
        break; }
    }
    InvalidateRect(g_hwnd, NULL, FALSE);
}

/* ================= 描画 ================= */
static void DrawGrip(HDC m, const RECT *gr)
{
    int s = TbH() / 10 + 1, step = s * 2, i, j;
    int ox = gr->right - GAP - 3 * step, oy = gr->bottom - GAP - 3 * step;
    HBRUSH b = CreateSolidBrush(RGB(0x99,0x99,0x99));
    for (j = 0; j < 3; j++)
        for (i = 0; i < 3; i++)
            if (i + j >= 2) {
                RECT d;
                d.left = ox + i * step; d.top = oy + j * step;
                d.right = d.left + s;   d.bottom = d.top + s;
                FillRect(m, &d, b);
            }
    DeleteObject(b);
}

static void Paint(HWND h)
{
    const LAYER *L = LY();
    PAINTSTRUCT ps; RECT cl, rc, tr, hr, gr;
    HDC dc = BeginPaint(h, &ps);
    HDC m; HBITMAP bm, ob; HBRUSH br; HGDIOBJ of;
    WCHAR buf[2], hint[2];
    int r, c, i;

    GetClientRect(h, &cl);
    m  = CreateCompatibleDC(dc);
    bm = CreateCompatibleBitmap(dc, cl.right, cl.bottom);
    ob = (HBITMAP)SelectObject(m, bm);

    br = CreateSolidBrush(RGB(0x10,0x10,0x10)); FillRect(m, &cl, br); DeleteObject(br);
    of = SelectObject(m, g_fMain);
    SetBkMode(m, TRANSPARENT);
    buf[1] = 0; hint[1] = 0;

    if (g_min) {                                   /* 最小化中: [KB][X] */
        RECT k1, k2; int split = cl.right * 2 / 3;
        k1 = cl; k1.right = split; InflateRect(&k1, -GAP, -GAP);
        k2 = cl; k2.left  = split; InflateRect(&k2, -GAP, -GAP);
        br = CreateSolidBrush(g_mpress == 1 ? RGB(0x00,0x78,0xD7) : RGB(0x3A,0x3A,0x3A));
        FillRect(m, &k1, br); DeleteObject(br);
        br = CreateSolidBrush(g_mpress == 2 ? RGB(0xC4,0x2B,0x1C) : RGB(0x2B,0x2B,0x2B));
        FillRect(m, &k2, br); DeleteObject(br);
        SetTextColor(m, RGB(255,255,255));
        DrawText(m, L"KB", -1, &k1, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        DrawText(m, L"X",  -1, &k2, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        BitBlt(dc, 0, 0, cl.right, cl.bottom, m, 0, 0, SRCCOPY);
        SelectObject(m, of);
        SelectObject(m, ob); DeleteObject(bm); DeleteDC(m);
        EndPaint(h, &ps);
        return;
    }

    /* ---- ツールバー ---- */
    for (i = 0; i < TB_N; i++) {
        TbRect(i, &rc);
        br = CreateSolidBrush(i == g_ptb ? RGB(0x00,0x78,0xD7) : RGB(0x2B,0x2B,0x2B));
        FillRect(m, &rc, br); DeleteObject(br);
        SetTextColor(m, RGB(255,255,255));
        DrawText(m, TbLabel(i), -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
    }
    if (g_float) {                               /* 移動ハンドル領域とリサイズグリップ */
        GripRect(&cl, &gr);
        DrawGrip(m, &gr);
        TbRect(TB_N - 1, &rc);
        hr.left = rc.right + GAP; hr.right = gr.left; hr.top = 0; hr.bottom = TbH();
        SetTextColor(m, RGB(0x66,0x66,0x66));
        DrawText(m, L"::::::", -1, &hr, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
    }

    /* ---- キー ---- */
    for (r = 0; r < L->nrows; r++)
        for (c = 0; c < L->rows[r].n; c++) {
            const KEY *k = &L->rows[r].k[c];
            const WCHAR *label = k->label;
            COLORREF col;
            int pressed, flicking, active;
            if (!k->vk) continue;

            KeyRect(r, c, &cl, &rc);
            pressed  = (r == g_pr && c == g_pc);
            flicking = pressed && g_flick;
            active   = (k->vk == VK_SHIFT   && g_shift) || (k->vk == VK_CONTROL && g_ctrl) ||
                       (k->vk == VK_MENU    && g_alt)   || (k->vk == VK_CAPITAL && g_caps);
            if (pressed)      { col = RGB(0x00,0x78,0xD7); InflateRect(&rc, -2, -2); }
            else if (active)    col = RGB(0x00,0x78,0xD7);
            else                col = RGB(0x3A,0x3A,0x3A);
            br = CreateSolidBrush(col); FillRect(m, &rc, br); DeleteObject(br);

            if (flicking) {
                buf[0] = (WCHAR)k->flick; label = buf;
            } else if (k->vk >= 'A' && k->vk <= 'Z') {
                if (g_shift ^ g_caps) { buf[0] = (WCHAR)k->vk; label = buf; }
            } else if (g_shift && k->slabel && g_layer == LY_FULL) {
                label = k->slabel;
            } else if (k->vk == VKX_TOGGLE) {
                label = g_layer ? L"ABC" : L"&123";
            }

            tr = rc;
            if (k->flick && !flicking) {         /* 上に小さく数字ヒント */
                hr = rc; hint[0] = (WCHAR)k->flick;
                SelectObject(m, g_fSmall);
                SetTextColor(m, RGB(0x99,0x99,0x99));
                DrawText(m, hint, 1, &hr, DT_CENTER | DT_TOP | DT_SINGLELINE | DT_NOPREFIX);
                SelectObject(m, g_fMain);
                tr.top += (rc.bottom - rc.top) / 6;
            }
            SetTextColor(m, RGB(255,255,255));
            DrawText(m, label, -1, &tr, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        }

    BitBlt(dc, 0, 0, cl.right, cl.bottom, m, 0, 0, SRCCOPY);
    SelectObject(m, of);
    SelectObject(m, ob); DeleteObject(bm); DeleteDC(m);
    EndPaint(h, &ps);
}

/* ================= ウィンドウプロシージャ ================= */
static LRESULT CALLBACK WndProc(HWND h, UINT msg, WPARAM wp, LPARAM lp)
{
    int r, c, x = (short)LOWORD(lp), y = (short)HIWORD(lp);

        if (g_min && (msg == WM_LBUTTONDOWN || msg == WM_MOUSEMOVE || msg == WM_LBUTTONUP)) {
        RECT cl; GetClientRect(h, &cl);

        if (msg == WM_LBUTTONDOWN) {
            RECT wr;
            g_mpress = (x < cl.right * 2 / 3) ? 1 : 2;
            g_mmoved = 0;
            g_dp.x = x; g_dp.y = y; ClientToScreen(h, &g_dp);
            GetWindowRect(h, &wr); g_dx0 = wr.left; g_dy0 = wr.top;
            SetCapture(h);
            InvalidateRect(h, NULL, FALSE);

        } else if (msg == WM_MOUSEMOVE && g_mpress) {
            POINT p; int dx, dy;
            p.x = x; p.y = y; ClientToScreen(h, &p);
            dx = p.x - g_dp.x; dy = p.y - g_dp.y;
            if (!g_mmoved && dx * dx + dy * dy > 64) g_mmoved = 1;   /* 8px超でドラッグ扱い */
            if (g_mmoved && g_mpress == 1) {                         /* KB側だけ移動可 */
                int sw = GetSystemMetrics(SM_CXSCREEN), sh = GetSystemMetrics(SM_CYSCREEN);
                int nx = g_dx0 + dx, ny = g_dy0 + dy;
                if (nx > sw - cl.right)  nx = sw - cl.right;
                if (ny > sh - cl.bottom) ny = sh - cl.bottom;
                if (nx < 0) nx = 0;
                if (ny < 0) ny = 0;
                g_mx = nx; g_my = ny; g_mvalid = 1;
                SetWindowPos(h, NULL, nx, ny, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
            }

        } else if (msg == WM_LBUTTONUP && g_mpress) {
            POINT p; int which = g_mpress, moved = g_mmoved, zone;
            p.x = x; p.y = y;
            zone = PtInRect(&cl, p) ? ((x < cl.right * 2 / 3) ? 1 : 2) : 0;
            g_mpress = 0; g_mmoved = 0; ReleaseCapture();
            if (!moved && zone == which) {
                if (which == 1) {                                    /* 復帰 */
                    if (g_float) {                                   /* フロートはタイルの位置に復帰 */
                        RECT wr; GetWindowRect(h, &wr);
                        g_fx = wr.left; g_fy = wr.top;
                    }
                    g_min = 0; Relayout();
                } else {                                             /* 終了 */
                    DestroyWindow(h);
                }
            } else {
                InvalidateRect(h, NULL, FALSE);
            }
        }
        return 0;
    }

    switch (msg) {
    case WM_CREATE:        MakeFonts(); return 0;
    case WM_MOUSEACTIVATE: return MA_NOACTIVATE;
    case WM_ERASEBKGND:    return 1;
    case WM_PAINT:         Paint(h); return 0;

    case WM_LBUTTONDOWN:
        if (y < TbH()) {                                   /* ツールバー */
            int b = TbHit(x, y);
            if (b >= 0) {
                g_ptb = b; SetCapture(h);
                InvalidateRect(h, NULL, FALSE);
            } else if (g_float) {                          /* 空き領域=移動、右端=リサイズ */
                RECT cl, gr, wr; POINT p;
                GetClientRect(h, &cl); GripRect(&cl, &gr);
                p.x = x; p.y = y;
                g_dp = p; ClientToScreen(h, &g_dp);
                GetWindowRect(h, &wr);
                g_dx0 = wr.left; g_dy0 = wr.top;
                g_dw0 = wr.right - wr.left; g_dh0 = wr.bottom - wr.top;
                g_drag = PtInRect(&gr, p) ? 2 : 1;
                SetCapture(h);
            }
            return 0;
        }
        if (HitTest(x, y, &r, &c)) {                       /* キー */
            g_pr = r; g_pc = c; g_sy = y; g_flick = 0;
            SetCapture(h);
            InvalidateRect(h, NULL, FALSE);
        }
        return 0;

    case WM_MOUSEMOVE:
        if (g_drag) {
            POINT p; int sw = GetSystemMetrics(SM_CXSCREEN), sh = GetSystemMetrics(SM_CYSCREEN);
            p.x = x; p.y = y; ClientToScreen(h, &p);
            if (g_drag == 1) {                             /* 移動 */
                int nx = g_dx0 + p.x - g_dp.x, ny = g_dy0 + p.y - g_dp.y;
                if (nx > sw - g_fw)   nx = sw - g_fw;
                if (ny > sh - WinH()) ny = sh - WinH();
                if (nx < 0) nx = 0;
                if (ny < 0) ny = 0;
                g_fx = nx; g_fy = ny;
                SetWindowPos(h, NULL, nx, ny, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
            } else {                                       /* リサイズ */
                int nw = g_dw0 + p.x - g_dp.x, nh = g_dh0 + p.y - g_dp.y;
                g_fw   = nw;
                g_rowH = nh * 4 / (LY()->nrows * 4 + 3);
                ApplySize();
            }
            return 0;
        }
        if (g_pr >= 0) {
            int f = LY()->rows[g_pr].k[g_pc].flick && (g_sy - y > g_rowH / 3);
            if (f != g_flick) { g_flick = f; InvalidateRect(h, NULL, FALSE); }
        }
        return 0;

    case WM_LBUTTONUP:
        if (g_drag) { g_drag = 0; ReleaseCapture(); return 0; }
        if (g_ptb >= 0) {
            int b = g_ptb, same = (TbHit(x, y) == b);
            g_ptb = -1; ReleaseCapture();
            InvalidateRect(h, NULL, FALSE);
            UpdateWindow(h);
            if (same) OnToolbar(b);
            return 0;
        }
        if (g_pr >= 0) {
            const KEY *k = &LY()->rows[g_pr].k[g_pc];
            int flick = g_flick;
            int same  = HitTest(x, y, &r, &c) && r == g_pr && c == g_pc;
            g_pr = g_pc = -1; g_flick = 0;
            ReleaseCapture();
            InvalidateRect(h, NULL, FALSE);
            UpdateWindow(h);
            if (flick)     Tap(k->flick, 0);
            else if (same) OnKey(k);
        }
        return 0;

    case WM_DESTROY:
        DeleteObject(g_fMain); DeleteObject(g_fSmall);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(h, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hi, HINSTANCE hp, LPWSTR cmd, int show)
{
    WNDCLASS wc; MSG msg;
    int sw = GetSystemMetrics(SM_CXSCREEN), sh = GetSystemMetrics(SM_CYSCREEN), hgt;

    g_rowH = sh / 10; ClampSize();
    hgt = WinH();

    memset(&wc, 0, sizeof(wc));
    wc.lpfnWndProc = WndProc; wc.hInstance = hi; wc.lpszClassName = L"MetroKB";
    RegisterClass(&wc);

    g_hwnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_NOACTIVATE, L"MetroKB", L"",
                            WS_POPUP | WS_VISIBLE, 0, sh - hgt, sw, hgt, NULL, NULL, hi, NULL);
    while (GetMessage(&msg, NULL, 0, 0)) { TranslateMessage(&msg); DispatchMessage(&msg); }
    return 0;
}