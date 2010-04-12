#include "Snake.h"
#include <limits>
#include <assert.h>
#include <siut/simd/Vec3f.hpp>
#include <GlTools.h>
#include "constants.h"
#include <siut/simd/MatTransforms.hpp>
#include <siut/simd/MatOperations.hpp>

using std::vector;
using siut::simd::Vec3f;
using siut::simd::Mat4f;


Snake*
Snake::factory(  )
{
	Snake *ret = new Snake();
	try
	{
		ret->init();
	} catch (...)
	{
		delete ret;
	}
	return ret;
}


Snake::Snake() : IndexedTriStrip(), m_weightVbo(0), m_boneVbo(0)
{
}


void Snake::init( )
{
	const int N=50;
	const int M=50;

	IndexedTriStrip::initAsCylinder(M, N);
	m_startPos.clear();
	m_startPos.push_back(Vec3f(0.f, 0.f, 0.f));
	m_startPos.push_back(Vec3f(0.f, 0.f, 0.25f));

	m_weight.resize(m_count * 4);
	m_boneIndex.resize(m_count * 4);

	for (int i=0; i<m_count; i++)
	{
		float posz = m_data[8*i+7];
		if (posz < 0.2)
		{
			m_boneIndex[4*i]=0;
			m_boneIndex[4*i+1]=0;
			m_boneIndex[4*i+2]=0;
			m_boneIndex[4*i+3]=0;

			m_weight[4*i] = 1.f;
			m_weight[4*i+1] = 0.f;
			m_weight[4*i+2] = 0.f;
			m_weight[4*i+3] = 0.f;
		} else if (posz < 0.3)
		{
			m_boneIndex[4*i]=0;
			m_boneIndex[4*i+1]=1;
			m_boneIndex[4*i+2]=0;
			m_boneIndex[4*i+3]=0;

			m_weight[4*i] = 1.f-(posz-0.2f)/0.1f;
			m_weight[4*i+1] = 1.f-m_weight[4*i];
			m_weight[4*i+2] = 0.f;
			m_weight[4*i+3] = 0.f;
		} else
		{
			m_boneIndex[4*i]=1;
			m_boneIndex[4*i+1]=1;
			m_boneIndex[4*i+2]=1;
			m_boneIndex[4*i+3]=1;

			m_weight[4*i] = 1.f;
			m_weight[4*i+1] = 0.f;
			m_weight[4*i+2] = 0.f;
			m_weight[4*i+3] = 0.f;
		} 
	}

	bindVao();

	glGenBuffers( 1, &m_weightVbo );
	assert(m_weightVbo>0);
	glBindBuffer( GL_ARRAY_BUFFER, m_weightVbo );
	glBufferData( GL_ARRAY_BUFFER,
			  sizeof(GLfloat)*4*m_count,
			  &m_weight[0],
			  GL_STATIC_DRAW );


	glVertexAttribPointer( SKINNINGWEIGHT_ATTRIB_LOCATION, 4, GL_FLOAT, GL_FALSE, 0, 
		(GLvoid*)(sizeof(GLfloat)*0) );
	glEnableVertexAttribArray( SKINNINGWEIGHT_ATTRIB_LOCATION );

	glGenBuffers( 1, &m_boneVbo );
	assert(m_boneVbo>0);
	glBindBuffer( GL_ARRAY_BUFFER, m_boneVbo );
	glBufferData( GL_ARRAY_BUFFER,
		sizeof(GLuint)*4*m_count,
			  &m_boneIndex[0],
			  GL_STATIC_DRAW );

	glVertexAttribIPointer( SKINNINGINDEX_ATTRIB_LOCATION, 4, GL_UNSIGNED_INT, 0, 
		0 );
	glEnableVertexAttribArray( SKINNINGINDEX_ATTRIB_LOCATION );
	
	ASSERT_GL;
}



void Snake::createBoneMatrices(float angle, std::vector<siut::simd::Mat4f> &matrices)
{
//TODO: implementer funksjonen som lager matrisene assosiert ved hvert ben
// Roter hvert ledd angle grader rundt y aksen
}
