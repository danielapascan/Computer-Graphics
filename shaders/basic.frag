#version 410 core

in vec4 fPosEye;
in vec3 fNormal;
in vec2 fTexCoords;
in vec4 fragPosLightSpace;
in vec4 fPosEyeLight;

out vec4 fColor;

//matrices
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;

//lighting
uniform vec3 lightDir;
uniform vec3 lightColor;

// textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

//components
vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shininess = 32.0f;

//parkLight
uniform vec3 posLight1;
uniform vec3 posLight2;
uniform float con;
uniform float quad;
uniform float lin;
vec3 parkColor = vec3(1.0f, 0.5f, 0.0f);

//fog
uniform float fogDensity;

void computeDirLight()
{
    vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	//transform normal
	vec3 normalEye = normalize(fNormal);	
	
	//compute light direction
	vec3 lightDirN = normalize(lightDir);
	
	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);
		
	//compute ambient light
	ambient = ambientStrength * lightColor;
	
	//compute diffuse light
	diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	
	//compute specular light
	vec3 reflection = reflect(-lightDirN, normalEye);
	float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
	specular = specularStrength * specCoeff * lightColor;

}

float computeShadow()
{

	// perform perspective divide
	vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

	// Transform to [0,1] range
	normalizedCoords = normalizedCoords * 0.5 + 0.5;

	// Get closest depth value from light's perspective
	float closestDepth = texture(shadowMap, normalizedCoords.xy).r;

	// Get depth of current fragment from light's perspective
	float currentDepth = normalizedCoords.z;

	// Check whether current frag pos is in shadow
	float bias = 0.005f;
	float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;

	if (normalizedCoords.z > 1.0f)
		return 0.0f;
	else 
		return shadow;

}

void makeParkLight1(){

	vec3 cameraPosEye = vec3(0.0f);
	vec3 normalEye = normalize(fNormal);	
	vec3 lightDirN = normalize(posLight1 - fPosEyeLight.xyz);
	vec3 viewDirN = normalize(cameraPosEye - fPosEyeLight.xyz);
	vec3 reflection = reflect(lightDirN, normalEye);

	float diff = max(dot(normalEye,lightDirN),1.0);
	float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);

	//Calculam distanta dintre sursa de lumina si punctul pe suprafata obiectului de iluminat
	float distance =length(posLight1 - fPosEyeLight.xyz);

	//calculul disiparii luminii pe masura ce ne departam de ea
	//constant, linear, și quadratic sunt coeficienți care pot fi ajustați pentru a regla modul în care lumina se atenuează odată cu distanța
	float diss = 25.0 / (con + quad * (distance * distance) + lin * distance);
				 
	vec3 ambientNew = diss * parkColor * texture(diffuseTexture, fTexCoords).rgb;
	vec3 diffuseNew = diff * diss * parkColor * texture(diffuseTexture, fTexCoords).rgb;
	vec3 specularNew =specCoeff  * diss * parkColor * texture(specularTexture, fTexCoords).rgb;

	ambient += ambientNew;
	diffuse += diffuseNew;
	specular += specularNew;
}

void makeParkLight2(){

	vec3 cameraPosEye = vec3(0.0f);
	vec3 normalEye = normalize(fNormal);	
	vec3 lightDirN = normalize(posLight2 - fPosEyeLight.xyz);
	vec3 viewDirN = normalize(cameraPosEye - fPosEyeLight.xyz);
	vec3 reflection = reflect(lightDirN, normalEye);

	float diff = max(dot(normalEye,lightDirN),1.0);
	float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);

	//Calculam distanta dintre sursa de lumina si punctul pe suprafata obiectului de iluminat
	float distance =length(posLight2 - fPosEyeLight.xyz);

	//calculul disiparii luminii pe masura ce ne departam de ea
	//constant, linear, și quadratic sunt coeficienți care pot fi ajustați pentru a regla modul în care lumina se atenuează odată cu distanța
	float diss = 25.0 / (con + quad * (distance * distance) + lin * distance);
				 
	vec3 ambientNew = diss * parkColor * texture(diffuseTexture, fTexCoords).rgb;
	vec3 diffuseNew = diff * diss * parkColor * texture(diffuseTexture, fTexCoords).rgb;
	vec3 specularNew =specCoeff  * diss * parkColor * texture(specularTexture, fTexCoords).rgb;

	ambient += ambientNew;
	diffuse += diffuseNew;
	specular += specularNew;
}


float computeFog()
{
 	float fragmentDistance = length(fPosEye);
 	float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

 	return clamp(fogFactor, 0.0f, 1.0f);
}

void main() 
{
	computeDirLight();
	ambient*=texture(diffuseTexture , fTexCoords).rgb;
	diffuse*=texture(diffuseTexture , fTexCoords).rgb;
	specular*= texture(specularTexture , fTexCoords).rgb;

	float shadow = computeShadow();
	makeParkLight1();
	makeParkLight2();

	vec3 color = min((ambient + (1.0f-shadow)*diffuse)+(1.0f - shadow)*specular , 1.0f);

	vec4 colorFromTexture = texture (diffuseTexture, fTexCoords);

	if(colorFromTexture.a < 0.1)
		discard;
	fColor = colorFromTexture;

	vec4 newColor = vec4(color,1.0f);
	
	float fogFactor = computeFog();
	vec4 fogColor=vec4(0.5f,0.5f,0.5f,1.0f);
	fColor = mix(fogColor,newColor,fogFactor);
}
