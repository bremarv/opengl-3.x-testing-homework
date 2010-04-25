#include <iostream>
#include <cstring>
#include <stdexcept>
#include <vector>
#include <GlTools.h>
#include "Cube.h"
#include "constants.h"
#include <siut/simd/Vec2f.hpp>
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


	std::vector<GLfloat> TNB(6*m_count);
	int counter = 0;
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

	    calctanbitannormal(&vec0[0], &vec1[0], &vec2[0],
			       &tex0[0], &tex1[0], &tex2[0], &norm0[0],
			       &TNB[triangle * 18], &TNB[triangle * 18 + 3]);

	    // calctanbitannormal(&vec0[0], &vec1[0], &vec2[0],
	    // 		       &tex0[0], &tex1[0], &tex2[0], &norm0[0],
	    // 		       &TNB[triangle * 18 + 6], &TNB[triangle * 18 + 9],
	    // 		       &normals[triangle * 9 + 3]);

	    // calctanbitannormal(&vec0[0], &vec1[0], &vec2[0],
	    // 		       &tex0[0], &tex1[0], &tex2[0], &norm0[0],
	    // 		       &TNB[triangle * 18 + 12], &TNB[triangle * 18 + 15],
	    // 		       &normals[triangle * 9 + 6]);	    
	    calctanbitannormal(&vec1[0], &vec2[0], &vec0[0],
	    		       &tex1[0], &tex2[0], &tex0[0], &norm1[0],
	    		       &TNB[triangle * 18 + 6], &TNB[triangle * 18 + 9]);

	    calctanbitannormal(&vec2[0], &vec0[0], &vec1[0],
	    		       &tex2[0], &tex0[0], &tex1[0], &norm2[0],
	    		       &TNB[triangle * 18 + 12], &TNB[triangle * 18 + 15]);
	    

	    // for( int i = 0; i < 3; ++i)
	    // {
	    // 	Vec3f p1 = vec1 - vec0;
	    // 	Vec3f p2 = vec2 - vec0;
	    // 	Vec2f uv1 = tex1 - tex0;
	    // 	Vec2f uv2 = tex2 - tex0;

	    // 	Vec3f tangent = normalize(p1 * uv2.y() - p2 * uv1.y());
	    // 	Vec3f bitangent = normalize(p2 * uv1.x() - p1 * uv2.x());
	    // 	norm0 = cross(tangent, bitangent);


	    // 	// float x1 = vec1.x() - vec0.x();
	    // 	// float x2 = vec2.x() - vec0.x();
	    // 	// float y1 = vec1.y() - vec0.y();
	    // 	// float y2 = vec2.y() - vec0.y();
	    // 	// float z1 = vec1.z() - vec0.z();
	    // 	// float z2 = vec2.z() - vec0.z();

	    // 	// float s1 = tex1.x() - tex0.x();
	    // 	// float s2 = tex2.x() - tex0.x();
	    // 	// float t1 = tex1.y() - tex0.y();
	    // 	// float t2 = tex2.y() - tex0.y();

	    // 	// float r = 1.f / (s1 * t2 - s2 * t1);

	    // 	// Vec3f tangent((t2 * x1 - t1 * x2) * r,
	    // 	// 	   (t2 * y1 - t1 * y2) * r,
	    // 	// 	   (t2 * z1 - t1 * z2) * r);
	    // 	// Vec3f bitangent((s2 * x1 - s1 * x2) * r,
	    // 	// 	   (s2 * y1 - s1 * y2) * r,
	    // 	// 	    (s2 * z1 - s1 * z2) * r);

	    // 	tangent = tangent - norm0 * dot(norm0, tangent);
	    // 	bitangent = bitangent - norm0 * dot(norm0, bitangent) -
	    // 	    tangent * dot(tangent, bitangent);
	    // 	int handedness = (dot(cross(norm0, tangent), bitangent) > 0.f) ?
	    // 	    -1.f: 1.f;
	    // 	bitangent = normalize(handedness * cross(norm0, tangent));
		
	    // 	tangent = normalize(tangent);
	    // 	//bitangent = normalize(bitangent);
		
	    // 	// tangent = normalize(tangent - normal * dot(normal, tangent));

	    // 	// bitangent = normalize(handedness * cross(normal, tangent));
	    // 	// normal = normalize(normal);



	    // 	for(int j = 0; j < 3; ++j)
	    // 	{
	    // 	    TNB[triangle * 6 * 3 + i * 2 * 3 + j] = tangent[j];
	    // 	    //std::cout<<triangle * 6 * 3 + i * 2 * 3 + j;
	    // 	    TNB[triangle * 6 * 3 + i * 2 * 3 + j + 3] = bitangent[j];
	    // 	    //std::cout<<" "<<triangle * 6 * 3 + i * 2 * 3 + j + 3<<std::endl;
	    // 	    counter += 2;
	    // 	    // std::cout<<tangent[j]<<"\t";
	    // 	    // std::cout<<bitangent[j]<<std::endl;
	    // 	    // normals[triangle * 3 * 3 + i * 3 + j] = normal[j];
	    // 	}

	    // 	// std::cout<<std::endl;
		
	    // 	Vec3f tmpvert(vec0);
	    // 	vec0 = vec1;
	    // 	vec1 = vec2;
	    // 	vec2 = tmpvert;
	    // 	Vec2f tmptex(tex0);
	    // 	tex0 = tex1;
	    // 	tex1 = tex2;
	    // 	tex2 = tmptex;
	    // 	Vec3f tmpnorm(norm0);
	    // 	norm0 = norm1;
	    // 	norm1 = norm2;
	    // 	norm2 = tmpnorm;
	    // }
	}

	// std::cout<<counter<<"\t"<<TNB.size()<<std::endl;
	// for(int i = 0; i < TNB.size(); ++i)
	// {
	//     std::cout << TNB[i] << " ";
	//     if(i % 3 == 2)
	//     {
	// 	std::cout << std::endl;
	//     }
	//     if(i % 6 == 5)
	// 	std::cout << std::endl;
	    
	// }
	    

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

	// glBufferData( GL_ARRAY_BUFFER,
	// 	      sizeof(GLfloat)*3*m_count,
	// 	      &normals[0],
	// 	      GL_STATIC_DRAW );
	// glVertexAttribPointer( NORMAL_ATTRIB_LOCATION, 3, GL_FLOAT, GL_FALSE,
	// 		       sizeof(GLfloat)*3, 0);
	// glEnableVertexAttribArray(NORMAL_ATTRIB_LOCATION);
	

	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	ASSERT_GL;
}

void
Cube::calctanbitannormal( GLfloat *vec0, GLfloat *vec1, GLfloat *vec2,
			  GLfloat *tex0, GLfloat *tex1, GLfloat *tex2, GLfloat *norm,
			GLfloat *tangentsaveloc, GLfloat *bitangentsaveloc/*,
									    GLfloat *normalsaveloc*/)
{
    Vec3f v0(vec0);
    Vec3f v1(vec1);
    Vec3f v2(vec2);

    Vec2f t0(tex0);
    Vec2f t1(tex1);
    Vec2f t2(tex2);

    Vec3f normal(norm);

    Vec3f p1 = v1 - v0;
    Vec3f p2 = v2 - v0;
    Vec2f uv1 = t1 - t0;
    Vec2f uv2 = t2 - t0;

    float r = 1.f / (uv1.x() * uv2.y() - uv2.x() * uv1.y());

    // Vec3f tangent = (p1 * uv2.y() + p2 *(-uv1.y())) * r;
    // Vec3f bitangent = (p2 * uv1.x() + p1 *(-uv2.x())) * r;

    // tangent = normalize(tangent - normal * dot(normal, tangent));
    // int handedness = (dot(cross(normal, tangent), bitangent) < 0.f) ?
    // 	 -1.f : 1.f;

    // bitangent = normalize(handedness * cross(normal, tangent));
    
    Vec3f tangent = (p1 * uv2.y() + p2 * -uv1.y()) * r;
    Vec3f bitangent = (p2 * uv1.x() + p1 * -uv2.x()) * r;
//    Vec3f normal = cross(tangent, bitangent);
//    Vec3f normal(norm);

    tangent = tangent - normal * dot(normal, tangent);
    bitangent = bitangent - dot(normal, bitangent) * normal -
	dot(tangent, bitangent) * tangent;
    // bitangent = bitangent - normal * dot(normal, bitangent) -
    // 	tangent * dot(tangent, bitangent);
    // int handedness = (dot(cross(normal, tangent), bitangent) < 0.f) ?
    // 	-1.f : 1.f;
    // bitangent = normalize(handedness * cross(normal, tangent));

    bitangent = normalize(bitangent);
    tangent = normalize(tangent);

    memcpy(tangentsaveloc, &tangent[0], sizeof(GLfloat)*3);
    memcpy(bitangentsaveloc, &bitangent[0], sizeof(GLfloat)*3);
//    memcpy(normalsaveloc, &normal[0], sizeof(GLfloat)*3);
}



void
Cube::draw()
{
	glBindVertexArray( m_vao );
	glDrawArrays(GL_TRIANGLES, 0, m_count );
}

