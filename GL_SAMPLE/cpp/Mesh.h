


#include "./glm/glm.hpp"




struct VERTEX 
{
    glm::vec3 Position;
    glm::vec3 Normal;
    
	glm::vec2 TexCoord;
	glm::vec4 Color;
};

/*
struct GL_Texture 
{
    unsigned int id;	// texture id
    std::string type;	// diffuse texture / specular texture ??
};
*/



#include <vector>


class MESH
{
public:
	// Common for mesh data
	std::vector<VERTEX> vertices;
	std::vector<unsigned int> indices;
	

	void Init( const char *path );
	bool Load_OBJ( const char *path );

	MESH(){ }

};


