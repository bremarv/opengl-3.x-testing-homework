#include <RenderList.h>

#include <siut/simd/Utils.hpp>
#include "GlTools.h"
using std::vector;
using siut::simd::Vec3f;
using siut::simd::Vec4f;
using siut::simd::BBox3f;
using siut::simd::Mat4f;
using siut::simd::asEuclidean;
using siut::simd::asHomogeneous;


RenderList::RenderList()
{
}

BBox3f
RenderList::getViewerSpaceBoundingBox( const Mat4f& world_to_viewer )
{

    BBox3f viewspace_bbox;
    for( vector<RenderItem>::iterator it = m_items.begin(); it != m_items.end(); ++it )
    {
        // transform matrix from object space to viewer space
        Object* obj = it->m_shape;
        HasGeometry* hg = dynamic_cast<HasGeometry*>( it->m_shape );
        if( hg != NULL ) {
            Mat4f obj_to_view = world_to_viewer * obj->getWorldFromObjectMatrix();

            // bounding box of shape
            const BBox3f& bbox = hg->getGeometry()->getObjectSpaceBoundingBox();

            // run through all eight corners of the bounding box, transform them and
            // make sure that the viewer bounding box is large enough to contain them.
            for( size_t i=0; i<8; i++ ) {
                Vec3f corner_in_viewspace =
                        asEuclidean( obj_to_view * asHomogeneous( corner( bbox, i) ) );
                include( viewspace_bbox, corner_in_viewspace );
            }
        }
    }
    return viewspace_bbox;
}

void
RenderList::render( Viewer* viewer, Light* light )
{
    // Perform a render pass. There is a lot of room for optimization here with
    // regard to state changes. Currently, programs are set for every object,
    // and grouping these together could remove the program state change.
    // Further, within each program, each set of geometries could be grouped
    // and rendered in one call using instanced rendering.
    //
    // Using a "taint" variable that is set by addRenderItem, we could add an
    // optimize-function which sorts the render list according to state changes
    // and build some structure we could use to utilize this info in this
    // function.

    for( vector<RenderItem>::iterator it = m_items.begin(); it != m_items.end(); ++it )
    {
		    ASSERT_GL;

        HasGeometry* hg = dynamic_cast<HasGeometry*>( it->m_shape );
        if( hg != NULL ) {
            it->m_program->setUp( it->m_shape, viewer, light );
    ASSERT_GL;
            hg->getGeometry()->draw();
        }
    ASSERT_GL;
    }
}



void
RenderList::addRenderItem( Object* shape, ShaderProgram* program )
{
    m_items.push_back( RenderItem( shape, program ) );
}

RenderList::RenderItem::RenderItem( Object* shape, ShaderProgram* program )
: m_shape( shape ),
  m_program( program )
{
}
