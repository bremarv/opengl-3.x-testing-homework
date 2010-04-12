#ifndef _RENDER_INSTANCE_H_
#define _RENDER_INSTANCE_H_

#include <vector>
#include <GL/glew.h>
#include <Geometry.h>
#include <ShaderProgram.h>
#include <siut/simd/BBox3f.hpp>
#include <siut/simd/Mat4f.hpp>

class RenderList
{
public:
    RenderList();

    void
    addRenderItem( Object* shape, ShaderProgram* program );

    siut::simd::BBox3f
    getViewerSpaceBoundingBox( const siut::simd::Mat4f& world_to_viewer );

    void
    render( Viewer* viewer, Light* light );

protected:
    struct RenderItem
    {
        RenderItem( Object* shape, ShaderProgram* program );

        Object*    m_shape;
        ShaderProgram*  m_program;
    };

    std::vector<RenderItem>  m_items;

};



#endif
