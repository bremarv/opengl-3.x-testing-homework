#ifndef _TEXTUREDSHADER_H_
#define _TEXTUREDSHADER_H_

#include <boost/shared_ptr.hpp>
#include "ShaderProgram.h"
#include "TextureHandle.h"

class TexturedShader :
	public StandardShaderProgram
{
public:
	~TexturedShader(void);
    static
    TexturedShader*
    factory( ShaderType type );
	void setTexture(boost::shared_ptr<TextureHandle> tex);

    virtual void
    setUp( Object*  shape,
           Viewer*  viewer,
           Light*   light );


protected:
	TexturedShader(ShaderType type);
	TexturedShader() {} // do not use

	void init();
	void init(const std::string &vertexShader, const std::string &fragmentShader);

    GLint   m_diffusemap_sampler_loc;
	GLint   m_lightpos_in_objspace_loc;
	GLint   m_cam_pos_in_objspace_loc;
	boost::shared_ptr<TextureHandle> m_tex;

private:
	TexturedShader(const TexturedShader &o) {}

};

#endif // _TEXTUREDSHADER_H_