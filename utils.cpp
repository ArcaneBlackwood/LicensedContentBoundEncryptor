#include "main.hpp"

#include <ctype.h>
#include <string>
#include <cassert>

bool strequal(const char* a, const char* b) {
	for (size_t i = 0; i < 256 && tolower(a[i]) == b[i]; i++)
		if (a[i] == 0) return true;
	return false;
}
std::shared_ptr<char[]> getFileName(const char* fullPath) {
	size_t lastSlash = -1;
	size_t inputSize = 0;
	for (; inputSize < MAX_FILE_PATH_SIZE && fullPath[inputSize] != 0; inputSize++)
		if (fullPath[inputSize] == '/' || fullPath[inputSize] == '\\')
			lastSlash = inputSize + 1;
	assert(inputSize != MAX_FILE_PATH_SIZE);

	size_t outputSize = (lastSlash == -1) ? inputSize : inputSize - lastSlash;
	std::shared_ptr<char[]> output = std::make_shared<char[]>(outputSize+1);

	if (lastSlash == -1) lastSlash = 0;
	for (size_t iIn = lastSlash, iOut = 0; iOut < outputSize; iIn++, iOut++)
		output[iOut] = fullPath[iIn];
	output[outputSize] = 0;

	return output;
}
std::shared_ptr<char[]> suffixFileName(const char* string, const char* suffix) {
	size_t lastPeriod = -1;
	size_t inputSize = 0;
	for (; inputSize < MAX_FILE_PATH_SIZE && string[inputSize] != 0; inputSize++)
		if (string[inputSize] == '.')
			lastPeriod = inputSize;
	assert(inputSize != MAX_FILE_PATH_SIZE);
	if (lastPeriod == -1) lastPeriod = inputSize;

	size_t suffixSize = 0;
	for (; suffixSize < 256 && suffix[suffixSize] != 0; suffixSize++);
	assert(suffixSize != 256);

	size_t outputSize = inputSize + suffixSize;
	size_t extSize = inputSize - lastPeriod;
	std::shared_ptr<char[]> output = std::make_shared<char[]>(outputSize+1);

	for (size_t iSO = 0; iSO < lastPeriod; iSO++)
		output[iSO] = string[iSO]; //Original file name
	for (size_t iO = lastPeriod, iX = 0; iX < suffixSize; iX++, iO++)
		output[iO] = suffix[iX]; //Suffix file name
	for (size_t iO = lastPeriod + suffixSize, iS = lastPeriod; iS < inputSize; iS++, iO++)
		output[iO] = string[iS]; //Original file prefix
	output[outputSize] = 0;

	return output;
}
std::shared_ptr<char[]> splitString(const char* string, char seperator, size_t& offset) {
	if (string[offset] == 0) return nullptr;
	size_t start = offset;
	while (string[offset] != seperator && string[offset] != 0)
		offset++;
	size_t size = offset - start;
	if (string[offset] == seperator) offset++;
	std::shared_ptr<char[]> stringPart = std::make_shared<char[]>(size + 1);
	for (size_t i = 0; i < size; i++, start++)
		stringPart[i] = string[start];
	stringPart[size] = 0;
	return stringPart;
}



std::shared_ptr<char[]> ensureFileExtension(const char* input, const char* defaultExt) {
	size_t lastPeriod = -1;
	size_t inputSize = 0;
	for (; inputSize < MAX_FILE_PATH_SIZE && input[inputSize] != 0; inputSize++)
		if (input[inputSize] == '.')
			lastPeriod = inputSize;
	assert(inputSize != MAX_FILE_PATH_SIZE);

	size_t extSize = 0;
	if (lastPeriod == -1) {
		for (; extSize < 256 && defaultExt[extSize] != 0; extSize++);
		assert(extSize != 256);
	}
	size_t outputSize = inputSize + extSize;
	std::shared_ptr<char[]> output = std::make_shared<char[]>(outputSize+1);

	for (size_t i = 0; i < inputSize; i++)
		output[i] = input[i];
	if (lastPeriod == -1) for (size_t iE = 0, iO = inputSize; iE < extSize; iE++, iO++)
		output[iO] = defaultExt[iE];
	output[outputSize] = 0;

	return output;
}
std::shared_ptr<char[]> appendExtension(const char* string, const char* ext) {
	size_t inputSize = 0;
	for (; inputSize < MAX_FILE_PATH_SIZE && string[inputSize] != 0; inputSize++);
	assert(inputSize != MAX_FILE_PATH_SIZE);

	size_t extSize = 0;
	for (; extSize < 256 && ext[extSize] != 0; extSize++);
	assert(extSize != 256);

	size_t outputSize = inputSize + extSize;
	std::shared_ptr<char[]> output = std::make_shared<char[]>(outputSize+1);

	for (size_t i = 0; i < inputSize; i++)
		output[i] = string[i];
	for (size_t iE = 0, iO = inputSize; iE < extSize; iE++, iO++)
		output[iO] = ext[iE];
	output[outputSize] = 0;

	return output;
}
std::shared_ptr<char[]> removeExtension(const char* string, const char* ext) {
	size_t lastPeriod = -1;
	size_t inputSize = 0;
	for (; inputSize < MAX_FILE_PATH_SIZE && string[inputSize] != 0; inputSize++)
		if (string[inputSize] == '.')
			lastPeriod = inputSize;
	assert(inputSize != MAX_FILE_PATH_SIZE);

	size_t extSize = 0;
	if (lastPeriod != -1) {
		for (size_t i=lastPeriod; extSize < 256; extSize++, i++)
			if (string[i] != ext[extSize]) {
				extSize = 0;
				break;
			} else if (string[i] == 0) break;
		assert(extSize != 256);
	}
	size_t outputSize = inputSize - extSize;
	std::shared_ptr<char[]> output = std::make_shared<char[]>(outputSize+1);

	for (size_t i = 0; i < outputSize; i++)
		output[i] = string[i];
	output[outputSize] = 0;

	return output;
}