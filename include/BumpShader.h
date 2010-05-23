#ifndef _BUMPSHADER_H_
#define _BUMPSHADER_H_

#include <boost/shared_ptr.hpp>
#include "ShaderProgram.h"
#include "TextureHandle.h"

class BumpShader :
	public StandardShaderProgram
{
public:
	~BumpShader(void);
    static
    BumpShader*
    factory( ShaderType type );
	void setBumpMapTexture(boost::shared_ptr<TextureHandle> tex);

    virtual void
    setUp( Object*  shape,
           Viewer*  viewer,
           Light*   light );


protected:
	BumpShader(ShaderType type);
	BumpShader() {} // do not use

	void init();
	void init(const std::string &vertexShader, const std::string &fragmentShader);

    GLint   m_bumpmap_sampler_loc;
	GLint   m_lightpos_in_objspace_loc;
	GLint   m_cam_pos_in_objspace_loc;
	boost::shared_ptr<TextureHandle> m_bump_tex;

private:
	BumpShader(const BumpShader &o) {}

};

#endif // _BUMPSHADER_H_