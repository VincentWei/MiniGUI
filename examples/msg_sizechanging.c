/*
 * The handler set the actual size of the window always
 * to be _WIDTH wide and _HEIGHT high.
 */
		case MSG_SIZECHANGING:
		{
			const RECT* rcExpect = (const RECT*)wParam;
			RECT* rcResult = (RECT*)lParam;

			rcResult->left = rcExpect->left;
			rcResult->top = rcExpect->top;
			rcResult->right = rcExpect->left + _WIDTH;
			rcResult->bottom = rcExpect->top + _HEIGHT;
		    return 0;
		}
