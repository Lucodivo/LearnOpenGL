#include <glad/glad.h>

#include <iostream>

#include "ObjectData.h"

void deleteVertexAtt(VertexAtt vertexAtt) {
  glDeleteBuffers(1, &vertexAtt.indexObject);
  glDeleteVertexArrays(1, &vertexAtt.arrayObject);
  glDeleteBuffers(1, &vertexAtt.bufferObject);
}

void deleteFrameBuffer(Framebuffer framebuffer)
{
  glDeleteFramebuffers(1, &framebuffer.id);
  glDeleteTextures(1, &framebuffer.colorAttachment);
  glDeleteRenderbuffers(1, &framebuffer.depthStencilAttachment);
}

VertexAtt initializeCubePosNormTexVertexAttBuffers(bool invertNormals) {
  VertexAtt vertexAtt;
  glGenVertexArrays(1, &vertexAtt.arrayObject);
  glGenBuffers(1, &vertexAtt.bufferObject);
  glGenBuffers(1, &vertexAtt.indexObject);

  glBindVertexArray(vertexAtt.arrayObject);

  const float32* attributes = invertNormals ? cubePosTexInvertedNormAttributes : cubePosTexNormAttributes;

  glBindBuffer(GL_ARRAY_BUFFER, vertexAtt.bufferObject);
  glBufferData(GL_ARRAY_BUFFER, // which buffer data is being entered in
               sizeof(cubePosTexNormAttributes), // size
               attributes,        // data
               GL_STATIC_DRAW); // GL_STATIC_DRAW (most likely not change), GL_DYNAMIC_DRAW (likely to change), GL_STREAM_DRAW (changes every time drawn)

  // position attribute
  glVertexAttribPointer(0, // position vertex attribute (used for location = 0 of Vertex Shader)
                        3, // size (we're using vec3)
                        GL_FLOAT, // type of data
                        GL_FALSE, // whether the data needs to be normalized
                        cubePosTexNormAttSizeInBytes, // vertex attribute stride
                        (void*)0); // offset
  glEnableVertexAttribArray(0);

  // normal attribute
  glVertexAttribPointer(1,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        cubePosTexNormAttSizeInBytes,
                        (void*)(3 * sizeof(float32)));
  glEnableVertexAttribArray(1);

  // texture coords attribute
  glVertexAttribPointer(2,
                        2,
                        GL_FLOAT,
                        GL_FALSE,
                        cubePosTexNormAttSizeInBytes,
                        (void*)(6 * sizeof(float32)));
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexAtt.indexObject);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubePosNormIndices), cubePosNormIndices, GL_STATIC_DRAW);

  // unbind VBO, VAO, & EBO
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  // Must unbind EBO AFTER unbinding VAO, since VAO stores all glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _) calls
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  return vertexAtt;
}

VertexAtt initializeCubePosNormVertexAttBuffers() {
  VertexAtt vertexAtt;
  glGenVertexArrays(1, &vertexAtt.arrayObject);
  glGenBuffers(1, &vertexAtt.bufferObject);
  glGenBuffers(1, &vertexAtt.indexObject);

  glBindVertexArray(vertexAtt.arrayObject);

  glBindBuffer(GL_ARRAY_BUFFER, vertexAtt.bufferObject); // bind object to array buffer
  glBufferData(GL_ARRAY_BUFFER, // which buffer data is being entered in
               sizeof(cubePosNormAttributes), // size
               cubePosNormAttributes,        // dat
               GL_STATIC_DRAW); // GL_STATIC_DRAW (most likely not change), GL_DYNAMIC_DRAW (likely to change), GL_STREAM_DRAW (changes every time drawn)

  // position attribute
  glVertexAttribPointer(0, // position vertex attribute (used for location = 0 of Vertex Shader)
                        3, // size (we're using vec3)
                        GL_FLOAT, // type of data
                        GL_FALSE, // whether the data needs to be normalized
                        cubePosNormAttSizeInBytes, // vertex attribute stride
                        (void*)0); // offset
  glEnableVertexAttribArray(0);

  // normal attribute
  glVertexAttribPointer(1,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        cubePosNormAttSizeInBytes,
                        (void*)(3 * sizeof(float32)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexAtt.indexObject);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubePosNormIndices), cubePosNormIndices, GL_STATIC_DRAW);

  // unbind VBO, VAO, & EBO
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  // Must unbind EBO AFTER unbinding VAO, since VAO stores all glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _) calls
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  return vertexAtt;
}

VertexAtt initializeCubePositionVertexAttBuffers() {
  VertexAtt vertexAtt;
  glGenVertexArrays(1, &vertexAtt.arrayObject); // vertex array object
  glGenBuffers(1, &vertexAtt.bufferObject); // vertex buffer object backing the VAO
  glGenBuffers(1, &vertexAtt.indexObject);

  glBindVertexArray(vertexAtt.arrayObject);
  glBindBuffer(GL_ARRAY_BUFFER, vertexAtt.bufferObject);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cubePositionAttributes), cubePositionAttributes, GL_STATIC_DRAW);

  // set the vertex attributes
  // position attribute
  glVertexAttribPointer(0, // index
                        3, // size
                        GL_FLOAT, // type of data
                        GL_FALSE, // whether the data needs to be normalized
                        cubePositionSizeInBytes, // stride
                        (void*)0); // offset
  glEnableVertexAttribArray(0);

  // bind element buffer object to give indices
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexAtt.indexObject);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubePositionIndices), cubePositionIndices, GL_STATIC_DRAW);

  // unbind VBO, VAO, & EBO
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  // Must unbind EBO AFTER unbinding VAO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  return vertexAtt;
}

VertexAtt initializeQuadPosNormTexVertexAttBuffers() {
  VertexAtt vertexAtt;
  glGenVertexArrays(1, &vertexAtt.arrayObject);
  glGenBuffers(1, &vertexAtt.bufferObject);
  glGenBuffers(1, &vertexAtt.indexObject);

  glBindVertexArray(vertexAtt.arrayObject);

  glBindBuffer(GL_ARRAY_BUFFER, vertexAtt.bufferObject);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(quadPosNormTexVertexAttributes),
               quadPosNormTexVertexAttributes,
               GL_STATIC_DRAW);

  // set the vertex attributes (position, normal, and texture)
  // position attribute
  glVertexAttribPointer(0,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        quadPosNormTexVertexAttSizeInBytes,
                        (void*)0);
  glEnableVertexAttribArray(0);

  // normal attribute
  glVertexAttribPointer(1,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        quadPosNormTexVertexAttSizeInBytes,
                        (void*)(3 * sizeof(float32)));
  glEnableVertexAttribArray(1);

  // texture attribute
  glVertexAttribPointer(2,
                        2,
                        GL_FLOAT,
                        GL_FALSE,
                        quadPosNormTexVertexAttSizeInBytes,
                        (void*)(6 * sizeof(float32)));
  glEnableVertexAttribArray(2);

  // bind element buffer object to give indices
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexAtt.indexObject);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

  // unbind VBO, VAO, & EBO
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  // Must unbind EBO AFTER unbinding VAO, since VAO stores all glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _) calls
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  return vertexAtt;
}

VertexAtt initializeQuadPosNormTexTanBiVertexAttBuffers() {
  VertexAtt vertexAtt;
  glGenVertexArrays(1, &vertexAtt.arrayObject);
  glGenBuffers(1, &vertexAtt.bufferObject);
  glGenBuffers(1, &vertexAtt.indexObject);

  glBindVertexArray(vertexAtt.arrayObject);

  glBindBuffer(GL_ARRAY_BUFFER, vertexAtt.bufferObject);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(quadPosNormTexTanBiVertexAttributes),
               quadPosNormTexTanBiVertexAttributes,
               GL_STATIC_DRAW);

  // set the vertex attributes (position, normal, texture, tangent, and bitangent)
  // position attribute
  glVertexAttribPointer(0,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        quadPosNormTexTanBiVertexAttSizeInBytes,
                        (void*)0);
  glEnableVertexAttribArray(0);

  // normal attribute
  glVertexAttribPointer(1,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        quadPosNormTexTanBiVertexAttSizeInBytes,
                        (void*)(3 * sizeof(float32)));
  glEnableVertexAttribArray(1);

  // texture attribute
  glVertexAttribPointer(2,
                        2,
                        GL_FLOAT,
                        GL_FALSE,
                        quadPosNormTexTanBiVertexAttSizeInBytes,
                        (void*)(6 * sizeof(float32)));
  glEnableVertexAttribArray(2);

  // tangent attribute
  glVertexAttribPointer(3,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        quadPosNormTexTanBiVertexAttSizeInBytes,
                        (void*)(8 * sizeof(float32)));
  glEnableVertexAttribArray(3);

  // bitangent attribute
  glVertexAttribPointer(4,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        quadPosNormTexTanBiVertexAttSizeInBytes,
                        (void*)(11 * sizeof(float32)));
  glEnableVertexAttribArray(4);

  // bind element buffer object to give indices
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexAtt.indexObject);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

  // unbind VBO, VAO, & EBO
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  // Must unbind EBO AFTER unbinding VAO, since VAO stores all glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _) calls
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  return vertexAtt;
}

VertexAtt initializeFrameBufferQuadVertexAttBuffers() {
  VertexAtt vertexAtt;
  glGenVertexArrays(1, &vertexAtt.arrayObject);
  glGenBuffers(1, &vertexAtt.bufferObject);
  glGenBuffers(1, &vertexAtt.indexObject);

  glBindVertexArray(vertexAtt.arrayObject);

  glBindBuffer(GL_ARRAY_BUFFER, vertexAtt.bufferObject);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(quadPosTexVertexAttributes),
               quadPosTexVertexAttributes,
               GL_STATIC_DRAW);

  // set the vertex attributes (position and texture)
  // position attribute
  glVertexAttribPointer(0,
                        2,
                        GL_FLOAT,
                        GL_FALSE,
                        quadPosTexVertexAttSizeInBytes,
                        (void*)0);
  glEnableVertexAttribArray(0);

  // texture attribute
  glVertexAttribPointer(1,
                        2,
                        GL_FLOAT,
                        GL_FALSE,
                        quadPosTexVertexAttSizeInBytes,
                        (void*)(2 * sizeof(float32)));
  glEnableVertexAttribArray(1);

  // bind element buffer object to give indices
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexAtt.indexObject);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

  // unbind VBO, VAO, & EBO
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  // Must unbind EBO AFTER unbinding VAO, since VAO stores all glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _) calls
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  return vertexAtt;
}

Framebuffer initializeFrameBuffer(uint32 width, uint32 height)
{
  Framebuffer resultBuffer;

  // creating frame buffer
  glGenFramebuffers(1, &resultBuffer.id);
  glBindFramebuffer(GL_FRAMEBUFFER, resultBuffer.id);

  // creating frame buffer color texture
  glGenTextures(1, &resultBuffer.colorAttachment);
  // NOTE: Binding the texture to the GL_TEXTURE_2D target, means that
  // NOTE: gl operations on the GL_TEXTURE_2D target will affect our texture
  // NOTE: while it is remains bound to that target
  glActiveTexture(GL_TEXTURE0);
  GLint originalTexture;
  glGetIntegerv(GL_ACTIVE_TEXTURE, &originalTexture);
  glBindTexture(GL_TEXTURE_2D, resultBuffer.colorAttachment);
  glTexImage2D(GL_TEXTURE_2D, 0/*LoD*/, GL_RGB, width, height, 0/*border*/, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, originalTexture); // re-bind original texture

  // attach texture w/ color to frame buffer
  glFramebufferTexture2D(GL_FRAMEBUFFER, // frame buffer we're targeting (draw, read, or both)
                         GL_COLOR_ATTACHMENT0, // type of attachment
                         GL_TEXTURE_2D, // type of texture
                         resultBuffer.colorAttachment, // texture
                         0); // mipmap level

  // creating render buffer to be depth/stencil buffer
  glGenRenderbuffers(1, &resultBuffer.depthStencilAttachment);
  glBindRenderbuffer(GL_RENDERBUFFER, resultBuffer.depthStencilAttachment);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
  glBindRenderbuffer(GL_RENDERBUFFER, 0); // unbind
  // attach render buffer w/ depth & stencil to frame buffer
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, // frame buffer target
                            GL_DEPTH_STENCIL_ATTACHMENT, // attachment point of frame buffer
                            GL_RENDERBUFFER, // render buffer target
                            resultBuffer.depthStencilAttachment);  // render buffer

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  return resultBuffer;
}