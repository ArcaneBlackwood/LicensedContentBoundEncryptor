// Depends on OpenSSL https://slproweb.com/products/Win32OpenSSL.html
#pragma comment(lib, "libssl.lib")
#pragma comment(lib, "libcrypto.lib")
//#pragma comment(linker, "/DELAYLOAD:libssl-3-x64.dll")
//#pragma comment(linker, "/DELAYLOAD:libcrypto-3-x64.dll")

#include "main.hpp"

#include <iostream>
#include <memory>

int main(int argc, char *argv[]) {
#ifdef _DEBUG
	if (argc == 2 && strequal(argv[1], "test")) {
		//static const char* original = "TestFiles//Original.png";
		//static const char* modified = "TestFiles//Modified.gif";
		//static const char* encrypted = "TestFiles//Encrypted.gif";
		//static const char* decrypted = "TestFiles//Decrypted.gif";
		static const char* original = "TestFiles//key.txt";
		static const char* modified = "TestFiles//input.txt";
		static const char* encrypted = "TestFiles//Encrypted.txt";
		static const char* decrypted = "TestFiles//Decrypted.txt";
		std::cout << "## Test: Encrypt" << std::endl;
		if (!encryptWithOriginal(original, modified, encrypted)) return -1;
		std::cout << "## Test: Decrypt" << std::endl;
		if (!decryptWithOriginal(original, encrypted, decrypted)) return -1;
		//std::cout << "## Test: Decrypt invalid key" << std::endl;
		//if (!decryptWithOriginal(modified, encrypted, decrypted)) return -2;
	} else 
#endif
	if (argc == 5 || argc == 4 && strequal(argv[1], "decrypt")) {
		if (!decryptMultipleWithOriginal(argv[2], argv[3], argc == 5 ? argv[4] : nullptr)) return -2;
	} else if (argc == 5 || argc == 4 && strequal(argv[1], "encrypt")) {
		if (!encryptMultipleWithOriginal(argv[2], argv[3], argc == 5 ? argv[4] : nullptr)) return -3;
	} else if (argc == 3 && strequal(argv[1], "info")) {
		if (!logEncryptedInfo(argv[2])) {
			std::cerr << "Is this input file a valid encrypted file by this program?" << std::endl;
			return -4;
		}
	} else if (argc > 1 && !strequal(argv[1], "help")) {
		std::cerr << "Unkown command '" << argv[1] << "' with " << (argc-1) << " arguments.  Use 'help' for a list of commands." << std::endl;
		return -1;
	} else {
		std::shared_ptr<char[]> program = getFileName(argv[0]);
		std::cout << "Licensed Content Bound Encryptor:  Encrypts and decrypts files that contain sensitive or licenced content using another file as a key." << std::endl;
		std::cout << "The input file may be a secret file, a licenced file or a file behind a paywall." << std::endl;
		std::cout << "One use could be to use the original licenced/paid file as a key, and encrypt a modified file to distrubute to other users." << std::endl;
		std::cout << std::endl << "Command usage:" << std::endl;
		std::cout << "\t" << program << "decrypt <key input file> <encrypted input file(s)> [decrypted output file(s)]" << std::endl;
		std::cout << "\t" << program << "encrypt <key input file> <decrypted input file(s)> [encrypted output file(s)]" << std::endl;
		std::cout << "\t" << program << "info <encrypted input file>" << std::endl;
		std::cout << "\t\t" << "Shows information about the input file such as estimated decrypt size and key file name at encryption." << std::endl;
		std::cout << "\t" << program << "help" << std::endl;
	}
	return 0;
}
/*
* TODO:
*	Default prefix fix
*	make output argument optional.  (Uses input file for decrypt removes .lenc if exists, else uses origonal name prefixed '-decrypt'.  For encrypt appends .lenc
* Make input encrypt/decrypt files able to take multiple files.
*/