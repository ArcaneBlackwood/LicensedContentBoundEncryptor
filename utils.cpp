#include "main.hpp"

#include <ctype.h>
#include <string>
#include <cassert>
#include <sstream>
#include <iomanip>

bool strequal(const char* a, const char* b) {
	for (size_t i = 0; i < 256 && tolower(a[i]) == b[i]; i++)
		if (a[i] == 0) return true;
	return false;
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
std::string formatBytes(size_t bytes) {
	const char *units[] = { "B", "KB", "MB", "GB", "TB", "PB" };
	size_t unit = 0;
	size_t size = bytes;
	while (size >= 1024 && unit < 5) {
		size >>= 10;
		++unit;
	}
	std::ostringstream out;
	out << size << units[unit];
	return out.str();
}