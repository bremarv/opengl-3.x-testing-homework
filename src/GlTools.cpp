#include "GlTools.h"

#include <sstream>
#include <vector>
#include <assert.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>

using std::cerr;
using std::endl;
using std::string;

namespace GlTools
{


// --- checks status of an FBO -------------------------------------------------
#define ERROR_MSG_HELPER(a) case a: error = #a; break;

void
throwInformativeFBOError( GLenum fboerror, const std::string& file, const int line )
{
    string error = "unknown error";
    switch( fboerror ) {
        ERROR_MSG_HELPER( GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT );
        ERROR_MSG_HELPER( GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT );
        //            ERROR_MSG_HELPER( GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS );
        //            ERROR_MSG_HELPER( GL_FRAMEBUFFER_INCOMPLETE_FORMATS );
        ERROR_MSG_HELPER( GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER );
        ERROR_MSG_HELPER( GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER );
        ERROR_MSG_HELPER( GL_FRAMEBUFFER_UNSUPPORTED );
    }
    std::stringstream out;
    out << file << '@' << line
        << ": glCheckFramebufferStatus returned " << error
        << " (error code 0x" << std::hex << fboerror << ").";
    throw std::runtime_error( out.str() );
}

void
throwInformativeGLError( GLenum glerror, const std::string& file, const int line )
{
    string error = "unknown error";
    switch( glerror ) {
    ERROR_MSG_HELPER( GL_INVALID_ENUM );
    ERROR_MSG_HELPER( GL_INVALID_VALUE );
    ERROR_MSG_HELPER( GL_INVALID_OPERATION );
    ERROR_MSG_HELPER( GL_STACK_OVERFLOW );
    ERROR_MSG_HELPER( GL_STACK_UNDERFLOW );
    ERROR_MSG_HELPER( GL_OUT_OF_MEMORY );
    ERROR_MSG_HELPER( GL_TABLE_TOO_LARGE );
    ERROR_MSG_HELPER( GL_INVALID_FRAMEBUFFER_OPERATION );
    }
    std::stringstream out;
    out << file << '@' << line
        << ": glGetError returned " << error
        << " (error code 0x" << std::hex << glerror << ").";
    throw std::runtime_error( out.str() );
}



#undef ERROR_MSG_HELPER

// --- compile shader and check for errors -------------------------------------
GLuint
compileShader( const std::string& src, GLenum type )
{
    // create shader object
    GLuint s = glCreateShader( type );
    if( s == 0 ) {
        cerr << "Failed to create shader of type " << type << endl;
        exit( EXIT_FAILURE );
    }

    // set source code and compile
    const GLchar* src_list[1] = {
        src.c_str()
    };
    glShaderSource( s, 1, src_list, NULL );
    glCompileShader( s );

    // check for errors
    GLint compile_status;
    glGetShaderiv( s, GL_COMPILE_STATUS, &compile_status );
    if( compile_status != GL_TRUE ) {
        // compilation failed
		std::stringstream exceptMessage;
		std::stringstream sourceStream(src, std::stringstream::in);
		char line[500];
        exceptMessage << "Compilation failed!" << endl;
        exceptMessage << "--- source code ---" << endl;
		for(int lineNum=1; sourceStream.good(); lineNum++)
		{
			sourceStream.getline(line, 500);
			exceptMessage.width(3);
			exceptMessage << lineNum << " " << line << endl;
			//cerr << exceptMessage << std::endl;
		}
        exceptMessage << endl;


        GLint logsize;
        glGetShaderiv( s, GL_INFO_LOG_LENGTH, &logsize );

        if( logsize > 0 ) {
			std::vector<GLchar> infolog( logsize+1 );
            glGetShaderInfoLog( s, logsize, NULL, &infolog[0] );
            
            exceptMessage << "--- error log ---" << endl;
			exceptMessage << std::string( infolog.begin(), infolog.end() ) << endl;
        }
        else {
            exceptMessage << "--- empty log message ---" << endl;
        }
		throw std::runtime_error(exceptMessage.str().c_str());
    }
    return s;
}

// --- link shader program -----------------------------------------------------
void
linkProgram( GLuint program )
{
    // link
    glLinkProgram( program );

    // check for errors
    GLint linkstatus;
    glGetProgramiv( program, GL_LINK_STATUS, &linkstatus );
    if( linkstatus != GL_TRUE ) {
        cerr << "Linking failed!" << endl;

        GLint logsize;
        glGetProgramiv( program, GL_INFO_LOG_LENGTH, &logsize );

        if( logsize > 0 ) {
			std::vector<GLchar> infolog( logsize+1 );
            glGetProgramInfoLog( program, logsize, NULL, &infolog[0] );
            cerr << "--- error log ---" << endl;
			cerr << std::string( infolog.begin(), infolog.end() ) << endl;
        }
        else {
            cerr << "--- empty log message ---" << endl;
        }
        cerr << "Exiting." << endl;
        exit( EXIT_FAILURE );
    }
}

} // namespace GlTools
