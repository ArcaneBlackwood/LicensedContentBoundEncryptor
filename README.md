# Licensed Content Bound Encryptor

**LicensedContentBoundEncryptor** is a command-line tool for encrypting and decrypting files that contain licensed, paid, or otherwise sensitive content. Encryption is bound to a *key input file*—usually the original licensed content itself—ensuring that only users who possess the correct file can decrypt and access the content.

---

# 🔐 Purpose

This tool is designed to enable secure redistribution of modified files tied to original licensed content. For example, it can be used to:

- Encrypt a modified version of a paid/licensed asset.
- Distribute it publicly while ensuring only legitimate users with the original asset can decrypt it.
- Avoid embedding or redistributing the original licensed material.

Encryption uses AES-256-GCM and a SHA-512/HKDF-based key derived from the key input file.

---

# 📦 Download

You can download the latest release [from the GitHub Releases page](https://github.com/ArcaneBlackwood/LicensedContentBoundEncryptor/releases).

### 🛠️ Requirements

- [OpenSSL 3.x](https://slproweb.com/products/Win32OpenSSL.html)
Tested and working on Windows x64 with version v3.5.0.  Note there are many places offering prebuild OpenSSL installers.  Optionally heres a direct [download for Windows x64 OpenSSL v3.5.0](https://slproweb.com/download/Win64OpenSSL-3_5_0.msi)

---

# 🚀 Usage
In your command line you can use the commands listed below.
Note for windows, if you shift right click in the folder you have the `LicensedContentBoundEncryptor.exe` file downloaded, you can click on "Open PowerShall window here" to quickly open a terminal!

Begin each command with `LicensedContentBoundEncryptor <command>` with command as:
- `encrypt <key input file> <decrypted input file> <encrypted output file>`
Encrypts a file using a key file (e.g., the original licensed content).  Note that if no file extension is given for encrypted output file, `*.lenc` is used by default
- `decrypt <key input file> <encrypted input file> <decrypted output file>`
Decrypts a file using the same key file originally used for encryption.
- `info <encrypted input file>`
Displays metadata from an encrypted file, including the original key file name and estimated decrypted size.  Can be useful in identifying the correct key file to use.
- `help`
Displays command help text.

### 💡 Examples

- `LicensedContentBoundEncryptor encrypt SpecialSecret.blend MyModifiedFile.blend MyModifiedFile.lenc`
Encrypts a blender file `MyModifiedFile.blend` that is modified from `SpecialSecret.blend`.  The output file could now safely be distrabuted without worry of the original file content being used or exposed, unless someone has the original content to begin with.
- `LicensedContentBoundEncryptor decrypt SpecialSecret.blend MyModifiedFile.lenc MyModifiedFileDecrypted.blend`
Decrypts the file from above, note that we cannot decrypt it without the original file!
- `LicensedContentBoundEncryptor info MyModifiedFile.lenc`
Gets information about the encrypted file.  Its output would look something like this
```text
Encrypted file 'MyModifiedFile.lenc' info:
        Original key name: 'SpecialSecret.blend'
        Decrypted data size: 135MB
```

---

# ⚠️ Disclaimer

This project does **not** bypass any licensing, DRM, or copyright restrictions.  
It is designed to **enforce ownership or access to original content** and should be used responsibly within legal bounds.