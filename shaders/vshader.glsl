#version 410

uniform vec4 vColor;
uniform mat4 model;
uniform mat4 projection;
uniform mat4 transform;
uniform mat4 camera;
uniform mat3 normalMatrix;
uniform mat4 modelView;

uniform vec3 theta;
uniform vec4 lightPos;

in vec4 vPosition;
in vec2 vTexture;
in vec3 vNormal;

out float intensity;
out vec2 texCoord;
out vec4 color;

void main() 
{
    vec4 ambient, diffuse, specular;

    vec3 N = normalize(normalMatrix * vNormal);

    vec3 L = normalize((camera * lightPos).xyz - (modelView * vPosition).xyz);

    vec3 E = -normalize((model * vPosition).xyz); // from pt to viewer

    vec3 R = 2 * dot(L,N) * N - L;
    vec3 H = normalize(L + E);
    float Kd = max(dot(L, N), 0.0);
    float Ks = pow(max(dot(N, H), 0.0), 0.8);

    vec4 light_color = vec4(.69, .69, .69, 0.0);;

    ambient = vColor * 0.2;
    diffuse = vColor * Kd * 0.5;

    //vSColor
    specular = vColor * Ks * 0.3;

    gl_Position = projection*camera*modelView*transform*vPosition;

    intensity = 0.6 + Kd * 0.9;

    // color = vec4((ambient + diffuse + specular).xyz, 1);      
    color = intensity * vColor + 0.3 * Ks * light_color;

    //color = vColor;
    texCoord = vTexture;
} 
