#include "main.hpp"

#include <ios>
#include <vector>
#include <fstream>
#include <iostream>
#include <iterator>

bool logEncryptedInfo(const std::string& encryptedInputPath) {
	std::ifstream encryptedFile(encryptedInputPath, std::ios::binary);
	if (!encryptedFile) {
		std::cerr << "Failed to open file." << std::endl;
		return false;
	}

	std::shared_ptr<char[]> inputFileName = getFileName(encryptedInputPath.c_str());
	std::cout << "Encrypted file '" << inputFileName << "' info:" << std::endl;

	//Get file size
	encryptedFile.seekg(0, std::ios::end);
	std::streamoff totalSize = encryptedFile.tellg();
	encryptedFile.seekg(0, std::ios::beg);


	// Get original input file name
	std::string origFileName;
	std::getline(encryptedFile, origFileName, '\0');
	if (encryptedFile.gcount() == totalSize) {
		std::cerr << "Failed to read key file name." << std::endl;
		return false;
	}
	size_t origNameLen = origFileName.size();

	std::cout << "\tOriginal key name: '" << origFileName << "'" << std::endl;


	//Get data size
	size_t proofSize = sizeof(PROOF_OF_KNOWLEDGE) - 1;
	std::streamoff cipherSize = totalSize - CRYPTO_IV_SIZE - 16 - origNameLen - 1 - proofSize;
	if (cipherSize < 1) {
		std::cerr << "Failed to calculate cipher size." << std::endl;
		return false;
	}

	std::cout << "\tDecrypted data size: " << formatBytes(cipherSize) << std::endl;


	return true;
}