/*
 Copyright (c) 2012, The Cinder Project, All rights reserved.

 This code is intended for use with the Cinder C++ library: http://libcinder.org

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

#include "cinder/app/android/AppImplAndroid.h"
#include "cinder/app/android/EventManagerAndroid.h"
#include "cinder/app/android/WindowImplAndroid.h"
#include "cinder/app/AppBase.h"
#include <sys/types.h>
#include <unistd.h>
#include <fstream>
#include <sstream>

namespace cinder { namespace app {

AppImplAndroid::AppImplAndroid( AppAndroid *aApp, const AppAndroid::Settings &settings )
	: mApp( aApp ), mSetupHasBeenCalled( false ), mCanProcessEvents( false ), mActive( false ) //, mFocused( false )
{
	EventManagerAndroid::instance()->setAppImplInst( this );
	mNativeApp = EventManagerAndroid::instance()->getNativeApp();

	mFrameRate = settings.getFrameRate();
	mFrameRateEnabled = settings.isFrameRateEnabled();
	mQuitOnLastWindowClosed = settings.isQuitOnLastWindowCloseEnabled();

	auto formats = settings.getWindowFormats();
	if( formats.empty() ) {
		formats.push_back( settings.getDefaultWindowFormat() );
	}

	for( auto &format : formats ) {
		if( ! format.isTitleSpecified() ) {
			format.setTitle( settings.getTitle() );
		}

		createWindow( format );
	}

	// Set the active window
	if( ! mWindows.empty() ) {
		setWindow( mWindows.back()->getWindow() );
	}	
}

AppImplAndroid::~AppImplAndroid()
{
}

AppAndroid* AppImplAndroid::getApp() 
{ 
	return mApp; 
}

struct android_app *AppImplAndroid::getNative()
{
	return mNativeApp;
}

void AppImplAndroid::setup()
{
	mApp->privateSetup__();
	mSetupHasBeenCalled = true;

	// issue initial app activation event
	mApp->emitDidBecomeActive();

	for( auto &window : mWindows ) {
		window->resize();
	}

	// initialize our next frame time
	mNextFrameTime = getElapsedSeconds();

	// enable event processing
	mCanProcessEvents = true;
}

void AppImplAndroid::sleepUntilNextFrame()
{
	// get current time in seconds
	double currentSeconds = getElapsedSeconds();

	// calculate time per frame in seconds
	double secondsPerFrame = 1.0 / mFrameRate;

	// determine if application was frozen for a while and adjust next frame time
	double elapsedSeconds = currentSeconds - mNextFrameTime;
	if( elapsedSeconds > 1.0 ) {
		int numSkipFrames = (int)(elapsedSeconds / secondsPerFrame);
		mNextFrameTime += (numSkipFrames * secondsPerFrame);
	}

	// determine when next frame should be drawn
	mNextFrameTime += secondsPerFrame;

	// sleep and process messages until next frame
	if( ( mFrameRateEnabled ) && ( mNextFrameTime > currentSeconds ) ) {
		sleep(mNextFrameTime - currentSeconds);
	}	
}

void AppImplAndroid::updateAndDraw()
{
	mApp->privateUpdate__();
	for( auto &window : mWindows ) {
		window->draw();
	}
}

WindowRef AppImplAndroid::createWindow( Window::Format format )
{
	if( ! format.getRenderer() ) {
		format.setRenderer( mApp->getDefaultRenderer()->clone() );
	}

	mWindows.push_back( new WindowImplAndroid( mNativeApp->window, format, nullptr, this ) );

	// emit initial resize if we have fired setup
	if( mSetupHasBeenCalled ) {
		mWindows.back()->getWindow()->emitResize();
	}

	return mWindows.back()->getWindow();
}

void AppImplAndroid::quit()
{
	// Close all windows, forcing the application to quit.
	while( ! mWindows.empty() )
		mWindows.back()->close();

	// Always quit, even if ! isQuitOnLastWindowCloseEnabled()
	EventManagerAndroid::instance()->setShouldQuit( true );
}

void AppImplAndroid::sleep( double seconds )
{
	useconds_t milliseconds = (useconds_t)(seconds*1000.0);
	useconds_t microsecs = milliseconds * 1000;
	::usleep( microsecs );	    
}

bool AppImplAndroid::setupHasBeenCalled() const 
{ 
	return mSetupHasBeenCalled; 
}

float AppImplAndroid::getFrameRate() const 
{ 
	return mFrameRate; 
}

void AppImplAndroid::setFrameRate( float frameRate )
{
	mFrameRate = frameRate;
	mFrameRateEnabled = true;
	mNextFrameTime = mApp->getElapsedSeconds();
}

void AppImplAndroid::disableFrameRate()
{
	mFrameRateEnabled = false;
}

bool AppImplAndroid::isFrameRateEnabled() const
{
	return mFrameRateEnabled;
}

RendererRef AppImplAndroid::findSharedRenderer( const RendererRef &searchRenderer )
{
	if( ! searchRenderer )
		return RendererRef();

	for( const auto &win : mWindows ) {
		RendererRef renderer = win->getRenderer();
		if( renderer && ( typeid(*renderer) == typeid(*searchRenderer) ) )
			return renderer;
	}

	return RendererRef();	
}

WindowRef AppImplAndroid::getWindow() const 
{ 
	return mActiveWindow; 
}

void AppImplAndroid::setWindow( WindowRef window ) 
{ 
	mActiveWindow = window; 
}

size_t AppImplAndroid::getNumWindows() const
{
	return mWindows.size();
}

WindowRef AppImplAndroid::getWindowIndex( size_t index ) const
{
	if( index >= mWindows.size() )
		return cinder::app::WindowRef();
	
	auto winIt = mWindows.begin();
	std::advance( winIt, index );
	return (*winIt)->mWindowRef;
}

WindowRef AppImplAndroid::getForegroundWindow() const
{
	return mForegroundWindow;
}

void AppImplAndroid::setForegroundWindow( WindowRef window )
{
	mForegroundWindow = window;
}

void AppImplAndroid::closeWindow( WindowImplAndroid *windowImpl )
{
	auto winIt = find( mWindows.begin(), mWindows.end(), windowImpl );
	if( winIt != mWindows.end() ) {
		windowImpl->getWindow()->emitClose();
		windowImpl->privateClose();
		delete windowImpl; // this corresponds to winIt
		mWindows.erase( winIt );
	}

	if( mWindows.empty() && mQuitOnLastWindowClosed ) {
		EventManagerAndroid::instance()->setShouldQuit( true );
	}
}

void AppImplAndroid::hideCursor()
{
	throw (std::string(__FUNCTION__) + " not implemented yet").c_str();
}

void AppImplAndroid::showCursor()
{
	throw (std::string(__FUNCTION__) + " not implemented yet").c_str();
}

ivec2 AppImplAndroid::getMousePos() const
{
	throw (std::string(__FUNCTION__) + " not implemented yet").c_str();

	return ivec2( -1, -1 );	
}

fs::path AppImplAndroid::getAppPath()
{
	fs::path result;

	pid_t pid = getpid();
	std::stringstream ss;
	ss << "/proc/" << pid << "/cmdline";

	std::string procPath = ss.str();

	std::ifstream is( procPath.c_str() );
	if( is.is_open() ) {
		std::vector<char> buf;
		
		char c;
		while( is.get( c ) ) {
			buf.push_back( c );

			if( 0 == c ) {
				break;
			}
		}

		if( ! buf.empty() ) {
			result = std::string( (const char *)(&buf[0]), buf.size() );
		}
	}	

	return result;
}

fs::path AppImplAndroid::getOpenFilePath( const fs::path &initialPath, std::vector<std::string> extensions )
{
	throw (std::string(__FUNCTION__) + " not implemented yet").c_str();

	return fs::path();
}

fs::path AppImplAndroid::getSaveFilePath( const fs::path &initialPath, std::vector<std::string> extensions )
{
	throw (std::string(__FUNCTION__) + " not implemented yet").c_str();

	return fs::path();
}

fs::path AppImplAndroid::getFolderPath( const fs::path &initialPath )
{
	throw (std::string(__FUNCTION__) + " not implemented yet").c_str();

	return fs::path();
}

} } // namespace cinder::app
