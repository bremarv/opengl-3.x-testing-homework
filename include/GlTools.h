#ifndef _GLTOOLS_H_
#define _GLTOOLS_H_
#include <sstream>
#include <string>
#include <stdexcept>
#include <GL/glew.h>


#define ASSERT_GL do { \
    GLenum err = glGetError(); \
    if( err != GL_NO_ERROR ) { \
        ::GlTools::throwInformativeGLError( err, __FILE__, __LINE__ ); \
    } \
} while( 0 )

#define ASSERT_FBO do { \
    GLenum status = glCheckFramebufferStatusEXT( GL_FRAMEBUFFER ); \
    if( status != GL_FRAMEBUFFER_COMPLETE ) { \
        ::GlTools::throwInformativeFBOError( status, __FILE__, __LINE__ ); \
    } \
} while( 0 )


namespace GlTools
{

// --- compile shader and check for errors -------------------------------------
GLuint
compileShader( const std::string& src, GLenum type );
// --- link shader program -----------------------------------------------------
void
linkProgram( GLuint program );

void
throwInformativeFBOError( GLenum fboerror, const std::string& file, const int line );

void
throwInformativeGLError( GLenum glerror, const std::string& file, const int line );


} // namespace GlTools

#endif // _GLTOOLS_H_

