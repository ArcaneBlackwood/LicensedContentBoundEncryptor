#pragma once

#include <string>
#include <memory>
#include <openssl/sha.h>

#define PROOF_OF_KNOWLEDGE "LicensedContentProof_v1"
#define MAX_FILE_PATH_SIZE 4096
#define CRYPTO_SALT ""
#define CRYPTO_INFO ""

#define CRYPTO_IV_SIZE 12

bool decryptWithOriginal(
	const std::string& originalPath,
	const std::string& encryptedInputPath,
	const std::string& outputPath
);
bool encryptWithOriginal(
	const std::string& originalPath,
	const std::string& inputPath,
	const std::string& encryptedOutputPath
);
bool logEncryptedInfo(const std::string& encryptedInputPath);

bool hashFileSHA512(std::istream& stream, unsigned char outHash[SHA512_DIGEST_LENGTH]);
bool deriveKeyHKDF(
	const unsigned char* inputKeyMaterial, size_t ikmLen,
	unsigned char* outKey, size_t keyLen);


bool strequal(const char* a, const char* b);
std::shared_ptr<char[]> ensureFileExtension(const char* input, const char* defaultExt);
std::shared_ptr<char[]> getFileName(const char* fullPath);
std::string formatBytes(size_t bytes);

/*
	File structure:
		IV vector (usually 12 byte)
		Encrypted proof of knowledge
		Encrypted file
		Tag/checksum
 */