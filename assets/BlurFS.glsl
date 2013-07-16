// Blur map Fragment Shader

precision mediump float;
varying vec2 ftexCoord;
varying vec4 oPos;
uniform sampler2D texture;
void main()
{
  gl_FragColor = texture2D(texture, ftexCoord);
  gl_FragColor *= 0.8;
}

