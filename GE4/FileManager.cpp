#include "pch.h"
#include "FileManager.h"


FileManager::FileManager()
{
}

void FileManager::Startup()
{
}

FbxScene* FileManager::GetScene()
{
	return nullptr;
}

void FileManager::GetRootNode()
{
}

void FileManager::Write()
{
}

void FileManager::GetNodes(FbxNode* node)
{
}

void FileManager::getMaterial()
{
}

void FileManager::getLight(FbxNode* lightNode)
{
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
//void FileManager::GetBlendShapes(FbxGeometry* geometry, fbxsdk::FbxScene* scene, FbxMesh* meshNode, GOFile::MESH& mesh)
//{
//	bool shouldSwitchAxisSystem = false;
//	FbxAxisSystem leftAxis(FbxAxisSystem::DirectX);
//
//	if (m_currentAxisSystem.GetCoorSystem() != leftAxis.GetCoorSystem())
//	{
//		shouldSwitchAxisSystem = true;
//	}
//
//	int blendShapeDeformerCount = geometry->GetDeformerCount(FbxDeformer::eBlendShape);
//	FbxAnimStack* stack = scene->GetSrcObject<FbxAnimStack>(0);
//	FbxTakeInfo* take = scene->GetTakeInfo(stack->GetName());
//
//	FbxTime timeInfo = take->mLocalTimeSpan.GetDuration();
//
//	m_fileinfo.animationLenght = (float)timeInfo.GetFrameCountPrecise();
//	int layerCount = stack->GetMemberCount<FbxAnimLayer>();
//	for (int bsd = 0; bsd < blendShapeDeformerCount; bsd++)
//	{
//		FbxBlendShape* blendShape = (FbxBlendShape*)geometry->GetDeformer(bsd, FbxDeformer::eBlendShape);
//		int blendShapeCount = blendShape->GetBlendShapeChannelCount();
//		/*m_blendshapes
//		m_morphTargets.resize(blendShapeCount);
//		m_morphKeyframes.resize(blendShapeCount);
//		m_morphVertices.resize(blendShapeCount);
//		m_morphIndices.resize(blendShapeCount);*/
//		for (int layer = 0; layer < layerCount; layer++)
//		{
//			FbxAnimLayer* animLayer = stack->GetMember<FbxAnimLayer>(layer);
//
//			for (int bs = 0; bs < blendShapeCount; bs++)
//			{
//				GOFile::MORPH_TARGET mt;
//				FbxBlendShapeChannel* channel = blendShape->GetBlendShapeChannel(bs);
//				
//				strcpy_s(mt.sourceName, mesh.meshName);
//				strcpy_s(mt.name, channel->GetName());
//				FbxAnimCurve* animCurve = geometry->GetShapeChannel(bsd, bs, animLayer);
//				if (animCurve)
//				{
//					int keyCount = animCurve->KeyGetCount();
//					/*m_morphKeyframes[bs].resize(keyCount);*/
//					for (int key = 0; key < keyCount; key++)
//					{
//						GOFile::MORPH_KEYFRAME kf;
//						kf.weight = animCurve->KeyGetValue(key) / 100.0f;
//						kf.timeStamp = (float)animCurve->KeyGetTime(key).GetFrameCountPrecise();
//					}
//
//					mt.morphKeyframeCount = (unsigned int)keyCount;
//				}
//				else
//				{
//					m_morphKeyframes.clear();
//				}
//				FbxShape* shape = channel->GetTargetShape(0);
//				FbxLayerElementArrayTemplate<FbxVector4>* narr = nullptr;
//				bool normalBool = shape->GetNormals(&narr);
//				FbxLayerElementArrayTemplate<FbxVector4>* tarr = nullptr;
//				bool tangentBool = shape->GetTangents(&tarr);
//				FbxLayerElementArrayTemplate<FbxVector4>* barr = nullptr;
//				bool biNormalsBool = shape->GetBinormals(&barr);
//				FbxLayerElementArrayTemplate<int>* nInd = nullptr;
//				bool normalIndiceseBool = shape->GetNormalsIndices(&nInd);
//				FbxLayerElementArrayTemplate<int>* tInd = nullptr;
//				bool tangentIndiceseBool = shape->GetTangentsIndices(&tInd);
//				FbxLayerElementArrayTemplate<int>* bInd = nullptr;
//				bool biNormalIndiceseBool = shape->GetBinormalsIndices(&bInd);
//				int* posIndices = shape->GetControlPointIndices();
//				FbxVector4* cp = shape->GetControlPoints();
//
//				//
//				FbxAMatrix inverseT;
//				inverseT.SetIdentity();
//				FbxAMatrix globalTransform;
//				globalTransform.SetIdentity();
//
//				//FbxAMatrix localMatrix = geometry->GetNode()->EvaluateLocalTransform();
//
//				//FbxNode* pParentNode = geometry->GetNode()->GetParent();
//				//FbxAMatrix parentMatrix = pParentNode->EvaluateLocalTransform();
//				//while ((pParentNode = pParentNode->GetParent()) != NULL)
//				//{
//				//	parentMatrix = pParentNode->EvaluateLocalTransform() * parentMatrix;
//				//}
//
//
//				//;// = meshNode->GetNode()->EvaluateGlobalTransform();
//				//globalTransform.SetIdentity();
//				//FbxAMatrix geoMat;
//				//geoMat.SetIdentity();
//				//geoMat.SetS(geometry->GetNode()->GetGeometricScaling(FbxNode::EPivotSet::eSourcePivot));
//				//geoMat.SetR(geometry->GetNode()->GetGeometricRotation(FbxNode::EPivotSet::eSourcePivot));
//				//geoMat.SetT(geometry->GetNode()->GetGeometricTranslation(FbxNode::EPivotSet::eSourcePivot));
//
//				//globalTransform = parentMatrix * localMatrix * geoMat;
//
//				//inverseT = globalTransform.Inverse();
//				//inverseT.Transpose();
//
//				//
//
//
//
//				std::map< unsigned int, unsigned int> controlTargets;
//				std::map< unsigned int, unsigned int> controlTargets2;
//				int c = 0;
//				std::vector<IndexStruct> totalIndices;
//				for (int i = 0; i < shape->GetControlPointIndicesCount(); i++)
//				{
//					IndexStruct indexStruct;
//					indexStruct.p = cp[posIndices[i]];
//					if (normalBool && normalIndiceseBool)
//					{
//						indexStruct.n = narr->GetAt(nInd->GetAt(i));
//					}
//					else
//					{
//						indexStruct.n.mData[0] = 0;
//						indexStruct.n.mData[1] = 0;
//						indexStruct.n.mData[2] = 0;
//					}
//					if (tangentBool && tangentIndiceseBool)
//					{
//						indexStruct.t = tarr->GetAt(tInd->GetAt(i));
//					}
//					else
//					{
//						indexStruct.t.mData[0] = 0;
//						indexStruct.t.mData[1] = 0;
//						indexStruct.t.mData[2] = 0;
//					}
//					if (biNormalsBool && biNormalIndiceseBool)
//					{
//						indexStruct.b = barr->GetAt(bInd->GetAt(i));
//					}
//					else
//					{
//						indexStruct.b.mData[0] = 0;
//						indexStruct.b.mData[1] = 0;
//						indexStruct.b.mData[2] = 0;
//					}
//
//
//					/*unsigned int correctIndex = posIndices[i];*/
//					auto it = std::find(totalIndices.begin(), totalIndices.end(), indexStruct);
//					/*X::MORPH_INDEX morphIndex;*/
//
//					if (it == totalIndices.end())
//					{
//						totalIndices.push_back(indexStruct);
//						X::MORPH_VERTEX vertex;
//
//						if (global)
//						{
//							FbxDouble4 transformedT = globalTransform.MultT(indexStruct.p.mData);
//							indexStruct.p.mData[0] = transformedT[0];
//							indexStruct.p.mData[1] = transformedT[1];
//							indexStruct.p.mData[2] = transformedT[2];
//
//							FbxDouble4 transformedN = inverseT.MultR(indexStruct.n.mData);
//							indexStruct.n.mData[0] = transformedN[0];
//							indexStruct.n.mData[1] = transformedN[1];
//							indexStruct.n.mData[2] = transformedN[2];
//
//							FbxDouble4 transformedTan = inverseT.MultR(indexStruct.t.mData);
//							indexStruct.t.mData[0] = transformedTan[0];
//							indexStruct.t.mData[1] = transformedTan[1];
//							indexStruct.t.mData[2] = transformedTan[2];
//
//							FbxDouble4 transformedBN = inverseT.MultR(indexStruct.b.mData);
//							indexStruct.b.mData[0] = transformedBN[0];
//							indexStruct.b.mData[1] = transformedBN[1];
//							indexStruct.b.mData[2] = transformedBN[2];
//						}
//
//
//						if (shouldSwitchAxisSystem)
//						{
//							vertex.posX = (float)indexStruct.p.mData[0];
//							vertex.posY = (float)indexStruct.p.mData[1];
//							vertex.posZ = -(float)indexStruct.p.mData[2];
//							vertex.normalX = (float)indexStruct.n.mData[0];
//							vertex.normalY = (float)indexStruct.n.mData[1];
//							vertex.normalZ = -(float)indexStruct.n.mData[2];
//							vertex.tangentX = (float)indexStruct.t.mData[0];
//							vertex.tangentY = (float)indexStruct.t.mData[1];
//							vertex.tangentZ = -(float)indexStruct.t.mData[2];
//							vertex.biNormalX = (float)indexStruct.b.mData[0];
//							vertex.biNormalY = (float)indexStruct.b.mData[1];
//							vertex.biNormalZ = -(float)indexStruct.b.mData[2];
//						}
//						else
//						{
//							vertex.posX = (float)indexStruct.p.mData[0];
//							vertex.posY = (float)indexStruct.p.mData[1];
//							vertex.posZ = (float)indexStruct.p.mData[2];
//							vertex.normalX = (float)indexStruct.n.mData[0];
//							vertex.normalY = (float)indexStruct.n.mData[1];
//							vertex.normalZ = (float)indexStruct.n.mData[2];
//							vertex.tangentX = (float)indexStruct.t.mData[0];
//							vertex.tangentY = (float)indexStruct.t.mData[1];
//							vertex.tangentZ = (float)indexStruct.t.mData[2];
//							vertex.biNormalX = (float)indexStruct.b.mData[0];
//							vertex.biNormalY = (float)indexStruct.b.mData[1];
//							vertex.biNormalZ = (float)indexStruct.b.mData[2];
//						}
//						m_morphVertices[bs].push_back(vertex);
//
//
//						controlTargets.insert({ (unsigned int)posIndices[i], c++ });
//					}
//					else
//					{
//
//						/*controlTargets2.insert({ (unsigned int)posIndices[i] , std::distance(totalIndices.begin(), it) });*/
//					}
//				}
//
//
//				X::MORPH_INDEX morphIndex;
//				std::vector<unsigned int> tempTest;
//				for (int i = 0; i < m_blendShapeC.size(); i++)
//				{
//					auto it = std::find(tempTest.begin(), tempTest.end(), m_blendShapeC[i].controlPointIndex);
//					if (it == tempTest.end())
//					{
//						if (controlTargets.find(m_blendShapeC[i].controlPointIndex) != controlTargets.end())
//						{
//							morphIndex.targetIndex = controlTargets[m_blendShapeC[i].controlPointIndex];
//							morphIndex.sourceIndex = m_blendShapeC[i].sourceIndex;
//							m_morphIndices[bs].push_back(morphIndex);
//
//						}
//						tempTest.push_back(m_blendShapeC[i].controlPointIndex);
//					}
//
//				}
//				m_morphIndices[bs] = m_morphIndices[bs];
//				m_morphVertices[bs] = m_morphVertices[bs];
//				m_morphTargets[bs].indexCount = (unsigned int)m_morphIndices[bs].size();
//				m_morphTargets[bs].vertexCount = (unsigned int)m_morphVertices[bs].size();
//			}
//
//		}
//
//
//	}
//}
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
	FbxAxisSystem m_currentAxisSystem;
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

					strcpy_s(vertex.boneName[j], skinningData[positionIndices[i]].boneName[j]);
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