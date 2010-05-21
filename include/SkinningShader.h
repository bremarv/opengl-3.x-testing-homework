#ifndef _SKINNINGSHADER_H_
#define _SKINNINGSHADER_H_

#include <boost/shared_ptr.hpp>
#include <vector>
#include "ShaderProgram.h"
#include <siut/simd/Mat4f.hpp>


class SkinningShader :
public StandardShaderProgram
{
  public:
    ~SkinningShader(void);
    static
	SkinningShader*
	factory( ShaderType type );
    void setMatrices(const std::vector<siut::simd::Mat4f> &matrix);

    virtual void
	setUp( Object*  shape,
	       Viewer*  viewer,
	       Light*   light );


  protected:
    SkinningShader(ShaderType type);
    SkinningShader() {} // do not use

    void init();
    virtual void bindAttribLocations();
    virtual void setupUniforms();

    GLint m_boneMatrices_loc;
    GLint   m_diffusemap_sampler_loc;
    GLint   m_lightpos_in_objspace_loc;
    GLint   m_cam_pos_in_objspace_loc;

  private:
    SkinningShader(const SkinningShader &o) {}

};

#endif // _SKINNINGSHADER_H_
