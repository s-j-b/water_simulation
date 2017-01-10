#version 410

uniform sampler2D tex0;

//in float intensity;
//in vec2 texCoord;
in vec4 color;

out vec4 fragColor;


void main() 
{ 
    
    fragColor = color;
    //texture(tex0,texCoord);
    //    fragColor *= (1 -
    //            (1 - intensity) *
    //(1 - intensity));

    

  //fragColor = vec4(texCoord.x,texCoord.y,0,1.);
      //fragColor = color;
} 

