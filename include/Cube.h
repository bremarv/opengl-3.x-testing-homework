#ifndef _CUBE_H_
#define _CUBE_H_
#include "Geometry.h"

class Cube : public Geometry
{
public:
	static Cube *
	factory(  );

	const siut::simd::BBox3f&
	getObjectSpaceBoundingBox() const;

	virtual void
	draw();

	static void
    calctanbitannormal( GLfloat *vec0, GLfloat *vec1, GLfloat *vec2,
			GLfloat *tex0, GLfloat *tex1, GLfloat *tex2,
			GLfloat *normal, GLfloat *tangentsaveloc,
			GLfloat *bitangentsaveloc, GLfloat *normalsaveloc);

protected:
	Cube() {}; // used by factory

	void init();

	GLuint              m_vao;
	GLsizei             m_count;
	GLuint              m_vbo;
	GLuint              m_tanVbo;
private:
	Cube(const Cube &o) {};
	
};

#endif // _CUBE_H_
