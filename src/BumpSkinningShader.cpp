#include <string>
#include <stdexcept>
#include "SkinningShape.h"
#include "constants.h"
#include "GlTools.h"
#include <iostream>

#include "BumpSkinningShader.h"


BumpSkinningShader::BumpSkinningShader(ShaderType type) :
    BumpShader(type)
{    
}

void BumpSkinningShader::init()
{
    std::string vs;
    std::string fs;

    switch(m_type)
    {
    case PHONG_SHADER:
	vs = readshaderfromfile( "bump_map_skinning.vert" );
	fs = readshaderfromfile( "bump_map.frag" );
	break;
    default:
	throw std::runtime_error(__FILE__);
    }
    
    BumpShader::init(vs, fs);
}

BumpSkinningShader*
BumpSkinningShader::factory( ShaderType type)
{
    BumpSkinningShader* ret = new BumpSkinningShader(type);
    try
    {
	ret->init();
    }catch (...)
    {
	delete ret;
	throw;
    }

    return ret;
}

void BumpSkinningShader::bindAttribLocations()
{
    BumpShader::bindAttribLocations();
    glBindAttribLocation( m_prog, SKINNINGINDEX_ATTRIB_LOCATION, "in_v_Bones" );
    glBindAttribLocation( m_prog, SKINNINGWEIGHT_ATTRIB_LOCATION, "in_v_Weights" );
}

void BumpSkinningShader::setupUniforms()
{
    BumpShader::setupUniforms();
    m_bonematrices_loc =
	glGetUniformLocation( m_prog, "boneMatrices" );
}

void BumpSkinningShader::setUp(Object*  shape,
			       Viewer*  camera,
			       Light*   light )
{
    BumpShader::setUp(shape, camera, light);
    SkinningShape *sp = dynamic_cast<SkinningShape *>(shape);
    assert(sp);
    std::vector<siut::simd::Mat4f> bonematrices = sp->getboneMatrices();

    // only got space for 4 bones, so dont overstep that
    glUniformMatrix4fv(
	m_bonematrices_loc,
	(bonematrices.size() > 4 ? 4 : bonematrices.size()),
	GL_FALSE, bonematrices[0].c_ptr() );

}


BumpSkinningShader::~BumpSkinningShader()
{
}
