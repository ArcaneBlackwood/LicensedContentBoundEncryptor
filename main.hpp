#pragma once

#include <string>
#include <memory>
#include <openssl/sha.h>

#define PROOF_OF_KNOWLEDGE "LicensedContentProof_v1"
#define MAX_FILE_PATH_SIZE 4096
#define CRYPTO_SALT ""
#define CRYPTO_INFO ""
#define DEFAULT_EXTENSION ".lenc"
#define DEFAULT_DECRYPT_PREFIX "-decrypted"

#define CRYPTO_IV_SIZE 12

bool decryptMultipleWithOriginal(
	const char*  originalPath,
	const char*  encryptedInputPaths,
	const char*  outputPaths
);
bool decryptWithOriginal(
	const char*  originalPath,
	const char*  encryptedInputPath,
	const char*  outputPath
);
bool encryptMultipleWithOriginal(
	const char*  originalPath,
	const char*  inputPaths,
	const char*  encryptedOutputPaths
);
bool encryptWithOriginal(
	const char*  originalPath,
	const char*  inputPath,
	const char*  encryptedOutputPath
);
bool logEncryptedInfo(const char*  encryptedInputPath);

bool hashFileSHA512(std::istream& stream, unsigned char outHash[SHA512_DIGEST_LENGTH]);
bool deriveKeyHKDF(
	const unsigned char* inputKeyMaterial, size_t ikmLen,
	unsigned char* outKey, size_t keyLen);



bool strequal(const char* a, const char* b);
std::shared_ptr<char[]> getFileName(const char* fullPath);
std::shared_ptr<char[]> suffixFileName(const char* string, const char* suffix); 
std::shared_ptr<char[]> splitString(const char* string, char seperator, size_t& offset);

std::shared_ptr<char[]> ensureFileExtension(const char* input, const char* defaultExt);
std::shared_ptr<char[]> appendExtension(const char* string, const char* ext);
std::shared_ptr<char[]> removeExtension(const char* string, const char* ext);

/*
	File structure:
		IV vector (usually 12 byte)
		Encrypted proof of knowledge
		Encrypted file
		Tag/checksum
 */