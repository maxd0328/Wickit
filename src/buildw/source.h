#pragma once

#include "include/definitions.h"
#include "base/url.h"
#include "error/error.h"

namespace wckt::build
{
	class SourceTable
	{
		public:
			typedef struct 
			{
				uint32_t row;
				uint32_t col;
			} coords1_t;
		
		private:
			base::URL url;
			std::string source;
			std::vector<size_t> lines;
			
		public:
			SourceTable(const base::URL& url);
			~SourceTable() = default;
			
			base::URL getURL() const;
			const std::string& getSource() const;
			
			coords1_t getCoords(size_t pos) const;
			/* Row starting from 1 */
			std::string getLine(uint32_t row) const;
	};
	
	class IntrasourceContextLayer : public err::ErrorContextLayer
	{
		private:
			size_t position;
			size_t length;
			std::shared_ptr<SourceTable> sourceTable;
			
		public:
			IntrasourceContextLayer(err::PTR_ErrorContextLayer ptr, size_t position, size_t length, std::shared_ptr<SourceTable> sourceTable);
			~IntrasourceContextLayer() = default;
			
			size_t getPosition() const;
			size_t getLength() const;
			std::shared_ptr<SourceTable> getSourceTable() const;
			
			std::string what() const override;
	};
	
	class IntersourceContextLayer : public err::ErrorContextLayer
	{
		private:
			
		public:
			IntersourceContextLayer(std::unique_ptr<IntrasourceContextLayer> ptr);
			~IntersourceContextLayer() = default;
	};
}
