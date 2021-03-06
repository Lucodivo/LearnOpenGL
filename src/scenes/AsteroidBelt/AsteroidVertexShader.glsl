#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 3) in mat4 aModel;

out VS_OUT{
  vec3 Normal;
  vec3 Position;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 orbit;

void main()
{
  mat4 model = orbit * aModel;
  mat3 normalMat = mat3(transpose(inverse(model)));
  vs_out.Normal = normalize(normalMat * aNormal);
  vs_out.Position = vec3(model * vec4(aPosition, 1.0));
  gl_Position = projection * view * model * vec4(aPosition, 1.0);
}