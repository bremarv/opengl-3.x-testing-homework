#ifndef TRACKBALL_H_
#define TRACKBALL_H_

#include <siut/simd/Vec3f.hpp>
#include <siut/simd/Vec2f.hpp>
#include <siut/simd/Quatf.hpp>

class TrackBall
{
  public:
    TrackBall(float radius = 3.f)
	: m_radius(radius)
    {
    }

    void begin_drag(float const &x, float const &y,
		    siut::simd::Quatf const &current_rotation);
    void drag(float const &x, float const &y);
    void end_drag();
    const siut::simd::Quatf& getrotation();
    const siut::simd::Quatf& getoriginalrotation();
    siut::simd::Quatf gettotalrotation();
    void setballsize(float radius);
    const bool& isactive();
  protected:
    void project_onto_surface(siut::simd::Vec3f &p);
    void compute_rotation();
    
    siut::simd::Vec3f m_anchor;
    siut::simd::Vec3f m_current;
    siut::simd::Quatf m_rotation;
    siut::simd::Quatf m_original_rotation;
    float m_radius;
    bool m_active;
    
};

#endif
