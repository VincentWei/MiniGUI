/**
 * The handler makes sure that the client area always
 * is _WIDTH wide and _HEIGHT high.
 */
		case MSG_SIZECHANGED:
		{
			RECT* rcClient = (RECT*)lParam;
			rcClient->right = rcClient->left + _WIDTH;
			rcClient->bottom = rcClient->top + _HEIGHT;
			return 1;
		}
