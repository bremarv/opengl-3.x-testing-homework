#ifndef _SHADER_PROGRAM_H_
#define _SHADER_PROGRAM_H_

#include <GL/glew.h>
#include <Object.h>

class ShaderProgram
{
public:
    virtual void
    setUp( Object*  shape,
           Viewer*  viewer,
           Light*   light ) =0;

};

class StandardShaderProgram : public ShaderProgram
{
public:
    enum ShaderType {
        LIGHT_SHADER,
        PHONG_SHADER,
        PHONG_WITH_SHADOW,
	SHADOW_CAST
    };

    static
    StandardShaderProgram*
    factory( ShaderType type );

    virtual void
    setUp( Object*  shape,
           Viewer*  viewer,
           Light*   light );

protected:
	StandardShaderProgram(ShaderType type); // used by factory
	StandardShaderProgram() {} // Do not use
	void init();
	void init(const std::string &vertexShader, const std::string &fragmentShader);
	void init(const std::string &vertexShader, const std::string &gemetryShader,
		const std::string &fragmentShader);
	virtual void bindAttribLocations();
	virtual void setupUniforms();
	std::string readshaderfromfile(const std::string filename) const;

	std::string getVanillaVsSrc() const;
    std::string getLightShapeFsSrc() const;
	std::string getPhongFsSrc() const;
	std::string getPhongWithShadowFsSrc() const;
	static std::string getShadowcastFsSrc();

	ShaderType m_type;
    GLuint  m_vs;
    GLuint  m_gs;
    GLuint  m_fs;
    GLuint  m_prog;

    GLint   m_modelview_loc;
    GLint   m_modelview_it_loc;
    GLint   m_modelviewprojection_loc;
    GLint   m_color_loc;
    GLint   m_lightpos_in_camspace_loc;
    GLint   m_lighttex_from_camspace_loc;
    GLint   m_lighttex_from_objspace_loc;
    GLint   m_shadowmap_sampler_loc;
    GLint   m_boneMatrices_loc;

private:
	StandardShaderProgram(const StandardShaderProgram &o) {}
};

#endif
