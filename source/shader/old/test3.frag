#version 330 core

uniform vec3 lightPosition;
uniform vec3 eyePosition;

uniform float shininess;

uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;

in vec3 worldPosition;
in vec3 worldNormal;

out vec4 out_Color;


void main(){
    vec3 _worldNormal = worldNormal;
    vec3 phong_ads_light; 
    
    if (!gl_FrontFacing) {
        _worldNormal = -worldNormal;
    }

    vec3 L = normalize( lightPosition - worldPosition);//light direction
    vec3 V = normalize( eyePosition - worldPosition);//view direction

    //float LdotN = max(0.0f, dot(L , _worldNormal));

    vec3 diffuse = kd * max(dot(L, _worldNormal), 0.0f);

    vec3 ambient = ka;

    vec3 specular = ks * pow(max(dot(reflect(-L, _worldNormal), V), 0.0f), shininess);

    

    phong_ads_light = ambient/2 + diffuse + specular;

    out_Color = vec4(phong_ads_light, 1);
}
