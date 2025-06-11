#include "main.hpp"

#include <iostream>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/kdf.h>

constexpr char kSalt[] = CRYPTO_SALT;
constexpr char kInfo[] = CRYPTO_INFO;
bool hashFileSHA512(std::istream &stream, unsigned char outHash[SHA512_DIGEST_LENGTH]) {
	EVP_MD_CTX *ctx = EVP_MD_CTX_new();
	if (!ctx) return false;

	const EVP_MD *md = EVP_sha512();
	if (!md) {
		EVP_MD_CTX_free(ctx);
		return false;
	}

	if (!EVP_DigestInit_ex(ctx, md, nullptr)) {
		EVP_MD_CTX_free(ctx);
		return false;
	}

	char buffer[8192];
	while (stream.read(buffer, sizeof(buffer)) || stream.gcount()) {
		if (!EVP_DigestUpdate(ctx, buffer, stream.gcount())) {
			EVP_MD_CTX_free(ctx);
			return false;
		}
	}

	if (!EVP_DigestFinal_ex(ctx, outHash, nullptr)) {
		EVP_MD_CTX_free(ctx);
		return false;
	}

	EVP_MD_CTX_free(ctx);
	return true;
}
bool deriveKeyHKDF(
	const unsigned char *inputKeyMaterial, size_t ikmLen,
	unsigned char *outKey, size_t keyLen) {
	EVP_PKEY_CTX *pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_HKDF, nullptr);
	if (!pctx) return false;

	const unsigned char* salt = reinterpret_cast<const unsigned char*>(kSalt);
	const size_t saltLen = sizeof(kSalt) - 1;
	const unsigned char* info = reinterpret_cast<const unsigned char*>(kInfo);
	const size_t infoLen = sizeof(kInfo) - 1;

	bool ok = EVP_PKEY_derive_init(pctx) > 0 &&
		EVP_PKEY_CTX_set_hkdf_md(pctx, EVP_sha512()) > 0 &&
		EVP_PKEY_CTX_set1_hkdf_salt(pctx, salt, saltLen) > 0 &&
		EVP_PKEY_CTX_set1_hkdf_key(pctx, inputKeyMaterial, (int)ikmLen) > 0 &&
		EVP_PKEY_CTX_add1_hkdf_info(pctx, info, infoLen) > 0 &&
		EVP_PKEY_derive(pctx, outKey, &keyLen) > 0;

	EVP_PKEY_CTX_free(pctx);
	return ok;
}