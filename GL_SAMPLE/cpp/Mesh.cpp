#pragma warning(disable: 4996)

#include "Mesh.h"



bool loadOBJ( const char * path,	std::vector<glm::vec3> & out_vertices,
									std::vector<glm::vec2> & out_uvs,
									std::vector<glm::vec3> & out_normals,
									std::vector<unsigned int> & out_indices		);


void MESH::Init( const char *path )
{
	std::vector<glm::vec3> pos;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> norms;
	
//	loadOBJ( path , pos , uvs , norms , MESH::indices );
	Load_OBJ( path );

/*	unsigned int num_vert = (unsigned int)pos.size();

	for( unsigned int v=0 ; v<num_vert ; v++ )
	{
		VERTEX vtx;
		vtx.Position  = pos[v];
		vtx.Normal	  = norms[v];
		vtx.TexCoord  = uvs[v];

		MESH::vertices.push_back( vtx );
	}
*/
}









/////////////////////////////// Loading OBJ ///////////////////////////////////////

#include <string>
#include <stdio.h>


bool MESH::Load_OBJ(	const char * filePath )
{
	printf("Loading OBJ file: \"%s\" ---> ", filePath );

	std::vector<unsigned int> rd_pos_Idxs, rd_uv_Idxs, rd_norm_Idxs;
	std::vector<glm::vec3> rd_poss;
	std::vector<glm::vec2> rd_uvs;
	std::vector<glm::vec3> rd_norms;
	std::vector<glm::vec4> rd_cols;


	FILE * file = fopen(filePath, "r");
	if (file == NULL) 
	{
		printf("Impossible to open the file !\n");
		getchar();
		return false;
	}


	while (1) 
	{

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader

		if (strcmp(lineHeader, "v") == 0) 
		{
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			rd_poss.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) 
		{
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
		
			// Invert V coordinate since we will only use DDS texture, which are inverted. 
			// Remove if you want to use TGA or BMP loaders.
			uv.y = -uv.y;

			rd_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) 
		{
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			normal = glm::normalize( normal );
			rd_norms.push_back(normal);
		}
		else if (strcmp(lineHeader, "vc") == 0)		// vertex color added by Y.S.Shim - 2021.09.28
		{
			glm::vec4 col;
			float r,g,b,a;
			fscanf(file, "%f %f %f %f\n", &r, &g, &b, &a );
			col = glm::vec4(r,g,b,a);
			
			rd_cols.push_back(col);
		}
		else if (strcmp(lineHeader, "f") == 0) 
		{
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",	&vertexIndex[0], &uvIndex[0], &normalIndex[0], 
																		&vertexIndex[1], &uvIndex[1], &normalIndex[1], 
																		&vertexIndex[2], &uvIndex[2], &normalIndex[2]  );
			if (matches != 9) 
			{
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				fclose(file);
				return false;
			}

			rd_pos_Idxs.push_back(vertexIndex[0]);
			rd_pos_Idxs.push_back(vertexIndex[1]);
			rd_pos_Idxs.push_back(vertexIndex[2]);

			rd_uv_Idxs.push_back(uvIndex[0]);
			rd_uv_Idxs.push_back(uvIndex[1]);
			rd_uv_Idxs.push_back(uvIndex[2]);

			rd_norm_Idxs.push_back(normalIndex[0]);
			rd_norm_Idxs.push_back(normalIndex[1]);
			rd_norm_Idxs.push_back(normalIndex[2]);
		}
		else 
		{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}


	// int vi_num = vertexIndices.size();
	// 1044 * 3 = 3132 vi's


	// Re-Arrange Attributes & Indices
	std::vector<glm::vec3> tmp_pos;
	std::vector<glm::vec3> tmp_norms;
	std::vector<glm::vec2> tmp_uvs;
	std::vector<glm::vec4> tmp_cols;

	// For each vertex of each triangle
	for (unsigned int i = 0; i < rd_pos_Idxs.size(); i++) 
	{
		// Get the indices of its attributes
		unsigned int pi = rd_pos_Idxs[i];
		unsigned int ui = rd_uv_Idxs[i];
		unsigned int ni = rd_norm_Idxs[i];

		// Get the attributes thanks to the index
		glm::vec3 vertex = rd_poss[pi - 1];		tmp_pos.push_back(vertex);
		glm::vec3 normal = rd_norms[ni - 1];	tmp_norms.push_back(normal);
		
		if( rd_uvs.size() )
		{
			glm::vec2 uv	 = rd_uvs[ui - 1];
			tmp_uvs.push_back(uv);
		}
		
		if( rd_cols.size() )
		{
			glm::vec4 col = rd_cols[pi - 1];
			tmp_cols.push_back(col);
		}


		this->indices.push_back(i);

	}


	// Put into MESH
	unsigned int num_vert = (unsigned int)tmp_pos.size();

	for( unsigned int v=0 ; v<num_vert ; v++ )
	{
		VERTEX vtx;

		vtx.Position  = tmp_pos[v];
		vtx.Normal	  = tmp_norms[v];

		if( rd_uvs.size() )	vtx.TexCoord  = tmp_uvs[v];
		if( rd_cols.size() ) vtx.Color = tmp_cols[v];

		this->vertices.push_back( vtx );
	}	

	


	fclose(file);

	printf( "Done!\n\n" );

	return true;

}



bool loadOBJ(	const char * path,		
	
	std::vector<glm::vec3> & out_vertices,
	std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals,
	std::vector<unsigned int> & out_indices		)

{

	printf("Loading OBJ file: \"%s\" ---> ", path);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;


	FILE * file = fopen(path, "r");
	if (file == NULL) 
	{
		printf("Impossible to open the file !\n");
		getchar();
		return false;
	}

	printf( "Done!\n" );

	while (1) 
	{

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader

		if (strcmp(lineHeader, "v") == 0) 
		{
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) 
		{
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) 
		{
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) 
		{
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",	&vertexIndex[0], &uvIndex[0], &normalIndex[0], 
																		&vertexIndex[1], &uvIndex[1], &normalIndex[1], 
																		&vertexIndex[2], &uvIndex[2], &normalIndex[2]  );
			if (matches != 9) 
			{
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				fclose(file);
				return false;
			}

			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
		else 
		{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}


	// int vi_num = vertexIndices.size();
	// 1044 * 3 = 3132 vi's

	// For each vertex of each triangle
	for (unsigned int i = 0; i < vertexIndices.size(); i++) 
	{
		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];

		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		glm::vec2 uv = temp_uvs[uvIndex - 1];
		glm::vec3 normal = temp_normals[normalIndex - 1];

		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		out_uvs.push_back(uv);
		out_normals.push_back(normal);

		out_indices.push_back(i);

	}


	
	
	fclose(file);

	return true;


}
