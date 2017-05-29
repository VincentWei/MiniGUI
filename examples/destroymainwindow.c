/*
 * The following code destroies all resource used by the main window
 * and then destroies the main window itself.
 */
		case MSG_CLOSE:
		    /* Destroy the resource used by the main window. */
		    DestroyLogFont (logfont1);
		    DestroyLogFont (logfont2);
		    DestroyLogFont (logfont3);
		    
		    /* Destroy the child windows. */
		    DestroyWindow(hWndButton);
		    DestroyWindow(hWndEdit);

		    /* Destroy the main window. */
		    DestroyMainWindow (hWnd);

		    /* Send a MSG_QUIT message to quit the message loop. */
		    PostQuitMessage(hWnd);
		    return 0;
