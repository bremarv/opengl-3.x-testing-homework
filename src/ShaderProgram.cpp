#include "ShaderProgram.h"
#include <stdexcept>
#include <GlTools.h>
#include "constants.h"
#include <iostream>
#include <fstream>

#include <siut/simd/MatOperations.hpp>
#include <siut/simd/MatTransforms.hpp>
using std::string;
using std::runtime_error;
using GlTools::compileShader;
using GlTools::linkProgram;

using siut::simd::Vec4f;
using siut::simd::Mat4f;
using siut::simd::matrixFromRowMajorEntriesf;
using siut::simd::frustumMatrixf;

static string vanilla_vs_src =
	"#version 130\n"
	"\n"
	"in  vec2 in_v_TexCoord;\n"
	"in  vec3 in_v_Normal;\n"
	"in  vec3 in_v_Position;\n"
	"\n"
	"out vec2 in_f_TexCoord;\n"
	"out vec3 in_f_CamSpaceNormal;\n"
	"out vec3 in_f_CamSpacePosition;\n"
        "out vec4 in_f_LightTexPosition;\n"
	"\n"
	"uniform mat4 ModelView;\n"
	"uniform mat4 ModelViewProjection;\n"
	"uniform mat3 ModelViewInverseTranspose;\n"
        "uniform mat4 LightTexFromObjectMatrix;\n"
	"\n"
        "void\n"
        "main()\n"
        "{\n"
	"    in_f_TexCoord = in_v_TexCoord;\n"
	"    in_f_CamSpaceNormal = ModelViewInverseTranspose * in_v_Normal;\n"
	"    vec4 csp = ModelView * vec4( in_v_Position, 1.0 );\n"
	"    in_f_CamSpacePosition = (1.0/csp.w)*csp.xyz;\n"
        "    vec4 p =  vec4( in_v_Position, 1.0 );\n"
        "    vec4 lsp = LightTexFromObjectMatrix * p;\n"
        "    in_f_LightTexPosition = lsp;\n"
        "    gl_Position = ModelViewProjection * p;\n"
        "}\n";



static string phong_fs_src =
	"#version 130\n"
	"\n"
	"in vec2 in_f_TexCoord;\n"
	"in vec3 in_f_CamSpaceNormal;\n"
	"in vec3 in_f_CamSpacePosition;\n"
	"\n"
	"out vec4 res_Color;\n"
	"\n"
	"uniform vec4 color;\n"
	"uniform vec3 CamSpaceLightPos;\n"
	"\n"
        "void\n"
        "main()\n"
        "{\n"
	"    vec3 p = in_f_CamSpacePosition;\n"
        "    vec3 l = CamSpaceLightPos-p;\n"
	"    float d = 1.0/length(l);\n"
	"    l *= d;\n"
        "    vec3 n = normalize( in_f_CamSpaceNormal );\n"
	"    vec3 v = normalize( p );\n"
	"    vec3 r = reflect( v, n );\n"
        "    float diff = d*max( 0.1, dot( n, l ) );\n"
        "    float spec = pow( max( 0.0, dot(n, r)), 20.0);\n"
	"    res_Color = diff * color + \n"
        "                spec * vec4(1.0);\n"
        "}\n";

static string phong_with_shadow_fs_src =
        "#version 130\n"
        "\n"
        "in vec2 in_f_TexCoord;\n"
        "in vec3 in_f_CamSpaceNormal;\n"
        "in vec3 in_f_CamSpacePosition;\n"
        "in vec4 in_f_LightTexPosition;\n"
        "\n"
        "out vec4 res_Color;\n"
        "\n"
        "uniform vec4 color;\n"
        "uniform vec3 CamSpaceLightPos;\n"
        "uniform mat4 CamSpaceToLightTex;\n"
        "uniform sampler2DShadow ShadowMap;\n"
        "\n"
        "float\n"
        "getIllumination( vec4 lph )\n"
        "{\n"
        "    return textureProj( ShadowMap, lph )*float(lph.z > 0.0);\n"
        "    return float( all(greaterThan(lph.xyz, vec3(0))) && all(lessThan(lph.xy,lph.ww)) ) ;\n"
        "}\n"
        "void\n"
        "main()\n"
        "{\n"
        "    vec3 p = in_f_CamSpacePosition;\n"
        "    vec3 l = CamSpaceLightPos-p;\n"
        "    float d = 1.0/length(l);\n"
        "    l *= d;\n"
        "    vec3 n = normalize( in_f_CamSpaceNormal );\n"
        "    vec3 v = normalize( p );\n"
        "    vec3 r = reflect( v, n );\n"
        "    float i = getIllumination( in_f_LightTexPosition );\n"
        "    float amb = 0.2*max(0.0, dot(n,-v ) );\n"
        "    float diff = i*max( 0.0, dot( n, l ) );\n"
        "    float spec = i*pow( max( 0.0, dot(n, r)), 50.0);\n"
        "    res_Color = (amb+diff) * color + \n"
        "                spec * vec4(1.0);\n"
        "}\n";

static string lightshape_fs_src =
	"#version 130\n"
	"\n"
	"in vec2 in_f_TexCoord;\n"
	"in vec3 in_f_CamSpaceNormal;\n"
	"in vec3 in_f_CamSpacePosition;\n"
	"\n"
	"out vec4 res_Color;\n"
	"\n"
	"uniform vec4 color;\n"
        "void\n"
        "main()\n"
        "{\n"
        "    vec3 ambient = vec3(0.3,0.3,0);\n"
        "    vec3 glow = pow(in_f_TexCoord.y, 10.0 )*vec3(1);\n"
        "    res_Color = vec4( ambient + glow, 1.0);\n"
        "}\n";

static string shadowcast_fs_src =
        "#version 140\n"
        "\n"
        "out vec4 res_Color;\n"
        "\n"
        "void\n"
        "main()\n"
        "{\n"
        "    res_Color = vec4( vec3( 1.0-2.5*gl_FragCoord.w), 1.0 );\n"
        "}\n";

StandardShaderProgram::StandardShaderProgram( ShaderType type ) :
ShaderProgram(), m_type(type)
{
}

void StandardShaderProgram::init()
{
    string vs;
    string fs;
	string gs;

    switch( m_type ) {

    case LIGHT_SHADER:
        vs = vanilla_vs_src;
        fs = lightshape_fs_src;
        break;

    case PHONG_SHADER:
        vs = vanilla_vs_src;
        fs = phong_fs_src;
        break;

    case PHONG_WITH_SHADOW:
        vs = vanilla_vs_src;
        fs = phong_with_shadow_fs_src;
        break;

    case SHADOW_CAST:
        vs = vanilla_vs_src;
        fs = shadowcast_fs_src;
        break;

    default:
        throw std::runtime_error( __FILE__ );
    }
	init(vs, gs, fs);
}

void StandardShaderProgram::init(const std::string &vs, 
								 const std::string &gs,
								 const std::string &fs)
{
    ASSERT_GL;
    m_prog = glCreateProgram();
    if( !vs.empty() ) {
        m_vs = compileShader( vs, GL_VERTEX_SHADER);
        glAttachShader( m_prog, m_vs);
    }
    else {
        m_vs = 0;
    }
    if( !gs.empty() ) {
		m_gs = compileShader( gs, GL_GEOMETRY_SHADER_EXT);
        glAttachShader( m_prog, m_gs );
    }
    else {
        m_gs = 0;
    }
    ASSERT_GL;
    if( !fs.empty() ) {
        m_fs = compileShader( fs, GL_FRAGMENT_SHADER);
        glAttachShader( m_prog, m_fs );
    }
    else {
        m_fs = 0;
    }

	bindAttribLocations();
    ASSERT_GL;
    linkProgram( m_prog );
    ASSERT_GL;

	setupUniforms();
    ASSERT_GL;
}

void StandardShaderProgram::init(const std::string &vs, const std::string &fs)
{
	std::string gs;
	StandardShaderProgram::init(vs, gs, fs);
	return;
    ASSERT_GL;
    m_prog = glCreateProgram();
    if( !vs.empty() ) {
        m_vs = compileShader( vs, GL_VERTEX_SHADER);
        glAttachShader( m_prog, m_vs);
    }
    else {
        m_vs = 0;
    }
    if( !fs.empty() ) {
        m_fs = compileShader( fs, GL_FRAGMENT_SHADER);
        glAttachShader( m_prog, m_fs );
    }
    else {
        m_fs = 0;
    }
    ASSERT_GL;

	bindAttribLocations();
    ASSERT_GL;
    linkProgram( m_prog );
    ASSERT_GL;

	setupUniforms();
    ASSERT_GL;
}

void StandardShaderProgram::bindAttribLocations()
{
    glBindAttribLocation( m_prog, TEXCOORD_ATTRIB_LOCATION, "in_v_TexCoord" );
    glBindAttribLocation( m_prog, NORMAL_ATTRIB_LOCATION, "in_v_Normal" );
    glBindAttribLocation( m_prog, POSITION_ATTRIB_LOCATION, "in_v_Position" );
}

void StandardShaderProgram::setupUniforms()
{
    m_modelview_loc =
            glGetUniformLocation( m_prog, "ModelView" );
    m_modelview_it_loc =
            glGetUniformLocation( m_prog, "ModelViewInverseTranspose" );
    m_modelviewprojection_loc =
            glGetUniformLocation( m_prog, "ModelViewProjection" );
    m_color_loc =
            glGetUniformLocation( m_prog, "color" );
    m_lightpos_in_camspace_loc =
            glGetUniformLocation( m_prog, "CamSpaceLightPos" );
    m_lighttex_from_camspace_loc =
            glGetUniformLocation( m_prog, "CamSpaceToLightTex" );
    m_lighttex_from_objspace_loc =
            glGetUniformLocation( m_prog, "LightTexFromObjectMatrix");
    m_shadowmap_sampler_loc =
            glGetUniformLocation( m_prog, "ShadowMap" );
}

StandardShaderProgram*
StandardShaderProgram::factory( ShaderType type )
{
    StandardShaderProgram* ret = new StandardShaderProgram(type);
	try
	{
		ret->init();
	} catch (...)
	{
		delete ret;
		throw;
	}

    return ret;
}

void
StandardShaderProgram::setUp( Object*  shape,
                      Viewer*  camera,
                      Light*   light )
{
    HasGeometry* hg = dynamic_cast<HasGeometry*>( shape );
    glUseProgram( m_prog );

    if( hg != NULL ) {
        if( m_color_loc != -1 ) {
            Vec4f c = hg->getColor();
            glUniform4f( m_color_loc, c[0], c[1], c[2], c[3] );
        }
    }

    if( camera != 0 ) {
        Mat4f MV = camera->getObjectFromWorldMatrix() *
                   shape->getWorldFromObjectMatrix();

        // set modelview matrix
        if( m_modelview_loc != -1 ) {
            glUniformMatrix4fv( m_modelview_loc,
                                1, GL_FALSE,
                                MV.c_ptr() );
        }
        // set modelview inverse transpose (normal matrix)
        if( m_modelview_it_loc != -1 ) {
            Mat4f MVi = shape->getObjectFromWorldMatrix() *
                        camera->getWorldFromObjectMatrix();

            GLfloat MVit[9] =
            {
                MVi(0,0), MVi(1,0), MVi(2,0),
                MVi(0,1), MVi(1,1), MVi(2,1),
                MVi(0,2), MVi(1,2), MVi(2,2)
            };
            glUniformMatrix3fv( m_modelview_it_loc,
                                1, GL_FALSE,
                                &MVit[0] );
        }
        // set modelview-projection matrix
        if( m_modelviewprojection_loc != -1 ) {
            Mat4f MVP = camera->getProjectionMatrix() * MV;
            glUniformMatrix4fv( m_modelviewprojection_loc,
                                1, GL_FALSE,
                                MVP.c_ptr() );
        }
        // set light-related matrices
        if( light != NULL ) {
            // calculate and set position of light source in cam space
            if( m_lightpos_in_camspace_loc != -1 ) {
                Vec4f lp = camera->getObjectFromWorldMatrix() *
                           light->getWorldFromObjectMatrix() *
                           Vec4f( 0.0f, 0.0f, 0.0f, 1.0f );
                float s = 1.0f/lp[3];
                glUniform3f( m_lightpos_in_camspace_loc, s*lp[0], s*lp[1], s*lp[2] );
            }
            // for use with forward shading (positions given in obj space)
            if( m_lighttex_from_objspace_loc != -1 ) {
                Mat4f MWO_o = shape->getWorldFromObjectMatrix();
                Mat4f MOW_l = light->getObjectFromWorldMatrix();
                Mat4f PCO_l = light->getProjectionMatrix();

                // converts from clip coordinates to texture space coords.
                Mat4f TTC_l =
                        matrixFromRowMajorEntriesf( 0.5f, 0.0f, 0.0f, 0.5f,
                                                    0.0f, 0.5f, 0.0f, 0.5f,
                                                    0.0f, 0.0f, 0.5f, 0.5f,
                                                    0.0f, 0.0f, 0.0f, 1.0f );

                Mat4f LightTexFromObjectMatrix = TTC_l * PCO_l  * MOW_l * MWO_o;
                glUniformMatrix4fv( m_lighttex_from_objspace_loc,
                                    1, GL_FALSE,
                                    LightTexFromObjectMatrix.c_ptr() );
            }
            // for use with deferred shading (positions given in cam space)
            if( m_lighttex_from_camspace_loc != -1 ) {
                Mat4f MWO_c = camera->getWorldFromObjectMatrix();
                Mat4f MOW_l = light->getObjectFromWorldMatrix();
                Mat4f PCO_l  = light->getProjectionMatrix();

                // converts from clip coordinates to texture space coords.
                Mat4f TTC_l =
                        matrixFromRowMajorEntriesf( 0.5f, 0.0f, 0.0f, 0.5f,
                                                    0.0f, 0.5f, 0.0f, 0.5f,
                                                    0.0f, 0.0f, 0.5f, 0.5f,
                                                    0.0f, 0.0f, 0.0f, 1.0f );

                Mat4f LightTexFromCamMatrix = TTC_l * PCO_l * MOW_l * MWO_c;
                glUniformMatrix4fv( m_lighttex_from_camspace_loc,
                                    1, GL_FALSE,
                                    LightTexFromCamMatrix.c_ptr() );
            }

            // set and enable shadow map texture sampler
            if( m_shadowmap_sampler_loc != -1 ) {
                glUniform1i( m_shadowmap_sampler_loc, 0 );
                glActiveTexture( GL_TEXTURE0 );
                glBindTexture( GL_TEXTURE_2D, light->getShadowMapTex() );
            }
        }
    }
}

					  	
std::string StandardShaderProgram::getVanillaVsSrc() const
{
	return vanilla_vs_src;
}

std::string StandardShaderProgram::getLightShapeFsSrc() const
{
	return lightshape_fs_src;
}
	
std::string StandardShaderProgram::getPhongFsSrc() const
{
	return phong_fs_src;
}

std::string StandardShaderProgram::getPhongWithShadowFsSrc() const
{
	return phong_with_shadow_fs_src;
}

std::string StandardShaderProgram::getShadowcastFsSrc() 
{
	return shadowcast_fs_src;
}

std::string StandardShaderProgram::readshaderfromfile(const std::string filename) const
{
    int lenght;
    char *buffer;

    std::ifstream ifs;
    ifs.open(filename.c_str(), std::ios::in);
    if(ifs.fail())
    {
	return NULL;//temp, throw error here??
    }

    ifs.seekg(0, std::ios::end);
    lenght = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    buffer = new char[lenght];

    ifs.read(buffer, lenght);
    ifs.close();

    std::string returnstring(buffer, lenght);

    delete[] buffer;

    return returnstring;
}
