#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "scene.h"
#include "glslprogram.h"
#include "objmesh.h"
#include <glm/glm.hpp>

class SceneBasic_Uniform : public Scene
{
private:
    GLSLProgram prog;
    std::unique_ptr<ObjMesh> mesh;
    GLuint normalTextureID;
    float lightPhase;
    float lightSpeed;
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
    int width, height;

    void compile();
    void setMatrices();
    void handleInput();

public:
    SceneBasic_Uniform();
    void initScene();
    void update(float t);
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H
