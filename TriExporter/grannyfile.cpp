#include "stdafx.h"
#include "grannyfile.h"
#include "half.h"
#include <grannystuff/AutoGranny.h>
#include <grannystuff/Granny.h>
#include <vector>

bool GrannyTriFile::LoadFile(StuffFileEntry &sfe)
{
	if(!dllloaded)
		return false;
	Clear();
	std::vector<char> mem;
	mem.resize(sfe.fileSize);
	sfe.handle->seekg(sfe.fileOffset);
	sfe.handle->read(reinterpret_cast<char*>(&mem[0]), sfe.fileSize);
	GrannyFile* file = (*GrannyReadEntireFileFromMemory)(sfe.fileSize, reinterpret_cast<char*>(&mem[0]));
	mem.clear();
	t_FileInfo* modelinfo = (*GrannyGetFileInfo)(file);
	t_Meshes* mesh = modelinfo->Meshes[0];

	header.numVertices = (*GrannyGetMeshVertexCount)(mesh);
	t_Groups* triangleGroups = (*GrannyGetMeshTriangleGroups)(mesh);
	
	header.sizeVertex = 32;
	numTriangles = (*GrannyGetMeshTriangleCount)(mesh);
	header.numTriangles = numTriangles;
	header.numSurfaces = (*GrannyGetMeshTriangleGroupCount)(mesh);
	triangles.resize(header.numSurfaces);
	surfaces.resize(header.numSurfaces);
	
	int nIndices = (*GrannyGetMeshIndexCount)(mesh);
	vector<dword> ind;
	ind.resize(nIndices);
	(*GrannyCopyMeshIndices)(mesh, 4, &ind[0]);
	for(unsigned int i = 0; i < header.numSurfaces; i++)
	{
		triangles[i].resize(triangleGroups[i].TriCount);
		surfaces[i].numTriangles = triangleGroups[i].TriCount;
			
		for (unsigned int j = 0; j < surfaces[i].numTriangles; ++j)
		{
			int w = (triangleGroups[i].TriFirst + j)*3 ;
			triangles[i][j].resize(3);
			triangles[i][j][0] = ind[w];
			triangles[i][j][1] = ind[w+1];
			triangles[i][j][2] = ind[w+2];
		}
	}
	m_vertices = new Vertex[header.numVertices];
	(*GrannyCopyMeshVertices)(mesh, GrannyPNT332VertexType, m_vertices);
	(*GrannyFreeFile)(file);
	return true;
}


bool GrannyTriFile::LoadFile(string filename)
{
	return false;
}
bool GrannyTriFile::LoadFile(std::ifstream &is)
{
	return false;
}

bool GrannyTriFile::loadedstuff = false;
bool GrannyTriFile::dllloaded = false;

GrannyTriFile::GrannyTriFile():TriFile()
{
	if(!loadedstuff)
	{
		dllloaded = LoadStuff();
		loadedstuff = true;
	}
}