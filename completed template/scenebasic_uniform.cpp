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
#include <glfw3.h>

using std::string;
using std::cerr;
using std::endl;

using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::mat3;

SceneBasic_Uniform::SceneBasic_Uniform() {
    mesh = ObjMesh::load("media/Tachi_Sword_MESH.obj", true);
    lightPhase = 0.0f;
    lightSpeed = 0.0001f;
    model = mat4(1.0f);
    rotationX = 0.0f;
    rotationY = 0.0f;
    isMousePressed = false;
}

void SceneBasic_Uniform::initScene()
{
    compile();
    glEnable(GL_DEPTH_TEST);

    view = glm::lookAt(vec3(0.0f, 0.0f, 32.9f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    projection = mat4(1.0f);

    prog.setUniform("Spot.L", vec3(2.5f));
    prog.setUniform("Spot.La", vec3(0.5f));
    prog.setUniform("Spot.Exponent", 1.0f);
    prog.setUniform("Spot.Cutoff", glm::radians(90.0f));

    prog.setUniform("fogColor", vec3(0.5f, 0.5f, 0.5f));
    prog.setUniform("FogDensity", 0.001f);

    normalTextureID = Texture::loadTexture("media/Textures/Tachi_Sword_MESH2_Tachi_Sword_SG_Normal.jpg");
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalTextureID);
    prog.setUniform("NormalText", 1);

    GLuint texID = Texture::loadTexture("media/Textures/Tachi_Sword_MESH2_Tachi_Sword_SG_BaseColor.jpg");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID);
    prog.setUniform("MainText", 0);
}

void SceneBasic_Uniform::update(float t)
{
    lightPhase += lightSpeed;
    handleInput();
    handleMouseInput();
}

void SceneBasic_Uniform::handleInput()
{
    // Keyboard input for rotation
    const float rotationSpeed = 0.01f; 

    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_W) == GLFW_PRESS) {
        model = glm::rotate(mat4(1.0f), glm::radians(-rotationSpeed), vec3(1.0f, 0.0f, 0.0f)) * model;
    }
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_S) == GLFW_PRESS) {
        model = glm::rotate(mat4(1.0f), glm::radians(rotationSpeed), vec3(1.0f, 0.0f, 0.0f)) * model;
    }
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_A) == GLFW_PRESS) {
        model = glm::rotate(mat4(1.0f), glm::radians(-rotationSpeed), vec3(0.0f, 1.0f, 0.0f)) * model;
    }
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_D) == GLFW_PRESS) {
        model = glm::rotate(mat4(1.0f), glm::radians(rotationSpeed), vec3(0.0f, 1.0f, 0.0f)) * model;
    }
}

void SceneBasic_Uniform::handleMouseInput()
{
    double mouseX, mouseY;
    glfwGetCursorPos(glfwGetCurrentContext(), &mouseX, &mouseY);

    if (glfwGetMouseButton(glfwGetCurrentContext(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (!isMousePressed) {
            isMousePressed = true;
            prevMouseX = mouseX;
            prevMouseY = mouseY;
        }
        else {
            float deltaX = static_cast<float>(mouseX - prevMouseX);
            float deltaY = static_cast<float>(mouseY - prevMouseY);

            const float mouseSensitivity = 0.0001f; 
            rotationY += deltaX * mouseSensitivity;
            rotationX += deltaY * mouseSensitivity;

            model = glm::rotate(mat4(1.0f), glm::radians(rotationX), vec3(1.0f, 0.0f, 0.0f)) *
                glm::rotate(mat4(1.0f), glm::radians(rotationY), vec3(0.0f, 1.0f, 0.0f)) *
                model;

            prevMouseX = mouseX;
            prevMouseY = mouseY;
        }
    }
    else {
        isMousePressed = false;
    }
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

    prog.use();

    float lightY = 10.0f * std::sin(lightPhase);
    vec4 lightPos = vec4(0.0f, lightY, 5.0f, 1.0f);
    GLint loc = glGetUniformLocation(prog.getHandle(), "Spot.Position");
    if (loc >= 0) {
        glUniform3fv(loc, 1, glm::value_ptr(vec3(view * lightPos)));
    }

    loc = glGetUniformLocation(prog.getHandle(), "Spot.Direction");
    if (loc >= 0) {
        vec3 lightDir = glm::normalize(vec3(-lightPos) + vec3(0.0f, -0.3f, 0.0f));
        glUniform3fv(loc, 1, glm::value_ptr(lightDir));
    }

    float timeValue = (float)glfwGetTime();
    loc = glGetUniformLocation(prog.getHandle(), "Time");
    if (loc >= 0) {
        glUniform1f(loc, timeValue);
    }

    // Set material properties
    prog.setUniform("Material.Ka", vec3(0.5f, 0.5f, 0.5f));
    prog.setUniform("Material.Kd", vec3(0.5f, 0.5f, 0.5f));
    prog.setUniform("Material.Ks", vec3(0.5f, 0.5f, 0.5f));
    prog.setUniform("Material.Shininess", 100.0f);

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
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    prog.setUniform("MVP", projection * mv);
}
