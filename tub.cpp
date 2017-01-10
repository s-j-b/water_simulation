#include "tub.h"
#include <iostream>

using namespace cs40;
using std::cout;
using std::endl;

//wavelength for ripple
#define LAMBDA (.1)

//period for ripple
#define T (0.1)

//amplitude of ripple
#define AMP (0.01)

#define T_MAX (1.0)
#define R_MAX (1.0)

#define SCALE_1 (0.007)
#define SCALE_2 (-0.005)
void v_print(vec3 v) {
    //cout << "(" << v.x() << ", " << v.y() << ", " << v.z() << ")";
}

Tub::Tub():
    m_t(0.0),
    m_rows(100),
    m_cols(100),
    m_num_verts(2 * ((m_rows - 1) * m_cols) + 4 * (m_rows - 2)),

    m_scale_1(SCALE_1),
    m_scale_2(SCALE_2),
    m_wind(1.0),
    m_ripple_scale(1.0),

    m_color(0,1,1,0.9999),    
    m_vao(NULL),
    m_vbo(NULL)
{

    m_angle_1 = vec2(0.701, .701);
    m_angle_2 = vec2(0.701, -.701);
    
    m_vertices_cpu = new vec3[m_rows * m_cols];
    m_normals_cpu = new vec3[m_rows * m_cols];

    m_heights_cpu = new float[m_rows * m_cols];

    m_vertices = new vec3[m_num_verts];
    m_texCoords = new vec2[m_num_verts];
    m_normals = new vec3[m_num_verts];


    int r, c;
    float x, y, z;
    int i = 0;
    for (r = 0; r < m_rows; r++) {
        for (c = 0; c < m_cols; c++) {
            m_heights_cpu[i] = 0.0;

            x = ((float)r - float(m_rows - 1)/2.0)
                / ((m_rows - 1)/2.0);

            y = ((float)c - float(m_cols - 1)/2.0)
                / ((m_rows - 1)/2.0);
            if (x > 1.0) {

                cout << x << endl;
            }

            z = (float)m_heights_cpu[i];

            m_vertices_cpu[i] = vec3(x, y, z);

            i++;
        }
    }
    
    if(initVBO()){
        
        int VertSize = (m_num_verts) * sizeof(vec3);

        int TexSize = (m_num_verts) * sizeof(vec2);
        //TexSize += p
        //cout << "DataSize: " << DataSize << endl;
        m_vbo->bind();

        m_vbo->allocate(VertSize + TexSize + VertSize);

        updateNormals();
        updateVBO();
        
        m_vbo->release();
    }
}

void Tub::makeRipple() {

    float rx = 2.0 * ((float)qrand() / RAND_MAX) - 1.0;
    float ry = 2.0 * ((float)qrand() / RAND_MAX) - 1.0;

    Ripple jamestown;
    jamestown.x = rx;
    jamestown.y = ry;
    jamestown.t = 0.0;
    jamestown.r_max = 0.02;
    m_ripples.append(jamestown);
}


void Tub::updateVBO() {

    copyData();

    int VerticesSize = (m_num_verts) * sizeof(vec3);
    int TexSize = (m_num_verts) * sizeof(vec2);
    int NormalsSize = (m_num_verts) * sizeof(vec3);

    m_vbo->bind();
    m_vbo->write(0,
                 m_vertices,
                 VerticesSize);

    m_vbo->write(VerticesSize,
                 m_texCoords,
                 TexSize);

    m_vbo->write(VerticesSize + TexSize,
                 m_normals,
                 NormalsSize);

    m_vbo->release();
}       

void Tub::copyData() {
    int coord;

    int i = 0;
    for (int r = 0; r < m_rows - 1; r++) {
        for (int c = 0; c < m_cols; c++) {

            coord = (r * m_cols) + c;
            m_vertices[i] = m_vertices_cpu[coord];
            m_texCoords[i] = vec2(r, c);
            m_normals[i] = m_normals_cpu[coord];
            i++;
            
            coord = ((r + 1) * m_cols) + c;
            m_vertices[i] = m_vertices_cpu[coord];
            m_texCoords[i] = vec2(r + 1, c);
            m_normals[i] = m_normals_cpu[coord];
            i++;
        }

        if (r < m_rows - 2) {

            coord = (r + 1) * m_cols + (m_cols - 1);
            m_vertices[i] = m_vertices_cpu[coord];
            m_texCoords[i] = vec2(r, m_cols - 1);
            m_normals[i] = m_normals_cpu[coord];
            i++;

            coord = (r + 1) * m_cols;
            m_vertices[i] = m_vertices_cpu[coord];
            m_texCoords[i] = vec2(r + 1, 0);
            m_normals[i] = m_normals_cpu[coord];
            i++;

            m_vertices[i] = m_vertices_cpu[coord];
            m_texCoords[i] = vec2(r + 1, 0);
            m_normals[i] = m_normals_cpu[coord];
            i++;

            m_vertices[i] = m_vertices_cpu[coord];
            m_texCoords[i] = vec2(r + 1, 0);
            m_normals[i] = m_normals_cpu[coord];
            i++;
        }
    }
}

bool Tub::initVBO(){
    m_vao = new QOpenGLVertexArrayObject();
    bool ok = m_vao->create();
    if( !ok ){ return false; }
    m_vbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    ok = m_vbo->create();
    if( !ok ){ return false; }
    m_vbo->setUsagePattern(QOpenGLBuffer::StaticDraw);
    return ok;
}

Tub::~Tub(){
    if(m_vbo){
        m_vbo->release();
        delete m_vbo; m_vbo=NULL;
    }
    if(m_vao){
        m_vao->release();
        delete m_vao; m_vao=NULL;
    }
}

void Tub::draw(QOpenGLShaderProgram* prog){
    if(!prog){ return; }
    m_vao->bind();
    m_vbo->bind();
    prog->bind();

    // color
    prog->setUniformValue("vColor",m_color);

    // position (m_num_verts 3d points)
    prog->enableAttributeArray("vPosition");
    prog->setAttributeBuffer("vPosition",GL_FLOAT,0,3,0);

    // texture (m_num_verts 2D points, after the 3D ones)
    prog->enableAttributeArray("vTexture");
    prog->setAttributeBuffer("vTexture", GL_FLOAT,
                             m_num_verts * sizeof(vec3), 2, 0);

    
    prog->enableAttributeArray("vNormal");
    prog->setAttributeBuffer("vNormal", GL_FLOAT,
                             m_num_verts * (sizeof(vec3) + sizeof(vec2)), 3, 0);

    // int texOffset = (m_num_verts) * sizeof(vec3);

    // prog->setAttributeBuffer("vTexture", GL_FLOAT, texOffset, 2, 0);

    glDrawArrays(GL_TRIANGLE_STRIP,0, (m_num_verts));
    // meh. no release. just bind something else if you want
}

void Tub::update(QOpenGLShaderProgram* prog, float t, float ts) {

    if (t - m_t >= .01) {
        m_t = t;
        updateSurface(t, ts);
    }

    updateVertices();
    updateNormals();
    updateVBO();
}

void Tub::updateSurface(float t, float ts) {

    updateWindDir(t);

    for (int i = 0; i < m_rows * m_cols; i++) {
        m_heights_cpu[i] = 0.0;
        m_heights_cpu[i] += getWind(i, t);
    }


    int i;

    for (i = 0; i < m_ripples.length(); i++) {
        m_ripples[i].t += ts;

        if (m_ripples[i].r_max < R_MAX) {
            m_ripples[i].r_max += 0.01;
        }


        
        applyRipple(i);
        
        if (m_ripples[i].t >= T_MAX) {
            m_ripples.removeAt(i);
        }
    }
}

void Tub::applyRipple(int id) {

    float theta, z, r, age, height, amp; 
    float r_max = m_ripples[id].r_max;
    
    for (int i = 0; i < m_rows * m_cols; i++) {
        r = QLineF(m_vertices_cpu[i].x(),
                   m_vertices_cpu[i].y(),
                   m_ripples[id].x,
                   m_ripples[id].y).length();

        if (r < r_max) {

            age = ((T_MAX - m_ripples[id].t)/T_MAX);
            height = (AMP/r_max) * (r_max - qAbs(2 * r - r_max));
            //height = (AMP/r_max) * (r_max - r);
            amp = m_ripple_scale * age * height;

            theta = 2*M_PI * (r/(m_ripple_scale * LAMBDA) - m_ripples[id].t/T);

            z = amp * cos(theta);
            m_heights_cpu[i] += z;
        }


    }
}

void Tub::printVec(vec2 v) {
    cout << v.x() << ", " << v.y() << endl;
}

void Tub::updateVertices() {

    for (int i = 0; i < m_rows * m_cols; i++) {
        m_vertices_cpu[i] = vec3(m_vertices_cpu[i].x(),
                                 m_vertices_cpu[i].y(),
                                 m_heights_cpu[i]);
    }
}

void Tub::updateWindDir(float t) {
    //    cout << "- - - \n T: " << t << endl;
    float r1, r2;

    float x1 = m_angle_1.x();
    float y1 = m_angle_1.y();
    float x2 = m_angle_2.x();
    float y2 = m_angle_2.y();
    
    r1 = m_scale_1 * sin(t / 10.0);
    r2 = m_scale_2 * cos(t / 3.0);
    
    m_angle_1 = vec2(cos(r1) * x1 - sin(r1) * y1,
                     sin(r1) * x1 + cos(r1) * y1);

    m_angle_2 = vec2(cos(r2) * x2 - sin(r2) * y2,
                     sin(r2) * x2 + cos(r2) * y2);

}

float Tub::getWind(int i, float t) {

    float l_1 = 1.0;    
    float w_1 = 2*M_PI/l_1;

    float l_2 = 0.4;    
    float w_2 = 2*M_PI/l_2;

    float phi_1 = 5;
    float phi_2 = 17;

    float x_1 = QVector3D::dotProduct(m_angle_1,
                                      vec2(m_vertices_cpu[i].x(),
                                           m_vertices_cpu[i].y()));
    float x_2 = QVector3D::dotProduct(m_angle_2,
                                      vec2(m_vertices_cpu[i].x(),
                                           m_vertices_cpu[i].y()));
    float z_1, z_2;
    z_1 = 0.0;
    z_1 = m_wind * 0.003 * (sin(x_1 * w_1 + t * phi_1));

    z_2 = 0.0;
    z_2 = m_wind * 0.002 * (sin(x_2 * w_2 + t * phi_2));

    return z_1 + z_2;
}

void Tub::toggleWind(float delta) {
    m_wind += delta;
    if (m_wind < 0.0) {
        m_wind = 0.0;
    }
}

void Tub::toggleScale(float delta) {
    m_scale_1 *= delta;
    m_scale_2 *= delta;
    if (m_scale_1 < 0.0) {
        m_scale_1 = 0.0;
    }
    if (m_scale_2 < 0.0) {
        m_scale_2 = 0.0;
    }
}

void Tub::toggleRipples(float delta) {
    m_ripple_scale += delta;

    if (m_ripple_scale < 1.0) {
        m_ripple_scale = 1.0;
    }
}


void Tub::updateNormals() {

    int i = 0;
    for (int r = 0; r < m_rows; r++) {
        for (int c = 0; c < m_cols; c++) {
            m_normals_cpu[i] = getNormal(i);
            i++;
        }
    }
}

vec3 Tub::getNormal(int i) {

    vec3 left, right, top, bottom;
    int at_left, at_right, at_top, at_bottom;

    // Check boundaries!!!
    at_left = (i % m_cols == 0);
    at_right = (i % m_cols == m_cols - 1);
    at_top = i < m_cols;
    at_bottom = i >= m_cols * (m_rows - 1);

    /*
      .   ---  a   ---  b

      |    /   |    /   |

      f  ---   i   ---  c

      |   /    |    /   |
      
      e  ---   d   ---  .

    */


    vec3 normal = vec3(0, 0, 0);
    int a, b, c, d, e, f;

    int num_triangles = 0;
    
    if (!at_left) {

        f = i - 1;

        if (!at_top) {

            a = i - m_cols;

            //afi triangle
            normal += triangleNormal(a, f, i);
            num_triangles++;
        }

        if (!at_bottom) {

            d = i + m_cols;

            e = d - 1;
            
            //ife triangle
            normal += triangleNormal(i, f, e);
            num_triangles++;

            //ied triable
            normal += triangleNormal(i, e, d);
            num_triangles++;
        }
    }

    if (!at_right) {

        c = i + 1;
        
        if (!at_top) {

            a = i - m_cols;
            
            b = a + 1;

            //aib triangle
            normal += triangleNormal(a, i, b);
            num_triangles++;

            //bic triangle
            normal += triangleNormal(b, i, c);
            num_triangles++;
        }

        if (!at_bottom) {

            d = i + m_cols;

            //icd triangle
            normal += triangleNormal(i, c, d);
            num_triangles++;
        }
    }

    return normal/normal.length();
}

vec3 Tub::triangleNormal(int i1, int i2, int i3) {
    
    return QVector3D::crossProduct(m_vertices_cpu[i3] - m_vertices_cpu[i2],
                                   m_vertices_cpu[i1] - m_vertices_cpu[i2]);
}

void Tub::reset() {
    m_wind = 0;
    m_scale_1 = 0.00000007;
    m_scale_2 = -0.00000005;
    m_ripple_scale = 1.0;
}
