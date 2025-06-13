#include "main.hpp"

#include <ios>
#include <vector>
#include <fstream>
#include <iostream>
#include <cassert>
#include <iterator>
#include "openssl/sha.h"
#include "openssl/evp.h"
#include "openssl/rand.h"

bool encryptMultipleWithOriginal(
	const char* originalPath,
	const char* inputPaths,
	const char* encryptedOutputPaths
) {
	size_t offsetInputs = 0, offsetOutputs = 0;
	std::shared_ptr<char[]> input, output;
	bool success = true;
	std::cout << "#TEST 1 " << inputPaths << std::endl;
	std::cout << "#TEST 2 " << (encryptedOutputPaths==nullptr ? "T" : "F") << std::endl;
	while (
		(bool)(input = splitString(inputPaths, ';', offsetInputs)) &&
		(encryptedOutputPaths == nullptr || (bool)(output = splitString(encryptedOutputPaths, ';', offsetOutputs)))
	) {
		if (encryptedOutputPaths == nullptr)
			output = appendExtension(input.get(), DEFAULT_EXTENSION);
		else
			ensureFileExtension(output.get(), DEFAULT_EXTENSION);
		bool iterSuccess = encryptWithOriginal(originalPath, input.get(), output.get());
		success = success && iterSuccess;
		std::cout << ( iterSuccess ? "Successfully encrypted '" : "Failed to encrypt ''");
		std::cout << input << "' to '" << output << "'" << std::endl;
	}
	return success;
}
bool encryptWithOriginal(
	const char* originalPath,
	const char* inputPath,
	const char* encryptedOutputPath
) {
	std::ifstream origFile(originalPath, std::ios::binary);
	std::ifstream inputFile(inputPath, std::ios::binary);
	std::ofstream outputFile(encryptedOutputPath, std::ios::binary);
	if (!origFile || !inputFile || !outputFile) {
		std::cerr << "Failed to open one or more files." << std::endl;
		return false;
	}


	// Generate key with hash
	unsigned char hash[SHA512_DIGEST_LENGTH];
	if (!hashFileSHA512(origFile, hash)) {
		std::cerr << "Failed to compute key file hash" << std::endl;
		return false;
	}
	unsigned char key[SHA512_DIGEST_LENGTH];
	if (!deriveKeyHKDF(hash, SHA512_DIGEST_LENGTH, key, SHA512_DIGEST_LENGTH)) {
		std::cerr << "Failed to compute key from file hash" << std::endl;
		return false;
	}


	// Write input file name
	std::shared_ptr<char[]> origFileName = getFileName(originalPath);
	size_t keyPathSize = strnlen(origFileName.get(), MAX_FILE_PATH_SIZE);
	assert(keyPathSize != MAX_FILE_PATH_SIZE);
	outputFile.write((const char*)origFileName.get(), keyPathSize + 1);


	// Write the IV vector
	std::vector<unsigned char> iv(12);
	if (RAND_bytes(iv.data(), (int)iv.size()) != 1) {
		std::cerr << "Failed to generate IV." << std::endl;
		return false;
	}
	outputFile.write((char *)iv.data(), (int)iv.size());


	// Setup encryptor
	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
	if (!ctx) {
		std::cerr << "EVP_CIPHER_CTX_new failed" << std::endl;
		return false;
	}
	if (!EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr)) {
		std::cerr << "EncryptInit failed" << std::endl;
		EVP_CIPHER_CTX_free(ctx);
		return false;
	}
	EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, (int)iv.size(), nullptr);
	if (!EVP_EncryptInit_ex(ctx, nullptr, nullptr, key, iv.data())) {
		std::cerr << "Key/IV init failed" << std::endl;
		EVP_CIPHER_CTX_free(ctx);
		return false;
	}


	int outlen;
	unsigned char outbuf[4096];

	// Encrypt the proof-of-knowledge first
	const char *proof = PROOF_OF_KNOWLEDGE;
	if (!EVP_EncryptUpdate(ctx, outbuf, &outlen, (const unsigned char *)proof, (int)strnlen(proof, 2048))) {
		std::cerr << "Encrypting proof failed" << std::endl;
		EVP_CIPHER_CTX_free(ctx);
		return false;
	}
	outputFile.write((char *)outbuf, outlen);


	// Encrypt the file stream
	while (!inputFile.eof()) {
		inputFile.read((char *)outbuf, sizeof(outbuf));
		std::streamsize readLen = inputFile.gcount();
		if (readLen <= 0) break;

		if (!EVP_EncryptUpdate(ctx, outbuf, &outlen, outbuf, (int)readLen)) {
			std::cerr << "Encrypting file chunk failed" << std::endl;
			EVP_CIPHER_CTX_free(ctx);
			return false;
		}
		outputFile.write((char *)outbuf, outlen);
	}
	if (!EVP_EncryptFinal_ex(ctx, outbuf, &outlen)) {
		std::cerr << "EncryptFinal failed" << std::endl;
		EVP_CIPHER_CTX_free(ctx);
		return false;
	}
	outputFile.write((char *)outbuf, outlen);


	// Write the tag
	unsigned char tag[16];
	EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, sizeof(tag), tag);
	outputFile.write((char *)tag, sizeof(tag));


	EVP_CIPHER_CTX_free(ctx);
	return true;
}