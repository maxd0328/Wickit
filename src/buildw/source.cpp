#include "buildw/source.h"
#include "include/exception.h"
#include "include/strutil.h"

using namespace wckt;
using namespace wckt::build;

SourceTable::SourceTable(const base::URL& url)
: url(url)
{
	this->source = url.read(true);
	
	this->lines.push_back(0);
	
	size_t start = this->source.find('\n');
	while(start != std::string::npos)
	{
		this->lines.push_back(start + 1);
		start = this->source.find('\n', start + 1);
	}
}

base::URL SourceTable::getURL() const
{
	return this->url;
}

const std::string& SourceTable::getSource() const
{
	return this->source;
}

/* Returns { row, col }, both starting from 1, not 0, using an iterative binary search */
SourceTable::coords1_t SourceTable::getCoords(size_t pos) const
{
	if(pos >= this->source.length())
	{
		size_t last = this->lines[this->lines.size() - 1];
		return { (uint32_t) this->lines.size(), (uint32_t) (this->source.length() - last + 1) };
	}
	
	uint32_t lo = 0, hi = this->lines.size() - 1;
	
	while(lo < hi)
	{
		uint32_t index = (lo + hi) / 2;
		size_t cur = this->lines[index], next = this->lines[index + 1];
		if(cur <= pos && next > pos)
			return { index + 1, (uint32_t) (pos - cur + 1) };
		else if(pos == next)
			return { index + 2, 1 };
		
		if(pos < cur)
			hi = index - 1;
		else
			lo = index + 1;
	}
	
	size_t start = this->lines[lo];
	return { lo + 1, (uint32_t) (pos - start + 1) };
}

std::string SourceTable::getLine(uint32_t row) const
{
	row--; // make zero-oriented
	if(row >= this->lines.size())
		return "";
	
	if(row == this->lines.size() - 1)
		return this->source.substr(this->lines[row]);
	
	return this->source.substr(this->lines[row], this->lines[row + 1] - this->lines[row] - 1);
}

IntrasourceContextLayer::IntrasourceContextLayer(err::PTR_ErrorContextLayer ptr, size_t position, size_t length, std::shared_ptr<SourceTable> sourceTable)
: ErrorContextLayer(std::move(ptr)), position(position), length(length), sourceTable(sourceTable)
{}

size_t IntrasourceContextLayer::getPosition() const
{
	return this->position;
}

size_t IntrasourceContextLayer::getLength() const
{
	return this->length;
}

std::shared_ptr<SourceTable> IntrasourceContextLayer::getSourceTable() const
{
	return this->sourceTable;
}

/**
 * Returns an error message that looks something like this:
 * 
 * ["file://path/to/file.wckt":10:6] error in source: Could not resolve 'value'
 *  10 | this.value++;
 *     |      ^~~~~
 */
std::string IntrasourceContextLayer::what() const
{
	SourceTable::coords1_t coords = this->sourceTable->getCoords(this->position);
	std::string header = "[\"" + this->sourceTable->getURL().toString() + "\":" + std::to_string(coords.row)
		+ ":" + std::to_string(coords.col) + "] error in source: " + this->getNext()->what();
		
	std::string lineNo = std::to_string(coords.row);
	std::string line = this->sourceTable->getLine(coords.row);
	size_t originalLength = line.length();
	ltrim(line);
	size_t diff = originalLength - line.length();
	if(coords.col - 1 > diff)
		coords.col -= diff;
	else coords.col = 1;
	rtrim(line);
	
	size_t tabPosition;
	while((tabPosition = line.find('\t')) != std::string::npos)
	{
		line = line.substr(0, tabPosition) + "    " + line.substr(tabPosition + 1);
		if(coords.col - 1 > tabPosition)
			coords.col += 3;
	}
	
	std::string problemLine = " " + lineNo + " | " + line;
	std::string pointerLine = std::string(lineNo.length() + 2, ' ') + "| " + std::string(coords.col - 1, ' ') + '^' + std::string(this->length - 1, '~');
	
	return header + "\n" + problemLine + "\n" + pointerLine;
}
