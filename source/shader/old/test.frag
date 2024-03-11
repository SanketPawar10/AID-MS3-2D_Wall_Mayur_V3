#version 330 core

uniform vec3 lightPosition;
uniform vec3 eyePosition;
uniform float lightIntensity;

uniform float shininess;

uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;

in vec3 worldPosition;
in vec3 worldNormal;

out vec4 out_Color;

void main() {
    vec3 _worldNormal = worldNormal; 
    vec3 phong_ads_light;
    
    if (!gl_FrontFacing) {
        _worldNormal = -worldNormal;
    }

    vec3 L = normalize(lightPosition - worldPosition); // Light direction
    vec3 V = normalize(eyePosition - worldPosition); // View direction

    vec3 diffuse = kd * max(dot(L, _worldNormal), 0.0f) * lightIntensity;
    vec3 specular = ks * pow(max(dot(reflect(-L, _worldNormal), V), 0.0f), shininess) * lightIntensity;
    vec3 ambient = ka/10;

    phong_ads_light = ambient + diffuse + specular;

    out_Color = vec4(phong_ads_light, 1.0);
}
