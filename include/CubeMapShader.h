#ifndef _CUBEMAPSHADER_H_
#define _CUBEMAPSHADER_H_

#include <boost/shared_ptr.hpp>
#include <vector>
#include "ShaderProgram.h"
#include <siut/simd/Mat4f.hpp>


class CubeMapShader :
	public StandardShaderProgram
{
public:
	~CubeMapShader(void);
    static
    CubeMapShader*
    factory( ShaderType type );

    virtual void
    setUp( Object*  shape,
           Viewer*  viewer,
           Light*   light );


protected:
	CubeMapShader(ShaderType type);
	CubeMapShader() {} // do not use

	void init();
	virtual void setupUniforms();
	GLint m_cubeMap_loc;

private:
	CubeMapShader(const CubeMapShader &o) {}

};

#endif // _CUBEMAPSHADER_H_