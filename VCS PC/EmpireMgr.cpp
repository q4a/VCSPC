#include "StdAfx.h"
#include "EmpireMgr.h"

#include "ModelInfo.h"
#include "World.h"

CEmpire			CEmpireManager::m_empire[NUM_EMPIRES];
tEmpireType		CEmpireManager::m_empireType[NUM_EMPIRE_TYPES];

void CEmpireManager::Initialise()
{
	if ( FILE* hFile = CFileMgr::OpenFile("DATA\\EMPIRES.DAT", "r") )
	{
		enum
		{
			SECTION_NOTHING,
			SECTION_PLACES,
			SECTION_TYPES,
			SECTION_DATA
		} curFileSection = SECTION_NOTHING;
		CAtomicModelInfo*		pCurrentModelForDataParsing = nullptr;

		while ( const char* pLine = CFileLoader::LoadLine(hFile) )
		{
			if ( pLine[0] && pLine[0] != '#' )
			{
				switch ( curFileSection )
				{
				case SECTION_NOTHING:
					{
						if ( SECTION4(pLine, 'p', 'l', 'a', 'c') )
							curFileSection = SECTION_PLACES;
						else if ( SECTION4(pLine, 't', 'y', 'p', 'e') )
							curFileSection = SECTION_TYPES;
						else if ( SECTION4(pLine, 'd', 'a', 't', 'a') )
							curFileSection = SECTION_DATA;
						break;
					}
				case SECTION_PLACES:
					{
						if ( SECTION3(pLine, 'e', 'n', 'd') )
							curFileSection = SECTION_NOTHING;
						else
						{
							CSimpleTransform		tempTransform;
							int						nIndex;

							sscanf(pLine, "%d %f %f %f %f", &nIndex, &tempTransform.m_translate.x, &tempTransform.m_translate.y, &tempTransform.m_translate.z, &tempTransform.m_heading);
							assert(nIndex >= 0 && nIndex < NUM_EMPIRES);

							tempTransform.m_heading *= static_cast<float>(DEG_TO_RAD);
							m_empire[nIndex].SetTransform(tempTransform);
							m_empire[nIndex].MarkAsDefined();
						}
						break;
					}
				case SECTION_TYPES:
					{
						if ( SECTION3(pLine, 'e', 'n', 'd') )
							curFileSection = SECTION_NOTHING;
						else
						{
							char			cNormalName[24], cDamagedName[24];
							int				nIndex;
							int				nNormalModel, nDamagedModel;
							char			cType;

							sscanf(pLine, "%d %c %s %s", &nIndex, &cType, cNormalName, cDamagedName);
							assert(nIndex >= 0 && nIndex < NUM_EMPIRE_TYPES);

							CModelInfo::GetModelInfo(cNormalName, &nNormalModel);
							CModelInfo::GetModelInfo(cDamagedName, &nDamagedModel);
							m_empireType[nIndex].nNormalBuilding[toupper(cType) - 'A'] = nNormalModel;
							m_empireType[nIndex].nDamagedBuilding[toupper(cType) - 'A'] = nDamagedModel;

							// Collisions are MINE, bitch!
							// EDIT: Crashes
							//CModelInfo::ms_modelInfoPtrs[nNormalModel]->bDoWeOwnTheColModel = false;
							//CModelInfo::ms_modelInfoPtrs[nDamagedModel]->bDoWeOwnTheColModel = false;
						}
						break;
					}
				case SECTION_DATA:
					{
						if ( SECTION3(pLine, 'e', 'n', 'd') )
						{
							if ( pCurrentModelForDataParsing )
								pCurrentModelForDataParsing->GetEmpireData()->ReduceContainerSize();
							curFileSection = SECTION_NOTHING;
						}
						else
						{
							if ( pLine[0] == '%' )
							{
								if ( pCurrentModelForDataParsing )
									pCurrentModelForDataParsing->GetEmpireData()->ReduceContainerSize();
	
								pCurrentModelForDataParsing = CModelInfo::GetModelInfo(pLine+2)->AsAtomicModelInfoPtr();
								pCurrentModelForDataParsing->InitEmpireData();
							}
							else
							{		
								CSimpleTransform		tempTransform;
								unsigned int			nIndex;

								assert(pCurrentModelForDataParsing);
								sscanf(pLine, "%d %f %f %f %f", &nIndex, &tempTransform.m_translate.x, &tempTransform.m_translate.y, &tempTransform.m_translate.z, &tempTransform.m_heading);

								pCurrentModelForDataParsing->GetEmpireData()->AddEntry(static_cast<unsigned short>(nIndex), tempTransform);
							}
						}
						break;
					}
				}

			}
		}


		CFileMgr::CloseFile(hFile);
	}
}

void CEmpireManager::Process()
{
	/*for ( int i = 0; i < NUM_EMPIRES; ++i )
	{
		if ( m_empire[i].IsDefined() )
		{
			//if ( !m_empire[i].IsPlaced() )
				
				//m_empire[i].Place();
		}
	}*/
}

void CEmpireBuildingData::ReduceContainerSize()
{
	for ( auto it = m_buildingData.begin(); it != m_buildingData.end(); it++ )
		it->second.shrink_to_fit();
}

void CEmpire::Place()
{
	if ( m_nType == -1 )
		return;

	int				nModelIndex = CEmpireManager::GetInfo(m_nType)->nNormalBuilding[m_nScale];
	CBuilding*		pEmpireBuilding = new CBuilding;

	m_pBuilding = pEmpireBuilding;
	m_bPlaced = true;

	// Temp
	pEmpireBuilding->SetModelIndexNoCreate(nModelIndex);
	pEmpireBuilding->SetCoords(m_placement.m_translate);
	pEmpireBuilding->SetHeading(m_placement.m_heading);

	CColModel*		pColModel = CModelInfo::ms_modelInfoPtrs[nModelIndex]->GetColModel();
	if ( pColModel )
	{
		unsigned char	bIndex = pColModel->level;
		if ( bIndex )
		{
			CRect		boundingRect = pEmpireBuilding->GetBoundRect();
			CRect*		pModelRect;
			// Pure laziness
			_asm
			{
				movzx		eax, bIndex
				push		eax
				mov			eax, 410800h
				call		eax
				add			esp, 4
				mov			pModelRect, eax
			}
			if ( boundingRect.x1 < pModelRect->x1 )
				pModelRect->x1 = boundingRect.x1;
			if ( boundingRect.x2 > pModelRect->x2 )
				pModelRect->x2 = boundingRect.x2;
			if ( boundingRect.y2 < pModelRect->y2 )
				pModelRect->y2 = boundingRect.y2;
			if ( boundingRect.y1 > pModelRect->y1 )
				pModelRect->y1 = boundingRect.y1;
		}
	}

	CWorld::Add(pEmpireBuilding);
}

void CEmpire::Remove()
{
	if ( m_pBuilding )
	{
		CWorld::Remove(m_pBuilding);

		delete m_pBuilding;
		m_pBuilding = nullptr;
	}
	m_bPlaced = false;
}