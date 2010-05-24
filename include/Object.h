#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <GL/glew.h>
#include <siut/simd/Vec3f.hpp>
#include <siut/simd/Vec4f.hpp>
#include <siut/simd/Quatf.hpp>
#include <siut/simd/Mat4f.hpp>
#include <siut/simd/BBox3f.hpp>
#include <Geometry.h>


class Object
{
public:
    Object()
    : m_transform_tainted( true ),
      m_position( siut::simd::Vec3f( 0.0f ) ),
      m_scale( siut::simd::Vec3f( 1.0f ) ),
//      m_orientation( siut::simd::Quatf( 0.0f, 0.0f, 0.0f, 1.0f ) ),
      m_orientation( siut::simd::Quatf( 1.0f, 0.0f, 0.0f, 0.0f ) ),
      m_local_position( siut::simd::Vec3f( 0.0f ) )
    {
    }

    virtual
    void
    dummy() {}

    void
    setPosition( const siut::simd::Vec3f& pos )
    {
        m_position = pos; m_transform_tainted = true;
    }

    void
    setLocalPosition( const siut::simd::Vec3f& pos )
    {
        m_local_position = pos; m_transform_tainted = true;
    }

    void
    setScale( float scale )
    {
        m_scale = siut::simd::Vec3f( scale ); m_transform_tainted = true;
    }

    void
    setScale( const siut::simd::Vec3f& scale )
    {
        m_scale = scale; m_transform_tainted = true;
    }

    void
    setOrientation( const siut::simd::Quatf& orientation )
    {
        m_orientation = orientation; m_transform_tainted = true;
    }

    const siut::simd::Quatf& getOrientation()
    {
	return m_orientation;
    }
    
    const siut::simd::Vec3f& getPosition()
    {
	return m_position;
    }

    const siut::simd::Mat4f&
    getWorldFromObjectMatrix()
    {
        if( m_transform_tainted ) {
            calculateTransformMatrices();
        }
        return m_obj_to_world;
    }

    const siut::simd::Mat4f&
    getObjectFromWorldMatrix()
    {
        if( m_transform_tainted ) {
            calculateTransformMatrices();
        }
        return m_world_to_obj;
    }

    void
    calculateTransformMatrices();

protected:
    bool               m_transform_tainted;

    siut::simd::Mat4f  m_obj_to_world;
    siut::simd::Mat4f  m_world_to_obj;

    siut::simd::Vec3f  m_position;
    siut::simd::Vec3f  m_scale;
    siut::simd::Quatf  m_orientation;
    siut::simd::Vec3f  m_local_position;

};

class Viewer : public Object
{
public:
    Viewer()
    : Object(),
      m_projection_tainted( true ),
      m_view_volume( siut::simd::BBox3f( siut::simd::Vec3f(0.0f),
                                         siut::simd::Vec3f(1.0f) ) ),
      m_aspect_ratio( 1.0f ),
      m_field_of_view( 0.2f )
    {}

    const siut::simd::Mat4f&
    getProjectionMatrix()
    {
        if( m_projection_tainted ) {
            calculateProjectionMatrices();
        }
        return m_projection;
    }

    /** Set the view volume of the viewer.
      *
      * \param volume The view volume, given as a bounding box in the viewer's
      *               local coordinate system.
      */
    void
    setViewVolume( const siut::simd::BBox3f& volume )
    {
        m_view_volume = volume; m_projection_tainted = true;
    }

    /** Set the aspect ratio of the viewer.
      *
      * \param aspect  Aspect ratio, defined as width divided by height.
      */
    void
    setAspectRatio( float aspect )
    {
        m_aspect_ratio = aspect; m_projection_tainted = true;
    }

    /** Set the field-of-view of the viewer.
      *
      * \param fov  The field of view along the height, specified in radians.
      */
    void
    setFieldOfView( float fov )
    {
        m_field_of_view = fov; m_projection_tainted = true;
    }

    void
    calculateProjectionMatrices();

protected:
    bool                 m_projection_tainted;
    siut::simd::Mat4f    m_projection;

    siut::simd::BBox3f   m_view_volume;
    float                m_aspect_ratio;
    float                m_field_of_view;


};

class HasGeometry
{
public:
    void setColor( const siut::simd::Vec4f& color ) { m_color = color; }

    const siut::simd::Vec4f& getColor() const { return m_color; }

    Geometry* getGeometry() const { return m_geo; }

protected:
    HasGeometry( Geometry *geo )
    : m_geo( geo ),
      m_color( siut::simd::Vec4f( 0.5f, 0.5f, 0.5f, 1.0f ) )
    { }

    HasGeometry();
    Geometry*	       m_geo;
    siut::simd::Vec4f  m_color;
};

class Shape : public Object, public HasGeometry
{
public:
    Shape( Geometry* geo )
    : Object(),
      HasGeometry( geo )
    {}

protected:
    Shape();

};


class Camera : public Viewer
{
public:
    Camera() : Viewer() {}
};

class Light : public Viewer, public HasGeometry
{
public:
    Light( Geometry* geo, GLsizei shadow_map_size );

    GLuint getShadowMapTex() { return m_shadow_map_tex; }

protected:
    Light();

    GLsizei  m_shadow_map_size;
    GLuint   m_shadow_map_tex;

};

#endif
