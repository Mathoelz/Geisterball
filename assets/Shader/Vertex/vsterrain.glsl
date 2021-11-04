#version 400

layout(location=0) in vec4 VertexPos;
layout(location=1) in vec4 VertexNormal;
layout(location=2) in vec2 VertexTexcoord;

out vec3 Position;
out vec3 Normal;
out vec2 Texcoord;

uniform mat4 ModelMat;
uniform mat4 ModelViewProjMat;
uniform vec3 Scaling;

/* Quellen:
    https://stackoverflow.com/questions/36635824/how-to-scale-a-model-in-opengl/36636532
    https://community.khronos.org/t/problems-with-tbo-on-ati/66554
 */

void main()
{
    // Exercise 1 --> für Abgabe einkommentieren

     // Durch vec4(Scaling, 1) wird das Terrain in XZ Richtung skaliert, dafür ist die Größe Richtig
    Position = (ModelMat * VertexPos * vec4(Scaling, 1)).xyz;
    Normal = (ModelMat * vec4(VertexNormal.xyz,0)).xyz;
    Normal = Normal/ Scaling;
    
    Texcoord = VertexTexcoord;
    
    gl_Position = ModelViewProjMat * vec4(Position, 150);
}
