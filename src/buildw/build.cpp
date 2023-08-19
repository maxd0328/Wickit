#include "buildw/build.h"
#include "buildw/tokenizer.h"
#include "buildw/parser.h"
#include "include/exception.h"

using namespace wckt;
using namespace wckt::build;

const uint32_t BuildContext::npos = (uint32_t) -1;

BuildContext::BuildContext(std::shared_ptr<base::EngineContext> context, ARG_moduleid_t moduleID)
: context(context), moduleID(moduleID), nextAssetID(0)
{}

BuildContext::BuildContext(const BuildContext& src)
: context(src.context), moduleID(src.moduleID), assets(src.assets), nextAssetID(src.nextAssetID)
{
	for(const auto& entry : src.buildInfo)
		this->buildInfo.insert(std::pair(entry.first, std::make_unique<build_info_t>(*entry.second)));
}

BuildContext& BuildContext::operator=(const BuildContext& src)
{
	if(&src == this)
		return *this;
	
	this->context = src.context;
	this->moduleID = src.moduleID;
	this->nextAssetID = src.nextAssetID;
	this->assets = src.assets;
	this->buildInfo.clear();
	for(const auto& entry : src.buildInfo)
		this->buildInfo.insert(std::pair(entry.first, std::make_unique<build_info_t>(*entry.second)));
	return *this;
}

const base::EngineContext& BuildContext::getContext() const
{
	return *this->context;
}

uint32_t BuildContext::getContextID() const
{
	return this->context->getContextID();
}

RET_moduleid_t BuildContext::getModuleID() const
{
	return this->moduleID;
}

std::vector<uint32_t> BuildContext::getAssetIDs() const
{
	std::vector<uint32_t> keys;
	for(const auto& asset : this->assets)
		keys.push_back(asset.first);
	return keys;
}

uint32_t BuildContext::findAssetID(const base::URL& url) const
{
	for(const auto& asset : this->assets)
	{
		if(asset.second.url == url)
			return asset.first;
	}
	return npos;
}

asset_info_t BuildContext::getAsset(uint32_t assetID) const
{
	if(assetID == npos)
		throw BadArgumentError("npos is not a valid asset ID");
	if(this->assets.find(assetID) == this->assets.end())
		throw ElementNotFoundError("No such asset with ID " + std::to_string(assetID));
	return this->assets.at(assetID);
}

build_info_t& BuildContext::getBuildInfo(uint32_t assetID) const
{
	if(assetID == npos)
		throw BadArgumentError("npos is not a valid asset ID");
	if(this->buildInfo.find(assetID) == this->buildInfo.end())
		throw ElementNotFoundError("No such asset with ID " + std::to_string(assetID));
	return *this->buildInfo.at(assetID);
}

uint32_t BuildContext::addAsset(const base::URL& url, const sym::Locator& pckg)
{
	if(findAssetID(url) != npos)
		throw BadArgumentError("Asset with this URL already exists");
	
	uint32_t assetID = nextAssetID++;
	
	this->assets[assetID] = { url, pckg };
	this->buildInfo[assetID] = std::make_unique<build_info_t>();
	return assetID;
}

void services::buildFromContext(const BuildContext& context, err::ErrorSentinel* parentSentinel)
{
	std::vector<uint32_t> assetIDs = context.getAssetIDs();
	for(uint32_t assetID : assetIDs)
	{
		asset_info_t asset = context.getAsset(assetID);
		build_info_t& buildInfo = context.getBuildInfo(assetID);
		err::ErrorSentinel sentinel(parentSentinel, err::ErrorSentinel::COLLECT, err::ErrorSentinel::NO_CONTEXT_FN);
		
		sentinel.guard<IOError>([&buildInfo, &asset](err::ErrorSentinel&) {
			buildInfo.sourceTable = std::make_shared<SourceTable>(asset.url);
		});
		if(sentinel.hasErrors())
			continue;
		
		services::tokenize(buildInfo, &sentinel);
		
		for(const auto& token : *buildInfo.tokenSequence)
			std::cout << token.toString() << std::endl;
		
		services::parse(buildInfo, &sentinel);
		// ...
	}
}
