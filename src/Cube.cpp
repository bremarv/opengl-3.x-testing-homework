#include <iostream>
#include <cstring>
#include <stdexcept>
#include <vector>
#include <GlTools.h>
#include "Cube.h"
#include "constants.h"

using siut::simd::BBox3f;
using siut::simd::Vec3f;



static GLfloat cube[8*6*6] = {
	0, 0, 0, 0, 1, 0, 1, 1,  // u, v, nx, ny, nz, x, y, z
	0, 1, 0, 0, 1, 0, 0, 1,
	1, 1, 0, 0, 1, 1, 0, 1,
	1, 1, 0, 0, 1, 1, 0, 1,
	1, 0, 0, 0, 1, 1, 1, 1,
	0, 0, 0, 0, 1, 0, 1, 1,

	0, 0, 1, 0, 0, 1, 1, 1,
	0, 1, 1, 0, 0, 1, 0, 1,
	1, 1, 1, 0, 0, 1, 0, 0,
	1, 1, 1, 0, 0, 1, 0, 0,
	1, 0, 1, 0, 0, 1, 1, 0,
	0, 0, 1, 0, 0, 1, 1, 1,

	0, 0, 0, 0, -1, 1, 1, 0,
	0, 1, 0, 0, -1, 1, 0, 0,
	1, 1, 0, 0, -1, 0, 0, 0,
	1, 1, 0, 0, -1, 0, 0, 0,
	1, 0, 0, 0, -1, 0, 1, 0,
	0, 0, 0, 0, -1, 1, 1, 0,

	0, 0, -1, 0, 0, 0, 1, 0,
	0, 1, -1, 0, 0, 0, 0, 0,
	1, 1, -1, 0, 0, 0, 0, 1,
	1, 1, -1, 0, 0, 0, 0, 1,
	1, 0, -1, 0, 0, 0, 1, 1,
	0, 0, -1, 0, 0, 0, 1, 0,

	0, 0, 0, 1, 0, 0, 1, 1,
	0, 1, 0, 1, 0, 1, 1, 1,
	1, 1, 0, 1, 0, 1, 1, 0,
	1, 1, 0, 1, 0, 1, 1, 0,
	1, 0, 0, 1, 0, 0, 1, 0,
	0, 0, 0, 1, 0, 0, 1, 1,

	0, 0, 0, -1, 0, 1, 0, 1,
	0, 1, 0, -1, 0, 0, 0, 1,
	1, 1, 0, -1, 0, 0, 0, 0,
	1, 1, 0, -1, 0, 0, 0, 0,
	1, 0, 0, -1, 0, 1, 0, 0,
	0, 0, 0, -1, 0, 1, 0, 1
};



Cube*
Cube::factory(  )
{
	Cube *ret = new Cube;
	try
	{
		ret->init();
	} catch (...)
	{
		delete ret;
	}
	return ret;
}

void Cube::init()
{
	glGenBuffers( 1, &m_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
	ASSERT_GL;

	bool success = false;
	m_count = 12*3;
	glBufferData( GL_ARRAY_BUFFER,
				  sizeof(GLfloat)*8*m_count,
				  &cube[0],
				  GL_STATIC_DRAW );
	m_bbox = BBox3f( Vec3f(0.0f), Vec3f(1.0f) );

	glGenVertexArrays( 1, &m_vao );
            glBindVertexArray( m_vao );

	glVertexAttribPointer( TEXCOORD_ATTRIB_LOCATION, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*8, (GLvoid*)(sizeof(GLfloat)*0) );
	glEnableVertexAttribArray( TEXCOORD_ATTRIB_LOCATION );
	glVertexAttribPointer( NORMAL_ATTRIB_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*8, (GLvoid*)(sizeof(GLfloat)*2) );
	glEnableVertexAttribArray( NORMAL_ATTRIB_LOCATION );
	glVertexAttribPointer( POSITION_ATTRIB_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*8, (GLvoid*)(sizeof(GLfloat)*5) );
	glEnableVertexAttribArray( POSITION_ATTRIB_LOCATION );


	std::vector<GLfloat> TNB(6*m_count);
	// TODO: Generate TNB frame

	glGenBuffers( 1, &m_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
	ASSERT_GL;
	glBufferData( GL_ARRAY_BUFFER,
				  sizeof(GLfloat)*6*m_count,
				  &TNB[0],
				  GL_STATIC_DRAW );
	glVertexAttribPointer( TANGENT_ATTRIB_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*6, 
		(GLvoid*)(sizeof(GLfloat)*0) );
	glEnableVertexAttribArray( TANGENT_ATTRIB_LOCATION );
	glVertexAttribPointer( BITANGENT_ATTRIB_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*6, 
		(GLvoid*)(sizeof(GLfloat)*3) );
	glEnableVertexAttribArray( BITANGENT_ATTRIB_LOCATION );
	

	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	ASSERT_GL;
}

void
Cube::draw()
{
	glBindVertexArray( m_vao );
	glDrawArrays(GL_TRIANGLES, 0, m_count );
}

