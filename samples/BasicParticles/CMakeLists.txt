set(SAMPLE_SOURCE src/BasicParticleApp.cpp)

message(${CMAKE_CURRENT_SOURCE_DIR})
message(${CMAKE_SOURCE_DIR})
message(${PROJECT_SOURCE_DIR})
message(${PROJECT_BINARY_DIR})

include_directories("${CMAKE_SOURCE_DIR}/include")
include_directories("${CMAKE_SOURCE_DIR}/boost/include")
include_directories("${CMAKE_SOURCE_DIR}/include/rapidxml")
include_directories("${CMAKE_SOURCE_DIR}/include/cinde/app")
include_directories("${CMAKE_SOURCE_DIR}/include/cairo")

find_library(CURL_LIB curl)
find_library(CINDER_LIB libcinder PATHS ${CMAKE_BINARY_DIR}/src)
find_package(OpenGL REQUIRED)
find_package(Boost 1.53 REQUIRED thread system filesystem)
find_package(ZLIB REQUIRED)

if(!CINDER_LIB-NOTFOUND)
	add_executable(BasicParticleApp ${SAMPLE_SOURCE})
	target_link_libraries(BasicParticleApp ${CINDER_LIB} ${Boost_LIBRARIES}
									   ${OPENGL_LIBRARIES}
									   ${CURL_LIB}
									   ${ZLIB_LIBRARIES})
endif(!CINDER_LIB-NOTFOUND)

