#include <iostream>
#include <cstring>
#include <stdexcept>
#include <GlTools.h>
#include <Geometry.h>
#include "constants.h"

using siut::simd::BBox3f;
using siut::simd::Vec3f;
using siut::simd::Vec2f;


const siut::simd::BBox3f&
Geometry::getObjectSpaceBoundingBox() const
{
	return m_bbox;
}


static GLfloat inverted_cube[8*6*6] =
{
	0, 0, 0, 0, -1, 0, 2, 2,
	1, 1, 0, 0, -1, 2, 0, 2,
	0, 1, 0, 0, -1, 0, 0, 2,
	1, 1, 0, 0, -1, 2, 0, 2,
	0, 0, 0, 0, -1, 0, 2, 2,
	1, 0, 0, 0, -1, 2, 2, 2,
	0, 0, -1, 0, 0, 2, 2, 2,
	1, 1, -1, 0, 0, 2, 0, 0,
	0, 1, -1, 0, 0, 2, 0, 2,
	1, 1, -1, 0, 0, 2, 0, 0,
	0, 0, -1, 0, 0, 2, 2, 2,
	1, 0, -1, 0, 0, 2, 2, 0,
	0, 0, 0, 0, 1, 2, 2, 0,
	1, 1, 0, 0, 1, 0, 0, 0,
	0, 1, 0, 0, 1, 2, 0, 0,
	1, 1, 0, 0, 1, 0, 0, 0,
	0, 0, 0, 0, 1, 2, 2, 0,
	1, 0, 0, 0, 1, 0, 2, 0,
	0, 0, 1, 0, 0, 0, 2, 0,
	1, 1, 1, 0, 0, 0, 0, 2,
	0, 1, 1, 0, 0, 0, 0, 0,
	1, 1, 1, 0, 0, 0, 0, 2,
	0, 0, 1, 0, 0, 0, 2, 0,
	1, 0, 1, 0, 0, 0, 2, 2,
	0, 0, 0, -1, 0, 0, 2, 2,
	1, 1, 0, -1, 0, 2, 2, 0,
	0, 1, 0, -1, 0, 2, 2, 2,
	1, 1, 0, -1, 0, 2, 2, 0,
	0, 0, 0, -1, 0, 0, 2, 2,
	1, 0, 0, -1, 0, 0, 2, 0,
	0, 0, 0, 1, 0, 2, 0, 2,
	1, 1, 0, 1, 0, 0, 0, 0,
	0, 1, 0, 1, 0, 0, 0, 2,
	1, 1, 0, 1, 0, 0, 0, 0,
	0, 0, 0, 1, 0, 2, 0, 2,
	1, 0, 0, 1, 0, 2, 0, 0
};

static GLfloat cube[8*6*6] = {
	0, 0, 0, 0, 1, 0, 1, 1,
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

static GLfloat pyramid[8*6*3] =
{
	0.0, 1.0, 0.0, 0.0, 0.0, -1.0, -1.0, -1.0,
	0.0, 1.0, 0.0, 0.0, 0.0,  1.0,  1.0, -1.0,
	0.0, 1.0, 0.0, 0.0, 0.0,  1.0, -1.0, -1.0,

	0.0, 1.0, 0.0, 0.0, 0.0,  1.0,  1.0, -1.0,
	0.0, 1.0, 0.0, 0.0, 0.0, -1.0, -1.0, -1.0,
	0.0, 1.0, 0.0, 0.0, 0.0, -1.0,  1.0, -1.0,

	0.0, 1.0, 0.0, 0.0, 0.0, -1.0, -1.0, -1.0,
	0.0, 0.0, 0.0, 0.0, 0.0,  0.0,  0.0,  0.0,
	0.0, 1.0, 0.0, 0.0, 0.0, -1.0,  1.0, -1.0,

	0.0, 1.0, 0.0, 0.0, 0.0, -1.0,  1.0, -1.0,
	0.0, 0.0, 0.0, 0.0, 0.0,  0.0,  0.0,  0.0,
	0.0, 1.0, 0.0, 0.0, 0.0,  1.0,  1.0, -1.0,

	0.0, 1.0, 0.0, 0.0, 0.0,  1.0,  1.0, -1.0,
	0.0, 0.0, 0.0, 0.0, 0.0,  0.0,  0.0,  0.0,
	0.0, 1.0, 0.0, 0.0, 0.0,  1.0, -1.0, -1.0,

	0.0, 1.0, 0.0, 0.0, 0.0,  1.0, -1.0, -1.0,
	0.0, 0.0, 0.0, 0.0, 0.0,  0.0,  0.0,  0.0,
	0.0, 1.0, 0.0, 0.0, 0.0, -1.0, -1.0, -1.0
};

StandardGeometry::StandardGeometry() :
	Geometry()
{
}

StandardGeometry*
StandardGeometry::factory( const GeometryType type )
{
	StandardGeometry *ret = new StandardGeometry();
	try
	{
		ret->init(type);
	} catch (...)
	{
		delete ret;
	}
	return ret;
}

void StandardGeometry::init(StandardGeometry::GeometryType type)
{
	glGenBuffers( 1, &m_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
	ASSERT_GL;

	bool success = false;
	switch( type ) {

	case CUBE:
		m_count = 12*3;
		glBufferData( GL_ARRAY_BUFFER,
					  sizeof(GLfloat)*8*m_count,
					  &cube[0],
					  GL_STATIC_DRAW );
		m_bbox = BBox3f( Vec3f(0.0f), Vec3f(1.0f) );
		success = true;
		break;

	case INVERTED_CUBE:
		m_count = 12*3;
		glBufferData( GL_ARRAY_BUFFER,
					  sizeof(GLfloat)*8*m_count,
					  &inverted_cube[0],
					  GL_STATIC_DRAW );
		m_bbox = BBox3f( Vec3f(0.0f), Vec3f(2.0f) );
		success = true;
		break;

	case PYRAMID:
		m_count = 6*3;
		glBufferData( GL_ARRAY_BUFFER,
					  sizeof(GLfloat)*8*m_count,
					  &pyramid[0],
					  GL_STATIC_DRAW );
		m_bbox = BBox3f( Vec3f(-1.0f), Vec3f(1.0f, 1.0f, 0.0f) );
		success = true;
		break;
	}

	if( success ) {
            glGenVertexArrays( 1, &m_vao );
            glBindVertexArray( m_vao );

		glVertexAttribPointer( TEXCOORD_ATTRIB_LOCATION, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*8, 
			(GLvoid*)(sizeof(GLfloat)*0) );
		glEnableVertexAttribArray( TEXCOORD_ATTRIB_LOCATION );
		glVertexAttribPointer( NORMAL_ATTRIB_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*8, 
			(GLvoid*)(sizeof(GLfloat)*2) );
		glEnableVertexAttribArray( NORMAL_ATTRIB_LOCATION );
		glVertexAttribPointer( POSITION_ATTRIB_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*8, 
			(GLvoid*)(sizeof(GLfloat)*5) );
		glEnableVertexAttribArray( POSITION_ATTRIB_LOCATION );

		glBindVertexArray( 0 );
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		ASSERT_GL;
	}
	else {
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glDeleteBuffers( 1, &m_vbo );
		throw std::runtime_error( __FILE__ );
	}
}

void
StandardGeometry::draw()
{
	glBindVertexArray( m_vao );
	glDrawArrays(GL_TRIANGLES, 0, m_count );
}

void
Geometry::calctanbitannormal(
    Vec3f &v0, Vec3f &v1, Vec3f &v2,
    Vec2f &t0, Vec2f &t1, Vec2f &t2,
    GLfloat *ts0, GLfloat *bts0, GLfloat *ts1, GLfloat *bts1,
    GLfloat *ts2, GLfloat *bts2
    )
{
    Vec3f p1 = v1 - v0;
    Vec3f p2 = v2 - v0;
    Vec2f uv1 = t1 - t0;
    Vec2f uv2 = t2 - t0;
    
    float r = 1.f / (uv1.x() * uv2.y() - uv2.x() * uv1.y());

    // std::cout<<p2 * uv1.x() *r<<"\t"<<p1 * -uv2.x()*r<<std::endl;
    // std::cout<<p1 * uv2.y()*r<<"\t"<<p2 * -uv1.y()*r<<std::endl;
    // std::cout<<std::endl;
    std::cout<<uv1<<"\t\t"<<uv2<<std::endl;
    
    Vec3f tangent = (p1 * uv2.y() + p2 * -uv1.y()) * r;
    Vec3f bitangent = (p2 * uv1.x() + p1 * -uv2.x()) * r;



    for(int i = 0; i < 3; ++i)
    {
	ts0[i] += tangent[i];
	ts1[i] += tangent[i];
	ts2[i] += tangent[i];
	bts0[i] += bitangent[i];
	bts1[i] += bitangent[i];
	bts2[i] += bitangent[i];
    }
}

void
Geometry::orthogonolizetnb(
    Vec3f &tangent, Vec3f &bitangent,
    Vec3f &normal, GLfloat *saveloc)
{
    tangent = normalize(tangent);
    bitangent = normalize(bitangent);
    Vec3f orthtangent = normalize(tangent - normal * dot(normal, tangent));
    GLfloat handedness = (dot(cross(normal, tangent), bitangent) < 0.f) ?
	-1.f : 1.f;

    // std::cout<<tangent<<"\t"<<bitangent<<std::endl;
    
    for(int i = 0; i < 3; ++i)
	saveloc[i] = orthtangent[i];
    saveloc[3] = handedness;
}

void Geometry::orthogonolizetnb(
    GLfloat *tangent, GLfloat *bitangent,
    GLfloat *normal, GLfloat *saveloc)
{
    Vec3f t(tangent);
    Vec3f b(bitangent);
    Vec3f n(normal);

    orthogonolizetnb(t, b, n, saveloc);
}

