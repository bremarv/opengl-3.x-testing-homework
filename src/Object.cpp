#include <GlTools.h>
#include <algorithm>
#include <limits>
#include <Object.h>
#include <siut/simd/QuatOperations.hpp>
#include <siut/simd/MatTransforms.hpp>

using std::max;
using siut::simd::Vec3f;
using siut::simd::Vec4f;
using siut::simd::Mat4f;
using siut::simd::Quatf;
using siut::simd::perspectiveMatrixf;
using siut::simd::frustumMatrixf;
using siut::simd::translationMatrixf;
using siut::simd::translationMatrixInversef;
using siut::simd::rotationMatrix4f;
using siut::simd::rotationMatrixInverse4f;


void
Object::calculateTransformMatrices()
{
    m_obj_to_world = translationMatrixf( m_position ) *
                     Mat4f( m_scale[0], 0.0f,       0.0f, 0.0f,
                            0.0f, m_scale[1],       0.0f, 0.0f,
                            0.0f,       0.0f, m_scale[2], 0.0f,
                            0.0f,       0.0f,       0.0f, 1.0f ) *
                     rotationMatrix4f( m_orientation ) *
                     translationMatrixf( m_local_position );

    m_world_to_obj = translationMatrixInversef( m_local_position ) *
                     rotationMatrixInverse4f( m_orientation ) *
                     Mat4f( 1.0f/m_scale[0], 0.0f,            0.0f, 0.0f,
                            0.0f, 1.0f/m_scale[1],            0.0f, 0.0f,
                            0.0f,            0.0f, 1.0f/m_scale[2], 0.0f,
                            0.0f,            0.0f,            0.0f, 1.0f ) *
                     translationMatrixInversef( m_position );
    m_transform_tainted = false;
}


void
Viewer::calculateProjectionMatrices()
{
    // make sure that far is never negative
    float far = max( 0.0f, -getMin( m_view_volume )[2] ) + 0.2f;

    // and never let near get nearer than 1% of the far value.
    float near = max( 0.001f*far, -getMax( m_view_volume )[2] );

    m_projection = perspectiveMatrixf( m_field_of_view,
                                       m_aspect_ratio,
                                       near,
                                       far );
    m_projection_tainted = false;
}

Light::Light( Geometry* geo, GLsizei shadow_map_size )
: Viewer(),
  HasGeometry( geo ),
  m_shadow_map_size( shadow_map_size )
{
    glGenTextures( 1, &m_shadow_map_tex );
    glBindTexture( GL_TEXTURE_2D, m_shadow_map_tex );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24,
                  m_shadow_map_size, m_shadow_map_size, 0,
                  GL_DEPTH_COMPONENT, GL_FLOAT, NULL );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    // when accessed, do a depth test (and not a fetch)
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE );

    // compare function, use less than or equal
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL );

    glBindTexture( GL_TEXTURE_2D, 0 );
    ASSERT_GL;

}

