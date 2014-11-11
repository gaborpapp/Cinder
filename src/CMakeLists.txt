set(CINDER_SOURCE Area.cpp
				  AxisAlignedBox.cpp
				  BSpline.cpp
				  BSplineFit.cpp  
		 		  BandedMatrix.cpp
				  Buffer.cpp
				  Camera.cpp 			 
				  Channel.cpp
				  CinderMath.cpp
				  Color.cpp
				  DataSource.cpp  
		          DataTarget.cpp
				  Exception.cpp
				  Frustum.cpp  
				  ImageIo.cpp  
				  Matrix.cpp
				  ObjLoader.cpp  
				  Path2d.cpp
				  Perlin.cpp
				  Plane.cpp
				  PolyLine.cpp
				  Rand.cpp
				  Ray.cpp
				  Rect.cpp  
				  Serial.cpp
				  Shape2d.cpp
				  Sphere.cpp
				  Stream.cpp
				  Surface.cpp
				  System.cpp  
				  Text.cpp
				  Timer.cpp
				  Triangulate.cpp
				  TriMesh.cpp
				  Url.cpp
				  Xml.cpp  
				  Capture.cpp
				  Display.cpp Font.cpp  
				  Timeline.cpp
				  TimelineItem.cpp
				  Tween.cpp  
				  Utilities.cpp
				  UrlImplCurl.cpp
				  Clipboard.cpp)

string (REGEX REPLACE "^([^;]*);" "cinder/\\1;" CINDER_SOURCE "${CINDER_SOURCE}")
string (REGEX REPLACE ";([^;]*)$" ";cinder/\\1" CINDER_SOURCE "${CINDER_SOURCE}")

set(CAIRO_SOURCE  cinder/cairo/Cairo.cpp)
set(PARAMS_SOURCE cinder/params/Params.cpp)
set(IP_SOURCE Blend.cpp
			  EdgeDetect.cpp
			  Fill.cpp
			  Flip.cpp
			  Grayscale.cpp
		      Hdr.cpp
		   	  Premultiply.cpp
			  Resize.cpp
			  Threshold.cpp
			  Trim.cpp)
string (REGEX REPLACE "^([^;]*);" "cinder/ip/\\1;" IP_SOURCE "${IP_SOURCE}")
string (REGEX REPLACE ";([^;]*)$" ";cinder/ip/\\1" IP_SOURCE "${IP_SOURCE}")

set(SVG_SOURCE cinder/svg/Svg.cpp)
set(GL_SOURCE DisplayList.cpp
			  Fbo.cpp
			  GLee.c
			  GlslProg.cpp
			  Light.cpp 
		      Material.cpp
			  Texture.cpp
			  TextureFont.cpp
			  TileRender.cpp
			  Vbo.cpp
			  gl.cpp)
string (REGEX REPLACE "^([^;]*);" "cinder/gl/\\1;" GL_SOURCE "${GL_SOURCE}")
string (REGEX REPLACE ";([^;]*)$" ";cinder/gl/\\1" GL_SOURCE "${GL_SOURCE}")

set(APP_SOURCE App.cpp
			   AppBasic.cpp
			   KeyEvent.cpp
			   Renderer.cpp
			   AppImplLinux.cpp 
			   AppImplLinuxBasic.cpp 
			   AppImplLinuxRendererGl.cpp)
string (REGEX REPLACE "^([^;]*);" "cinder/app/\\1;" APP_SOURCE "${APP_SOURCE}")
string (REGEX REPLACE ";([^;]*)$" ";cinder/app/\\1" APP_SOURCE "${APP_SOURCE}")

set(ANTTWEAK_SOURCE LoadOGL.cpp
					TwBar.cpp
					TwColors.cpp
					TwFonts.cpp
					TwMgr.cpp
					TwOpenGL.cpp
					TwPrecomp.cpp)			   
string (REGEX REPLACE "^([^;]*);" "AntTweakBar/\\1;" ANTTWEAK_SOURCE "${ANTTWEAK_SOURCE}")
string (REGEX REPLACE ";([^;]*)$" ";AntTweakBar/\\1" ANTTWEAK_SOURCE "${ANTTWEAK_SOURCE}")
					
set(LIBTESS_SOURCE bucketalloc.c
				   dict.c
				   geom.c
				   mesh.c
				   priorityq.c
				   sweep.c
				   tess.c)
string (REGEX REPLACE "^([^;]*);" "libtess2/\\1;" LIBTESS_SOURCE "${LIBTESS_SOURCE}")
string (REGEX REPLACE ";([^;]*)$" ";libtess2/\\1" LIBTESS_SOURCE "${LIBTESS_SOURCE}")
					   
set(ALL_SOURCE ${CINDER_SOURCE}
			   ${CAIRO_SOURCE}
			   ${PARAMS_SOURCE}
			   ${IP_SOURCE}
			   ${GL_SOURCE}
			   ${ANTTWEAK_SOURCE}
			   ${SVG_SOURCE}
			   ${LIBTESS_SOURCE}
			   ${APP_SOURCE})

find_package(Boost 1.53 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

#FIX for wrong cairo header path in src
find_path(CAIRO_HEADER cairo.h HINTS /usr/include/cairo) 
include_directories(${CAIRO_HEADER})

include_directories( ${CMAKE_SOURCE_DIR}/include)
include_directories("../include/rapidxml")
include_directories("../include/cinde/app")
include_directories("../include/cairo")
include_directories("../src/AntTweakBar")
include_directories("../src/libtess2")

add_library(libcinder ${ALL_SOURCE})
