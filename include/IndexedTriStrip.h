#ifndef _INDEXEDTRISTRIP_H_
#define _INDEXEDTRISTRIP_H_
#include "Geometry.h"
#include <vector>


class IndexedTriStrip : 
	public Geometry
{
public:
	const siut::simd::BBox3f&
	getObjectSpaceBoundingBox() const;

	static IndexedTriStrip *factory(const std::vector<GLfloat> &data, const std::vector<GLuint> &index);
	static IndexedTriStrip *createCylinder(int N, int M);

	void setupTNB();
	void recomputeNormals();

	virtual void
	draw();

protected:
	IndexedTriStrip (); // used by factory

	void init(const std::vector<GLfloat> &data, const std::vector<GLuint> &index);
	void initAsCylinder(int M, int N);
	void bindVao();

	std::vector<GLfloat> m_data;
	std::vector<GLuint> m_index;

	GLuint              m_vao;
	GLsizei             m_count;
	GLuint              m_vbo;
	GLuint              m_TNBVbo;
	GLuint              m_indexVbo;
private:
	IndexedTriStrip(const IndexedTriStrip &o);
};

#endif // _INDEXEDTRISTRIP_H_
