#ifndef _SNAKE_H_
#define _SNAKE_H_
#include "IndexedTriStrip.h"
#include <siut/simd/Mat4f.hpp>
#include <vector>


class Snake : 
	public IndexedTriStrip
{
public:
	const siut::simd::BBox3f&
	getObjectSpaceBoundingBox() const;

	static Snake *factory();
	void createBoneMatrices(float angle, std::vector<siut::simd::Mat4f> &matrices);

	// Update boneMatrices based on local rotation matrices
	// void update(const siut::simd::Mat4f *BoneMatrices);

protected:
	Snake (); // used by factory
	void init();

	std::vector<GLfloat> m_weight;
	std::vector<GLuint> m_boneIndex;

	std::vector<siut::simd::Vec3f> m_startPos;

	GLuint              m_weightVbo;
	GLuint              m_boneVbo;
private:
	Snake(const Snake &o);
};

#endif // _SNAKE_H_