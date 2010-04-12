#ifndef _SKINNINGSHAPE_H_
#define _SKINNINGSHAPE_H_

#include <GL/glew.h>
#include <vector>
#include "Object.h"
#include <siut/simd/Vec3f.hpp>
#include <siut/simd/Vec4f.hpp>
#include <siut/simd/Quatf.hpp>
#include <siut/simd/Mat4f.hpp>
#include <siut/simd/BBox3f.hpp>
#include <Geometry.h>




class SkinningShape : public Shape
{
public:
    SkinningShape( Geometry* geo )
    : Shape( geo )
    {}

	const std::vector<siut::simd::Mat4f> &getboneMatrices() const
	{ return m_boneMatrices; }
	
	std::vector<siut::simd::Mat4f> &getboneMatrices() 
	{ return m_boneMatrices; }

	void setboneMatrices(const std::vector<siut::simd::Mat4f> &m)
	{ m_boneMatrices  = m; }

protected:
    SkinningShape();
	std::vector<siut::simd::Mat4f> m_boneMatrices;

};

#endif // _SKINNINGSHAPE_H_
