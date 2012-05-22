#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Unicode.h"
#include "cinder/Text.h"
#include "cinder/gl/Texture.h"

using namespace ci;
using namespace ci::app;
using namespace std;

Font font;
Vec2f pos;
int maxWidth = 230;

class LineBreakTestApp : public AppNative {
  public:
	void setup();
	void mouseDrag( MouseEvent event );	
	void update();
	void draw();
}; 

bool lineWidth( const char *s, size_t len, size_t maxLen )
{
//return len <= 6;
	TextBox box;
	box.setFont( font );
	box.setText( string( s, len ) );
	return box.measure().x < maxLen;
}

void print( const char *s, size_t len )
{
/*	size_t lenBeforeBreak = 0;
	bool found = false;
	while( lenBeforeBreak < len ) {
		size_t tempLen = lenBeforeBreak;
		uint32_t nextChar = nextCharUtf8( s, &tempLen, len );
		if( nextChar == '\r' || nextChar == '\n' ) {
			found = true;
			break;
		}
		lenBeforeBreak = tempLen;
	}
	app::console() << "{" << string( s, lenBeforeBreak ) << ( (found)?"!":"" ) << "}" << std::endl;*/
	
	gl::drawString( string( s, len ), pos, ColorA::white(), font );
int asc = font.getAscent();
int desc = font.getDescent();
int lead = font.getLeading();
	pos.y += font.getLeading() + font.getAscent() + font.getDescent();
}

void LineBreakTestApp::setup()
{
	font = Font( "Arial", 24 );

//	const char *s = "just some stuff\n\nthat\nis longer than one line";
}

void LineBreakTestApp::mouseDrag( MouseEvent event )
{
	maxWidth = event.getPos().x;
}

void LineBreakTestApp::update()
{
}

void LineBreakTestApp::draw()
{
	// clear out the window with black
	gl::enableAlphaBlending();
	gl::clear( Color( 0, 0, 0 ) ); 
	gl::color( ColorA::white() );

	pos.x = 0;
	pos.y = 10;
//	const char *s = "a just.";
	const char *s = "just some stuff\n\nthat\nis longer than one line. I mean substantially longer...";
//	const char *s = "消費増税\n\n法案をめぐる事前事前事前審査を行っていた民主党税調などの合同総会は２８日未明、「名目３％程度、実質２％程度」の経済成長率の数値目標を付則に盛り込んだ新たな修正案を了承し、前原誠司政調会長に対応を一任した。野内閣は３０日に閣議決定を行う。";
//	for( size_t l = 1; l < 30; ++l ) {
	//	std::cout << l << ":" << std::endl;
		lineBreakUtf8( s,std::bind( lineWidth, _1, _2, maxWidth ), print );
//	}
	gl::color( ColorA( 1, 0, 0, 0.5 ) );
	TextBox box;
	box.setSize( Vec2i( maxWidth, TextBox::GROW ) );
	box.setFont( font );
	box.setText( s );
	gl::Texture t = box.render();
	gl::draw( t );
	gl::color( Color( 0, 1, 0 ) );
	gl::drawStrokedRect( Rectf( 0, 10, maxWidth, 800 ) );
}

CINDER_APP_NATIVE( LineBreakTestApp, RendererGl )