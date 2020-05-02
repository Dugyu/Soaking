#version 150

uniform mat4 uProjMatrix;
uniform mat4 uModelViewMatrix;
uniform mat4 uNormalMatrix;
uniform int uTimer;
uniform sampler2D uTexNoise;

in vec3 aPosition;
in vec3 aNormal;
in vec3 aCentroid;
in vec3 aLocalPos;
in vec2 aTexCoord;

out vec3 vNormal;
out vec3 vPosition;
out vec2 vTexCoord;

void main() {
    vTexCoord = aTexCoord;
    vNormal = vec3(uNormalMatrix * vec4(aNormal, 0.0));
    
    vec2 noiseTexCoord = vec2((aCentroid.x +10.0 )/ 20.0, (aCentroid.z + 10.0)/20.0);;
    vec4 noiseTex = texture(uTexNoise,noiseTexCoord);

    vec3 newCentroid = vec3(aCentroid.x, aCentroid.y + noiseTex.a * 3.0, aCentroid.z);
    //vec3 newCentroid = vec3(aCentroid.x, aCentroid.y, aCentroid.z);

    // send position (eye coordinates) to fragment shader
    
    vec4 tPosition = uModelViewMatrix * vec4(aPosition, 1.0);
    vec3 centerAnimate = vec3(newCentroid.x,newCentroid.y+ 0.1 * sin(0.001*uTimer),newCentroid.z);
    //vec3 centerAnimate = vec3(newCentroid.x,newCentroid.y,newCentroid.z);

    vec4 tCenterPos = uModelViewMatrix * vec4(centerAnimate, 1.0);
    vPosition = vec3(uModelViewMatrix *  vec4(aCentroid,1.0));
    
    //vec2 noiseTexCoord = vec2((vPosition.x +10.0 )/ 20.0, (vPosition.z + 10.0)/20.0);;
    //vec4 noiseTex = texture(uTexNoise,noiseTexCoord);
    
    vec4 centerGlPos = uProjMatrix *  tCenterPos;
    gl_Position = centerGlPos + uProjMatrix * vec4(aLocalPos,1.0) / tCenterPos.w;

}

