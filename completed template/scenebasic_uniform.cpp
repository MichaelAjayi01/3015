#include "scenebasic_uniform.h"

#include <cstdio>
#include <cstdlib>
#include <objbase.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <string>
using std::string;

#include <iostream>
using std::cerr;
using std::endl;

#include "helper/glutils.h"
#include <glm/gtx/string_cast.hpp>

using glm::vec3;
using glm::mat4;

GLuint textureID;

SceneBasic_Uniform::SceneBasic_Uniform() : angle(0.0f) {
    mesh = ObjMesh::load("media/sword.obj", true);
}

void SceneBasic_Uniform::initScene()
{
    compile();
    glEnable(GL_DEPTH_TEST);
    model = glm::mat4(1.0f);

    // Load the texture
    int width, height, nrChannels;
    std::cout << "Attempting to load texture..." << std::endl;
    unsigned char* data = stbi_load("media/Textures/BaseColor.png", &width, &height, &nrChannels, 0);
    std::cout << "Texture function executed!" << std::endl;
    if (data)
    {
        std::cout << "Texture loaded: " << width << "x" << height << " Channels: " << nrChannels << std::endl;
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Set the texture wrapping/filtering options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cerr << "Failed to load texture" << std::endl;
        stbi_image_free(data);
    }

    // Check for OpenGL errors
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        std::cerr << "OpenGL error: " << err << std::endl;
    }

    // Set the camera position to be further back to zoom out
    view = glm::lookAt(vec3(0.0f, 0.0f, 15.0f), // Camera position (further back)
        vec3(0.0f, 0.0f, 0.0f), // Look at the origin
        vec3(0.0f, 1.0f, 0.0f)); // Up vector

    // Apply rotations to the model matrix to make the sword stand vertically
    model = glm::rotate(model, glm::radians(76.0f), vec3(1.0f, 0.0f, 0.0f)); // Rotate around the x-axis to make it stand vertically
    projection = mat4(1.0f);

    // Position the light above the torus
    prog.setUniform("Light.position", view * glm::vec4(0.0f, 5.0f, 0.0f, 1.0f));
    // Ambient light intensity 
    prog.setUniform("Light.La", vec3(0.4f, 0.4f, 0.4f));
    // Diffuse light intensity 
    prog.setUniform("Light.Ld", vec3(0.5f, 0.5f, 1.0f));
    // Specular light intensity
    prog.setUniform("Light.Ls", vec3(0.5f, 0.5f, 0.5f));
    // Spotlight direction pointing down
    prog.setUniform("Light.direction", vec3(0.0f, -1.0f, 0.0f));
    // Spotlight cutoff angle
    prog.setUniform("Light.cutoff", glm::cos(glm::radians(15.0f)));

    // Set material properties to white
    prog.setUniform("Material.Ka", vec3(1.0f, 1.0f, 1.0f));
    prog.setUniform("Material.Kd", vec3(1.0f, 1.0f, 1.0f)); // Set diffuse reflectivity to white
    prog.setUniform("Material.Ks", vec3(1.0f, 1.0f, 1.0f));
    prog.setUniform("Material.Shininess", 100.0f);

    // Print material properties for debugging
    std::cout << "Material.Ka: " << glm::to_string(vec3(1.0f, 1.0f, 1.0f)) << std::endl;
    std::cout << "Material.Kd: " << glm::to_string(vec3(1.0f, 1.0f, 1.0f)) << std::endl;
    std::cout << "Material.Ks: " << glm::to_string(vec3(1.0f, 1.0f, 1.0f)) << std::endl;
    std::cout << "Material.Shininess: " << 100.0f << std::endl;

    // Set fog parameters
    prog.setUniform("fogColor", vec3(0.5f, 0.5f, 0.5f)); // Gray fog
    prog.setUniform("fogDensity", 0.0f);
}





void SceneBasic_Uniform::update(float t)
{
    // Increment the rotation angle
    angle += 0.0001f; // Decrease the increment value for very slow-motion rotation

    // Apply the rotation to the model matrix
    model = glm::mat4(1.0f); // Reset the model matrix
    model = glm::rotate(model, angle, vec3(0.0f, 1.0f, 0.0f)); // Rotate around the y-axis
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

    // Update the matrices
    setMatrices();

    // Bind the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    prog.setUniform("Texture", 0);

    // Check for OpenGL errors
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        std::cerr << "OpenGL error: " << err << std::endl;
    }

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
