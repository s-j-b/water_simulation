#include "mypanelopengl.h"
#include "camera.h"
#include "printmatrix.h"

#include <cmath>
#include <iostream>
#include <fstream>
#include <string>

#define NUM_TEXTURES 10

#define DELTA_ANGLE 0.01
#define ROTATE_ANGLE 1.0
#define ZOOM_STEP 1.0

#define TIME_RATE 0.001

#define TRANSLATE_AMT 0.5

#define PI 3.14
#define DEGREE 360

#define pl printf("- - - - - - - - - - - - - - - - - - - - - - - -\n")
#define printhere printf("HERE: %d\n", __LINE__)

using std::cout;
using std::endl;
//using cs40::Tub;
//using cs40::Walls;
using namespace cs40;

MyPanelOpenGL::MyPanelOpenGL(QWidget *parent) :
    QOpenGLWidget(parent), m_angles(0,0.,0.) {

    m_shaderProgram=NULL;
    m_vertexShader=NULL;
    m_fragmentShader=NULL;

    m_tub = NULL;
    m_walls = NULL;

    m_timer = NULL;

    m_time = 0.0;
    m_timestep = 0.01;
    
    m_drawTub = true;
    m_polymode = 2;
    m_cull = true;

    m_rotate_angle = 9.;

    //m_color = vec4(1.5, 2.5, 5.0, 0.0);
    m_color = vec4(1.0, 0, 0, 0.0);


    m_lightPos = vec4(10.0, 5.0, 0.0, 1.0);
    //m_theta = vec3(0.0, -1.0, -2.0);
    
    m_projection_rate = 1000.;
    m_perspectiveAngle = 10;
    
    showOptions();
}

MyPanelOpenGL::~MyPanelOpenGL(){

    int i;
    for (i=0; i<NUM_TEXTURES; i++) {
  
        delete m_textures[i];
        m_textures[i] = NULL;
  
    }
  
    m_shaderProgram->release();

  
    if(m_tub){ delete m_tub; m_tub=NULL; }
    if(m_walls){ delete m_walls; m_walls=NULL; }
    if(m_square){ delete m_square; m_square=NULL; }
    destroyShaders();
}

void MyPanelOpenGL::initializeGL()
{
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_TEXTURE_2D);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    createShaders();

    parsePlanets("solarData.txt");

    m_tub = new Tub();
    m_walls = new Walls(10.);
    m_square = new Square(10.);

    m_projection.setToIdentity();
    m_transform.setToIdentity();
    m_projection.perspective(m_perspectiveAngle, 1, 10, -20);

    m_camera = new Camera();
    m_camera->lookAt(vec3(0,0,10),
                     vec3(0,0,0),
                     vec3(0,1,0));

    m_shaderProgram->bind();

    m_timer = new QTimer(this);

    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateTime()));
    m_timer->start(30);  // trigger every 30ms 
                             }

void MyPanelOpenGL::resizeGL(int w, int h)
{
    glViewport(0,0,w, h);
}

void MyPanelOpenGL::paintGL(){

    /* clear both color and depth buffer */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
    
    updatePolyMode(m_polymode);
    if(m_cull){ glEnable(GL_CULL_FACE); }
    else{glDisable(GL_CULL_FACE); }



    if(!m_shaderProgram){return;}

    mat4 mview = m_camera->matrix() * m_model;
    
    m_shaderProgram->bind(); 
    m_shaderProgram->setUniformValue("lightPos", m_lightPos);
    //m_shaderProgram->setUniformValue("Tex0",0);
    m_shaderProgram->setUniformValue("model", m_model);
    m_shaderProgram->setUniformValue("projection", m_projection);
    m_shaderProgram->setUniformValue("transform", m_transform);
    m_shaderProgram->setUniformValue("camera", m_camera->matrix());
    m_shaderProgram->setUniformValue("modelView", mview);
    m_shaderProgram->setUniformValue("normalMatrix", mview.normalMatrix());

    //printhere;
    if(m_drawTub){
        m_modelStack.push();

        if(!m_shaderProgram){return;}
        m_shaderProgram->bind();
        
        m_modelStack.push();
        m_textures[0]->bind();

        float scale = 50./600.;

        m_tub->update(m_shaderProgram,
                      m_time, m_timestep);
        
        m_modelStack.scale(scale);
            
        m_shaderProgram->setUniformValue("model", m_modelStack.top());

        m_square->draw(m_shaderProgram);
        m_walls->draw(m_shaderProgram);
        m_tub->draw(m_shaderProgram);

        m_modelStack.pop();
    }
    else{
        m_walls->draw(m_shaderProgram);
    }
    glFlush();
}

void MyPanelOpenGL::showOptions(){

    printf("Application controls: \n");

    printf("h: Speed Up Time\n");
    printf("g: Slow Down Time\n\n");

    printf("r: Make random ripple\n");
    printf("b: Increase ripple size\n");
    printf("v: Decrease ripple size\n\n");

    printf("y: Increase wind\n");
    printf("t: Decrease wind\n");
    printf("m: Increase wind rotation\n");
    printf("n: Decrease wind rotation\n\n");

    printf("d: Roll Right\n");
    printf("a: Roll Left\n");
    printf("e: Yaw Right\n");
    printf("q: Yaw Left\n");
    printf("w: Pitch Down\n");
    printf("s: Pitch Up\n\n");
                    
    printf("i: Zoom in (small)\n");
    printf("o: Zoom out (small)\n");
    printf("k: Zoom in (big)\n");
    printf("l: Zoom out (big)\n\n");
                    
    printf("p: Toggle surfaces, edges, points\n\n");

    printf("x: RESET\n\n");

}

void MyPanelOpenGL::keyPressEvent(QKeyEvent *event)
{
    /*Enable strong Focus on GL Widget to process key events*/
    switch(event->key()){

        //----------------------------
        // Speed
        //----------------------------
    case Qt::Key_H:
        if (event->text()=="h"){
            m_timestep += TIME_RATE;
        }
        break;

    case Qt::Key_G:
        if (event->text()=="g"){
            m_timestep -= TIME_RATE;
            if (m_timestep < 0.0) {
                m_timestep = 0.0;
            }
        }
        break;

    case Qt::Key_R:
        if (event->text()=="r"){
            m_tub->makeRipple();
        }
        break;        

        //
        // Wind
        //  
    case Qt::Key_Y:
        if (event->text()=="y"){
            m_tub->toggleWind(1.0);
        }
        break;        

    case Qt::Key_T:
        if (event->text()=="t"){
            m_tub->toggleWind(-1.0);
        }
        break;        

        //---------------------
        // Scale
        //-------------------
    case Qt::Key_M:
        if (event->text()=="m"){
            m_tub->toggleScale(10.0);
        }
        break;        

    case Qt::Key_N:
        if (event->text()=="n"){
            m_tub->toggleScale(0.1);
        }
        break;        

        //---------------------
        // Ripples
        //-------------------
    case Qt::Key_B:
        if (event->text()=="b"){
            m_tub->toggleRipples(1.0);
        }
        break;        

    case Qt::Key_V:
        if (event->text()=="v"){
            m_tub->toggleRipples(-1.0);
        }
        break;        

        //----------------------------
        // Rotate
        //----------------------------

        
    case Qt::Key_D:
        if (event->text()=="d"){
            m_transform.rotate(m_rotate_angle, 0.0, .10, 0.0);
        }            
        break;

    case Qt::Key_A:

        if (event->text()=="a"){
            m_transform.rotate(m_rotate_angle, 0.0, -.10, 0.0);
        }
        break;


    case Qt::Key_W:
        if (event->text()=="w"){
            m_transform.rotate(m_rotate_angle, -.10, 0.0, 0.0);
        }
        break;

    case Qt::Key_S:
        if (event->text()=="s") {
            m_transform.rotate(m_rotate_angle, .10, 0.0, 0.0);
        }
        break;

    case Qt::Key_E:
        if (event->text()=="e"){
            m_transform.rotate(m_rotate_angle, 0.0, 0.0, -1.0);
        }
        break;

    case Qt::Key_Q:
        if (event->text()=="q"){
            m_transform.rotate(m_rotate_angle, 0.0, 0.0, 1.0);
        }
        break;

        //----------------------------
        // Translate
        //----------------------------


    case Qt::Key_O:
        if (event->text()=="o"){
            m_camera->back(TRANSLATE_AMT);
        }
        break;

    case Qt::Key_I:
        if (event->text()=="i"){
            m_camera->forward(TRANSLATE_AMT);
        }
        break;

        // Change perspective angle
    case Qt::Key_L:
        if (event->text()=="l"){
            m_perspectiveAngle += 2.0;
            m_projection.setToIdentity();
            m_projection.perspective(m_perspectiveAngle, 1, 10, -20);
        }
        break;

    case Qt::Key_K:
        if (event->text()=="k"){
            m_perspectiveAngle -= 2.0;
            m_projection.setToIdentity();
            m_projection.perspective(m_perspectiveAngle, 1, 10, -20);
        }
        break;

        // Reset

    case Qt::Key_X:
        if (event->text()=="x"){
            m_perspectiveAngle = 10.0;
            m_projection.setToIdentity();
            m_projection.perspective(m_perspectiveAngle, 1, 10, -20);

            m_camera->lookAt(vec3(0,0,10),
                             vec3(0,0,0),
                             vec3(0,1,0));

            m_tub->reset();
        }
        break;        

    case Qt::Key_P:
        m_polymode = (m_polymode+1)%3;
        break;
    // case Qt::Key_S:
    //     m_drawTub = !m_drawTub;
    //     break;
    default:
        QWidget::keyPressEvent(event); /* pass to base class */
    }
    update();
}

void MyPanelOpenGL::parsePlanets(const QString &fname){
    //m_textures[0] = new QOpenGLTexture(QImage("data/neptune.png").mirrored());
}

void MyPanelOpenGL::updateTime(){
    /* trigger a refresh */
    m_time += m_timestep;
     update();
}

void MyPanelOpenGL::updatePolyMode(int val){
    GLenum mode=GL_NONE;
    if(val==0){mode=GL_POINT;}
    else if(val==1){mode=GL_LINE;}
    else if(val==2){mode=GL_FILL;}

    if(mode != GL_NONE){
        glPolygonMode(GL_FRONT_AND_BACK, mode);
    }
}

void MyPanelOpenGL::createShaders(){

    destroyShaders(); //get rid of any old shaders

    m_vertexShader = new QOpenGLShader(QOpenGLShader::Vertex);
    if (!m_vertexShader->compileSourceFile("shaders/vshader.glsl")){
        qWarning() << m_vertexShader->log();
    }

    m_fragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
    if(!m_fragmentShader->compileSourceFile("shaders/fshader.glsl")){
        qWarning() << m_fragmentShader->log();
    }

    m_shaderProgram = new QOpenGLShaderProgram();
    m_shaderProgram->addShader(m_vertexShader);
    m_shaderProgram->addShader(m_fragmentShader);

    if(!m_shaderProgram->link()){
        qWarning() << m_shaderProgram->log() << endl;
    }
}

void MyPanelOpenGL::destroyShaders(){

    delete m_vertexShader; m_vertexShader=NULL;
    delete m_fragmentShader; m_fragmentShader=NULL;

    if(m_shaderProgram){
        m_shaderProgram->release();
        delete m_shaderProgram; m_shaderProgram=NULL;
     }
}

void MyPanelOpenGL::updateAngles(int idx, qreal amt){
    if(idx == 0){
        m_angles.setX(m_angles.x()+amt);
    }else if(idx == 1){
        m_angles.setY(m_angles.y()+amt);
    }else if(idx == 2){
        m_angles.setZ(m_angles.z()+amt);
    }
}

qreal MyPanelOpenGL::wrap(qreal amt){
    if (amt > 360.){ return amt - 360.; }
    else if (amt < 0.){ return amt + 360.; }
    return amt;
}
