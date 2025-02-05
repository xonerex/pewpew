
#version 330 core

out vec4 fragcolor;

in vec3 fragpos;
in vec3 normal;

struct DLight {
	vec3 direction;		// directional light direction vector. keep w component 0.0f if vec4
	vec3 diffuse;
	vec3 ambient;
	vec3 specular;
};

uniform vec3 viewpos;
uniform DLight dlight;	// directional light (ie. the sun)

void main() {
	// compute fragment normal 
	vec3 norm = normalize(normal);
	
	// compute fragment ambient component
	vec3 ambient = dlight.ambient;
	
	// compute fragment diffuse component
	vec3 lightdir = normalize(-dlight.direction);
	float diff = max(dot(norm, lightdir), 0.0);
	vec3 diffuse = dlight.diffuse * diff;

	// compute fragment specular component - water should have a specular component
	vec3 viewdir = normalize(viewpos - fragpos);
	vec3 reflectdir = reflect(-lightdir, norm);
	float spec = pow(max(dot(viewdir, reflectdir), 0.0), 64);
	vec3 specular = dlight.specular * spec;
	
	// compute combined result
	vec3 result = (ambient + diffuse + specular) * vec3(0.01f, 0.3f, 0.76f);
	fragcolor = vec4(0.01f, 0.3f, 0.76f, 0.6);
}