#pragma once

#include "include/definitions.h"
#include "base/url.h"
#include "base/context.h"
#include "symbol/locator.h"
#include "buildw/source.h"

namespace wckt::build
{
	/* Forward declaration */
	class Token;
	
	typedef struct
	{
		base::URL url;
		sym::Locator pckg;
	} asset_info_t;
	
	typedef struct
	{
		std::shared_ptr<SourceTable> sourceTable;
		std::shared_ptr<std::vector<Token>> tokenSequence;
		// ...
	} build_info_t;
	
	class BuildContext
	{
		public:
			static const uint32_t npos;
		
		private:
			std::shared_ptr<base::EngineContext> context;
			moduleid_t moduleID;
			std::map<uint32_t, asset_info_t> assets;
			std::map<uint32_t, std::unique_ptr<build_info_t>> buildInfo;
			
			uint32_t nextAssetID;
			
		public:
			BuildContext(std::shared_ptr<base::EngineContext> context, ARG_moduleid_t moduleID);
			~BuildContext() = default;
			
			const base::EngineContext& getContext() const;
			uint32_t getContextID() const;
			RET_moduleid_t getModuleID() const;
			std::vector<uint32_t> getAssetIDs() const;
			uint32_t findAssetID(const base::URL& url) const;
			
			asset_info_t getAsset(uint32_t assetID) const;
			build_info_t& getBuildInfo(uint32_t assetID) const;
			
			uint32_t addAsset(const base::URL& url, const sym::Locator& pckg);
	};
	
	namespace services
	{
		void buildFromContext(const BuildContext& context, err::ErrorSentinel* parentSentinel);
	};
}
