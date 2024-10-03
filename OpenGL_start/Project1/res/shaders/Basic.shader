#shader vertex
#version 330 core  //Specifit the version 
       
layout(location = 0) in vec4 position;  
   
void main()  
{  
   gl_Position = position;  
};

#shader fragment
#version 330 core  //Specifit the version 
  
layout(location = 0) out vec4 color;  

uniform vec4 u_Color;
   
void main()  
{  
//   color = vec4(0.3,0,0.7,1.0);  
   color = u_Color;  
};