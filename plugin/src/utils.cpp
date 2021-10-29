/*
 * Copyright (c) 2021 Alessandro De Santis
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "utils.h"


void append_uint32(uint32_t v, std::vector<uint8_t> &buffer)
{
	buffer.push_back( v        & 0xFF);
	buffer.push_back((v >>  8) & 0xFF);
	buffer.push_back((v >> 16) & 0xFF);
	buffer.push_back((v >> 24) & 0xFF);
}

uint32_t read_uint32(size_t &pos, std::vector<uint8_t> &buffer)
{
//TODO: check size
	uint32_t v = 0;
	v =      buffer[pos];		++pos;
	v = v | (buffer[pos] << 8); ++pos;
	v = v | (buffer[pos] << 8); ++pos;
	v = v | (buffer[pos] << 8); ++pos;

	return v;
}

void append_float32(float v, std::vector<uint8_t> &buffer)
{
	auto *pv = (uint8_t *)&v;
	buffer.push_back(*pv); ++pv;
	buffer.push_back(*pv); ++pv;
	buffer.push_back(*pv); ++pv;
	buffer.push_back(*pv);
}

float read_float32(size_t &pos, std::vector<uint8_t> &buffer)
{
//TODO: check size
	float v = 0;
	uint8_t *pv = (uint8_t *)&v;
	*pv = buffer[pos]; ++pos; ++pv;
	*pv = buffer[pos]; ++pos; ++pv;
	*pv = buffer[pos]; ++pos; ++pv;
	*pv = buffer[pos]; ++pos;

	return v;
}

void append_string(std::string v, std::vector<uint8_t> &buffer)
{
	append_uint32(v.size(), buffer);
	for (size_t i = 0; i < v.size(); ++i)
	{
		buffer.push_back(v[i]);
	}
}

std::string read_string(size_t &pos, std::vector<uint8_t> &buffer)
{
//TODO: check size
	uint32_t len = read_uint32(pos, buffer); 
	std::string s((const char *)&buffer.data()[pos], len);
	pos += len;
	return s;	
}
