/*
 * Copyright deipi.com LLC and contributors. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "TypesFieldProcessor.h"
#include <xapian/query.h>


NumericFieldProcessor::NumericFieldProcessor(const std::string &prefix_): prefix(prefix_) {
	LOG(this, "PREFIX NUMERIC %s\n", prefix.c_str());
}


Xapian::Query
NumericFieldProcessor::operator()(const std::string &str)
{
	LOG(this, "Numeric FP %s!!\n", str.c_str());
	std::string serialise = serialise_numeric(str);
	if (serialise.size() == 0) {
		throw Xapian::QueryParserError("Didn't understand numeric specification '" + str + "'");
	}
	return Xapian::Query(prefix + serialise);
}


LatLongFieldProcessor::LatLongFieldProcessor(const std::string &prefix_): prefix(prefix_) {}


Xapian::Query
LatLongFieldProcessor::operator()(const std::string &str)
{
	LOG(NULL, "LatLong FP!!\n");
	std::string serialise = serialise_geo(str);
	if (serialise.size() == 0) {
		throw Xapian::QueryParserError("Didn't understand LatLongs specification '" + str + "'");
	}
	return Xapian::Query(prefix + serialise);
}


LatLongDistanceFieldProcessor::LatLongDistanceFieldProcessor(const std::string &prefix_): prefix(prefix_) {}


Xapian::Query
LatLongDistanceFieldProcessor::operator()(const std::string &str)
{
	return Xapian::Query("Procesing");
}


BooleanFieldProcessor::BooleanFieldProcessor(const std::string &prefix_): prefix(prefix_) {}


Xapian::Query BooleanFieldProcessor::operator()(const std::string &str)
{
	LOG(NULL, "Boolean FP!!\n");
	std::string serialise = serialise_bool(str);
	if (serialise.size() == 0) {
		throw Xapian::QueryParserError("Didn't understand bool specification '" + str + "'");
	}
	return Xapian::Query(prefix + serialise);
}


DateFieldProcessor::DateFieldProcessor(const std::string &prefix_): prefix(prefix_) {}


Xapian::Query DateFieldProcessor::operator()(const std::string &str)
{
	LOG(NULL, "Date FP!!\n");
	std::string serialise = serialise_date(str);
	if (serialise.size() == 0) {
		throw Xapian::QueryParserError("Didn't understand date specification '" + str + "'");
	}
	return Xapian::Query(prefix + serialise);
}


DateTimeValueRangeProcessor::DateTimeValueRangeProcessor(Xapian::valueno slot_, const std::string prefix_): valno(slot_), prefix(prefix_) {}


Xapian::valueno
DateTimeValueRangeProcessor::operator()(std::string &begin, std::string &end)
{
	std::string buf;
	LOG(this,"Inside of DateTimeValueRangeProcessor\n");
	
	if(begin.size() != 0) {
		buf = prefix + serialise_date(begin);
		if(buf != "") {
			begin.assign(buf.c_str(), buf.size());
			LOG(this,"Serialise of begin %s\n",buf.c_str());
		}
		else return Xapian::BAD_VALUENO;
	}
	buf = "";
	
	if(end.size() != 0) {
		buf = prefix + serialise_date(end);
		if(buf != "") {
			end.assign(buf.c_str(), buf.size());
			LOG(this,"Serialise of end %s\n",std::string(prefix + buf).c_str());
		}
		else return Xapian::BAD_VALUENO;
	}
	return valno;
}