#include "trackball.h"
#include <cmath>

using siut::simd::Vec3f;
using siut::simd::Quatf;

void TrackBall::begin_drag(float const &x, float const &y)
{
    m_anchor = Vec3f(x,y,0);
    project_onto_surface(m_anchor);
    m_current = Vec3f(x,y,0);
    project_onto_surface(m_current);
    compute_rotation();
}

void TrackBall::drag(float const &x, float const &y)
{
    m_current = Vec3f(x,y,0);
    project_onto_surface(m_current);
    compute_rotation();
}

void TrackBall::project_onto_surface(Vec3f &p)
{
    using std::sqrt;
    const static float radius2 = m_radius * m_radius;
    float length2 = p[0]*p[0] + p[1]*p[1];

    if(length2 <= radius2/2.f)
    {
	p[2] = sqrt(radius2 - length2);
    }
    else
    {
	p[2] = radius2 / (2.f * sqrt(length2));
	float length = sqrt(length2 + p[2] * p[2]);
	p /= length;
    }
    p = normalize(p);
}

void TrackBall::compute_rotation()
{
    // Vec3f axis = cross(m_anchor, m_current);
    // float angle = std::acos(dot(m_anchor, m_current));
    // m_transform = Quatf(std::cos(angle/2),
    // 			std::sin(angle/2)*axis);
    // m_transform = normalize(m_transform);
    
    using std::sqrt;
    float anglestuff = sqrt(2 + 2*dot(m_anchor, m_current));
    m_transform = Quatf(
    	anglestuff/2,
    	cross(m_anchor, m_current) /
    	anglestuff);
    m_transform = normalize(m_transform);
}
