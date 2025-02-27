#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "scenebasic_uniform.h"
#include "objmesh.h"
#include "glslprogram.h"
#include "helper/texture.h"

using std::string;
using std::cerr;
using std::endl;

using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::mat3;

SceneBasic_Uniform::SceneBasic_Uniform() {
    mesh = ObjMesh::load("media/Tachi_Sword_MESH.obj", true);
}

void SceneBasic_Uniform::initScene()
{
    compile();
    glEnable(GL_DEPTH_TEST);
    model = glm::scale(glm::mat4(0.1f), glm::vec3(0.1f));

    // Set the camera position to be to the side and slightly angled to the front
    view = glm::lookAt(vec3(0.0f, 0.0f, 32.9f),  // Camera position
        vec3(0.0f, 0.0f, 0.0f),  // Look at the origin
        vec3(0.0f, 1.0f, 0.0f)); // Up vector

    projection = mat4(1.0f);

    prog.setUniform("Spot.L", vec3(1.0f));
    prog.setUniform("Spot.La", vec3(0.5f));
    prog.setUniform("Spot.Exponent", 1.0f);
    prog.setUniform("Spot.Cutoff", glm::radians(90.0f));

    // Set fog parameters
    prog.setUniform("fogColor", vec3(0.5f, 0.5f, 0.5f));
    prog.setUniform("FogDensity", 0.001f);

	normalTextureID = Texture::loadTexture("media/Textures/Tachi_Sword_MESH2_Tachi_Sword_SG_Normal.jpg");
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalTextureID);
	prog.setUniform("NormalText", 1);
    
    // Load Base Color Texture
    GLuint texID = Texture::loadTexture("media/Textures/Tachi_Sword_MESH2_Tachi_Sword_SG_BaseColor.jpg");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID);
    prog.setUniform("MainText", 0);
}

// Add a new member variable to keep track of the phase of the sine wave
float lightPhase = 0.0f;
const float lightSpeed = 0.0001f; // Control the speed of the light animation

void SceneBasic_Uniform::update(float t)
{
    // Update the phase of the sine wave
    lightPhase += lightSpeed;
}

void SceneBasic_Uniform::compile() {
    try {
        prog.compileShader("shader/basic_uniform.vert");
        prog.compileShader("shader/basic_uniform.frag");
        prog.link();
        prog.use();
    }
    catch (GLSLProgramException& e) {
        cerr << e.what() << endl;
        exit(EXIT_FAILURE);
    }
}

void SceneBasic_Uniform::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calculate the new spotlight position using a sine wave
    float lightY = 10.0f * std::sin(lightPhase); // Declare and calculate lightY
    vec4 lightPos = vec4(0.0f, lightY, 5.0f, 1.0f);

    GLint loc = glGetUniformLocation(prog.getHandle(), "Spot.Position"); // Declare loc
    if (loc >= 0) {
        glUniform3fv(loc, 1, glm::value_ptr(vec3(view * lightPos)));
    }

    mat3 normalMatrix = mat3(vec3(view[0]), vec3(view[1]), vec3(view[2]));
    loc = glGetUniformLocation(prog.getHandle(), "Spot.Direction");
    if (loc >= 0) {
        vec3 lightDir = glm::normalize(vec3(-lightPos) + vec3(0.0f, -0.3f, 0.0f));
        glUniform3fv(loc, 1, glm::value_ptr(normalMatrix * lightDir));
    }

    // Set material properties to white
    prog.setUniform("Material.Ka", vec3(0.5f, 0.5f, 0.5f));
    prog.setUniform("Material.Kd", vec3(0.5f, 0.5f, 0.5f));
    prog.setUniform("Material.Ks", vec3(0.5f, 0.5f, 0.5f));
    prog.setUniform("Material.Shininess", 100.0f);

    model = mat4(1.0f);
    setMatrices();
    mesh->render();
}


void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
}

void SceneBasic_Uniform::setMatrices()
{
    prog.use();
    mat4 mv = view * model;

    GLint loc = glGetUniformLocation(prog.getHandle(), "ModelViewMatrix");
    if (loc >= 0) {
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mv));
    }

    mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(mv)));
    prog.setUniform("NormalMatrix", normalMatrix);

    loc = glGetUniformLocation(prog.getHandle(), "MVP");
    if (loc >= 0) {
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(projection * mv));
    }
}
