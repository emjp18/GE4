#include "pch.h"
#include "Game_Object_File.h"

Game_Object_File::Game_Object_File()
{
}

void Game_Object_File::Clear()
{
	m_meshes.clear();
	m_vertices.clear();
	m_indices.clear();
	m_blendshapes.clear();
	m_morphVertices.clear();
	m_morphIndices.clear();
	m_morphKeyframes.clear();
	m_materials.clear();
	m_lights.clear();
	m_offsetMatrices.clear();
	m_keyframeMatrices.clear();
	m_fileinfo.animationLenght = 0;
	m_fileinfo.BlendShapesCount = 0;
	m_fileinfo.fps = 0;
	m_fileinfo.lightCount = 0;
	m_fileinfo.materialCount = 0;
	m_fileinfo.meshCount = 0;
	m_fileinfo.skeletonBoneCount = 0;
	m_fileinfo.skeletonKeyframeCount = 0;
}
