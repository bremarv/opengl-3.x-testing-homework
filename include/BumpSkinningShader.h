#ifndef _BUMPSKINNINGSHADER_H_
#define _BUMPSKINNINGSHADER_H_

#include "BumpShader.h"

class BumpSkinningShader :
public BumpShader
{
  public:
    ~BumpSkinningShader(void);
    static BumpSkinningShader*
	factory(ShaderType type);
    virtual void
	setUp( Object* shape,
	       Viewer* viewer,
	       Light* light);

  protected:
    BumpSkinningShader(ShaderType type);
    BumpSkinningShader() {} //do not use

    void init();

    virtual void bindAttribLocations();
    virtual void setupUniforms();

    GLint m_bonematrices_loc;

  private:
    BumpSkinningShader(const BumpSkinningShader &o) {}
};

#endif
