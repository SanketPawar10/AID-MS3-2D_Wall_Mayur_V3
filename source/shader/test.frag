#version 330 core

uniform vec3 lightPosition1;
uniform vec3 lightPosition2;
uniform vec3 eyePosition;
uniform float lightIntensity1;
uniform float lightIntensity2;

uniform float shininess;

uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;

uniform sampler2D textureMap;
uniform bool useTextureMap;

in vec3 worldPosition;
in vec3 worldNormal;
in vec2 texCoord; 

out vec4 out_Color;

void main() {
    vec3 _worldNormal = worldNormal; 
    vec3 phong_ads_light;

    if (!gl_FrontFacing) {
        _worldNormal = -worldNormal;
    }

    vec3 L1 = normalize(lightPosition1 - worldPosition); // Light 1 direction
    vec3 L2 = normalize(lightPosition2 - worldPosition); // Light 2 direction
    vec3 V = normalize(eyePosition - worldPosition); // View direction

    vec3 texColor;
    vec3 diffuse;
    
    if (useTextureMap && textureSize(textureMap, 0).x > 0) {
        texColor = texture(textureMap, texCoord).rgb;
        diffuse += kd * max(dot(_worldNormal, L1), 0.0f) * lightIntensity1;
        diffuse += kd * max(dot(_worldNormal, L2), 0.0f) * lightIntensity2;
    } else {
        texColor = vec3(1.0);
        diffuse += kd * max(dot(L1, _worldNormal), 0.0f) * lightIntensity1;
        diffuse += kd * max(dot(L2, _worldNormal), 0.0f) * lightIntensity2;
    }

    vec3 specular =  ks * pow(max(dot(reflect(-L1, _worldNormal), V), 0.0f), shininess)
                  + ks * pow(max(dot(reflect(-L2, _worldNormal), V), 0.0f), shininess);
    
    vec3 ambient = ka / 10;

    phong_ads_light = (ambient + diffuse + specular) * texColor;

    out_Color = vec4(phong_ads_light, 1.0);
}
