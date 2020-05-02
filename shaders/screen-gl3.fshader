#version 150

uniform sampler2D uTexPrevFrame;
uniform sampler2D uTexCurFrame;
uniform sampler2D uTexNextFrame;

uniform int uDisplay;

uniform vec4 uColor;

in vec2 vTexCoord;
in vec3 vPosition;

out vec4 fragColor;

void main(){
    // blending
    if (uDisplay == 0){
       vec4 col = texture(uTexCurFrame, vTexCoord);
       fragColor =  vec4(col.rgb, col.a * 0.05);
      }
    else{
      if (uDisplay == 1){
      vec4 col_cur = texture(uTexCurFrame, vTexCoord);
      vec4 col_prev = texture(uTexPrevFrame, vTexCoord);
       fragColor = vec4(col_prev.rgb * 0.95 + col_cur.rgb * 0.05, 1.0);
      }else{
       fragColor = texture(uTexNextFrame,vTexCoord) ;
      }
    }
    // for test
    //fragColor = vec4(0.5,0.5,0.5,1.0);
}
