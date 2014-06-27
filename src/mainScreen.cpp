#include <SFML/OpenGL.hpp>
#include "mainScreen.h"
#include "shipSelectionScreen.h"

MainScreenUI::MainScreenUI()
{
    P<MouseRenderer> mouse_renderer = engine->getObject("mouseRenderer");
    mouse_renderer->visible = false;
}

void MainScreenUI::onGui()
{
    if (my_spaceship)
        render3dView(*getRenderTarget());
    else
        drawStatic();

    MainUI::onGui();
}

void MainScreenUI::destroy()
{
    P<MouseRenderer> mouse_renderer = engine->getObject("mouseRenderer");
    mouse_renderer->visible = true;
    MainUI::destroy();
}

void MainScreenUI::render3dView(sf::RenderTarget& window)
{
    window.pushGLStates();

    glClearDepth(1.f);
    glClear(GL_DEPTH_BUFFER_BIT);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.f, 1600.0/900.0, 1.f, 16000.f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(90, 1, 0, 0);
    glScalef(1,1,-1);
    glRotatef(-15, 1, 0, 0);
#ifdef DEBUG
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
        glRotatef(-60, 1, 0, 0);
#endif
    glRotatef(-my_spaceship->getRotation(), 0, 0, 1);
    //What is being drawn here? Skybox?
    sf::Texture::bind(texture_manager.getTexture("Stars"), sf::Texture::Pixels);
    glDepthMask(false);
    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(1024,    0); glVertex3f( 100, 100, 100);
    glTexCoord2f(   0,    0); glVertex3f( 100, 100,-100);
    glTexCoord2f(1024, 1024); glVertex3f(-100, 100, 100);
    glTexCoord2f(   0, 1024); glVertex3f(-100, 100,-100);
    glEnd();
    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(1024,    0); glVertex3f(-100, 100, 100);
    glTexCoord2f(   0,    0); glVertex3f(-100, 100,-100);
    glTexCoord2f(1024, 1024); glVertex3f(-100,-100, 100);
    glTexCoord2f(   0, 1024); glVertex3f(-100,-100,-100);
    glEnd();
    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(1024,    0); glVertex3f(-100,-100, 100);
    glTexCoord2f(   0,    0); glVertex3f(-100,-100,-100);
    glTexCoord2f(1024, 1024); glVertex3f( 100,-100, 100);
    glTexCoord2f(   0, 1024); glVertex3f( 100,-100,-100);
    glEnd();
    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(1024,    0); glVertex3f( 100,-100, 100);
    glTexCoord2f(   0,    0); glVertex3f( 100,-100,-100);
    glTexCoord2f(1024, 1024); glVertex3f( 100, 100, 100);
    glTexCoord2f(   0, 1024); glVertex3f( 100, 100,-100);
    glEnd();
    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(1024,    0); glVertex3f( 100,-100, 100);
    glTexCoord2f(   0,    0); glVertex3f(-100,-100, 100);
    glTexCoord2f(1024, 1024); glVertex3f( 100, 100, 100);
    glTexCoord2f(   0, 1024); glVertex3f(-100, 100, 100);
    glEnd();
    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(1024,    0); glVertex3f( 100,-100,-100);
    glTexCoord2f(   0,    0); glVertex3f(-100,-100,-100);
    glTexCoord2f(1024, 1024); glVertex3f( 100, 100,-100);
    glTexCoord2f(   0, 1024); glVertex3f(-100, 100,-100);
    glEnd();

    for(unsigned int n = 0; n < nebulaInfo.size(); n++)
    {
        sf::Texture::bind(texture_manager.getTexture(nebulaInfo[n].textureName), sf::Texture::Pixels);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glPushMatrix();
        glRotatef(180, nebulaInfo[n].vector.x, nebulaInfo[n].vector.y, nebulaInfo[n].vector.z);
        glColor4f(1,1,1,0.1);
        glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(1024,    0); glVertex3f( 100, 100, 100);
        glTexCoord2f(   0,    0); glVertex3f( 100, 100,-100);
        glTexCoord2f(1024, 1024); glVertex3f(-100, 100, 100);
        glTexCoord2f(   0, 1024); glVertex3f(-100, 100,-100);
        glEnd();
        glPopMatrix();
    }
    glDisable(GL_BLEND);
    sf::Texture::bind(NULL);
    glDepthMask(true);
    glEnable(GL_DEPTH_TEST);

    sf::Vector2f camera_position_2D = my_spaceship->getPosition() + sf::vector2FromAngle(my_spaceship->getRotation()) * -200.0f;
    sf::Vector3f target_camera_position(camera_position_2D.x, camera_position_2D.y, 100);
#ifdef DEBUG
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
        target_camera_position.z = 3000.0;
#endif
    camera_position = camera_position * 0.9f + target_camera_position * 0.1f;

    {
        float lightpos[4] = {0, 0, 1000, 1.0};
        glPushMatrix();
        glTranslatef(-camera_position.x,-camera_position.y, -camera_position.z);
        glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
        glPopMatrix();
    }

    foreach(SpaceObject, obj, space_object_list)
    {
        glPushMatrix();
        glTranslatef(-camera_position.x,-camera_position.y, -camera_position.z);
        glTranslatef(obj->getPosition().x, obj->getPosition().y, 0);
        glRotatef(obj->getRotation(), 0, 0, 1);

        obj->draw3D();
        glPopMatrix();
    }
    sf::Shader::bind(NULL);

#ifdef DEBUG
    glDisable(GL_DEPTH_TEST);
    foreach(SpaceObject, obj, space_object_list)
    {
        glPushMatrix();
        glTranslatef(-camera_position.x,-camera_position.y, -camera_position.z);
        glTranslatef(obj->getPosition().x, obj->getPosition().y, 0);
        glRotatef(obj->getRotation(), 0, 0, 1);

        std::vector<sf::Vector2f> collisionShape = obj->getCollisionShape();
        glBegin(GL_LINE_LOOP);
        for(unsigned int n=0; n<collisionShape.size(); n++)
            glVertex3f(collisionShape[n].x, collisionShape[n].y, 0);
        glEnd();
        glPopMatrix();
    }
#endif

    window.popGLStates();
}

void MainScreenUI::renderMap(sf::RenderTarget& window)
{
}

void MainScreenUI::renderRadar(sf::RenderTarget& window)
{
}
