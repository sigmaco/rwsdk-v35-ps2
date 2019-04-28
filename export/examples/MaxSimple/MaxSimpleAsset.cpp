// MaxSimpleAsset.cpp: implementation of the MaxSimpleAsset class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MaxSimpleAsset.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MaxSimpleAsset::MaxSimpleAsset()
{

}

MaxSimpleAsset::~MaxSimpleAsset()
{

}

RwCommError 
MaxSimpleAsset::SetName(const string &name)
{
    m_name = name;
    return RWCOMM_SUCCESS;
}

RwCommError 
MaxSimpleAsset::GetName(string &name) const
{
    name = m_name;
    return RWCOMM_SUCCESS;
}

RwCommError 
MaxSimpleAsset::GetEnable(bool &bEnable) const
{
    bEnable = true;
    return RWCOMM_SUCCESS;
}

RwCommError 
MaxSimpleAsset::GetAssetScope(AssetScope &scope) const
{
    scope = ASSET;
    return RWCOMM_SUCCESS;
}

RwCommError 
MaxSimpleAsset::SetAssetTemplateName(const string &templateName)
{
    m_templateName = templateName;
    return RWCOMM_SUCCESS;
}

RwCommError 
MaxSimpleAsset::GetAssetTemplateName(string &templateName) const
{
    templateName = m_templateName;
    return RWCOMM_SUCCESS;
}
