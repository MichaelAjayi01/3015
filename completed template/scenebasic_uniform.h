#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include <glad.h>
#include "helper/glslprogram.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "helper/torus.h"
#include "helper/objmesh.h"
#include "helper/skybox.h"
#include "helper/cube.h"

class SceneBasic_Uniform : public Scene
{

private:
    float lightPhase = 0.0f;
    float lightSpeed;

    SkyBox skybox;
    GLSLProgram skyBoxProg;
    GLuint skyboxTex;


    std::unique_ptr<ObjMesh> mesh;
    GLSLProgram prog;
    GLuint cubeTex;
    float angle;
    GLuint MainTextureID;
    GLuint normalTextureID;
    void setMatrices();
    void compile();

public:
    SceneBasic_Uniform();

    void initScene();
    void update(float t);
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H