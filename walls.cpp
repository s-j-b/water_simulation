#include "walls.h"
#include <iostream>

using namespace cs40;
using std::cout;
using std::endl;

Walls::Walls(float width):
    m_color(1,0,0,1), m_vao(NULL), m_vbo(NULL), m_width(width)
{
    vec3* vertices = new vec3[16];
    vec2* texCoords = new vec2[16];
    vec3* normals = new vec3[16];

    float d = 1.0;
    vertices[0]=vec3(-d, d,.2); texCoords[0]=vec2(0,0);
    vertices[1]=vec3(-d, d,-1); texCoords[1]=vec2(0,1);
    vertices[2]=vec3( d, d,.2); texCoords[2]=vec2(1,0);
    vertices[3]=vec3( d, d,-1); texCoords[3]=vec2(1,1);
    vertices[4]=vec3( d, d,.2); texCoords[4]=vec2(0,0);
    vertices[5]=vec3( d, d,-1); texCoords[5]=vec2(0,1);
    vertices[6]=vec3( d,-d,.2); texCoords[6]=vec2(1,0);
    vertices[7]=vec3( d,-d,-1); texCoords[7]=vec2(1,1);
    vertices[8]=vec3( d,-d,.2); texCoords[8]=vec2(0,0);
    vertices[9]=vec3( d,-d,-1); texCoords[9]=vec2(0,1);
    vertices[10]=vec3(-d,-d,.2); texCoords[10]=vec2(1,0);
    vertices[11]=vec3(-d,-d,-1); texCoords[11]=vec2(1,1);
    vertices[12]=vec3(-d,-d,.2); texCoords[12]=vec2(0,0);
    vertices[13]=vec3(-d,-d,-1); texCoords[13]=vec2(0,1);
    vertices[14]=vec3(-d, d,.2); texCoords[14]=vec2(1,0);
    vertices[15]=vec3(-d, d,-1); texCoords[15]=vec2(1,1);

    normals[0] = QVector3D::crossProduct(vertices[2] - vertices[1], vertices[0] - vertices[1]);
    normals[1] = QVector3D::crossProduct(vertices[2] - vertices[1], vertices[0] - vertices[1]);
    normals[2] = QVector3D::crossProduct(vertices[2] - vertices[1], vertices[0] - vertices[1]);
    normals[3] = QVector3D::crossProduct(vertices[2] - vertices[1], vertices[0] - vertices[1]);

    normals[4] = QVector3D::crossProduct(vertices[6] - vertices[5], vertices[4] - vertices[5]);
    normals[5] = QVector3D::crossProduct(vertices[6] - vertices[5], vertices[4] - vertices[5]);
    normals[6] = QVector3D::crossProduct(vertices[6] - vertices[5], vertices[4] - vertices[5]);
    normals[7] = QVector3D::crossProduct(vertices[6] - vertices[5], vertices[4] - vertices[5]);

    normals[8] = QVector3D::crossProduct(vertices[10] - vertices[9], vertices[8] - vertices[9]);
    normals[9] = QVector3D::crossProduct(vertices[10] - vertices[9], vertices[8] - vertices[9]);
    normals[10] = QVector3D::crossProduct(vertices[10] - vertices[9], vertices[8] - vertices[9]);
    normals[11] = QVector3D::crossProduct(vertices[10] - vertices[9], vertices[8] - vertices[9]);

    normals[12] = QVector3D::crossProduct(vertices[14] - vertices[13], vertices[12] - vertices[13]);
    normals[13] = QVector3D::crossProduct(vertices[14] - vertices[13], vertices[12] - vertices[13]);
    normals[14] = QVector3D::crossProduct(vertices[14] - vertices[13], vertices[12] - vertices[13]);
    normals[15] = QVector3D::crossProduct(vertices[14] - vertices[13], vertices[12] - vertices[13]);
    
    
    if(initVBO()){
        m_vbo->bind();
        m_vbo->allocate(16*sizeof(vec3)+
                        16*sizeof(vec2)+
                        16*sizeof(vec3));
        m_vbo->write(0,vertices,16*sizeof(vec3));
        m_vbo->write(16*sizeof(vec3),texCoords,16*sizeof(vec2));
        m_vbo->write(16*sizeof(vec3) + 16*sizeof(vec2),normals,16*sizeof(vec3));
        m_vbo->release();
    }

    delete [] vertices; vertices=NULL;
    delete [] texCoords; texCoords=NULL;
    delete [] normals; normals=NULL;
}

bool Walls::initVBO(){
    m_vao = new QOpenGLVertexArrayObject();
    bool ok = m_vao->create();
    if( !ok ){ return false; }
    m_vbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    ok = m_vbo->create();
    if( !ok ){ return false; }
    m_vbo->setUsagePattern(QOpenGLBuffer::StaticDraw);
    return ok;
}

Walls::~Walls(){
    if(m_vbo){
        m_vbo->release();
        delete m_vbo; m_vbo=NULL;
    }
    if(m_vao){
        m_vao->release();
        delete m_vao; m_vao=NULL;
    }
}

void Walls::draw(QOpenGLShaderProgram* prog){
    if(!prog){ return; }

    m_vao->bind();
    m_vbo->bind();
    prog->bind();
    prog->setUniformValue("vColor",m_color);

    prog->enableAttributeArray("vPosition");
    prog->setAttributeBuffer("vPosition",GL_FLOAT,0,3,0);

    prog->enableAttributeArray("vTexture");
    prog->setAttributeBuffer("vTexture",GL_FLOAT,16*sizeof(vec3),2,0);

    prog->enableAttributeArray("vNormal");
    prog->setAttributeBuffer("vNormal",GL_FLOAT,16*sizeof(vec3)+16*sizeof(vec2),3,0);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 16);
}
