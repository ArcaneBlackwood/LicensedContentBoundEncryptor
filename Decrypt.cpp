#include "main.hpp"

#include <ios>
#include <vector>
#include <fstream>
#include <iostream>
#include <iterator>
#include "openssl/sha.h"
#include "openssl/evp.h"


bool decryptWithOriginal(
	const std::string &originalPath,
	const std::string &encryptedInputPath,
	const std::string &outputPath
) {
	std::ifstream origFile(originalPath, std::ios::binary);
	std::ifstream encryptedFile(encryptedInputPath, std::ios::binary);
	if (!origFile || !encryptedFile) {
		std::cerr << "Failed to open one or more files." << std::endl;
		return false;
	}

	//Get file size
	encryptedFile.seekg(0, std::ios::end);
	std::streamoff totalSize = encryptedFile.tellg();
	encryptedFile.seekg(0, std::ios::beg);


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


	// Get original input file name
	std::string origFileName;
	std::getline(encryptedFile, origFileName, '\0');
	if (encryptedFile.gcount() == totalSize) {
		std::cerr << "Failed to read key file name" << std::endl;
		return false;
	}
	size_t origNameLen = origFileName.size();


	// Read the IV vector
	std::vector<unsigned char> iv(CRYPTO_IV_SIZE);
	encryptedFile.read((char *)iv.data(), iv.size());
	if (encryptedFile.gcount() != (std::streamsize)iv.size()) {
		std::cerr << "Failed to read IV" << std::endl;
		return false;
	}


	// Setup decryptor
	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
	if (!ctx) {
		std::cerr << "EVP_CIPHER_CTX_new failed" << std::endl;
		return false;
	}
	if (!EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr)) {
		std::cerr << "DecryptInit failed" << std::endl;
		EVP_CIPHER_CTX_free(ctx);
		return false;
	}
	EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, (int)iv.size(), nullptr);
	if (!EVP_DecryptInit_ex(ctx, nullptr, nullptr, key, iv.data())) {
		std::cerr << "Key/IV init failed" << std::endl;
		EVP_CIPHER_CTX_free(ctx);
		return false;
	}


	// Decrypt proof-of-knowledge and output file
	std::vector<unsigned char> buffer(4096);
	std::vector<unsigned char> outbuf(4096 + EVP_CIPHER_block_size(EVP_aes_256_gcm()));
	int outlen;

	std::vector<unsigned char> proof;
	proof.reserve(strlen(PROOF_OF_KNOWLEDGE));
	size_t proofRemaining = strlen(PROOF_OF_KNOWLEDGE);
	bool proofFailed = false;

	std::streamoff bytesRead = 0;
	std::streamoff cipherSize = totalSize - iv.size() - 16 - origNameLen - 1;
	std::ofstream outputFile(outputPath, std::ios::binary);
	if (!outputFile) {
		std::cerr << "Failed to open output file" << std::endl;
		EVP_CIPHER_CTX_free(ctx);
		return false;
	}
	while (bytesRead < cipherSize) {
		std::streamsize toRead = std::min((std::streamoff)buffer.size(), cipherSize - bytesRead);
		encryptedFile.read((char *)buffer.data(), toRead);
		std::streamsize readLen = encryptedFile.gcount();
		if (readLen <= 0) break;

		if (!EVP_DecryptUpdate(ctx, outbuf.data(), &outlen, buffer.data(), (int)readLen)) {
			std::cerr << "DecryptUpdate failed" << std::endl;
			EVP_CIPHER_CTX_free(ctx);
			return false;
		}

		size_t offset = 0;
		if (proofRemaining > 0) {
			size_t toCopy = std::min((size_t)outlen, proofRemaining);
			proof.insert(proof.end(), outbuf.data(), outbuf.data() + toCopy);
			proofRemaining -= toCopy;
			offset = toCopy;

			if (proofRemaining == 0 && memcmp(proof.data(), PROOF_OF_KNOWLEDGE, proof.size()) != 0) {
				std::cerr << "Proof-of-knowledge mismatch.  Are you sure you are using the same input file?" << std::endl;
				EVP_CIPHER_CTX_free(ctx);
				return false;
			}
		}

		if (proofRemaining == 0) {
			outputFile.write((char*)outbuf.data() + offset, outlen - offset);
		}

		bytesRead += readLen;
	}


	// Read tag from end
	unsigned char tag[16];

	if (!encryptedFile.read((char *)tag, sizeof(tag)) || encryptedFile.gcount() != sizeof(tag)) {
		std::cerr << "Failed to read GCM tag" << std::endl;
		std::cout << "encryptedFile.gcount(): " << (int)encryptedFile.gcount() << std::endl;
		EVP_CIPHER_CTX_free(ctx);
		return false;
	}
	EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, sizeof(tag), tag);

	if (!EVP_DecryptFinal_ex(ctx, outbuf.data(), &outlen)) {
		std::cerr << "DecryptFinal failed" << std::endl;
		EVP_CIPHER_CTX_free(ctx);
		return false;
	}


	outputFile.write((char *)outbuf.data(), outlen);
	EVP_CIPHER_CTX_free(ctx);
	return true;
}