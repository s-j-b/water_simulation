#pragma once

#include <QtOpenGL>


namespace cs40{

    typedef QVector2D vec2;
    typedef QVector3D vec3;
    typedef QVector4D vec4;

    typedef struct ripple {
        float x;
        float y;
        float t;
        float r_max;
    } Ripple;


 
class Tub {

public:
    /* construct a sphere centered at origin with given radius
         * by dividing it into vertical slices and horizontal stacks */
    Tub();

    ~Tub();

    /* draw the sphere using provided shader program  */
    void draw(QOpenGLShaderProgram* prog);
    void update(QOpenGLShaderProgram* prog, float t, float ts);
    void makeRipple();
    void toggleWind(float delta);
    void toggleScale(float delta);
    void toggleRipples(float delta);
    void reset();    
    
    /* Set color of Sphere */
    inline void setColor(const vec3& color){ m_color=color; m_color.setW(1.); }

    /* Get current color of object */
    inline vec4 getColor() const { return m_color; }

private:

    /* Generate Vertex Buffer Objects, but do not bind/allocate.
     * Return true if generate was a success, false otherwise
     */

    //////////////////////////////////////////
    // Functions /////////////////////////////
    //////////////////////////////////////////
    
    bool initVBO();

    void updateSurface(float t, float ts);
    void updateVertices();
    void updateNormals();
    void updateVBO();

    void updateWindDir(float t);
    float getWind(int i, float t);

    void applyRipple(int id);

    void copyData();    

    vec3 getNormal(int i);
    vec3 triangleNormal(int i1, int i2, int i3);

    void printVec(vec2 v);

    //////////////////////////////////////////
    // Data Members //////////////////////////
    //////////////////////////////////////////


    float m_t;
    
    int m_rows;
    int m_cols;

    int m_num_verts;

    vec3* m_vertices_cpu;
    vec3* m_normals_cpu;

    float* m_heights_cpu;
    
    vec3* m_vertices;
    vec2* m_texCoords;
    vec3* m_normals;

    vec2 m_angle_1;
    vec2 m_angle_2;

    float m_scale_1;
    float m_scale_2;

    float m_wind;
    float m_ripple_scale;

    QList<Ripple> m_ripples;

protected:
    vec4 m_color;
    QOpenGLVertexArrayObject* m_vao;
    QOpenGLBuffer* m_vbo;
};

} //namespace
