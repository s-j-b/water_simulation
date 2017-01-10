#include "camera.h"
#include <iostream>
#include "printmatrix.h"

using namespace cs40;
using namespace std;
typedef QVector3D vec3;
typedef QVector4D vec4;
typedef QMatrix4x4 mat4;

/* Construct a new camera
 * with eye at origin
 * looking down -z axis
 * up direction in +y axis
 */
Camera::Camera() {

    m_curTheta = 0;
    
    vec3 origin(0, 0, 0);
    vec3 z(0, 0, 1);
    vec3 y(0, 1, 0);
    m_transform = QMatrix4x4();
    m_lookAt = lookAt(origin, -z, y);
}

/* Reproduce behavior of QMatrix4x4::lookAt, return new matrix */
mat4 Camera::lookAt(const vec3& eye, const vec3& at, const vec3& up) {

    //vec3 z = (-1) * (at - eye);
    vec3 x = QVector3D::crossProduct(at - eye, up);
    vec3 y = QVector3D::crossProduct(x, at - eye);

    vec3 u = x / x.length();
    vec3 v = y / y.length();
    vec3 n = (at - eye) / (at - eye).length();

    mat4 R(u[0], u[1], u[2], 0,
           v[0], v[1], v[2], 0,
           -n[0], -n[1], -n[2], 0,
           0, 0, 0, 1);

    mat4 T(1, 0, 0, -eye[0],
           0, 1, 0, -eye[1],
           0, 0, 1, -eye[2],
           0, 0, 0, 1);

    m_lookAt = R * T;

    QMatrix4x4 new_mat = QMatrix4x4();
    new_mat.lookAt(eye, at, up);

    return m_lookAt;
}

//
// move camera right by amount amt. Scene should shift left
//  amt is in world units //
void Camera::right(float amt) {
    qreal x = -amt;
    qreal y = 0;
    qreal z = 0;
    m_transform.setToIdentity();
    m_transform.translate(x,y,z);
    m_lookAt = m_transform * m_lookAt;
}

// move camera up //
void Camera::up(float amt) {
    qreal x = 0;
    qreal y = -amt;
    qreal z = 0;
    m_transform.setToIdentity();
    m_transform.translate(x,y,z);
    m_lookAt = m_transform * m_lookAt;
}

// move camera forward //
void Camera::forward(float amt) {
        
    qreal x = 0;

    qreal y;
    if (amt > 0) {
        y = amt * qSin(m_curTheta)/3.5;
    } else {
        y = amt * qSin(-m_curTheta)/3.5;
    }
    qreal z = amt;
    y = 0;
    m_curTheta += 0.4;
    
    m_transform.setToIdentity();
    m_transform.translate(x,y,z);
    m_lookAt = m_transform * m_lookAt;
}

// rotate CCW about -z axis _of camera_
//  angle is in degrees //
void Camera::roll(float angle) {
  m_transform.setToIdentity();
  m_transform.rotate(angle, 0, 0, 1);
  m_lookAt = m_transform * m_lookAt;
}

// rotate CCW about +x // 
void Camera::pitch(float angle) {
  m_transform.setToIdentity();

  m_transform.rotate(angle, 1, 0, 0);
    
  m_lookAt = m_transform * m_lookAt;
}

// CCW about +y //
void Camera::yaw(float angle) {
  m_transform.setToIdentity();
  m_transform.rotate(angle, 0, 1, 0);
  m_lookAt = m_transform * m_lookAt;
}

// reset camera to default location 
//  as in constructor */
void Camera::reset() {
    vec3 origin(0, 0, 0);
    vec3 z(0, 0, 1);
    vec3 y(0, 1, 0);
    
    m_lookAt = lookAt(origin, -z, y);
}



