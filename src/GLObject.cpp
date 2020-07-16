#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <map>
#include <string>
#include <vector>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobj_loader.h"

#include "GLObject.h"
#include "Utils.h"

void GLObject::LoadObject(const char *filename, const GLShader &g_BasicShader) {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn, err;

  bool ret =
      tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename);
  if (!warn.empty()) {
    std::cout << warn << std::endl;
  }

  if (!err.empty()) {
    std::cerr << err << std::endl;
  }

  if (!ret) {
    exit(1);
  }

  bool hasNormals = !attrib.normals.empty();
  bool hasTexCoords = !attrib.texcoords.empty();

  std::vector<Vertex> vertices;

  GLushort idx = 0;
  for (const tinyobj::shape_t &shape : shapes) {
    for (const tinyobj::index_t &index : shape.mesh.indices) {
      Vertex vertex = {};

      // position
      vertex.x = attrib.vertices[3 * index.vertex_index + 0];
      vertex.y = attrib.vertices[3 * index.vertex_index + 1];
      vertex.z = attrib.vertices[3 * index.vertex_index + 2];

      if (hasNormals) {
        // normal
        vertex.nx = attrib.normals[3 * index.normal_index + 0];
        vertex.ny = attrib.normals[3 * index.normal_index + 1];
        vertex.nz = attrib.normals[3 * index.normal_index + 2];
      }

      if (hasTexCoords) {
        // texCoord
        vertex.texX = attrib.texcoords[2 * index.texcoord_index + 0];
        vertex.texY = 1.0f - attrib.texcoords[2 * index.texcoord_index + 1];
      }

      auto it = std::find(vertices.begin(), vertices.end(), vertex);
      if (it != vertices.end()) {
        indices.push_back(it->idx);
      } else {
        vertex.idx = idx;
        vertices.push_back(vertex);
        indices.push_back(idx);
        idx++;
      }
    }
  }

  for (const tinyobj::material_t &material : materials) {
    if (material.diffuse_texname.length() > 0) {
      if (textures.find(material.diffuse_texname) == textures.end()) {
        GLuint texture_id;
        int w, h;
        int comp;

        std::string texture_filename = material.diffuse_texname;
        unsigned char *image = Load(texture_filename.c_str(), w, h, comp);
        if (!image) {
          exit(1);
        }

        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, image);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);
        Free(image);
        textures.insert(std::make_pair(material.diffuse_texname, texture_id));
      }
    }

    Material m = {
        {material.ambient[0], material.ambient[1], material.ambient[2]},
        {material.diffuse[0], material.diffuse[1], material.diffuse[2]},
        {material.specular[0], material.specular[1], material.specular[2]},
        material.shininess,
        material.diffuse_texname};
    mats.push_back(m);
  }

  glGenVertexArrays(1, &m_vao);
  glBindVertexArray(m_vao);

  glGenBuffers(1, &m_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0],
               GL_STATIC_DRAW);

  glGenBuffers(1, &m_ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort),
               &indices[0], GL_STATIC_DRAW);

  shader = g_BasicShader.GetProgram();

  glUniform1i(glGetUniformLocation(shader, "u_TextureSampler"), 0);

  int loc_position = glGetAttribLocation(shader, "a_position");
  int loc_normal = glGetAttribLocation(shader, "a_normal");
  int loc_tex = glGetAttribLocation(shader, "a_texcoord");

  glEnableVertexAttribArray(loc_position);
  glEnableVertexAttribArray(loc_normal);
  glEnableVertexAttribArray(loc_tex);

  glVertexAttribPointer(loc_position, 3, GL_FLOAT, false, sizeof(Vertex), 0);
  glVertexAttribPointer(loc_normal, 3, GL_FLOAT, false, sizeof(Vertex),
                        (const void *)offsetof(Vertex, nx));
  glVertexAttribPointer(loc_tex, 2, GL_FLOAT, false, sizeof(Vertex),
                        (const void *)offsetof(Vertex, texX));

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GLObject::DrawObject() {
  if (mats.size() > 0) {
    glUniform3fv(glGetUniformLocation(shader, "ambientColor"), 1,
                 mats[0].ambient);
    glUniform3fv(glGetUniformLocation(shader, "diffuseColor"), 1,
                 mats[0].diffuse);
    glUniform3fv(glGetUniformLocation(shader, "specColor"), 1,
                 mats[0].specular);
    glUniform1f(glGetUniformLocation(shader, "shininess"), mats[0].shininess);

    std::string diffuse_texname = mats[0].diffuse_texname;
    if (textures.find(diffuse_texname) != textures.end()) {
      glBindTexture(GL_TEXTURE_2D, textures[diffuse_texname]);
    }
  }

  glBindVertexArray(m_vao);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, 0);
  glBindVertexArray(0);
}

void GLObject::DestroyObject() {
  glDeleteBuffers(1, &m_vbo);
  glDeleteBuffers(1, &m_ibo);
  glDeleteVertexArrays(1, &m_vao);

  for (const auto &tex : textures) {
    glDeleteTextures(1, &tex.second);
  }
}