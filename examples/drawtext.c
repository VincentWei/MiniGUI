/** 
 * Selecting a logical font to the DC and output text in GB2312 charset
 * by using DrawText.
 */
void OnModeDrawText (HDC hdc)
{
    RECT rc1, rc2, rc3, rc4;
    const char* szBuff1 = "This is a good day. \n"
            "This text is drawn by calling DrawText.";
    const char* szBuff2 = "This is a good day. \n"
            "This text is drawn by calling DrawText.";
    const char* szBuff3 = "Single line text, center.";
    const char* szBuff4 = 
            "This text is drawn by calling DrawText.";

    rc1.left = 1; rc1.top  = 1; rc1.right = 401; rc1.bottom = 101;
    rc2.left = 0; rc2.top  = 110; rc2.right = 401; rc2.bottom = 351;
    rc3.left = 0; rc3.top  = 361; rc3.right = 401; rc3.bottom = 451;
    rc4.left = 0; rc4.top  = 461; rc4.right = 401; rc4.bottom = 551;

    SetBkColor (hdc, COLOR_lightwhite);

    Rectangle (hdc, rc1.left, rc1.top, rc1.right, rc1.bottom);
    Rectangle (hdc, rc2.left, rc2.top, rc2.right, rc2.bottom);
    Rectangle (hdc, rc3.left, rc3.top, rc3.right, rc3.bottom);
    Rectangle (hdc, rc4.left, rc4.top, rc4.right, rc4.bottom);

    InflateRect (&rc1, -1, -1);
    InflateRect (&rc2, -1, -1);
    InflateRect (&rc3, -1, -1);
    InflateRect (&rc4, -1, -1);

    SelectFont (hdc, logfontgb12);
    DrawText (hdc, szBuff1, -1, &rc1, DT_NOCLIP | DT_CENTER | DT_WORDBREAK);

    SelectFont (hdc, logfontgb16);
    DrawText (hdc, szBuff2, -1, &rc2, DT_NOCLIP | DT_RIGHT | DT_WORDBREAK);

    SelectFont (hdc, logfontgb24);
    DrawText (hdc, szBuff3, -1, &rc3, DT_NOCLIP | DT_SINGLELINE | DT_CENTER | DT_VCENTER);

    SelectFont (hdc, logfontgb16);
    DrawTextEx (hdc, szBuff4, -1, &rc4, 32, DT_NOCLIP | DT_LEFT | DT_WORDBREAK);
}
