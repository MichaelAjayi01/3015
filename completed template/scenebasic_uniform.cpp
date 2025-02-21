#include "scenebasic_uniform.h"

#include <cstdio>
#include <cstdlib>
#include <objbase.h>

#include <string>
using std::string;

#include <iostream>
using std::cerr;
using std::endl;

#include "helper/glutils.h"

using glm::vec3;
using glm::mat4;

SceneBasic_Uniform::SceneBasic_Uniform() {
    mesh = ObjMesh::load("media/pig_triangulated.obj", true);
}

void SceneBasic_Uniform::initScene()
{
    compile();
    glEnable(GL_DEPTH_TEST);
    model = glm::mat4(1.0f);

    // Set the camera position to be to the side and slightly angled to the front
    view = glm::lookAt(vec3(0.0f, 0.0f, 2.0f), // Camera position (to the side and slightly angled)
                       vec3(0.0f, 0.0f, 0.0f), // Look at the origin
                       vec3(0.0f, 1.0f, 0.0f)); // Up vector

    // Apply rotations to the model matrix
    model = glm::rotate(model, glm::radians(-35.0f), vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(15.0f), vec3(0.0f, 1.0f, 0.0f));
    projection = mat4(1.0f);

    // Position the light above the torus
    prog.setUniform("Light.position", view * glm::vec4(0.0f, 5.0f, 0.0f, 1.0f));
    // Ambient light intensity
    prog.setUniform("Light.La", vec3(0.1f, 0.1f, 0.1f));
    // Diffuse light intensity
    prog.setUniform("Light.Ld", vec3(1.0f, 1.0f, 1.0f));
    // Specular light intensity
    prog.setUniform("Light.Ls", vec3(1.0f, 1.0f, 1.0f));
    // Spotlight direction pointing down
    prog.setUniform("Light.direction", vec3(0.0f, -1.0f, 0.0f));
    // Spotlight cutoff angle
    prog.setUniform("Light.cutoff", glm::cos(glm::radians(15.0f)));

    // Set material properties to white
    prog.setUniform("Material.Ka", vec3(1.0f, 1.0f, 1.0f));
    prog.setUniform("Material.Kd", vec3(1.0f, 1.0f, 1.0f));
    prog.setUniform("Material.Ks", vec3(1.0f, 1.0f, 1.0f));
    prog.setUniform("Material.Shininess", 100.0f);

    // Set fog parameters
    prog.setUniform("fogColor", vec3(0.5f, 0.5f, 0.5f)); // Gray fog
    prog.setUniform("fogDensity", 0.0f);
}


void SceneBasic_Uniform::update(float t)
{
    // Update logic here
}

void SceneBasic_Uniform::compile() {
    try {
        prog.compileShader("shader/basic_uniform.vert");
        prog.compileShader("shader/basic_uniform.frag");
        prog.link();
        prog.use();
    }
    catch (GLSLProgramException& e) {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}

void SceneBasic_Uniform::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    setMatrices();

    // Render the mesh
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
