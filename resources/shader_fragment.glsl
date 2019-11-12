#version 330 core
out vec3 color;
in vec3 vertex_normal;
in vec3 vertex_pos;
in vec2 vertex_tex;
uniform vec3 campos;

uniform sampler2D tex;
uniform sampler2D tex2;

void main()
{
//diffuse
vec3 lp = vec3(0, 3, 10);
vec3 n = normalize(vertex_normal);
vec3 ld = normalize(lp - vertex_pos);
float diffuse = clamp(dot(n, ld), 0, 1);

//specular
vec3 lightColor = vec3(1, 1, 1);
vec3 cd = normalize(campos - vertex_pos);
vec3 h = normalize(cd + ld);
float spec = dot(n, h);
spec = clamp(spec, 0, 1);
spec = pow(spec, 10);

//color
vec2 vertTex  = { vertex_tex.x, -vertex_tex.y };
vec3 baseColor = texture(tex, vertTex).rgb;
color = baseColor * (0.3 + diffuse*0.7) + lightColor*spec*3;
}