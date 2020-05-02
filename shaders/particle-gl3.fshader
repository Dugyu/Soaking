#version 150

uniform sampler2D uTexBase;
uniform sampler2D uTexNoise;

uniform int uTimer;

in vec2 vTexCoord;
in vec3 vPosition;

out vec4 fragColor;

void main(){
    vec4 baseTex = texture(uTexBase,vTexCoord);
    //vec2 noiseTexCoord = vec2((vPosition.x +10.0 )/ 20.0, (vPosition.z + 10.0)/20.0);
    //vec4 nosieTex = texture(uTexNoise,noiseTexCoord);
    fragColor = vec4(1.0 * abs(sin(0.001 * uTimer)), 0.4, 0.35 + 1.0 * (vPosition.x * 0.5 + 5.0) *0.025, baseTex.a);
    if (fragColor.a < 0.2)
    {
        discard;
    }
}
