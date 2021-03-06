#include "IndexedTriStrip.h"
#include <limits>
#include <assert.h>
#include <siut/simd/Vec3f.hpp>
#include <siut/simd/Vec2f.hpp>
#include <GlTools.h>
#include "constants.h"
#include <string.h>
using std::vector;
using siut::simd::Vec3f;
using siut::simd::Vec2f;


IndexedTriStrip*
IndexedTriStrip::factory( const std::vector<GLfloat> &data, const std::vector<GLuint> &index )
{
    IndexedTriStrip *ret = new IndexedTriStrip();
    try
    {
	ret->init(data, index);
    } catch (...)
    {
	delete ret;
    }
    return ret;
}

IndexedTriStrip *IndexedTriStrip::createCylinder(int N, int M)
{
    IndexedTriStrip *ret = new IndexedTriStrip();
    try
    {
	ret->initAsCylinder(N, M);
    } catch (...)
    {
	delete ret;
    }
    return ret;
}

void IndexedTriStrip::initAsCylinder(int M, int N)
{
#define PI 3.14159265
    const int FLOAT_PER_VERT = 8;
    std::vector<GLfloat> data(N*M*FLOAT_PER_VERT);
    for (int i=0; i<M; i++)
    {
	for (int j=0; j<N; j++)
	{	
	    GLfloat *fp = &data[j*8+i*8*N];
	    fp[0] = static_cast<float>(j) / (N-1); // tex coord x
	    fp[1] = static_cast<float>(i) / (M-1); // tex coord y
	    if (0 && (j==0 || j == N-1))
	    {
		fp[5] = 1;
		fp[6] = 0;
	    } else
	    {
		fp[5] = std::cos (2.0f * 3.141593f * static_cast<float>(j) / (N-1)); // position x
		fp[6] = std::sin (2.0f * 3.141593f * static_cast<float>(j) / (N-1)); // position y
	    }
	    fp[2] = fp[5]; // normal x
	    fp[3] = fp[6];                             // normal y
	    fp[4] = 0;                             // normal z

	    fp[7] = static_cast<float>(i) / (M-1);                             // position z 
	    fp[5] *= 0.02f;
	    fp[6] *= 0.02f;
	    fp[7] *= 0.5f;
	}
    }

    std::vector<GLuint> indices(2*(M-1)*N+(M-1));
    GLuint *ip = &indices[0];
    GLsizei count=0;

    for (int i=0; i<M-1; i++)
    {
	for (int j=0; j<N; j++)
	{	
	    *ip++ = N*(i+1)+j;
	    *ip++ = N*i+j;
	    count += 2;
	}
	*ip++ =std::numeric_limits<GLuint>::max(); // assumes primitive restart
	count++;
    }
    init(data, indices);
}

IndexedTriStrip::IndexedTriStrip() : m_vao(0), m_count(0),
				     m_vbo(0), m_TNBVbo(0), m_indexVbo(0)
{
}


void IndexedTriStrip::init(const vector<GLfloat> &data, const vector<GLuint> &index)
{
    assert(data.size() % 8 == 0);
    m_data = data;
    m_index = index;
    m_count = data.size() / 8;

    glGenVertexArrays( 1, &m_vao );
    assert(m_vao>0);
    glBindVertexArray( m_vao );

    glGenBuffers( 1, &m_vbo );
    assert(m_vbo>0);
    glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
    glBufferData( GL_ARRAY_BUFFER,
		  sizeof(GLfloat)*8*m_count,
		  &m_data[0],
		  GL_STATIC_DRAW );

    glGenBuffers( 1, &m_indexVbo );
    assert(m_indexVbo>0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexVbo);
    glBufferData( GL_ELEMENT_ARRAY_BUFFER,
		  sizeof(GLfloat)*index.size(),
		  &m_index[0],
		  GL_STATIC_DRAW );

    glVertexAttribPointer( TEXCOORD_ATTRIB_LOCATION, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*8, 
			   (GLvoid*)(sizeof(GLfloat)*0) );
    glEnableVertexAttribArray( TEXCOORD_ATTRIB_LOCATION );
    glVertexAttribPointer( NORMAL_ATTRIB_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*8, 
			   (GLvoid*)(sizeof(GLfloat)*2) );
    glEnableVertexAttribArray( NORMAL_ATTRIB_LOCATION );
    glVertexAttribPointer( POSITION_ATTRIB_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*8, 
			   (GLvoid*)(sizeof(GLfloat)*5) );
    glEnableVertexAttribArray( POSITION_ATTRIB_LOCATION );
    m_bbox = siut::simd::BBox3f( Vec3f(0.0f), Vec3f(1.0f, 0.0f, 1.0f) );
    ASSERT_GL;

    setupTNB();
}

void IndexedTriStrip::bindVao()
{
    assert(m_vao>0);
    glBindVertexArray( m_vao );
}

void
IndexedTriStrip::draw()
{
    glBindVertexArray( m_vao );
    glEnable(GL_PRIMITIVE_RESTART);
    glDrawElements(GL_TRIANGLE_STRIP, m_index.size(), GL_UNSIGNED_INT, 0);
    glDisable(GL_PRIMITIVE_RESTART);
}

void IndexedTriStrip::recomputeNormals()
{
    const GLuint resetIndex=std::numeric_limits<GLuint>::max();

    for (int i=0; i<m_count; i++)
    {
	m_data[8*i+2]=0;
	m_data[8*i+3]=0;
	m_data[8*i+4]=0;
    }
    for (size_t i=0; i<m_index.size()-2; i++)
    {
	GLuint i1=m_index[i++];
	assert (i1!=resetIndex);
	GLuint i2=m_index[i++];
	assert (i2!=resetIndex);
	Vec3f P1(&m_data[8*(i1)+5]);
	Vec3f P2(&m_data[8*(i2)+5]);
	float ccw=1;
	while (i<m_index.size() && m_index[i]!=resetIndex)
	{
	    GLuint i0;
	    Vec3f P0=P1; i0=i1;
	    P1=P2; i1=i2;
	    i2=m_index[i++]; 
	    P2=Vec3f(&m_data[8*(i2)+5]);

	    Vec3f v1=P1-P0;
	    Vec3f v2=P2-P0;
	    Vec3f n=v1.cross(v2);
	    n.normalize();
	    n*=ccw;
	    ccw*=-1;
	    for (int vi=0; vi<3; vi++)
	    {
		int vert= vi==0?i0:(i==1?i1:i2);
		for (int j=0;j<3;j++)
		    m_data[vert*8+2+j]+=n[j];
	    }

	}
    }
    glBindVertexArray( m_vao );
    glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
    GLfloat *buffData = static_cast<GLfloat*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
    memcpy(buffData, &m_data[0], m_count * 8* sizeof(GLfloat));
    glUnmapBuffer(GL_ARRAY_BUFFER);
}

void IndexedTriStrip::setupTNB()
{
//	TODO: lag tangent og bitangent
   
    const GLuint resetindex = std::numeric_limits<GLuint>::max();
    GLfloat *tangentdata = new GLfloat[m_count * 14];
    
    for(int i = 0; i < m_count; ++i)
    {
	for(int j = 0; j < 8; ++j)
	{
	    tangentdata[14 * i + j] = m_data[8 * i + j];	    
	}
	for(int j = 8; j < 14; ++j)
	{
	    tangentdata[14 * i + j] = 0;
	}
    }
    
    for(int i=0; i < m_index.size()-2; i++)
    {
	//get the two first vertecies to make up for triangle strip starting with the 3 first
	GLuint i1 = m_index[i++];
	assert(i1 != resetindex);
	GLuint i2 = m_index[i++];
	assert(i2 != resetindex);
	//theres 8 floats for each vertex, so jump index * 8 to get the right set
	//position info is from 5 to 7 in each set
	Vec3f vert1(&m_data[8 * i1 + 5]);
	Vec3f vert2(&m_data[8 * i2 + 5]);
	//and get the texcoords as well
	Vec2f texc1(&m_data[8 * i1 + 0]);
	Vec2f texc2(&m_data[8 * i2 + 0]);
	
	//now go trough the triangle strip, adding one each round
	while(i < m_index.size() && m_index[i] != resetindex)
	{
	    //move the information one step back
	    Vec3f vert0 = vert1;
	    GLuint i0 = i1;
	    Vec2f texc0 = texc1;
	    vert1 = vert2;
	    i1 = i2;
	    texc1 = texc2;
	    //and get the next set of info
	    i2 = m_index[i++];
	    vert2 = Vec3f(&m_data[8 * i2 + 5]);
	    texc2 = Vec2f(&m_data[8 * i2 + 0]);

	    float x1 = vert1.x() - vert0.x();
	    float x2 = vert2.x() - vert0.x();
	    float y1 = vert1.y() - vert0.y();
	    float y2 = vert2.y() - vert0.y();
	    float z1 = vert1.z() - vert0.z();
	    float z2 = vert2.z() - vert0.z();

	    float s1 = texc1.x() - texc0.x();
	    float s2 = texc2.x() - texc0.x();
	    float t1 = texc1.y() - texc0.y();
	    float t2 = texc2.y() - texc0.y();

	    float r = 1.f / (s1 * t2 - s2 * t1);

	    Vec3f sdir((t2 * x1 - t1 * x2) * r,
		       (t2 * y1 - t1 * y2) * r,
		       (t2 * z1 - t1 * z2) * r);
	    Vec3f tdir((s2 * x1 - s1 * x2) * r,
		       (s2 * y1 - s1 * y2) * r,
		       (s2 * z1 - s1 * z2) * r);

	    // add tangent data to the data array
	    for(int vi = 0; vi < 3; vi++)
	    {
		int vert = (vi == 0 ? i0 : (vi == 1 ? i1 : i2));
		for(int j = 0; j < 3; j++)
		{
		    tangentdata[vert * 14 + 8 + j] += sdir[j];
		    tangentdata[vert * 14 + 11 + j] += tdir[j];
		}
	    }
	}
    }

    for(int i = 0; i < m_count; ++i)
    {
	Vec3f tangent(&tangentdata[i * 14 + 8]);
	Vec3f bitangent(&tangentdata[i * 14 + 11]);
	Vec3f normal(&tangentdata[i * 14 + 2]);

	tangent = tangent - normal * dot(normal, tangent);
	bitangent = bitangent - normal * dot(normal, bitangent) -
	    tangent * dot(tangent, bitangent);
	tangent = normalize(tangent);
	bitangent = normalize(bitangent);

	for(int j = 0; j < 3; ++j)
	{
	    tangentdata[i * 14 + 8 + j] = tangent[j];
	    tangentdata[i * 14 + 11 + j] = bitangent[j];
	}
    }

    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_TNBVbo);
    assert(m_TNBVbo > 0);
    glBindBuffer( GL_ARRAY_BUFFER, m_TNBVbo);
    glBufferData(GL_ARRAY_BUFFER,
		 sizeof(GLfloat) * 14 * m_count,
		 &tangentdata[0],
		 GL_STATIC_DRAW);

    glVertexAttribPointer( TEXCOORD_ATTRIB_LOCATION, 2, GL_FLOAT, GL_FALSE,
			   sizeof(GLfloat)*14, (GLvoid*)(sizeof(GLfloat)*0) );
    glEnableVertexAttribArray( TEXCOORD_ATTRIB_LOCATION );
    
    glVertexAttribPointer( NORMAL_ATTRIB_LOCATION, 3, GL_FLOAT, GL_FALSE,
			   sizeof(GLfloat)*14, (GLvoid*)(sizeof(GLfloat)*2) );
    glEnableVertexAttribArray( NORMAL_ATTRIB_LOCATION );
    
    glVertexAttribPointer( POSITION_ATTRIB_LOCATION, 3, GL_FLOAT, GL_FALSE,
			   sizeof(GLfloat)*14, (GLvoid*)(sizeof(GLfloat)*5) );
    glEnableVertexAttribArray( POSITION_ATTRIB_LOCATION );
    
    glVertexAttribPointer( TANGENT_ATTRIB_LOCATION, 3, GL_FLOAT, GL_FALSE,
			   sizeof(GLfloat)*14, (GLvoid*)(sizeof(GLfloat)*8) );
    glEnableVertexAttribArray( TANGENT_ATTRIB_LOCATION );
    
    glVertexAttribPointer( BITANGENT_ATTRIB_LOCATION, 3, GL_FLOAT, GL_FALSE,
			   sizeof(GLfloat)*14, (GLvoid*)(sizeof(GLfloat)*11) );

    ASSERT_GL;
			   
}
