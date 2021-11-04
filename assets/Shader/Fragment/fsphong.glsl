#version 400

in vec3 Position;
in vec3 Normal;
in vec2 Texcoord;
in vec3 Tangent;
in vec3 BiTangent;

out vec4 FragColor;

uniform vec3 LightPos;
uniform vec3 LightColor;

uniform vec3 EyePos;
uniform vec3 DiffuseColor;
uniform vec3 SpecularColor;
uniform vec3 AmbientColor;
uniform float SpecularExp;
uniform sampler2D DiffuseTexture;
uniform sampler2D NormalTexture;

const int MAX_LIGHTS=14;

struct Light
{
        // Typ der Lichtquelle (0=POINT, 1=DIRECTIONAL, 2=SPOT)
    int Type;
        // Farbe der Lichtquelle in RGB.
    vec3 Color;
        // Position der Lichtquelle (für DIRECTIONAL uninteressant)
    vec3 Position;
        // Ausrichtung der Lichtquelle (für POINT uninteressant)
    vec3 Direction;
        // Koeffizienten für Intensitätsabfall über die Entfernung
    vec3 Attenuation;
        // SpotRadius.x enthält den inneren und SpotRadius.y den äußeren Radius der Spot-Lichtquelle (in Radien umgerechnet)
    vec3 SpotRadius;
        // Nur für die Zusatzaufgabe von Bedeutung
    int ShadowIndex;
};

uniform Lights
{
        // Anzahl der aktiven Lichtquellen.
    int LightCount;
    Light lights[MAX_LIGHTS];
};

float sat( in float a)
{
    return clamp(a, 0.0, 1.0);
}

void main()
{
    
    vec4 DiffTex = texture( DiffuseTexture, Texcoord);
    if(DiffTex.a <0.3f) discard;
    
    vec3 NormalTex = texture(NormalTexture, Texcoord).rgb;
    NormalTex.r = (NormalTex.r * 2.0) - 1.0;
    NormalTex.g = (NormalTex.g * 2.0) - 1.0;
    NormalTex.b = (NormalTex.b * 2.0) - 1.0;

    FragColor = vec4(DiffTex.rgb * AmbientColor, DiffTex.a);
    
    mat3 tbnMatrix = mat3(Tangent, -BiTangent, Normal);
    vec3 N = normalize((tbnMatrix * NormalTex).xyz);
    vec3 E = normalize(EyePos-Position);
    

    vec3 DiffuseComponent = vec3(0);
    vec3 SpecularComponent = vec3(0);

    for (int i = 0; i < LightCount; i++) {
        Light light = lights[i];
        
        vec3 L = normalize(LightPos-Position);
        vec3 R = reflect(-L,N);
        
        float intensity = 1.f;

            // Light Direction (OpenGL 4 Shading Language Cookbook --> Seite 113)
        if(light.Type == 1){
             L = normalize(-light.Direction);
        }else{
            L = normalize(light.Position - Position);
                // Der Abstand des Fragments zur Lichtquelle
            float Dist = length(light.Position-Position);
                // Intensitaetsabfall
            intensity = 1.0 / (light.Attenuation.x + light.Attenuation.y * Dist + light.Attenuation.z * Dist * Dist);
        }
        
                
        // Calculate Shadows
        float visibility = 1.0;
        
        vec3 Color = light.Color * intensity;

        if(light.Type == 2) {
                // https://learnopengl.com/Lighting/Light-casters
                float omega = acos(-dot(L, light.Direction));
                
                    // Vorlsung 3 --> Seite 22
                float saturateHelper = (omega - light.SpotRadius.x) / (light.SpotRadius.y - light.SpotRadius.x);
                Color = Color  * (1.0f - sat(saturateHelper));
        }
        
        // Vorlesung 3 --> Seite 19
        vec3 halfwayDir = normalize(E + L);
        
        DiffuseComponent = DiffuseColor * Color * sat(dot(N,L));
        SpecularComponent = SpecularColor  * Color * pow( sat(dot(N,halfwayDir)), SpecularExp);
        FragColor.rgb += (DiffuseComponent * DiffTex.rgb + SpecularComponent);
   }
}
