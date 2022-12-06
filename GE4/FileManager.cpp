#include "pch.h"
#include "FileManager.h"


FileManager::FileManager()
{
}

void FileManager::Startup()
{
	m_sdkManager = FbxManager::Create();
	FbxIOSettings* ios = FbxIOSettings::Create(m_sdkManager, IOSROOT);
	m_sdkManager->SetIOSettings(ios);
	
}

void FileManager::ImportFBX(const char* path)
{
	FbxImporter* importer = FbxImporter::Create(m_sdkManager, "");

	bool result;

	result = importer->Initialize(path, -1, m_sdkManager->GetIOSettings());
	assert(result);

	m_scene = FbxScene::Create(m_sdkManager, "scene");
	m_currentAxisSystem = m_scene->GetGlobalSettings().GetAxisSystem();
	importer->Import(m_scene);
	importer->Destroy();
	GetRootNode();
}

void FileManager::GetRootNode()
{
	FbxNode* node = m_scene->GetRootNode();
	if (node)
	{
		for (int i = 0; i < node->GetChildCount(); i++)
		{
			GetNodes(node->GetChild(i));
		}
	}
	getMaterial();
}

void FileManager::Write()
{
	m_fileinfo.BlendShapesCount = (unsigned short)m_blendshapes.size();
	m_fileinfo.lightCount = (unsigned short)m_lights.size();
	m_fileinfo.materialCount = (unsigned short)m_materials.size();
	m_fileinfo.meshCount = (unsigned short)m_meshes.size();
	m_fileinfo.skeletonBoneCount = (unsigned short)m_offsetMatrices.size();
	m_fileinfo.skeletonKeyframeCount = 0;
	if (m_keyframeMatrices.size() > 0)
		m_fileinfo.skeletonKeyframeCount = (unsigned short)m_keyframeMatrices[0].size();
	
	std::ofstream go;
	go.open(m_path + ".GO", std::ios::out | std::ios::binary);
	assert(go.is_open());
	
	
	go.write((const char*)&m_fileinfo, sizeof(GOFile::FILE_INFO));
	
	GOFile::TYPE type = GOFile::TYPE::MODEL;
	for (int i = 0; i < m_fileinfo.meshCount; i++)
	{
		go.write((const char*)&type, sizeof(GOFile::TYPE));

		go.write((const char*)&m_meshes[i], sizeof(GOFile::MESH));

		for (int j = 0; j < m_vertices[i].size(); j++)
		{
			go.write((const char*)&m_vertices[i][j], sizeof(GOFile::VERTEX));
		}

		for (int j = 0; j < m_indices[i].size(); j++)
		{
			go.write((const char*)&m_indices[i][j], sizeof(unsigned int));
		}


	}
	type = GOFile::TYPE::SKELETON;
	go.write((const char*)&type, sizeof(GOFile::TYPE));
	

	for (unsigned short i = 0; i < m_fileinfo.skeletonBoneCount; i++)
	{
		go.write((const char*)&m_offsetMatrices[i], sizeof(GOFile::SkeletonOffset));
	}
	for (unsigned short i = 0; i < m_fileinfo.skeletonBoneCount; i++)
	{
		for (unsigned int j = 0; j < m_keyframeMatrices.size(); j++)
		{
			go.write((const char*)&m_keyframeMatrices[i][j], sizeof(GOFile::SkeletonKeyFrame));
		}
	}
	type = GOFile::TYPE::BLEND_SHAPE;
	go.write((const char*)&type, sizeof(GOFile::TYPE));
	for (int m = 0; m < (int)m_blendshapes.size(); m++)
	{
		go.write((const char*)&m_blendshapes[m], sizeof(GOFile::MORPH_TARGET));
		for (int v = 0; v < (int)m_morphVertices[m].size(); v++)
		{
			go.write((const char*)&m_morphVertices[m][v], sizeof(GOFile::MORPH_VERTEX));
		}
		for (int i = 0; i < (int)m_morphIndices[m].size(); i++)
		{
			go.write((const char*)&m_morphIndices[m][i], sizeof(GOFile::MORPH_INDEX));
		}
		if (m_morphKeyframes.size() > m)
		{
			for (int k = 0; k < (int)m_morphKeyframes[m].size(); k++)
			{
				go.write((const char*)&m_morphKeyframes[m][k], sizeof(GOFile::MORPH_KEYFRAME));
			}
		}

	}
	
	type = GOFile::TYPE::LIGHT;
	
	go.write((const char*)&type, sizeof(GOFile::TYPE));

	for (unsigned short i = 0; i < m_fileinfo.lightCount; i++)
	{
		go.write((const char*)&m_lights[i], sizeof(GOFile::LIGHT));
	}

	

	type = GOFile::TYPE::MATERIAL;

	go.write((const char*)&type, sizeof(GOFile::TYPE));

	for (unsigned short i = 0; i < m_fileinfo.materialCount; i++)
	{
		go.write((const char*)&m_materials[i], sizeof(GOFile::MATERIAL));
	}
	
	go.close();
}

FileManager::~FileManager()
{
	
}

void FileManager::ShutDown()
{
	m_sdkManager->Destroy();
}

void FileManager::GetMesh(fbxsdk::FbxNode* node, fbxsdk::FbxScene* scene)
{
	GOFile::MESH mesh;

	FbxMesh* meshNode = (FbxMesh*)node->GetNodeAttribute();
	const char* meshName = node->GetName();
	strcpy_s(mesh.meshName, meshName);

	for (int i = 0; i < node->GetMaterialCount(); i++)
	{
		if (i > 9)
			break;
		FbxSurfaceMaterial* mat = node->GetMaterial(i);

		const char* matName = mat->GetName();

		strcpy_s(mesh.materialName[i], matName);
		
	}
	



	FbxGeometry* geometry = node->GetGeometry();
	ControlPoint* skinningData = nullptr;
	if (geometry->GetDeformerCount(FbxDeformer::eSkin) > 0)
	{
		FbxSkin* skin = (FbxSkin*)geometry->GetDeformer(0, FbxDeformer::eSkin);
		skinningData = GetSkinningInformation(skin, mesh);
	}
	if (geometry->GetDeformerCount(FbxDeformer::eBlendShape) > 0)
	{
		GetBlendShapes(geometry, scene,meshNode, mesh);
	}
	Array<GOFile::VERTEX> vertices;
	Array<unsigned int> indices;
	GetVerticesAndIndices(vertices, indices, meshNode,skinningData);
	mesh.indexCount = (unsigned int)indices.size();
	mesh.vertexCount = (unsigned int)vertices.size();
	m_meshes.push_back(mesh);
	m_vertices.push_back(vertices);
	m_indices.push_back(indices);

}

void FileManager::GetNodes(FbxNode* node)
{
	if (node->GetNodeAttribute())
	{
		FbxNodeAttribute::EType attributeType = (node->GetNodeAttribute()->GetAttributeType());

		switch (attributeType)
		{
		case FbxNodeAttribute::eLight:
			getLight(node);
			break;
		case FbxNodeAttribute::eMesh:

			GetMesh(node, m_scene);
			break;

		default:
			break;
		}

	}
	for (int i = 0; i < node->GetChildCount(); i++)
	{
		GetNodes(node->GetChild(i));
	}
}

void FileManager::getMaterial()
{
	int matCount = m_scene->GetMaterialCount();
	GOFile::MATERIAL mat;
	

	for (int i = 0; i < matCount; i++)
	{
		FbxSurfaceMaterial* material = m_scene->GetMaterial(i);

		FbxClassId type = material->GetClassId();

		const char* name = material->GetName();

		strcpy_s(m_materials[i].name, name);

		if (type.Is(FbxSurfaceLambert::ClassId))
		{
			FbxSurfaceLambert* lambert = (FbxSurfaceLambert*)material;



			FbxDouble3 ka = lambert->Ambient;
			FbxDouble3 kd = lambert->Diffuse;


			mat.ka[0] = (float)ka[0];
			mat.ka[1] = (float)ka[1];
			mat.ka[2] = (float)ka[2];

			mat.kd[0] = (float)kd[0];
			mat.kd[1] = (float)kd[1];
			mat.kd[2] = (float)kd[2];



		}
		if (type.Is(FbxSurfacePhong::ClassId))
		{
			FbxSurfacePhong* phong = (FbxSurfacePhong*)material;

			FbxDouble3 ka = phong->Ambient;
			FbxDouble3 kd = phong->Diffuse;
			FbxDouble3 ks = phong->Specular;
			FbxDouble exponent = phong->SpecularFactor;



			mat.ka[0] = (float)ka[0];
			mat.ka[1] = (float)ka[1];
			mat.ka[2] = (float)ka[2];

			mat.kd[0] = (float)kd[0];
			mat.kd[1] = (float)kd[1];
			mat.kd[2] = (float)kd[2];

			mat.ks[0] = (float)ks[0];
			mat.ks[1] = (float)ks[1];
			mat.ks[2] = (float)ks[2];

			mat.ks[3] = (float)exponent;


		}
		m_materials.push_back(mat);
	}
}

void FileManager::getLight(FbxNode* lightNode)
{

	FbxVector4 forward;
	bool shouldSwitchAxisSystem = false;
	FbxAxisSystem leftAxis(FbxAxisSystem::DirectX);

	if (m_scene->GetGlobalSettings().GetAxisSystem().GetCoorSystem() != leftAxis.GetCoorSystem())
	{
		shouldSwitchAxisSystem = true;
		forward.Set(0, 0, -1, 0);
	}
	bool global = true;

	GOFile::LIGHT lightStruct;

	FbxLight* light = nullptr;

	light = (FbxLight*)lightNode->GetNodeAttribute();



	//
	FbxLight::EType lightType = light->LightType.Get();

	if (lightType == FbxLight::EType::ePoint)
	{
		lightStruct.type = GOFile::LIGHT_TYPE::POINT;
		FbxDouble3 t_color = light->Color.Get();
		lightStruct.r = (float)t_color[0];
		lightStruct.g = (float)t_color[1];
		lightStruct.b = (float)t_color[2];

		FbxDouble t_intensity = light->Intensity.Get();
		lightStruct.intensity = (float)t_intensity / 100.0f;

		FbxDouble3 t_pos = lightNode->LclTranslation.Get();


		if (shouldSwitchAxisSystem)
		{
			lightStruct.posX = (float)t_pos.mData[0];
			lightStruct.posY = (float)t_pos.mData[1];
			lightStruct.posZ = -(float)t_pos.mData[2];
		}
		else
		{
			lightStruct.posX = (float)t_pos.mData[0];
			lightStruct.posY = (float)t_pos.mData[1];
			lightStruct.posZ = (float)t_pos.mData[2];
		}


		m_lights.push_back(lightStruct);
		return;
	}
	else if (lightType == FbxLight::EType::eDirectional)
	{
		lightStruct.type = GOFile::LIGHT_TYPE::DIR;
		auto eulerRot = lightNode->LclRotation.Get();
		FbxDouble3 t_color = light->Color.Get();
		lightStruct.r = (float)t_color[0];
		lightStruct.g = (float)t_color[1];
		lightStruct.b = (float)t_color[2];

		FbxDouble t_intensity = light->Intensity.Get();
		lightStruct.intensity = (float)t_intensity / 100.0f;

		FbxAMatrix rotM;
		rotM.SetIdentity();
		rotM.SetR({ eulerRot }, FbxEuler::EOrder::eOrderXYZ);

		forward = rotM.MultT(forward);

		if (shouldSwitchAxisSystem)
		{
			lightStruct.dirX = (float)forward.mData[0];
			lightStruct.dirY = (float)forward.mData[1];
			lightStruct.dirZ = -(float)forward.mData[2];
		}
		else
		{
			lightStruct.dirX = (float)forward.mData[0];
			lightStruct.dirY = (float)forward.mData[1];
			lightStruct.dirZ = (float)forward.mData[2];
		}


	}



		m_lights.push_back(lightStruct);
}


ControlPoint* FileManager::GetSkinningInformation(FbxSkin* skinNode, GOFile::MESH& mesh)
{
	int clusterCount = skinNode->GetClusterCount();
	Array<FbxString> uniqueClusters;
	Array<ControlPoint> controlPoints;
	for (int cluster = 0; cluster < clusterCount; cluster++)
	{
		FbxCluster* clusterNode = skinNode->GetCluster(cluster);
		FbxString boneName = clusterNode->GetLink()->GetName();
		
		short boneindex = (short)cluster;
		int clusterIndexCount = clusterNode->GetControlPointIndicesCount();
		int* clusterIndices = clusterNode->GetControlPointIndices();
		double* clusterWeights = clusterNode->GetControlPointWeights();

		for (int cp = 0; cp < clusterIndexCount; cp++)
		{
			ControlPoint controlpoint;
			controlpoint.index = clusterIndices[cp];

			auto it = controlPoints.find(controlpoint);
			
			if (it == controlPoints.end())
			{

				controlpoint.boneName.push_back(boneName);
				controlpoint.boneIndex.push_back(boneindex);
				controlpoint.skinWeight.push_back((float)clusterWeights[cp]);
				controlPoints.push_back(controlpoint);
			}
			else
			{
				int index = (int)controlPoints.findIndex(controlpoint);
				auto it2 = controlPoints[index].boneIndex.find(boneindex);

				if (it2 == controlPoints[index].boneIndex.end())
				{
					controlPoints[index].boneName.push_back(boneName);
					controlPoints[index].boneIndex.push_back(boneindex);
					controlPoints[index].skinWeight.push_back((float)clusterWeights[cp]);
				}


			}


		}
	}
	ControlPoint* controlpointsPtr = new ControlPoint[controlPoints.size()];
	int lod = -INT_MAX;
	for (int cp = 0; cp < (int)controlPoints.size(); cp++)
	{
		// the one with index n should be at n.
		controlpointsPtr[controlPoints[cp].index] = controlPoints[cp];

		lod = max(lod, (int)controlPoints[cp].boneIndex.size());
	}
	
	mesh.bonePerVertexCount = (unsigned short)lod;

	return controlpointsPtr;
}
void FileManager::GetBlendShapes(FbxGeometry* geometry, fbxsdk::FbxScene* scene, FbxMesh* meshNode, GOFile::MESH& mesh)
{
	bool shouldSwitchAxisSystem = false;
	FbxAxisSystem leftAxis(FbxAxisSystem::DirectX);

	if (m_currentAxisSystem.GetCoorSystem() != leftAxis.GetCoorSystem())
	{
		shouldSwitchAxisSystem = true;
	}

	int blendShapeDeformerCount = geometry->GetDeformerCount(FbxDeformer::eBlendShape);
	FbxAnimStack* stack = scene->GetSrcObject<FbxAnimStack>(0);
	FbxTakeInfo* take = scene->GetTakeInfo(stack->GetName());

	FbxTime timeInfo = take->mLocalTimeSpan.GetDuration();
	
	m_fileinfo.animationLenght = (float)timeInfo.GetFrameCountPrecise();
	int layerCount = stack->GetMemberCount<FbxAnimLayer>();
	for (int bsd = 0; bsd < blendShapeDeformerCount; bsd++)
	{
		FbxBlendShape* blendShape = (FbxBlendShape*)geometry->GetDeformer(bsd, FbxDeformer::eBlendShape);
		int blendShapeCount = blendShape->GetBlendShapeChannelCount();
		
		for (int layer = 0; layer < layerCount; layer++)
		{
			FbxAnimLayer* animLayer = stack->GetMember<FbxAnimLayer>(layer);

			for (int bs = 0; bs < blendShapeCount; bs++)
			{
				GOFile::MORPH_TARGET mt;
				FbxBlendShapeChannel* channel = blendShape->GetBlendShapeChannel(bs);
				
				strcpy_s(mt.sourceName, mesh.meshName);
				strcpy_s(mt.name, channel->GetName());
				FbxAnimCurve* animCurve = geometry->GetShapeChannel(bsd, bs, animLayer);
				Array<GOFile::MORPH_KEYFRAME> keyframes;
				if (animCurve)
				{
					int keyCount = animCurve->KeyGetCount();
					
					for (int key = 0; key < keyCount; key++)
					{
						GOFile::MORPH_KEYFRAME kf;
						kf.weight = animCurve->KeyGetValue(key) / 100.0f;
						kf.timeStamp = (float)animCurve->KeyGetTime(key).GetFrameCountPrecise();
						keyframes.push_back(kf);
					}
					m_morphKeyframes.push_back(keyframes);
					mt.morphKeyframeCount = (unsigned int)keyCount;
				}
				FbxShape* shape = channel->GetTargetShape(0); //only one target per shape.
				

				FbxLayerElementArrayTemplate<FbxVector4>* narr = nullptr;
				bool normalBool = shape->GetNormals(&narr);
				FbxLayerElementArrayTemplate<FbxVector4>* tarr = nullptr;
				bool tangentBool = shape->GetTangents(&tarr);
				FbxLayerElementArrayTemplate<FbxVector4>* barr = nullptr;
				bool biNormalsBool = shape->GetBinormals(&barr);
				FbxLayerElementArrayTemplate<int>* nInd = nullptr;
				bool normalIndiceseBool = shape->GetNormalsIndices(&nInd);
				FbxLayerElementArrayTemplate<int>* tInd = nullptr;
				bool tangentIndiceseBool = shape->GetTangentsIndices(&tInd);
				FbxLayerElementArrayTemplate<int>* bInd = nullptr;
				bool biNormalIndiceseBool = shape->GetBinormalsIndices(&bInd);
				int* posIndices = shape->GetControlPointIndices();
				FbxVector4* cp = shape->GetControlPoints();

				
				Array<GOFile::MORPH_VERTEX> mvertices;
				for (int j = 0; j < shape->GetControlPointIndicesCount(); j++)
				{
					GOFile::MORPH_VERTEX mvertex;
					GOFile::MORPH_INDEX mindex;
					mvertex.posX =(float)cp[posIndices[j]].mData[0];
					mvertex.posY =(float)cp[posIndices[j]].mData[1];
					mvertex.posZ =(float)cp[posIndices[j]].mData[2];
					mindex.targetIndex = posIndices[j];

					if (normalBool && narr && nInd->GetCount() == shape->GetControlPointIndicesCount())
					{
						mvertex.normalX = (float)narr->GetAt(nInd->GetAt(j)).mData[0];
						mvertex.normalY = (float)narr->GetAt(nInd->GetAt(j)).mData[1];
						mvertex.normalZ = (float)narr->GetAt(nInd->GetAt(j)).mData[2];
						
					}
					if (tangentBool && tarr && tInd->GetCount() == shape->GetControlPointIndicesCount())
					{
						mvertex.tangentX = (float)tarr->GetAt(tInd->GetAt(j)).mData[0];
						mvertex.tangentY = (float)tarr->GetAt(tInd->GetAt(j)).mData[1];
						mvertex.tangentZ = (float)tarr->GetAt(tInd->GetAt(j)).mData[2];

					}
					if (biNormalsBool && barr && bInd->GetCount() == shape->GetControlPointIndicesCount())
					{
						mvertex.biNormalX = (float)barr->GetAt(bInd->GetAt(j)).mData[0];
						mvertex.biNormalY = (float)barr->GetAt(bInd->GetAt(j)).mData[1];
						mvertex.biNormalZ = (float)barr->GetAt(bInd->GetAt(j)).mData[2];

					}

					if (shouldSwitchAxisSystem)
					{
						
						mvertex.posZ = -mvertex.posZ;
						
						mvertex.normalZ = -mvertex.normalZ;
						
						mvertex.tangentZ = -mvertex.tangentZ;
						
						mvertex.biNormalZ = -mvertex.biNormalZ;
					}
					mvertices.push_back(mvertex);
				}

				m_blendshapes.push_back(mt);
				m_morphVertices.push_back(mvertices);
			}
			
		}
		

	}
}
void FileManager::GetSkeleton(fbxsdk::FbxNode* node, fbxsdk::FbxScene* scene)
{
	FbxGeometry* geometry = node->GetGeometry();
	FbxSkin* skin = (FbxSkin*)geometry->GetDeformer(0, FbxDeformer::eSkin);

	if (!skin)
	{
		return;
	}
	bool shouldSwitchAxisSystem = false;
	FbxAxisSystem leftAxis(FbxAxisSystem::DirectX);

	if (m_currentAxisSystem.GetCoorSystem() != leftAxis.GetCoorSystem())
	{
		shouldSwitchAxisSystem = true;
	}
	FbxTime currTime;
	FbxAnimStack* stack = scene->GetSrcObject<FbxAnimStack>(0);
	FbxTakeInfo* take = scene->GetTakeInfo(stack->GetName());
	FbxTime timeInfo = take->mLocalTimeSpan.GetDuration();
	FbxTime start = take->mLocalTimeSpan.GetStart();
	FbxTime stop = take->mLocalTimeSpan.GetStop();
	scene->GetGlobalSettings().SetTimeMode(FbxTime::ConvertFrameRateToTimeMode((double)m_fps));
	auto timeMode = scene->GetGlobalSettings().GetTimeMode();
	int boneCount = skin->GetClusterCount();
	m_fileinfo.animationLenght = (float)timeInfo.GetFrameCountPrecise();
	
	for (int bone = 0; bone < boneCount; bone++)
	{
		FbxString name = skin->GetCluster(bone)->GetLink()->GetName();
		FbxNode* boneNode = skin->GetCluster(bone)->GetLink();
		FbxString parentName = skin->GetCluster(bone)->GetLink()->GetParent()->GetName();
		FbxAMatrix offsetM;
		skin->GetCluster(bone)->GetTransformLinkMatrix(offsetM);
		offsetM = offsetM.Inverse();
		GOFile::SkeletonOffset offset;
		if (shouldSwitchAxisSystem)
		{
			FbxQuaternion offsetQ = offsetM.GetQ();
			FbxVector4 offsetT = offsetM.GetT();
			FbxVector4 offsetS = offsetM.GetS();
			
			offsetT.mData[2] = -offsetT.mData[2];
			offsetQ.mData[0] = -offsetQ.mData[0];
			offsetQ.mData[1] = -offsetQ.mData[1];
			FbxAMatrix switchedOffsetM;
			switchedOffsetM.SetS(offsetS);
			switchedOffsetM.SetQ(offsetQ);
			switchedOffsetM.SetT(offsetT);
		
			offset.m[0][0] = (float)switchedOffsetM.Get(0, 0);
			offset.m[0][1] = (float)switchedOffsetM.Get(0, 1);
			offset.m[0][2] = (float)switchedOffsetM.Get(0, 2);
			offset.m[0][3] = (float)switchedOffsetM.Get(0, 3);
			offset.m[1][0] = (float)switchedOffsetM.Get(1, 0);
			offset.m[1][1] = (float)switchedOffsetM.Get(1, 1);
			offset.m[1][2] = (float)switchedOffsetM.Get(1, 2);
			offset.m[1][3] = (float)switchedOffsetM.Get(1, 3);
			offset.m[2][0] = (float)switchedOffsetM.Get(2, 0);
			offset.m[2][1] = (float)switchedOffsetM.Get(2, 1);
			offset.m[2][2] = (float)switchedOffsetM.Get(2, 2);
			offset.m[2][3] = (float)switchedOffsetM.Get(2, 3);
			offset.m[3][0] = (float)switchedOffsetM.Get(3, 0);
			offset.m[3][1] = (float)switchedOffsetM.Get(3, 1);
			offset.m[3][2] = (float)switchedOffsetM.Get(3, 2);
			offset.m[3][3] = (float)switchedOffsetM.Get(3, 3);
		}
		else
		{
			offset.m[0][0] = (float)offsetM.Get(0, 0);
			offset.m[0][1] = (float)offsetM.Get(0, 1);
			offset.m[0][2] = (float)offsetM.Get(0, 2);
			offset.m[0][3] = (float)offsetM.Get(0, 3);
			offset.m[1][0] = (float)offsetM.Get(1, 0);
			offset.m[1][1] = (float)offsetM.Get(1, 1);
			offset.m[1][2] = (float)offsetM.Get(1, 2);
			offset.m[1][3] = (float)offsetM.Get(1, 3);
			offset.m[2][0] = (float)offsetM.Get(2, 0);
			offset.m[2][1] = (float)offsetM.Get(2, 1);
			offset.m[2][2] = (float)offsetM.Get(2, 2);
			offset.m[2][3] = (float)offsetM.Get(2, 3);
			offset.m[3][0] = (float)offsetM.Get(3, 0);
			offset.m[3][1] = (float)offsetM.Get(3, 1);
			offset.m[3][2] = (float)offsetM.Get(3, 2);
			offset.m[3][3] = (float)offsetM.Get(3, 3);
		}

		

		strcpy_s(offset.boneName, name);
		strcpy_s(offset.boneParentName, parentName);
		offset.boneChildCount = (unsigned short)skin->GetCluster(bone)->GetLink()->GetChildCount();
		offset.boneIndex = (unsigned short)bone;
		m_offsetMatrices.push_back(offset);
		Array<GOFile::SkeletonKeyFrame> skeletonkeyframearr;
		for (FbxLongLong frame = start.GetFrameCount(timeMode); frame <= stop.GetFrameCount(timeMode); frame++)
		{
			currTime.SetFrame(frame, timeMode);
			FbxAMatrix localTransform = boneNode->EvaluateLocalTransform(currTime);
			FbxVector4 s = localTransform.GetS();
			FbxQuaternion r = localTransform.GetQ();
			FbxVector4 t = localTransform.GetT();
			GOFile::SkeletonKeyFrame skeletonKeyFrame;
			//r.Normalize();
			if (shouldSwitchAxisSystem)
			{
				skeletonKeyFrame.s[0] = (float)s.mData[0];
				skeletonKeyFrame.s[1] = (float)s.mData[1];
				skeletonKeyFrame.s[2] = (float)s.mData[2];
				skeletonKeyFrame.s[3] = (float)s.mData[3];
				skeletonKeyFrame.r[0] = -(float)r.mData[0];
				skeletonKeyFrame.r[1] = -(float)r.mData[1];
				skeletonKeyFrame.r[2] = (float)r.mData[2];
				skeletonKeyFrame.r[3] = (float)r.mData[3];
				skeletonKeyFrame.t[0] = (float)t.mData[0];
				skeletonKeyFrame.t[1] = (float)t.mData[1];
				skeletonKeyFrame.t[2] = -(float)t.mData[2];
				skeletonKeyFrame.t[3] = (float)t.mData[3];
			}
			else
			{
				skeletonKeyFrame.s[0] = (float)s.mData[0];
				skeletonKeyFrame.s[1] = (float)s.mData[1];
				skeletonKeyFrame.s[2] = (float)s.mData[2];
				skeletonKeyFrame.s[3] = (float)s.mData[3];
				skeletonKeyFrame.r[0] = (float)r.mData[0];
				skeletonKeyFrame.r[1] = (float)r.mData[1];
				skeletonKeyFrame.r[2] = (float)r.mData[2];
				skeletonKeyFrame.r[3] = (float)r.mData[3];
				skeletonKeyFrame.t[0] = (float)t.mData[0];
				skeletonKeyFrame.t[1] = (float)t.mData[1];
				skeletonKeyFrame.t[2] = (float)t.mData[2];
				skeletonKeyFrame.t[3] = (float)t.mData[3];
			}
			skeletonKeyFrame.boneIndex = (unsigned short)bone;
			skeletonkeyframearr.push_back(skeletonKeyFrame);
			
		}
		m_keyframeMatrices.push_back(skeletonkeyframearr);
	}
}
//I need to double check this is an accurate way of getting vertex data.
void FileManager::GetVerticesAndIndices(Array<GOFile::VERTEX>& vertices, Array<unsigned int>& indices, FbxMesh* meshNode, 
	ControlPoint* skinningData)
{
	int indexCount = meshNode->GetPolygonVertexCount();
	Array<unsigned int> positionIndices(indexCount);
	Array<unsigned int> normalIndices;
	Array<unsigned int> tangentIndices;
	Array<unsigned int> biNormalIndices;
	Array<unsigned int> uvIndices;
	Array<FbxDouble2> uvVector;
	Array<FbxDouble4> posVector;
	Array<FbxDouble4> normalVector;
	Array<FbxDouble4> tangentVector;
	Array<FbxDouble4> binormalVector;

	FbxAMatrix localMatrix = meshNode->GetNode()->EvaluateLocalTransform();

	FbxNode* pParentNode = meshNode->GetNode()->GetParent();
	FbxAMatrix parentMatrix = pParentNode->EvaluateLocalTransform();
	while ((pParentNode = pParentNode->GetParent()) != NULL)
	{
		parentMatrix = pParentNode->EvaluateLocalTransform() * parentMatrix;
	}


	FbxAMatrix globalTransform;// = meshNode->GetNode()->EvaluateGlobalTransform();
	globalTransform.SetIdentity();
	FbxAMatrix geoMat;
	geoMat.SetIdentity();
	geoMat.SetS(meshNode->GetNode()->GetGeometricScaling(FbxNode::EPivotSet::eSourcePivot));
	geoMat.SetR(meshNode->GetNode()->GetGeometricRotation(FbxNode::EPivotSet::eSourcePivot));
	geoMat.SetT(meshNode->GetNode()->GetGeometricTranslation(FbxNode::EPivotSet::eSourcePivot));

	globalTransform = parentMatrix * localMatrix * geoMat;

	FbxAMatrix inverseT = globalTransform.Inverse();
	inverseT.Transpose();

	bool shouldSwitchAxisSystem = false;
	FbxAxisSystem leftAxis(FbxAxisSystem::DirectX);
	
	if (m_currentAxisSystem.GetCoorSystem() != leftAxis.GetCoorSystem())
	{
		shouldSwitchAxisSystem = true;
	}
	FbxVector4* positions = meshNode->GetControlPoints();

	for (int i = 0; i < (int)meshNode->GetControlPointsCount(); i++)
	{
		positions[i] = globalTransform.MultT(positions[i]);
		FbxDouble4 double4;
		double4.mData[0] = positions[i].mData[0];
		double4.mData[1] = positions[i].mData[1];
		double4.mData[2] = positions[i].mData[2];
		double4.mData[3] = positions[i].mData[3];
		posVector.push_back(double4);

	}
	int faceCount = meshNode->GetPolygonCount();

	int indiceUV = 0;
	int indice = 0;
	int indiceNormal = 0;
	int indiceTangent = 0;
	int indiceBinormal = 0;
	bool controlPointUV = false;
	bool controlPointNormal = false;
	bool controlPointTangent = false;
	bool controlPointBiNormal = false;
	for (int face = 0; face < faceCount; face++)
	{
		int polygonSize = meshNode->GetPolygonSize(face);
		for (int polygonVertex = 0; polygonVertex < polygonSize; polygonVertex++)
		{
			int controlPointIndex = meshNode->GetPolygonVertex(face, polygonVertex);
			positionIndices[indice++] = controlPointIndex;
			FbxGeometryElementUV* elementUV = meshNode->GetElementUV();
			FbxGeometryElementNormal* elementNormal = meshNode->GetElementNormal();
			FbxGeometryElementTangent* elementTangent = meshNode->GetElementTangent();
			FbxGeometryElementBinormal* elementBiNormal = meshNode->GetElementBinormal();
			if (elementUV->GetMappingMode() == FbxGeometryElement::eByControlPoint)
			{
				controlPointUV = true;
				if (elementUV->GetReferenceMode() == FbxGeometryElement::eDirect)
				{
					FbxVector2 UV = elementUV->GetDirectArray().GetAt(controlPointIndex);
					FbxDouble2 double2;
					double2.mData[0] = UV.mData[0];
					double2.mData[1] = UV.mData[1];
					uvVector.push_back(double2);

				}
				else if (elementUV->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
				{
					int id = elementUV->GetIndexArray().GetAt(controlPointIndex);
					FbxVector2 UV = elementUV->GetDirectArray().GetAt(id);
					FbxDouble2 double2;
					double2.mData[0] = UV.mData[0];
					double2.mData[1] = UV.mData[1];
					uvVector.push_back(double2);

				}
				else
				{
					assert(0);
				}
			}
			else if (elementUV->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
			{
				
				int uvIndex = meshNode->GetTextureUVIndex(face, polygonVertex);
				FbxVector2 UV = elementUV->GetDirectArray().GetAt(uvIndex);
				FbxDouble2 double2;
				double2.mData[0] = UV.mData[0];
				double2.mData[1] = UV.mData[1];
				SortIndices(uvVector, uvIndices, double2, indiceUV);
				
			}
			else
			{
				assert(0);
			}
			if (elementNormal->GetMappingMode() == FbxGeometryElement::eByControlPoint)
			{
				controlPointNormal = true;
				if (elementNormal->GetReferenceMode() == FbxGeometryElement::eDirect)
				{
					FbxVector4 normal = elementNormal->GetDirectArray().GetAt(controlPointIndex);
					normal = inverseT.MultR(normal);
					normal.Normalize();
					FbxDouble4 double4;
					double4.mData[0] = normal.mData[0];
					double4.mData[1] = normal.mData[1];
					double4.mData[2] = normal.mData[2];
					double4.mData[3] = normal.mData[3];
					normalVector.push_back(double4);

				}
				else if (elementNormal->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
				{
					int id = elementNormal->GetIndexArray().GetAt(controlPointIndex);
					FbxVector4 normal = elementNormal->GetDirectArray().GetAt(id);
					normal = inverseT.MultR(normal);
					normal.Normalize();
					FbxDouble4 double4;
					double4.mData[0] = normal.mData[0];
					double4.mData[1] = normal.mData[1];
					double4.mData[2] = normal.mData[2];
					double4.mData[3] = normal.mData[3];
					normalVector.push_back(double4);

				}
				else
				{
					assert(0);
				}
			}
			else if (elementNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
			{
				
				FbxVector4 normal;
				bool result = meshNode->GetPolygonVertexNormal(face, polygonVertex, normal);
				assert(result);
				normal = inverseT.MultR(normal);
				normal.Normalize();
				FbxDouble4 double4;
				double4.mData[0] = normal.mData[0];
				double4.mData[1] = normal.mData[1];
				double4.mData[2] = normal.mData[2];
				double4.mData[3] = normal.mData[3];
				SortIndices(normalVector, normalIndices, double4, indiceNormal);
				
			}
			else
			{
				assert(0);
			}
			if (elementTangent->GetMappingMode() == FbxGeometryElement::eByControlPoint)
			{
				controlPointTangent = true;
				if (elementTangent->GetReferenceMode() == FbxGeometryElement::eDirect)
				{
					FbxVector4 tangent = elementTangent->GetDirectArray().GetAt(controlPointIndex);
					tangent = inverseT.MultR(tangent);
					tangent.Normalize();
					FbxDouble4 double4;
					double4.mData[0] = tangent.mData[0];
					double4.mData[1] = tangent.mData[1];
					double4.mData[2] = tangent.mData[2];
					double4.mData[3] = tangent.mData[3];
					tangentVector.push_back(double4);
				}
				else if (elementTangent->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
				{
					int id = elementTangent->GetIndexArray().GetAt(controlPointIndex);
					FbxVector4 tangent = elementTangent->GetDirectArray().GetAt(id);
					tangent = inverseT.MultR(tangent);
					tangent.Normalize();
					FbxDouble4 double4;
					double4.mData[0] = tangent.mData[0];
					double4.mData[1] = tangent.mData[1];
					double4.mData[2] = tangent.mData[2];
					double4.mData[3] = tangent.mData[3];
					tangentVector.push_back(double4);
				}
				else
				{
					assert(0);
				}

			}
			else if (elementTangent->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
			{

				if (elementTangent->GetReferenceMode() == FbxGeometryElement::eDirect)
				{
					FbxVector4 tangent = elementTangent->GetDirectArray().GetAt(controlPointIndex);
					tangent = inverseT.MultR(tangent);
					tangent.Normalize();
					FbxDouble4 double4;
					double4.mData[0] = tangent.mData[0];
					double4.mData[1] = tangent.mData[1];
					double4.mData[2] = tangent.mData[2];
					double4.mData[3] = tangent.mData[3];
					SortIndices(tangentVector, tangentIndices, double4, indiceTangent);
					
				}
				else if (elementTangent->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
				{
					int id = elementTangent->GetIndexArray().GetAt(controlPointIndex);
					FbxVector4 tangent = elementTangent->GetDirectArray().GetAt(id);
					tangent = inverseT.MultR(tangent);
					tangent.Normalize();
					FbxDouble4 double4;
					double4.mData[0] = tangent.mData[0];
					double4.mData[1] = tangent.mData[1];
					double4.mData[2] = tangent.mData[2];
					double4.mData[3] = tangent.mData[3];
					SortIndices(tangentVector, tangentIndices, double4, indiceTangent);
				}
				else
				{
					assert(0);
				}

			}
			else
			{
				assert(0);
			}
			if (elementBiNormal->GetMappingMode() == FbxGeometryElement::eByControlPoint)
			{
				controlPointBiNormal = true;
				if (elementBiNormal->GetReferenceMode() == FbxGeometryElement::eDirect)
				{
					FbxVector4 biNormal = elementBiNormal->GetDirectArray().GetAt(controlPointIndex);
					biNormal = inverseT.MultR(biNormal);
					biNormal.Normalize();
					FbxDouble4 double4;
					double4.mData[0] = biNormal.mData[0];
					double4.mData[1] = biNormal.mData[1];
					double4.mData[2] = biNormal.mData[2];
					double4.mData[3] = biNormal.mData[3];
					binormalVector.push_back(double4);
				}
				else if (elementBiNormal->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
				{
					int id = elementBiNormal->GetIndexArray().GetAt(controlPointIndex);
					FbxVector4 biNormal = elementBiNormal->GetDirectArray().GetAt(id);
					biNormal = inverseT.MultR(biNormal);
					biNormal.Normalize();
					FbxDouble4 double4;
					double4.mData[0] = biNormal.mData[0];
					double4.mData[1] = biNormal.mData[1];
					double4.mData[2] = biNormal.mData[2];
					double4.mData[3] = biNormal.mData[3];
					binormalVector.push_back(double4);
				}
				else
				{
					assert(0);
				}
			}
			else if (elementBiNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
			{

				if (elementBiNormal->GetReferenceMode() == FbxGeometryElement::eDirect)
				{
					FbxVector4 biNormal = elementBiNormal->GetDirectArray().GetAt(controlPointIndex);
					biNormal = inverseT.MultR(biNormal);
					biNormal.Normalize();
					FbxDouble4 double4;
					double4.mData[0] = biNormal.mData[0];
					double4.mData[1] = biNormal.mData[1];
					double4.mData[2] = biNormal.mData[2];
					double4.mData[3] = biNormal.mData[3];
					SortIndices(binormalVector, biNormalIndices, double4, indiceBinormal);
					
				}
				else if (elementBiNormal->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
				{
					int id = elementBiNormal->GetIndexArray().GetAt(controlPointIndex);
					FbxVector4 biNormal = elementBiNormal->GetDirectArray().GetAt(id);
					biNormal = inverseT.MultR(biNormal);
					biNormal.Normalize();
					FbxDouble4 double4;
					double4.mData[0] = biNormal.mData[0];
					double4.mData[1] = biNormal.mData[1];
					double4.mData[2] = biNormal.mData[2];
					double4.mData[3] = biNormal.mData[3];
					SortIndices(binormalVector, biNormalIndices, double4, indiceBinormal);
				}
				else
				{
					assert(0);
				}

			}
			else
			{
				assert(0);
			}



		}
	}
	if (controlPointUV)
	{
		uvIndices = positionIndices;
	}
	if (controlPointNormal)
	{
		normalIndices = positionIndices;
	}
	if (controlPointTangent)
	{
		tangentIndices = positionIndices;
	}
	if (controlPointBiNormal)
	{
		biNormalIndices = positionIndices;
	}

	
	unsigned int indexCounter = 0;
	Array<IndexStruct> allIndices;
	for (int i = 0; i < indexCount; i++)
	{
		IndexStruct indexStruct;
		indexStruct.p = posVector[positionIndices[i]];
		indexStruct.uv = uvVector[uvIndices[i]];
		indexStruct.n = normalVector[normalIndices[i]];
		indexStruct.t = tangentVector[tangentIndices[i]];
		indexStruct.b = binormalVector[biNormalIndices[i]];
		if (allIndices.find(indexStruct) == allIndices.end())
		{
			GOFile::VERTEX vertex;
			if (shouldSwitchAxisSystem)
			{
				vertex.posX = (float)indexStruct.p.mData[0];
				vertex.posY = (float)indexStruct.p.mData[1];
				vertex.posZ = -(float)indexStruct.p.mData[2];

				vertex.u = (float)indexStruct.uv.mData[0];
				vertex.v = -(float)indexStruct.uv.mData[1];

				vertex.normalX = (float)indexStruct.n.mData[0];
				vertex.normalY = (float)indexStruct.n.mData[1];
				vertex.normalZ = -(float)indexStruct.n.mData[2];

				vertex.tangentX = (float)indexStruct.t.mData[0];
				vertex.tangentY = (float)indexStruct.t.mData[1];
				vertex.tangentZ = -(float)indexStruct.t.mData[2];

				vertex.biNormalX = (float)indexStruct.b.mData[0];
				vertex.biNormalY = (float)indexStruct.b.mData[1];
				vertex.biNormalZ = -(float)indexStruct.b.mData[2];
				for (int j=0; j< m_blendshapes.size(); j++)
				{
					unsigned int cpIndex = positionIndices[i];
					for (int k = 0;k <m_morphIndices[j].size(); k++)
					{
						if (cpIndex == m_morphIndices[j][k].targetIndex)
						{
							m_morphIndices[j][k].sourceIndex = indexCounter;
							
						}
					}
				}

			}
			else
			{
				vertex.posX = (float)indexStruct.p.mData[0];
				vertex.posY = (float)indexStruct.p.mData[1];
				vertex.posZ = (float)indexStruct.p.mData[2];

				vertex.u = (float)indexStruct.uv.mData[0];
				vertex.v = (float)indexStruct.uv.mData[1];

				vertex.normalX = (float)indexStruct.n.mData[0];
				vertex.normalY = (float)indexStruct.n.mData[1];
				vertex.normalZ = (float)indexStruct.n.mData[2];

				vertex.tangentX = (float)indexStruct.t.mData[0];
				vertex.tangentY = (float)indexStruct.t.mData[1];
				vertex.tangentZ = (float)indexStruct.t.mData[2];

				vertex.biNormalX = (float)indexStruct.b.mData[0];
				vertex.biNormalY = (float)indexStruct.b.mData[1];
				vertex.biNormalZ = (float)indexStruct.b.mData[2];

			}
			if (skinningData)
			{
				

				for (int j = 0; j < (int)skinningData[positionIndices[i]].boneName.size(); j++)
				{

					
					vertex.boneIndex[j] = skinningData[positionIndices[i]].boneIndex[j];
					vertex.skinWeight[j] = skinningData[positionIndices[i]].skinWeight[j];
					if (j > 3)
					{
						break;
					}
				}


			}
			vertices.push_back(vertex);
			indices.push_back(indexCounter++);
			allIndices.push_back(indexStruct);
		}
		else
		{

			indices.push_back((unsigned int)allIndices.findIndex(indexStruct));


		}



	}

}