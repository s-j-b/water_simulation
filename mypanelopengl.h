#pragma once

#include <QtWidgets>
#include <QtOpenGL>
#include <QMatrix4x4>
#include "planet.h"
#include "sphere.h"
#include "tub.h"
#include "square.h"
#include "walls.h"

#include "camera.h"
#include "matrixstack.h"

class MyPanelOpenGL : public QOpenGLWidget
{
    Q_OBJECT

    typedef QVector4D point4;
    typedef QVector4D color4;
    typedef QVector3D vec3;
    typedef QMatrix4x4 mat4;

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void keyPressEvent(QKeyEvent* event);

public:
    explicit MyPanelOpenGL(QWidget *parent = 0);
    virtual ~MyPanelOpenGL();

private:

    /* simple test shapes */
    cs40::Tub* m_tub;
    cs40::Walls* m_walls;
    cs40::Square* m_square;

    float m_time;
    float m_timestep;

    QList<cs40::Planet*> m_planets;
    
    /* Polygon draw mode
     * 0 : point
     * 1 : line
     * 2 : polygon */
    int m_polymode;
    bool m_cull; /* is Culling enabled? */
    bool m_drawTub; /* Draw complex tuby scene (true) or simpler test scene (false) */

    float m_projection_rate;
    float m_zoom;
    
    /* texture data */
    QOpenGLTexture* m_textures[10];

    QVector4D m_color;

    QVector4D m_lightPos;
    vec3 m_theta;

    QMatrix4x4 m_transform; /* Takes objects coordinates into world coordinates */
    QMatrix4x4 m_model; /* Takes objects coordinates into world coordinates */
    QMatrix4x4 m_projection; /* Takes world coordinates into clip coordinates */
    
    float m_perspectiveAngle;
                          
    cs40::Camera* m_camera;

    cs40::MatrixStack m_modelStack; /* history of past model matrices */

    /* set this up when you are ready to animate */
    QTimer* m_timer;      /* event timer */

    vec3 m_angles; /* Euler angles for rotation */

    /* Shaders and program */
    QOpenGLShader *m_vertexShader;
    QOpenGLShader *m_fragmentShader;
    QOpenGLShaderProgram *m_shaderProgram;

    /* parse planet data make data structure for solar system*/
    void parsePlanets(const QString& fname);

    /* Print message showing keyboard controls */
    void showOptions();

    /* update Polygon draw mode based
     * 0 : point
     * 1 : line
     * 2 : polygon */
    void updatePolyMode(int val);

    void createShaders();
    void destroyShaders();

    float m_rotate_angle;

    /* update Euler angle at index idx by amt
     * idx=0,1,2 -> x,y,z */
    void updateAngles(int idx, qreal amt);

    /* wrap a angle value to the range 0..360*/
    qreal wrap(qreal amt);


signals:

public slots:
    /* trigger update of GL window */
    void updateTime(); 
};
