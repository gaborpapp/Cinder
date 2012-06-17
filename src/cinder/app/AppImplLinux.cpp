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

#include <X11/Xlib.h>

#include "cinder/Display.h"
#include "cinder/app/AppImplLinux.h"
#include "cinder/app/App.h"

using std::string;
using std::vector;
using std::pair;

namespace cinder { namespace app {

void AppImplLinux::hideCursor()
{
	//::ShowCursor( false );
}

void AppImplLinux::showCursor()
{
	//::ShowCursor( true );
}

Vec2i AppImplLinux::mouseLocation()
{
	_XDisplay *dpy = Display::getXDisplay();
	Window rootReturn, childReturn;
	int rootXReturn, rootYReturn, winXReturn, winYReturn;
	unsigned int maskReturn;

	::XQueryPointer( dpy, DefaultRootWindow( dpy ),
			&rootReturn, &childReturn,
			&rootXReturn, &rootYReturn,
			&winXReturn, &winYReturn,
			&maskReturn );

	return Vec2i( rootXReturn, rootYReturn );
}

Buffer AppImplLinux::loadResource( int id, const std::string &type )
{
	void *dataPtr=NULL;
	size_t dataSize=0;

	return Buffer( dataPtr, dataSize );
}

std::string AppImplLinux::getAppPath()
{
	char appPath[1000] = "";

	// fetch the path of the executable
	//::GetModuleFileNameA( 0, appPath, sizeof(appPath) - 1);

	// get a pointer to the last occurrence of the windows path separator
	char *appDir = strrchr( appPath, '\\' );
	if( appDir ) {
		++appDir;

		// always expect the unexpected - this shouldn't be null but one never knows
		if( appDir ) {
			// null terminate the string
			*appDir = 0;
		}
	}

	return std::string( appPath );
}

string AppImplLinux::getOpenFilePath( const string &initialPath, vector<string> extensions )
{
	/*OPENFILENAMEA ofn;       // common dialog box structure
	char szFile[260];       // buffer for file name

	// Initialize OPENFILENAME
	::ZeroMemory( &ofn, sizeof(ofn) );
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = App::get()->getRenderer()->getHwnd();
	ofn.lpstrFile = szFile;

	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not
	// use the contents of szFile to initialize itself.
	//
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof( szFile );
	if( extensions.empty() ) {
		ofn.lpstrFilter = "All\0*.*\0";
	}
	else {
		char extensionStr[10000];
		size_t offset = 0;

		strcpy( extensionStr, "Supported Types" );
		offset += strlen( extensionStr ) + 1;
		for( vector<string>::const_iterator strIt = extensions.begin(); strIt != extensions.end(); ++strIt ) {
			strcpy( extensionStr + offset, "*." );
			offset += 2;
			strcpy( extensionStr + offset, strIt->c_str() );
			offset += strIt->length();
			// append a semicolon to all but the last extensions
			if( strIt + 1 != extensions.end() ) {
				extensionStr[offset] = ';';
				offset += 1;
			}
			else {
				extensionStr[offset] = 0;
				offset += 1;
			}
		}

		extensionStr[offset] = 0;
		ofn.lpstrFilter = extensionStr;
	}
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	if( initialPath.empty() )
		ofn.lpstrInitialDir = NULL;
	else {
		char initialPathStr[MAX_PATH];
		strcpy( initialPathStr, initialPath.c_str() );
		ofn.lpstrInitialDir = initialPathStr;
	}
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Display the Open dialog box.
	if( ::GetOpenFileNameA( &ofn ) == TRUE ) {
		return string( ofn.lpstrFile );
	}
	else*/
		return string();
}

string AppImplLinux::getSaveFilePath( const string &initialPath, vector<string> extensions )
{
	/*OPENFILENAMEA ofn;       // common dialog box structure
	char szFile[260];       // buffer for file name

	// Initialize OPENFILENAME
	ZeroMemory( &ofn, sizeof(ofn) );
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = App::get()->getRenderer()->getHwnd();
	ofn.lpstrFile = szFile;

	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not
	// use the contents of szFile to initialize itself.
	//
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not
	// use the contents of szFile to initialize itself.
	//
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof( szFile );
	if( extensions.empty() ) {
		ofn.lpstrFilter = "All\0*.*\0";
	}
	else {
		char extensionStr[10000];
		size_t offset = 0;

		strcpy( extensionStr, "Supported Types" );
		offset += strlen( extensionStr ) + 1;
		for( vector<string>::const_iterator strIt = extensions.begin(); strIt != extensions.end(); ++strIt ) {
			strcpy( extensionStr + offset, "*." );
			offset += 2;
			strcpy( extensionStr + offset, strIt->c_str() );
			offset += strIt->length();
			// append a semicolon to all but the last extensions
			if( strIt + 1 != extensions.end() ) {
				extensionStr[offset] = ';';
				offset += 1;
			}
			else {
				extensionStr[offset] = 0;
				offset += 1;
			}
		}

		extensionStr[offset] = 0;
		ofn.lpstrFilter = extensionStr;
	}
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	if( initialPath.empty() )
		ofn.lpstrInitialDir = NULL;
	else {
		char initialPathStr[MAX_PATH];
		strcpy( initialPathStr, initialPath.c_str() );
		ofn.lpstrInitialDir = initialPathStr;
	}
	ofn.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT;

	// Display the Open dialog box.
	string result;
	if( ::GetSaveFileNameA( &ofn ) == TRUE ) {
		result = string( ofn.lpstrFile );
	}*/

	return string();
}

} } // namespace cinder::app
