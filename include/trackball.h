#ifndef TRACKBALL_H_
#define TRACKBALL_H_

#include <siut/simd/Vec3f.hpp>
#include <siut/simd/Vec2f.hpp>
#include <siut/simd/Quatf.hpp>

class TrackBall
{
  public:
    TrackBall()
	: m_radius(1.f)
    {
    }    

    void begin_drag(float const &x, float const &y);
    void drag(float const &x, float const &y);
    const siut::simd::Quatf& getrotation() {return m_transform; }
  protected:
    void project_onto_surface(siut::simd::Vec3f &p);
    void compute_rotation();
    
    siut::simd::Vec3f m_anchor;
    siut::simd::Vec3f m_current;
    siut::simd::Quatf m_transform;
    float m_radius;
};

#endif
