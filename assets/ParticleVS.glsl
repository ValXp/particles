// Particles Vertex Shader

attribute vec4 vPosition;
uniform mat4 mvp;
uniform float ptSize;
varying vec3 color;
void main()
{
// pos + half * (1.0 / MAX)
  color.r = (vPosition.x + 3.0) * (1.0/6.0);
  color.g = (vPosition.y + 9.0) * (1.0/18.0);
  color.b = (vPosition.z + 3.0) * (1.0/6.0);
  gl_Position = mvp *vPosition;
  gl_PointSize = ptSize;
}

