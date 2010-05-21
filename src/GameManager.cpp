#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <assert.h>
#include <cmath>
#include <stdexcept>
#include <siut/simd/Vec3f.hpp>
#include <siut/simd/BBox3f.hpp>
#include <siut/simd/Quatf.hpp>
#include <siut/simd/QuatOperations.hpp>
#include <siut/simd/Mat4f.hpp>
#include <siut/simd/MatTransforms.hpp>
#include <siut/simd/Utils.hpp>


#include "GameManager.h"
#include "BumpShader.h"
#include "CubeMapShader.h"
#include "Cube.h"
#include "IndexedTriStrip.h"
#include "Snake.h"
#include "constants.h"
#include "BumpSkinningShader.h"

#include "GlTools.h"
using std::cerr;
using std::endl;

using siut::simd::Vec3f;
using siut::simd::BBox3f;
using siut::simd::Vec4f;
using siut::simd::Quatf;
using siut::simd::rotateVec;
using siut::simd::quatFromAxisAngleRotation;
using siut::simd::Mat4f;
using siut::simd::asEuclidean;
using siut::simd::asHomogeneous;
using siut::simd::perspectiveMatrixf;
using siut::simd::frustumMatrixf;
using siut::simd::translationMatrixf;
using siut::simd::rotationMatrix4f;

GLuint createCubeMap()
{
    GLfloat cols[24] = {1.f, 0.f, 0.f, 1.f,
			1.f, 1.f, 0.f, 1.f,
			1.f, 0.f, 1.f, 1.f,
			0.f, 1.f, 0.f, 1.f,
			0.f, 1.f, 1.f, 1.f,
			0.f, 0.f, 1.f, 1.f};

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
    ASSERT_GL;
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    ASSERT_GL;
    for (unsigned int face = 0; face < 6; face++) {
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGBA,
		     1, 1, 0, GL_RGBA, GL_FLOAT, &cols[face*4]);
    }
    ASSERT_GL;
    return tex;
}

void GameManager::CreateCubeMapFrameBuffer(int width, int height)
{
    // depth cube map
    GLuint depth_tex;
    glGenTextures(1, &depth_tex);
	
    glGenTextures(1, &depth_tex);
    m_depthCubemap_tex.reset(new TextureHandle(depth_tex));
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthCubemap_tex->getTex());
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    for (int face = 0; face < 6; face++) {
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_DEPTH_COMPONENT24,
		     width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }

    // color cube map
    GLuint cubeMap;
    glGenTextures(1, &cubeMap);
    m_renderToCubemap_tex.reset(new TextureHandle(cubeMap));
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_renderToCubemap_tex->getTex());
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    for (int face = 0; face < 6; face++) {
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGBA,
		     width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    }

    // framebuffer object
    glGenFramebuffers(1, &m_cubemap_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_cubemap_fbo);
    glFramebufferTextureEXT(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthCubemap_tex->getTex(), 0);
    glFramebufferTextureEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_renderToCubemap_tex->getTex(), 0);

    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    ASSERT_FBO;
}




void
GameManager::init()
{
    // Initialize SDL
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        std::stringstream err;
        err << "Could not initialize SDL: " << SDL_GetError();
        throw std::runtime_error(err.str());
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    // Set OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);  // Use double buffering in OpenGL
    // SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);   // Use framebuffer with 16 bit depth buffer
    // SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);      // Use framebuffer with 8 bit for red
    // SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);    // Use framebuffer with 8 bit for green
    // SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);     // Use framebuffer with 8 bit for blue
    // SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);    // Use framebuffer with 8 bit for alpha

    SDL_WM_SetCaption("PG611 Framework", "");

    // Initalize video
    if(SDL_SetVideoMode(static_cast<int>(WINDOW_SIZE_WIDTH), static_cast<int>(WINDOW_SIZE_HEIGHT), 0, SDL_OPENGL) == NULL)
    {
        std::stringstream err;
        err << "SDL_SetVideoMode failed ";
        throw std::runtime_error(err.str());
    }

    // Init glew
    glewExperimental = GL_TRUE; // Required in openGL 3.1 forward compatible contexts because
    // GL_EXTENSIONS is deprecated argument for glGetString
    GLenum glewErr = glewInit();
    if(glewErr != GLEW_OK)
    {
        std::stringstream err;
        err << "Error initializing GLEW: " << glewGetErrorString(glewErr);
        throw std::runtime_error(err.str());
    }
    glGetError(); // glewInit finner ikke extension stringen
    // men setter funksjonspekere for core funksjoner
    ASSERT_GL;
    // Needed for geometry shader
    assert(GLEW_EXT_geometry_shader4);

    // create VAOs
    m_geo_outer_cube = StandardGeometry::factory( StandardGeometry::INVERTED_CUBE );
    m_geo_cube = Cube::factory(  );
//	m_geo_cube = IndexedTriStrip::createCylinder(20, 40);
    m_geo_snake = Snake::factory();
    m_geo_light = StandardGeometry::factory( StandardGeometry::PYRAMID );

    ASSERT_GL;
    // Load Textures
    m_diffuse_tex.reset(TextureHandle::createTexture("textures/Heightfield.png"));
    m_bump_tex.reset(TextureHandle::createNormalMapFromHeightField("textures/Heightfield2.png"));
    // m_bump_tex.reset(TextureHandle::createTexture("textures/rockwall.tga"));
    ASSERT_GL;
    glActiveTexture(GL_TEXTURE0+ENVIRONMENT_MAP_TEXTURE_UNIT);
    m_cubemap_tex.reset(new TextureHandle(createCubeMap()));
    glActiveTexture(GL_TEXTURE0+DIFFUSE_MAP_TEXTURE_UNIT);
    ASSERT_GL;

    // create shaders
    m_phong_shader = StandardShaderProgram::factory( StandardShaderProgram::PHONG_WITH_SHADOW );
    m_light_shader = StandardShaderProgram::factory( StandardShaderProgram::LIGHT_SHADER );
    m_shadowcast_shader = StandardShaderProgram::factory( StandardShaderProgram::SHADOW_CAST );
    // m_skinning_shader = SkinningShader::factory( StandardShaderProgram::PHONG_SHADER );
    m_skinning_shader = BumpSkinningShader::factory( StandardShaderProgram::PHONG_SHADER );
    m_skinning_shadowcast_shader = SkinningShader::factory(
	StandardShaderProgram::SHADOW_CAST);
    {
	BumpShader *tex_shader = BumpShader::factory(StandardShaderProgram::PHONG_SHADER);
	tex_shader->setBumpMapTexture(m_bump_tex);
	m_textured_shader = tex_shader;
    }
    m_cubemap_shader = CubeMapShader::factory(StandardShaderProgram::PHONG_SHADER);
    ASSERT_GL;

    // create objects
    m_camera = new Camera;
    m_camera->setPosition( Vec3f(0.0f, 0.0f, 2.0f ) );
    m_camera->setFieldOfView( 0.7f );
    m_camera->setAspectRatio( (float)WINDOW_SIZE_WIDTH/(float)WINDOW_SIZE_HEIGHT );

    m_light = new Light( m_geo_light, SHADOW_MAP_SIZE );
    m_light->setColor( Vec4f( 1.0f, 1.0f, 1.0f, 1.0f ) );
    m_light->setFieldOfView( 1.570796327f );
    m_light->setAspectRatio( 1.0f );


    m_outer_box = new Shape( m_geo_outer_cube );
    m_outer_box->setLocalPosition( Vec3f( -1.0f ) );
    m_outer_box->setColor( Vec4f( 0.8, 0.8, 1.0, 1.0 ) );

    for(size_t i=0; i<1; i++) {
        Shape* s = new Shape( m_geo_cube );
        s->setColor( Vec4f( i/30.0f, 1.0f-i/30.0f, 0.0f, 1.0f ) );
        s->setScale( 0.4f /*0.1f*/ );
        m_boxes.push_back( s );
    }

    for(size_t i=0; i<1;i++) {
	SkinningShape *s = new SkinningShape( m_geo_snake );
        s->setColor( Vec4f( i/30.0f, 1.0f-i/30.0f, 0.0f, 1.0f ) );
        s->setScale( 1.2f /*0.1f*/ );
	m_snakes.push_back( s );
    }

    // Set up render passes
    //
    // We only include the outer box and the light in the onscreen pass (and not
    // the shadow pass) as they don't cast shadows. Further, this allows us to
    // get tighter bounding boxes for the shadow-map pass, which makes better
    // utilization of the depth buffer.

    m_onscreen_pass.addRenderItem( m_outer_box, m_phong_shader );
//    m_shadow_pass.addRenderItem( m_outer_box, m_shadowcast_shader );

    m_onscreen_pass.addRenderItem( m_light, m_light_shader );
    for(size_t i=0; i<m_boxes.size(); i++) {
//		m_onscreen_pass.addRenderItem( m_boxes[i], m_textured_shader );
	// m_onscreen_pass.addRenderItem( m_boxes[i],  m_textured_shader );
        // m_onscreen_pass.addRenderItem( m_boxes[i], m_textured_shader );	
	// m_shadow_pass.addRenderItem( m_boxes[i], m_shadowcast_shader );
    }

    for(size_t i=0; i<m_snakes.size(); i++) {
	// m_cubemap_pass.addRenderItem( m_snakes[i], m_cubemap_gen_shader );
//        m_onscreen_pass.addRenderItem( m_boxes[i], m_textured_shader );
	m_onscreen_pass.addRenderItem( m_snakes[i], m_skinning_shader );
	// m_shadow_pass.addRenderItem( m_snakes[i], m_skinning_shadowcast_shader );
    }


    // Set up shadow map FBO
    glGenFramebuffers( 1, &m_shadow_fbo );
    glBindFramebuffer( GL_FRAMEBUFFER, m_shadow_fbo );
    glFramebufferTexture2D( GL_FRAMEBUFFER,
                            GL_DEPTH_ATTACHMENT,
                            GL_TEXTURE_2D,
                            m_light->getShadowMapTex(),
                            0 );
    glDrawBuffer( GL_NONE );
    ASSERT_FBO;
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glPrimitiveRestartIndex(std::numeric_limits<GLuint>::max());
    glEnable(GL_PRIMITIVE_RESTART);

    m_start_ticks = SDL_GetTicks();
    m_prev_fps_ticks = m_start_ticks;
    m_frames = 0;

}


void
GameManager::animateFrame()
{
    long ticks = SDL_GetTicks() - m_start_ticks;
    double t = (double)ticks/1000.0;

    m_light->setScale( 0.09f );
    m_light->setOrientation( quatFromAxisAngleRotation( Vec3f(1.0f, 0.0f, 0.0f ), static_cast<float>(0.1f*t)) *
                             quatFromAxisAngleRotation( Vec3f(0.0f, 1.0f, 0.0f ), static_cast<float>(0.3f*t)) *
                             quatFromAxisAngleRotation( Vec3f(0.0f, 0.0f, 1.0f ), static_cast<float>(0.2f*t)) );
    m_light->setLocalPosition( Vec3f( 0.0f, 0.0f, 10.0f ) );

    m_outer_box->setOrientation( quatFromAxisAngleRotation( Vec3f(1.0f, 0.0f, 0.0f ), static_cast<float>(0.3f*t)) *
                                 quatFromAxisAngleRotation( Vec3f(0.0f, 1.0f, 0.0f ), static_cast<float>(0.2f*t)) *
                                 quatFromAxisAngleRotation( Vec3f(0.0f, 0.0f, 1.0f ), static_cast<float>(0.1f*t)) );

    for(size_t i=0; i<m_boxes.size(); i++) {
        float tt = static_cast<float>(t+(0.2*i));
        //m_boxes[i]->setPosition( Vec3f( 0.5*cos(1.3*tt), 0.5*sin(1.6*tt), 0.5*cos(1.5*tt) ) );
        m_boxes[i]->setOrientation( quatFromAxisAngleRotation( Vec3f(0.0f, 1.0f, 0.0f ), 0.07f*tt ) *
                                    quatFromAxisAngleRotation( Vec3f(0.0f, 0.0f, 1.0f ), 0.1f*tt ) *
                                    quatFromAxisAngleRotation( Vec3f(1.0f, 0.0f, 0.0f ), 0.15f*tt ) );
	m_boxes[i]->setLocalPosition( Vec3f( -0.5f, -0.5f, -0.5f ) );
	m_boxes[i]->setPosition( Vec3f( 0.0f, 0.0f, 0.0f ) );
        //m_boxes[i]->setOrientation( quatFromAxisAngleRotation( Vec3f(0.0f, 1.0f, 0.0f ), 0.01f ));
    }

    for(size_t i=0; i<m_snakes.size(); i++) {
        float tt = static_cast<float>(t+(0.2*i+2));
        //m_boxes[i]->setPosition( Vec3f( 0.5*cos(1.3*tt), 0.5*sin(1.6*tt), 0.5*cos(1.5*tt) ) );
        m_snakes[i]->setOrientation( quatFromAxisAngleRotation( Vec3f(0.0f, 1.0f, 0.0f ), 0.07f*tt ) *
				     quatFromAxisAngleRotation( Vec3f(0.0f, 0.0f, 1.0f ), 0.1f*tt ) *
				     quatFromAxisAngleRotation( Vec3f(1.0f, 0.0f, 0.0f ), 0.15f*tt ) );
	//m_snakes[i]->setLocalPosition( Vec3f( -0.5f, -0.5f, -0.5f ) );
	m_snakes[i]->setPosition( Vec3f( 0.0f, 0.0f, 0.0f ) );
	std::vector<siut::simd::Mat4f> &matrs=m_snakes[i]->getboneMatrices();
	m_geo_snake->createBoneMatrices(0.5f, matrs);
    }

    // update bounding boxes
    m_camera->setViewVolume( m_onscreen_pass.getViewerSpaceBoundingBox( m_camera->getObjectFromWorldMatrix() ) );
    m_light->setViewVolume( m_shadow_pass.getViewerSpaceBoundingBox( m_light->getObjectFromWorldMatrix() ) );

}

void
GameManager::renderFrame()
{
    glEnable( GL_CULL_FACE );

    glBindFramebuffer( GL_FRAMEBUFFER, m_shadow_fbo );
    glViewport( 0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE );
    glClearColor ( 0.0, 0.0, 0.0, 0.0 );
    glClear ( GL_DEPTH_BUFFER_BIT );
    glEnable( GL_DEPTH_TEST );

    glEnable( GL_POLYGON_OFFSET_FILL );

    glPolygonOffset( 1.1f, 4.0f );
    m_shadow_pass.render( m_light, m_light );
    glDisable( GL_POLYGON_OFFSET_FILL );
    glCullFace( GL_BACK );
	
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glViewport( 0, 0, WINDOW_SIZE_WIDTH, WINDOW_SIZE_HEIGHT );
    glClearColor ( 0.0, 0.0, 0.0, 0.0 );
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );
    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glActiveTexture(GL_TEXTURE0+ENVIRONMENT_MAP_TEXTURE_UNIT);
    //glBindTexture(GL_TEXTURE_CUBE_MAP, m_renderToCubemap_tex->getTex());
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap_tex->getTex());
    m_onscreen_pass.render( m_camera, m_light );
    glEnable( GL_CULL_FACE );

}

void
GameManager::play()
{
    ASSERT_GL;

    bool finished = false;
    while(!finished) {

        // poll for pending events
        SDL_Event event;
        while ( SDL_PollEvent( &event ) ) {
            switch ( event.type ) {
            case SDL_KEYDOWN:
		switch ( event.key.keysym.sym )
		{
		case SDLK_ESCAPE:
		    finished = true;
		    break;
		case 'r':
		    recomputeNormals();
		    break;
                }
                break;
	    case SDL_QUIT:
                finished = true;
                break;
            }
        }

	m_frames++;
	{
	    long ticks = SDL_GetTicks();
	    if (ticks - m_prev_fps_ticks >= 5000)
	    {
		float fps = m_frames / ((ticks-m_prev_fps_ticks)/1000.f);
		std::stringstream caption;
		caption << "PG611 Framework @" << fps << "fps"; 
		SDL_WM_SetCaption(caption.str().c_str(), "");
		m_prev_fps_ticks = ticks;
		m_frames = 0;
	    }
	}
        animateFrame();
        renderFrame();
        SDL_GL_SwapBuffers();
    }
}

void GameManager::quit()
{
    SDL_Quit();
}

void GameManager::recomputeNormals()
{
    m_geo_snake->recomputeNormals();
}
