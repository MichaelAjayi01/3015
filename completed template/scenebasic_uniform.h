#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include <glad.h>
#include "helper/glslprogram.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "helper/torus.h"
#include "helper/objmesh.h"

class SceneBasic_Uniform : public Scene
{
private:
    std::unique_ptr<ObjMesh> mesh;
    GLSLProgram prog;
    float angle;
    GLuint textureID;
    GLuint metallicTextureID;
	GLuint heightTextureID;
	GLuint normalTextureID;
	GLuint roughnessTextureID;
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
