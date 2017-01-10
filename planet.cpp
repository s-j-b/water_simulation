#include "planet.h"

using namespace cs40;

Planet::Planet(QString name, float o_rad, float o_tau, float ecc,
               float inc, float rad, float r_tau, float tilt):
    m_name(name), m_orb_radius(o_rad), m_orb_tau(o_tau), m_texture(NULL),
    m_eccentricity(ecc), m_inclination(inc), m_radius(rad),
    m_rot_tau(r_tau), m_axial_tilt(tilt),
    m_has_texture(false), m_theta(0), m_phi(0), m_disc_displacement(o_rad)
{

}

void Planet::move(float delHr){
    m_phi += delHr / m_rot_tau;
    m_theta += delHr / m_orb_tau;
}

float Planet::wrap(float val){
    while(val > 180){val -= 360;}
    while(val < -180){val += 360;}
    return val;
}

void Planet::setTexture(const QString& fname){
  if(m_texture){
    delete m_texture;
  }
  m_texture = new QOpenGLTexture(QImage(fname).mirrored());
  m_has_texture = true;
}
