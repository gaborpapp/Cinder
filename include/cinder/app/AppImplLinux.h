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

#pragma once

#include "cinder/Stream.h"
#include "cinder/Vector.h"
#include <string>
#include <vector>

namespace cinder { namespace app {

class AppImplLinux {
 public:
	AppImplLinux( class App *aApp ) : mApp( aApp ), mWindowOffset( 0, 0 ) {}
	virtual ~AppImplLinux() {}

	int				getWindowWidth() const { return mWindowWidth; }
	int				getWindowHeight() const { return mWindowHeight; }
	virtual void	setWindowWidth( int aWindowWidth ) { }
	virtual void	setWindowHeight( int aWindowHeight ) { }
	virtual void	setWindowSize( int aWindowWidth, int aWindowHeight ) {}
	float			getFrameRate() const { return mFrameRate; }
	virtual float	setFrameRate( float aFrameRate ) { return -1.0f; }
	bool			isFullScreen() const { return mFullScreen; }
	virtual void	toggleFullScreen() {}
	virtual Vec2i	mouseLocation();
	virtual void	quit() = 0;

	virtual void	privateSetWindowOffset__( const Vec2i &aWindowOffset ) { mWindowOffset = aWindowOffset; }


	static void	hideCursor();
	static void	showCursor();

	static Buffer	loadResource( int id, const std::string &type );

	static std::string	getAppPath();
	static std::string	getOpenFilePath( const std::string &initialPath, std::vector<std::string> extensions );
	static std::string	getSaveFilePath( const std::string &initialPath, std::vector<std::string> extensions );

 protected:
	class App	*mApp;
	int			mWindowWidth, mWindowHeight;
	bool		mFullScreen, mBorderless, mAlwaysOnTop;
	Vec2i		mWindowOffset;
	float		mFrameRate;
};

} } // namespace cinder::app
