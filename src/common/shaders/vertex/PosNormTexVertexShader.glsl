#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTextureCoord;

uniform mat4 model;

layout (std140) uniform globalBlockVS {
                    // base alignment			aligned offset
  mat4 projection;  // 64						      0
  mat4 view;        // 64						      64
};

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

void main()
{
  gl_Position = projection * view * model * vec4(aPosition, 1.0);
  FragPos = vec3(model * vec4(aPosition, 1.0));
  mat3 normalMat = mat3(transpose(inverse(model)));
  Normal = normalMat * aNormal;
  TexCoords = aTextureCoord;
}