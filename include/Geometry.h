#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include <GL/glew.h>
#include <siut/simd/BBox3f.hpp>

class Geometry
{
public:
	const siut::simd::BBox3f&
	getObjectSpaceBoundingBox() const;

	virtual void
	draw() =0;

protected:
	siut::simd::BBox3f  m_bbox;
	bool m_isInitialized();
};


class StandardGeometry : public Geometry
{
public:
	enum GeometryType {
		CUBE,
		INVERTED_CUBE,
		PYRAMID
	};

	static StandardGeometry *
	factory( const GeometryType type );

	const siut::simd::BBox3f&
	getObjectSpaceBoundingBox() const;

	virtual void
	draw();

protected:
	StandardGeometry (); // Do not use
	void init(GeometryType type);

	GeometryType        m_type;
	GLuint              m_vao;
	GLsizei             m_count;
	GLuint              m_vbo;
private:
	StandardGeometry(const StandardGeometry &o) {};
	
};

#endif
