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
	
	class SourceSegment
	{
		private:
			size_t position;
			size_t length;
		
		public:
			SourceSegment();
			SourceSegment(size_t position, size_t length);
			virtual ~SourceSegment() = default;
			
			size_t getPosition() const;
			size_t getLength() const;
			
			SourceSegment operator|(const SourceSegment& other) const;
			SourceSegment after() const;
	};
	
	class IntrasourceContextLayer : public err::ErrorContextLayer
	{
		private:
			SourceSegment segment;
			std::shared_ptr<SourceTable> sourceTable;
			
		public:
			IntrasourceContextLayer(err::PTR_ErrorContextLayer ptr, const SourceSegment& segment, std::shared_ptr<SourceTable> sourceTable);
			~IntrasourceContextLayer() = default;
			
			SourceSegment getSegment() const;
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
