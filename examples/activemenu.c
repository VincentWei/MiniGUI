/*
 * This handler checks the specified menu items to contain
 * a radio bitmap by calling CheckMenuRadioItem function,
 * if the submenu at position 2 has been activated.
 */
        case MSG_ACTIVEMENU:
            if (wParam == 2) {
                CheckMenuRadioItem ((HMENU)lParam, 
                    IDM_40X15, IDM_CUSTOMIZE, 
                    pConInfo->termType, MF_BYCOMMAND);
                CheckMenuRadioItem ((HMENU)lParam, 
                    IDM_DEFAULT, IDM_BIG5, 
                    pConInfo->termCharset, MF_BYCOMMAND);
            }
        break;
