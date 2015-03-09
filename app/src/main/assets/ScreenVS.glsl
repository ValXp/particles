// Screen / Blur map Vertex Shader

attribute vec4 vPosition;
attribute vec2 texCoord;
varying vec2 ftexCoord;
varying vec4 oPos;
void main()
{
    oPos = vPosition;
    ftexCoord = texCoord;
    gl_Position = vPosition;
}

