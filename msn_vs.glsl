#version 400            
uniform mat4 PVM;
uniform float size;
uniform mat4 M2;
uniform vec4 diffuse_color;
uniform	vec4 diffuse_light;
uniform vec4 specular_color;
uniform vec4 specular_light;
uniform float shininess;
uniform mat4 V;

in vec3 pos_attrib;
in vec2 tex_coord_attrib;
in vec3 normal_attrib;

out vec2 tex_coord;  
out vec4 diffuse;
out vec4 specular;

vec3 world_normal;
vec3 normalizedN; 
vec4 p = M2 * vec4(pos_attrib, 1.0);
const vec3 l = vec3(0, 1, 0);
vec4 eyePos =  inverse(V)*vec4(0.0f, 0.0f, 0.0f, 1.0f);
vec3 viewVector = vec3(normalize(eyePos - p));

void main(void)
{
	world_normal= vec3(M2 * vec4(normal_attrib, 0));
	normalizedN = normalize(world_normal);
	vec3 refl = reflect(-l, normalizedN);
  //gl_Position = PVM*vec4(pos_attrib * size *sin(time * 0.2), 1.0);
  //gl_Position = PVM*vec4(pos_attrib.x*abs(sin(pos_attrib.x+time)),pos_attrib.y,pos_attrib.z,1.0f);
    gl_Position = PVM * vec4(pos_attrib, 1);
    tex_coord = tex_coord_attrib;
	diffuse = diffuse_color * diffuse_light * vec4(max(dot(l, normalizedN), 0));
	specular = specular_color * specular_light * vec4(max(pow(dot(refl, viewVector), shininess), 0));
}