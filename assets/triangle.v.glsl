attribute vec3 coord3d;
uniform mat4 mvp;
void main(void)
{
  gl_Position=vec4(mvp*vec4(coord3d, 1.0));
}