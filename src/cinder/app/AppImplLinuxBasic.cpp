/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#include <unistd.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>

#include "cinder/app/AppImplLinuxBasic.h"
#include "cinder/app/AppBasic.h"
#include "cinder/app/AppImplLinuxRenderer.h"
#include "cinder/app/Renderer.h"


using std::vector;
using std::string;

namespace cinder { namespace app {


AppImplLinuxBasic::AppImplLinuxBasic( AppBasic *aApp )
	: AppImplLinux( aApp ), mApp( aApp ), mHasBeenInitialized( false )
{
	mShouldQuit = false;
	mIsDragging = false;
}

unsigned int AppImplLinuxBasic::prepModifiers( unsigned int state )
{
	unsigned int result = 0;

	if( state & ShiftMask ) result |= KeyEvent::SHIFT_DOWN;
	if( state & ControlMask ) result |= KeyEvent::CTRL_DOWN;
	if( state & Mod1Mask ) result |= KeyEvent::ALT_DOWN;
	if( state & Mod4Mask ) result |= KeyEvent::META_DOWN;
	if( state & Button1Mask ) result |= MouseEvent::LEFT_DOWN;
	if( state & Button2Mask ) result |= MouseEvent::MIDDLE_DOWN;
	if( state & Button3Mask ) result |= MouseEvent::RIGHT_DOWN;
	return result;
}

unsigned int AppImplLinuxBasic::prepKeyEventModifiers( unsigned int state )
{
	unsigned int result = 0;
	if( state & ShiftMask ) result |= KeyEvent::SHIFT_DOWN;
	if( state & ControlMask ) result |= KeyEvent::CTRL_DOWN;
	if( state & Mod1Mask ) result |= KeyEvent::ALT_DOWN;
	if( state & Mod4Mask ) result |= KeyEvent::META_DOWN;
	return result;
}

void AppImplLinuxBasic::run()
{
	mDisplay = mApp->getSettings().getDisplay();
	if( ! mDisplay )
		mDisplay = cinder::Display::getMainDisplay().get();

	if( mApp->getSettings().isFullScreen() ) {
		mFullScreen = true;
		mWindowWidth = mDisplay->getWidth();
		mWindowHeight = mDisplay->getHeight();
	}
	else {
		mFullScreen = false;
		mWindowWidth = mApp->getSettings().getWindowWidth();
		mWindowHeight = mApp->getSettings().getWindowHeight();
	}

	mBorderless = mApp->getSettings().isBorderless();
	mAlwaysOnTop = mApp->getSettings().isAlwaysOnTop();
	mFrameRate = mApp->getSettings().getFrameRate();

	if( mApp->getSettings().isWindowPosSpecified() )
		mWindowedPos = mApp->getSettings().getWindowPos();
	else
		mWindowedPos = Vec2i(
			mDisplay->getArea().getX1() + ( mDisplay->getWidth() - mApp->getSettings().getWindowWidth() ) / 2,
			mDisplay->getArea().getY1() + ( mDisplay->getHeight() - mApp->getSettings().getWindowHeight() ) / 2 );	// center window

	createWindow( &mWindowWidth, &mWindowHeight );

	mApp->privateSetup__();
	mHasBeenInitialized = true;
	mApp->privateResize__( ResizeEvent( Vec2i( mWindowWidth, mWindowHeight ) ) );

	/*
	//::XSetInputFocus( dpy, mWin, RevertToParent, CurrentTime );
	::SetFocus( mWnd );
	::DragAcceptFiles( mWnd, TRUE );*/

	// initialize our next frame time to be definitely now
	mNextFrameTime = -1;

	_XDisplay *dpy = mDisplay->getXDisplay();

	while( ! mShouldQuit ) {

		double currentSeconds = mApp->getElapsedSeconds();
		if( currentSeconds >= mNextFrameTime ) {
			mNextFrameTime = currentSeconds + 1.0 / mFrameRate;
			mApp->privateUpdate__();
			mApp->getRenderer()->startDraw();
			mApp->draw();
			mApp->getRenderer()->finishDraw();
		}
		else
			::usleep( 1 );

		int pendingEvents = ::XPending( dpy );
		while( pendingEvents-- ) {
			XEvent xev;
			::XNextEvent( dpy, &xev );

			switch( xev.type ) {
			case Expose:
				mWindowWidth = xev.xexpose.width;
				mWindowHeight = xev.xexpose.height;
				mApp->privateResize__( ResizeEvent( Vec2i( mWindowWidth, mWindowHeight ) ) );
				break;

			case ButtonPress:
				mIsDragging = true;
				if( xev.xbutton.button == Button1 )
				   mApp->privateMouseDown__( MouseEvent( MouseEvent::LEFT_DOWN, xev.xbutton.x, xev.xbutton.y,
				                                         prepModifiers( xev.xbutton.state ), 0.0f, xev.xbutton.state ) );
				else if( xev.xbutton.button == Button2 )
				   mApp->privateMouseDown__( MouseEvent( MouseEvent::MIDDLE_DOWN, xev.xbutton.x, xev.xbutton.y,
				                                         prepModifiers( xev.xbutton.state ), 0.0f, xev.xbutton.state ) );
				else if( xev.xbutton.button == Button3 )
				   mApp->privateMouseDown__( MouseEvent( MouseEvent::RIGHT_DOWN, xev.xbutton.x, xev.xbutton.y,
				                                         prepModifiers( xev.xbutton.state ), 0.0f, xev.xbutton.state ) );
				break;

			case ButtonRelease:
				mIsDragging = false;
				if( xev.xbutton.button == Button1 )
				   mApp->privateMouseUp__( MouseEvent( MouseEvent::LEFT_DOWN, xev.xbutton.x, xev.xbutton.y,
				                                       prepModifiers( xev.xbutton.state ), 0.0f, xev.xbutton.state ) );
				else if( xev.xbutton.button == Button2 )
				   mApp->privateMouseUp__( MouseEvent( MouseEvent::MIDDLE_DOWN, xev.xbutton.x, xev.xbutton.y,
				                                       prepModifiers( xev.xbutton.state ), 0.0f, xev.xbutton.state ) );
				else if( xev.xbutton.button == Button3 )
				   mApp->privateMouseUp__( MouseEvent( MouseEvent::RIGHT_DOWN, xev.xbutton.x, xev.xbutton.y,
				                                       prepModifiers( xev.xbutton.state ), 0.0f, xev.xbutton.state ) );
				break;
				//Keyevent 
			case KeyRelease:
			   {
				   KeySym keysym = XkbKeycodeToKeysym(dpy,xev.xkey.keycode, 0, 0);
				   std::string keystring(XKeysymToString(keysym));
				   if(keystring.size() > 1){
					   getApp()->privateKeyUp__(
						   KeyEvent(KeyEvent::translateNativeKeyCode(keysym), static_cast<char>(0),
						            prepKeyEventModifiers(xev.xkey.state), keysym));
				   } else {
					   getApp()->privateKeyUp__(
						   KeyEvent(KeyEvent::translateNativeKeyCode(keysym), keystring[0],
						            prepKeyEventModifiers(xev.xkey.state), keysym));
				   }
				   break;
			   }
			case KeyPress:
			   {
				   KeySym keysym =
				      XkbKeycodeToKeysym(dpy,xev.xkey.keycode, 0, 0);
				   std::string keystring(XKeysymToString(keysym));
				   if(keystring.size() > 1){
					   getApp()->privateKeyDown__(
						   KeyEvent(KeyEvent::translateNativeKeyCode(keysym), static_cast<char>(0),
						            prepKeyEventModifiers(xev.xkey.state), keysym));
				   } else {
					   getApp()->privateKeyDown__(
						   KeyEvent(KeyEvent::translateNativeKeyCode(keysym), keystring[0],
						            prepKeyEventModifiers(xev.xkey.state), keysym));
				   }
				   break;
			   }
			case MotionNotify:
				if( mIsDragging ) {
					mApp->privateMouseDrag__( MouseEvent( 0, xev.xmotion.x, xev.xmotion.y,
					                                      prepModifiers( xev.xmotion.state ), 0.0f, xev.xmotion.state ) );
				}
				else {
					mApp->privateMouseMove__( MouseEvent( 0, xev.xmotion.x, xev.xmotion.y,
					                                      prepModifiers( xev.xmotion.state ), 0.0f, xev.xmotion.state ) );
				}
				break;
			case ClientMessage:
				if ( xev.xclient.data.l[0] == mAtomDeleteWindow )
				   mShouldQuit = true;
				break;

			case ResizeRequest:
				mWindowWidth = xev.xresizerequest.width;
				mWindowHeight = xev.xresizerequest.height;
				mApp->privateResize__( ResizeEvent( Vec2i( mWindowWidth, mWindowHeight ) ) );
				break;
			}
		}
	}

	killWindow( mFullScreen );
	mApp->privateShutdown__();
	delete mApp;
}

bool AppImplLinuxBasic::createWindow( int *width, int *height )
{
	Screen *screen = getDisplay()->getScreen();
	_XDisplay *dpy = getDisplay()->getXDisplay();
	int screenId = ::XScreenNumberOfScreen( screen );

	const char *title = "Cinder";

	if( *width <= 0 ) {
		*width = getDisplay()->getWidth();
		*height = getDisplay()->getHeight();
	}

	int x, y;
	if( mFullScreen ) {
		x = 0;
		y = 0;
	}
	else { // center the window on the display if windowed
		x = ( getDisplay()->getWidth() - *width ) / 2;
		y = ( getDisplay()->getHeight() - *height ) / 2;
	}

	mWin = ::XCreateSimpleWindow( dpy, RootWindow( dpy, screenId ),
			x, y, *width, *height, 1,
			BlackPixel( dpy, screenId ), WhitePixel( dpy, screenId ) );
	::XSelectInput( dpy, mWin,
					ButtonPressMask |
					ButtonReleaseMask |
					ExposureMask |
					KeyPressMask |
					KeyReleaseMask |
					PointerMotionMask );

	// Acquire atom to recognize window close events
	mAtomDeleteWindow = ::XInternAtom( dpy, "WM_DELETE_WINDOW", False );
	::XSetWMProtocols( dpy, mWin, &mAtomDeleteWindow, 1 );

	::XMapRaised( dpy, mWin );

	::XStoreName( dpy, mWin, title );

	mApp->getRenderer()->setup( mApp, dpy, mWin );

	return true;

	/*const char *title = "Cinder";
	int bits = 32;

	if( *width <= 0 ) {
		*width = ::GetSystemMetrics( SM_CXSCREEN );
		*height = ::GetSystemMetrics( SM_CYSCREEN );
	}

	WNDCLASS	wc;						// Windows Class Structure
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	if( mFullScreen ) {
		WindowRect.left = 0L;
		WindowRect.right = (long)*width;
		WindowRect.top = 0L;
		WindowRect.bottom = (long)*height;
	}
	else { // center the window on the display if windowed
		WindowRect.left = ( getDisplay()->getWidth() - *width ) / 2;
		WindowRect.right = ( getDisplay()->getWidth() - *width ) / 2 + *width;
		WindowRect.top = ( getDisplay()->getHeight() - *height ) / 2;
		WindowRect.bottom = ( getDisplay()->getHeight() - *height ) / 2 + *height;
	}

	mInstance			= ::GetModuleHandle( NULL );				// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= WndProc;						// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= mInstance;							// Set The Instance
	wc.hIcon			= ::LoadIcon( NULL, IDI_WINLOGO );		// Load The Default Icon
	wc.hCursor			= ::LoadCursor( NULL, IDC_ARROW );		// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= ( mFullScreen ) ? FULLSCREEN_WIN_CLASS_NAME : WINDOWED_WIN_CLASS_NAME;

	if( ! ::RegisterClass( &wc ) ) {								// Attempt To Register The Window Class
		DWORD err = ::GetLastError();
		return false;
	}

	if( mFullScreen ) {
		DEVMODE dmScreenSettings;
		memset( &dmScreenSettings, 0, sizeof(dmScreenSettings) );	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize = sizeof( dmScreenSettings );
		dmScreenSettings.dmPelsWidth	= *width;
		dmScreenSettings.dmPelsHeight	= *height;
		dmScreenSettings.dmBitsPerPel	= bits;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_mFullscreen Gets Rid Of Start Bar.
		if( ::ChangeDisplaySettings( &dmScreenSettings, CDS_FULLSCREEN ) != DISP_CHANGE_SUCCESSFUL )
			return false;

		mWindowExStyle = WS_EX_APPWINDOW;								// Window Extended Style
		mWindowStyle = WS_POPUP;										// Windows Style
		::ShowCursor( TRUE );										// Hide Mouse Pointer
	}
	else {
		mWindowExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		mWindowStyle = ( mApp->getSettings().isResizable() ) ? WS_OVERLAPPEDWINDOW
			:	( WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME );							// Windows Style
	}

	::AdjustWindowRectEx( &WindowRect, mWindowStyle, FALSE, mWindowExStyle );		// Adjust Window To True Requested Size

	wchar_t unicodeTitle[1024];
	wsprintfW( unicodeTitle, L"%S", title );

	// Create The Window
	if( ! ( mWnd = ::CreateWindowEx( mWindowExStyle,						// Extended Style For The Window
		( mFullScreen ) ? FULLSCREEN_WIN_CLASS_NAME : WINDOWED_WIN_CLASS_NAME,
		unicodeTitle,						// Window Title

		WindowRect.left, WindowRect.top,								// Window Position
		WindowRect.right-WindowRect.left,	// Calculate Window Width
		WindowRect.bottom-WindowRect.top,	// Calculate Window Height
		NULL,								// No Parent Window
		NULL,								// No Menu
		mInstance,							// Instance
		reinterpret_cast<LPVOID>( this ) )) )

	{
		//killWindow();							// Reset The Display
		return false;
	}

	mDC = ::GetDC( mWnd );
	if( ! mDC ) {
		killWindow( mFullScreen );
		return false;
	}

	mApp->getRenderer()->setup( mApp, mWnd, mDC );

	return true;									// Success
	*/
}

void AppImplLinuxBasic::killWindow( bool wasFullScreen )
{
	mApp->getRenderer()->kill();
	XDestroyWindow( getDisplay()->getXDisplay(), mWin );

	/*mApp->getRenderer()->kill();

	if( wasFullScreen )
		::ChangeDisplaySettings( NULL, 0 );

	if( mDC )
		::ReleaseDC( mWnd, mDC );

	if( mWnd )
		::DestroyWindow( mWnd );

	if( wasFullScreen )
		::UnregisterClass( FULLSCREEN_WIN_CLASS_NAME, mInstance );
	else
		::UnregisterClass( WINDOWED_WIN_CLASS_NAME, mInstance );

	mWnd = 0;*/
}

void AppImplLinuxBasic::toggleFullScreen()
{
	bool prevFullScreen = mFullScreen;
	mFullScreen = ! mFullScreen;
	int windowWidth, windowHeight;
	if( mApp->isFullScreen() ) {
		windowWidth = mDisplay->getWidth();
		windowHeight = mDisplay->getHeight();
		mWindowedPos = mWindowOffset;
	}
	else {
		windowWidth = mApp->getSettings().getWindowWidth();
		windowHeight = mApp->getSettings().getWindowHeight();
	}
	//TODO: Do we need to call these functions?
	//mApp->getRenderer()->prepareToggleFullScreen();
    //mApp->getRenderer()->finishToggleFullScreen();
	mWindowWidth = windowWidth;
	mWindowHeight = windowHeight;
	mApp->privateResize__(
		ResizeEvent( Vec2i( mApp->getWindowWidth(),
		                    mApp->getWindowHeight() ) ) );
	_XDisplay *dpy = getDisplay()->getXDisplay();
	XEvent xev;
	Atom wm_state = XInternAtom(dpy, "_NET_WM_STATE", False);
    Atom fullscreen = XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", False);
	xev.type = ClientMessage;
	xev.xclient.window = mWin;
	xev.xclient.message_type = wm_state;
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = 2;
	xev.xclient.data.l[1] = fullscreen;
	xev.xclient.data.l[2] = 0;
	XMapWindow(dpy, mWin);
	XSendEvent(dpy, DefaultRootWindow(dpy), 0,
	            SubstructureRedirectMask | SubstructureNotifyMask, &xev);
}

void AppImplLinuxBasic::setWindowWidth( int aWindowWidth )
{
	::XResizeWindow( mDisplay->getXDisplay(), mWin, aWindowWidth, mApp->getWindowHeight() );
}

void AppImplLinuxBasic::setWindowHeight( int aWindowHeight )
{
	::XResizeWindow( mDisplay->getXDisplay(), mWin, mApp->getWindowWidth(), aWindowHeight );
}

void AppImplLinuxBasic::setWindowSize( int aWindowWidth, int aWindowHeight )
{
	::XResizeWindow( mDisplay->getXDisplay(), mWin, aWindowHeight, aWindowHeight );
}

float AppImplLinuxBasic::setFrameRate( float aFrameRate )
{
	return aFrameRate;
}


/*
unsigned int prepMouseEventModifiers( WPARAM wParam )
{
	unsigned int result = 0;
	if( wParam & MK_CONTROL ) result |= MouseEvent::CTRL_DOWN;
	if( wParam & MK_LBUTTON ) result |= MouseEvent::LEFT_DOWN;
	if( wParam & MK_MBUTTON ) result |= MouseEvent::MIDDLE_DOWN;
	if( wParam & MK_RBUTTON ) result |= MouseEvent::RIGHT_DOWN;
	if( wParam & MK_SHIFT ) result |= MouseEvent::SHIFT_DOWN;
	if( ::GetKeyState( VK_MENU ) < 0 ) result |= MouseEvent::ALT_DOWN;
	if( (::GetKeyState( VK_LWIN ) < 0) || (::GetKeyState( VK_RWIN ) < 0) ) result |= MouseEvent::META_DOWN;
	return result;
}

// Certain key codes need to be refined, for example VK_MENU needs to be
// converted into VK_LALT or VK_RALT
int prepNativeKeyCode( WPARAM wParam )
{
	unsigned int result = (int)wParam;
	if( wParam == VK_MENU ) {
		result = ( ::GetKeyState( VK_RMENU ) ) ? VK_RMENU : VK_LMENU;
	}
	else if( wParam == VK_SHIFT ) {
		result = ( ::GetKeyState( VK_RSHIFT ) ) ? VK_RSHIFT : VK_LSHIFT;
	}
	else if( wParam == VK_CONTROL ) {
		result = ( ::GetKeyState( VK_RCONTROL ) ) ? VK_RCONTROL : VK_LCONTROL;
	}
	return result;
}

char mapVirtualKey( WPARAM wParam )
{
	BYTE keyboardState[256];
	::GetKeyboardState( keyboardState );
	WORD result[4];

	int resultLength = ::ToAscii( wParam, ::MapVirtualKey( wParam, 0 ), keyboardState, result, 0 );
	if( resultLength == 1 )
		return (char)result[0];
	else
		return 0;
}

unsigned int prepKeyEventModifiers()
{
	unsigned int result = 0;
	if( ::GetKeyState( VK_CONTROL ) & 0x8000 ) result |= KeyEvent::CTRL_DOWN;
	if( ::GetKeyState( VK_SHIFT ) & 0x8000 ) result |= KeyEvent::SHIFT_DOWN;
	if( ( ::GetKeyState( VK_LMENU ) & 0x8000 ) || ( ::GetKeyState( VK_RMENU ) & 0x8000 ) ) result |= KeyEvent::ALT_DOWN;
	if( ( ::GetKeyState( VK_LWIN ) < 0 ) || ( ::GetKeyState( VK_RWIN ) < 0 ) ) result |= KeyEvent::META_DOWN;
	return result;
}

extern "C" {
LRESULT CALLBACK WndProc(	HWND	mWnd,			// Handle For This Window
							UINT	uMsg,			// Message For This Window
							WPARAM	wParam,			// Additional Message Information
							LPARAM	lParam)			// Additional Message Information
{
	AppImplLinuxBasic* impl = 0;

	// if the message is WM_NCCREATE we need to hide 'this' in the window long
	if( uMsg == WM_NCCREATE ) {
		impl = reinterpret_cast<AppImplLinuxBasic*>(((LPCREATESTRUCT)lParam)->lpCreateParams);
		::SetWindowLongPtr( mWnd, GWL_USERDATA, (__int3264)(LONG_PTR)impl );
	}
	else // the warning on this line is harmless
		impl = reinterpret_cast<AppImplLinuxBasic*>( ::GetWindowLongPtr( mWnd, GWL_USERDATA ) );

	if( ! impl )
		return DefWindowProc( mWnd, uMsg, wParam, lParam );

	switch( uMsg ) {									// Check For Windows Messages
		case WM_SYSCOMMAND:							// Intercept System Commands
			switch( wParam ) {							// Check System Calls
				case SC_SCREENSAVE:					// Screensaver Trying To Start?
				case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
					if( impl->getApp()->getSettings().getPowerManagement() )
						return 0;							// prevent
			}
		break;
		case WM_CLOSE:								// Did We Receive A Close Message?
			::PostQuitMessage(0);					// Send A Quit Message
			impl->quit();
			return 0;
		break;
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:							// Is A Key Being Held Down?
			impl->getApp()->privateKeyDown__( KeyEvent( KeyEvent::translateNativeKeyCode( prepNativeKeyCode( (int)wParam ) ),
					mapVirtualKey( wParam ), prepKeyEventModifiers(), (int)wParam ) );
			return 0;								// Jump Back
		break;
		case WM_SYSKEYUP:
		case WM_KEYUP:								// Has A Key Been Released?
			impl->getApp()->privateKeyUp__( KeyEvent( KeyEvent::translateNativeKeyCode( prepNativeKeyCode( (int)wParam ) ),
					mapVirtualKey( wParam ), prepKeyEventModifiers(), (int)wParam ) );
			return 0;								// Jump Back
		break;

		// mouse events
		case WM_LBUTTONDOWN:
			::SetCapture( mWnd );
			impl->mIsDragging = true;
			impl->getApp()->privateMouseDown__( MouseEvent( MouseEvent::LEFT_DOWN, LOSHORT(lParam), HISHORT(lParam), prepMouseEventModifiers( wParam ), 0.0f, static_cast<unsigned int>( wParam ) ) );
			return 0;
		break;
		case WM_RBUTTONDOWN:
			::SetCapture( mWnd );
			impl->mIsDragging = true;
			impl->getApp()->privateMouseDown__( MouseEvent( MouseEvent::RIGHT_DOWN, LOSHORT(lParam), HISHORT(lParam), prepMouseEventModifiers( wParam ), 0.0f, static_cast<unsigned int>( wParam ) ) );
			return 0;
		break;
		case WM_MBUTTONDOWN:
			::SetCapture( mWnd );
			impl->mIsDragging = true;
			impl->getApp()->privateMouseDown__( MouseEvent( MouseEvent::MIDDLE_DOWN, LOSHORT(lParam), HISHORT(lParam), prepMouseEventModifiers( wParam ), 0.0f, static_cast<unsigned int>( wParam ) ) );
			return 0;
		break;
		case WM_LBUTTONUP:
			::ReleaseCapture();
			impl->mIsDragging = false;
			impl->getApp()->privateMouseUp__( MouseEvent( MouseEvent::LEFT_DOWN, LOSHORT(lParam), HISHORT(lParam), prepMouseEventModifiers( wParam ), 0.0f, static_cast<unsigned int>( wParam ) ) );
			return 0;
		break;
		case WM_RBUTTONUP:
			::ReleaseCapture();
			impl->mIsDragging = false;
			impl->getApp()->privateMouseUp__( MouseEvent( MouseEvent::RIGHT_DOWN, LOSHORT(lParam), HISHORT(lParam), prepMouseEventModifiers( wParam ), 0.0f, static_cast<unsigned int>( wParam ) ) );
			return 0;
		break;
		case WM_MBUTTONUP:
			::ReleaseCapture();
			impl->mIsDragging = false;
			impl->getApp()->privateMouseUp__( MouseEvent( MouseEvent::MIDDLE_DOWN, LOSHORT(lParam), HISHORT(lParam), prepMouseEventModifiers( wParam ), 0.0f, static_cast<unsigned int>( wParam ) ) );
			return 0;
		break;
		case WM_MOUSEWHEEL:
			impl->getApp()->privateMouseWheel__( MouseEvent( 0, LOSHORT(lParam), HISHORT(lParam), prepMouseEventModifiers( wParam ),
					GET_WHEEL_DELTA_WPARAM( wParam ) / 120.0f, static_cast<unsigned int>( wParam ) ) );
		break;
		case WM_KILLFOCUS:
			// if we lose capture during a drag, post a mouseup event as a notifier
			if( impl->mIsDragging ) {
				impl->getApp()->privateMouseUp__( MouseEvent( 0, LOSHORT(lParam), HISHORT(lParam), prepMouseEventModifiers( wParam ), 0.0f, static_cast<unsigned int>( wParam ) ) );
			}
			impl->mIsDragging = false;
		break;
		case WM_MOUSEMOVE:
			if( impl->mIsDragging ) {
				impl->getApp()->privateMouseDrag__( MouseEvent( 0, LOSHORT(lParam), HISHORT(lParam), prepMouseEventModifiers( wParam ),
						0.0f, static_cast<unsigned int>( wParam ) ) );
}
			else
				impl->getApp()->privateMouseMove__( MouseEvent( 0, LOSHORT(lParam), HISHORT(lParam), prepMouseEventModifiers( wParam ),
						0.0f, static_cast<unsigned int>( wParam ) ) );
		break;
		case WM_SIZE:
			if( impl->mHasBeenInitialized ) {
				impl->mWindowWidth = LOWORD(lParam);
				impl->mWindowHeight = HIWORD(lParam);
				impl->getApp()->privateResize__( impl->mWindowWidth, impl->mWindowHeight );
			}
			return 0;
		break;
		case WM_MOVE:
			if( impl->mHasBeenInitialized ) {
				impl->privateSetWindowOffset__( Vec2i( LOSHORT(lParam), HISHORT(lParam) ) );
			}
			return 0;
		break;
		case WM_DROPFILES: {
			HDROP dropH = (HDROP)wParam;
			POINT dropPoint;
			char fileName[8192];
			vector<string> files;

			int droppedFileCount = ::DragQueryFile( dropH, 0xFFFFFFFF, 0, 0 );
			for( int i = 0; i < droppedFileCount; ++i ) {
				::DragQueryFileA( dropH, i, fileName, 8192 );
				files.push_back( std::string( fileName ) );
			}

			::DragQueryPoint( dropH, &dropPoint );
			::DragFinish( dropH );

			FileDropEvent dropEvent( dropPoint.x, dropPoint.y, files );
			impl->getApp()->privateFileDrop__( dropEvent );
			return 0;
		}
		break;
		case WM_PAINT:
			impl->getApp()->getRenderer()->startDraw();
			impl->getApp()->draw();
			impl->getApp()->getRenderer()->finishDraw();
		break;
	}

	// unhandled messages To DefWindowProc
	return DefWindowProc( mWnd, uMsg, wParam, lParam );
}
} // extern "C"*/

} } // namespace cinder::app
