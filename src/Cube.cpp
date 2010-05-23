#include <iostream>
#include <cstring>
#include <stdexcept>
#include <vector>
#include <GlTools.h>
#include "Cube.h"
#include "constants.h"
#include <stdio.h>

using siut::simd::BBox3f;
using siut::simd::Vec3f;
using siut::simd::Vec2f;



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


	std::vector<GLfloat> TNB(7*m_count);
	// TODO: Generate TNB frame
	for(int triangle = 0; triangle < m_count / 3; ++triangle)
	{
	    Vec3f vec0(&cube[triangle * 3 * 8 + 5]);
	    Vec3f vec1(&cube[triangle * 3 * 8 + 8 + 5]);
	    Vec3f vec2(&cube[triangle * 3 * 8 + 16 + 5]);

	    Vec2f tex0(&cube[triangle * 3 * 8]);
	    Vec2f tex1(&cube[triangle * 3 * 8 + 8]);
	    Vec2f tex2(&cube[triangle * 3 * 8 + 16]);

	    Vec3f norm0(&cube[triangle * 3 * 8 + 2]);
	    Vec3f norm1(&cube[triangle * 3 * 8 + 8 + 2]);
	    Vec3f norm2(&cube[triangle * 3 * 8 + 16 + 2]);

	    calctanbitannormal(vec0, vec1, vec2,
			       tex0, tex1, tex2,
			       &TNB[triangle * 21],
			       &TNB[triangle * 21 + 4],
			       &TNB[triangle * 21 + 7],
			       &TNB[triangle * 21 + 11],
			       &TNB[triangle * 21 + 14],
			       &TNB[triangle * 21 + 18]);
	    
	    orthogonolizetnb(
		&TNB[triangle*21], &TNB[triangle*21+4],
		&norm0[0], &TNB[triangle*21]);
	    orthogonolizetnb(
		&TNB[triangle*21+7], &TNB[triangle*21+11],
		&norm1[0], &TNB[triangle*21+7]);
	    orthogonolizetnb(
		&TNB[triangle*21+14], &TNB[triangle*21+18],
		&norm2[0], &TNB[triangle*21+14]);

	}

	glGenBuffers( 1, &m_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
	ASSERT_GL;
	glBufferData( GL_ARRAY_BUFFER,
				  sizeof(GLfloat)*6*m_count,
				  &TNB[0],
				  GL_STATIC_DRAW );
	glVertexAttribPointer( TANGENT_ATTRIB_LOCATION, 4, GL_FLOAT, GL_FALSE,
			       sizeof(GLfloat)*7, (GLvoid*)(sizeof(GLfloat)*0));
	glEnableVertexAttribArray( TANGENT_ATTRIB_LOCATION );
	

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

