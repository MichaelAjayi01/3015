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
GLuint metallicTextureID;
GLuint heightTextureID;
GLuint normalTextureID;

void checkOpenGLError(const char* stmt, const char* fname, int line) {
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "OpenGL error " << err << " at " << fname << ":" << line << " - for " << stmt << std::endl;
        exit(EXIT_FAILURE);
    }
}

#define GL_CHECK(stmt) do { \
        stmt; \
        checkOpenGLError(#stmt, __FILE__, __LINE__); \
    } while (0)

// Constructor definition
SceneBasic_Uniform::SceneBasic_Uniform() : angle(0.0f), textureID(0), metallicTextureID(0), heightTextureID(0), normalTextureID(0) {
    
}

void SceneBasic_Uniform::initScene() {
    compile();
    glEnable(GL_DEPTH_TEST);
    model = glm::mat4(1.0f);

    // Load the mesh
    std::cout << "Attempting to load mesh..." << std::endl;
    mesh = ObjMesh::load("media/Tachi_Sword_MESH.obj", true);
    if (!mesh) {
        std::cerr << "Failed to load mesh" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "Mesh loaded successfully" << std::endl;

    // Load the base color texture
    int width, height, nrChannels;
    std::cout << "Attempting to load base color texture..." << std::endl;
    unsigned char* data = stbi_load("media/Textures/Tachi_Sword_MESH2_Tachi_Sword_SG_BaseColor.jpg", &width, &height, &nrChannels, 0);
    std::cout << "Texture function executed!" << std::endl;
    if (data) {
        std::cout << "Base color texture loaded: " << width << "x" << height << " Channels: " << nrChannels << std::endl;
        GLenum format = (nrChannels == 4) ? GL_RGBA : (nrChannels == 3) ? GL_RGB : GL_RED;
        GL_CHECK(glGenTextures(1, &textureID));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, textureID));
        GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data));
        GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));

        // Set the texture wrapping/filtering options
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE)); // Wrap horizontally
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE)); // Wrap vertically
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

        stbi_image_free(data);
    }
    else {
        std::cerr << "Failed to load base color texture" << std::endl;
        stbi_image_free(data);
    }

    // Load the metallic texture
    std::cout << "Attempting to load metallic texture..." << std::endl;
    data = stbi_load("media/Textures/Tachi_Sword_MESH2_Tachi_Sword_SG_Metallic.jpg", &width, &height, &nrChannels, 0);
    std::cout << "Texture function executed!" << std::endl;
    if (data) {
        std::cout << "Metallic texture loaded: " << width << "x" << height << " Channels: " << nrChannels << std::endl;
        GLenum format = (nrChannels == 4) ? GL_RGBA : (nrChannels == 3) ? GL_RGB : GL_RED;
        GL_CHECK(glGenTextures(1, &metallicTextureID));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, metallicTextureID));
        GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data));
        GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));

        // Set the texture wrapping/filtering options
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE)); // Wrap horizontally
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE)); // Wrap vertically
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

        // Debug print some of the texture data
        for (int i = 0; i < 10; ++i) {
            std::cout << "Metallic texture data[" << i << "]: " << static_cast<int>(data[i]) << std::endl;
        }

        stbi_image_free(data);
    }
    else {
        std::cerr << "Failed to load metallic texture" << std::endl;
        stbi_image_free(data);
    }

    // Load the height texture
    std::cout << "Attempting to load height texture..." << std::endl;
    data = stbi_load("media/Textures/Tachi_Sword_MESH2_Tachi_Sword_SG_Height.jpg", &width, &height, &nrChannels, 0);
    std::cout << "Texture function executed!" << std::endl;
    if (data) {
        std::cout << "Height texture loaded: " << width << "x" << height << " Channels: " << nrChannels << std::endl;
        GLenum format = (nrChannels == 4) ? GL_RGBA : (nrChannels == 3) ? GL_RGB : GL_RED;
        GL_CHECK(glGenTextures(1, &heightTextureID));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, heightTextureID));
        GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data));
        GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));

        // Set the texture wrapping/filtering options
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

        stbi_image_free(data);
    }
    else {
        std::cerr << "Failed to load height texture" << std::endl;
        stbi_image_free(data);
    }

    // Load the normal map texture
    std::cout << "Attempting to load normal map texture..." << std::endl;
    data = stbi_load("media/Textures/Tachi_Sword_MESH2_Tachi_Sword_SG_Normal.jpg", &width, &height, &nrChannels, 0);
    std::cout << "Texture function executed!" << std::endl;
    if (data) {
        std::cout << "Normal map texture loaded: " << width << "x" << height << " Channels: " << nrChannels << std::endl;
        GLenum format = (nrChannels == 4) ? GL_RGBA : (nrChannels == 3) ? GL_RGB : GL_RED;
        GL_CHECK(glGenTextures(1, &normalTextureID));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, normalTextureID));
        GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data));
        GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));

        // Set the texture wrapping/filtering options
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

        stbi_image_free(data);
    }
    else {
        std::cerr << "Failed to load normal map texture" << std::endl;
        stbi_image_free(data);
    }


    // Check for OpenGL errors
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << std::endl;
    }


    view = glm::lookAt(vec3(0.0f, 0.0f, 35.0f), 
        vec3(0.0f, 0.0f, 0.0f), 
        vec3(0.0f, 1.0f, 0.0f));

    
    model = glm::rotate(model, glm::radians(45.0f), vec3(1.0f, 0.0f, 0.0f));
    projection = mat4(1.0f);

    // Position the light above the blade
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
    prog.setUniform("Material.Kd", vec3(1.0f, 1.0f, 1.0f));
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



void SceneBasic_Uniform::update(float t) {

    angle += 0.0001f;

    // Apply the rotation to the model matrix
    model = glm::mat4(1.0f); // Reset the model matrix
    model = glm::rotate(model, angle, vec3(0.0f, 1.0f, 0.0f)); // Rotate around the y-axis
}

void SceneBasic_Uniform::compile() {
    try {
        prog.compileShader("shader/basic_uniform.vert");
        prog.compileShader("shader/basic_uniform.frag");
        prog.link();
        prog.validate();
        prog.use();
    }
    catch (GLSLProgramException& e) {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}


void SceneBasic_Uniform::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update the matrices
    setMatrices();

    // Bind the base color texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    prog.setUniform("Texture", 0);

    // Bind the metallic texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, metallicTextureID);
    prog.setUniform("MetallicTexture", 1);

    // Bind the height texture
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, heightTextureID);
    prog.setUniform("HeightTexture", 2);

    // Bind the normal map texture
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, normalTextureID);
    prog.setUniform("NormalTexture", 3);

    // Check for OpenGL errors
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << std::endl;
    }

    // Render the mesh
    mesh->render();
}



void SceneBasic_Uniform::resize(int w, int h) {
    glViewport(0, 0, w, h);
    width = w;
    height = h;

    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
}

void SceneBasic_Uniform::setMatrices() {
    prog.use();

    mat4 mv = view * model;
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    prog.setUniform("MVP", projection * mv);
}
