//https://wiki.openssl.org/index.php/EVP_Symmetric_Encryption_and_Decryption
//https://wiki.openssl.org/index.php/EVP_Authenticated_Encryption_and_Decryption
//http://stackoverflow.com/questions/35768059/encrypt-decrypt-output-buffer-size-and-when-to-call-evp-encryptupdate-multiple-t

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <string.h>

#include "Crypto.h"

class Crypto
{
public:
    Crypto(const unsigned char *key, const unsigned char *iv)
        : m_ctx(nullptr)
        , m_key((unsigned char *)strdup((const char*)key))
        , m_iv((unsigned char *)strdup((const char*)iv))
    {
        ERR_load_crypto_strings();
        OpenSSL_add_all_algorithms();
        OPENSSL_config(NULL);
    }

    ~Crypto()
    {
        EVP_cleanup();
        ERR_free_strings();
        free(m_key);
        free(m_iv);
    }

public:
    EVP_CIPHER_CTX *m_ctx;
    unsigned char *m_key;
    unsigned char *m_iv;
};

Encrypt::Encrypt(const unsigned char *key, const unsigned char *iv)
    : m_crypto(new Crypto(key, iv))
{
    m_crypto->m_ctx = EVP_CIPHER_CTX_new();
}

Encrypt::~Encrypt()
{
    EVP_CIPHER_CTX_free(m_crypto->m_ctx);
    delete m_crypto;
}

bool Encrypt::initialize()
{
    if(1 != EVP_EncryptInit_ex(m_crypto->m_ctx, EVP_aes_256_cbc(), NULL, m_crypto->m_key, m_crypto->m_iv))
    {
        return false;
    }
    return true;
}

int Encrypt::calculateCipherLen(int plaintext_len) const
{
    return plaintext_len + (EVP_MAX_BLOCK_LENGTH - plaintext_len % EVP_MAX_BLOCK_LENGTH);
}

bool Encrypt::toCipher(unsigned char* plaintext, int plaintext_len, unsigned char* ciphertext, int &ciphertext_len)
{
    if(1 != EVP_EncryptUpdate(m_crypto->m_ctx, ciphertext, &ciphertext_len, plaintext, plaintext_len))
    {
        return false;
    }
    return true;
}

bool Encrypt::finalize(unsigned char *ciphertext, int &ciphertext_len)
{
    if(1 != EVP_EncryptFinal_ex(m_crypto->m_ctx, ciphertext, &ciphertext_len))
    {
        return false;
    }
    return true;
}


Decrypt::Decrypt(const unsigned char *key, const unsigned char *iv)
    : m_crypto(new Crypto(key, iv))
{
    m_crypto->m_ctx = EVP_CIPHER_CTX_new();
}

Decrypt::~Decrypt()
{
    EVP_CIPHER_CTX_free(m_crypto->m_ctx);
}

bool Decrypt::initialize()
{
    if(1 != EVP_DecryptInit_ex(m_crypto->m_ctx, EVP_aes_256_cbc(), NULL, m_crypto->m_key, m_crypto->m_iv))
    {
        return false;
    }
    return true;
}

bool Decrypt::toPlain(unsigned char *ciphertext, int ciphertext_len, unsigned char *plaintext, int &plaintext_len)
{
    if(1 != EVP_DecryptUpdate(m_crypto->m_ctx, plaintext, &plaintext_len, ciphertext, ciphertext_len))
    {
        return false;
    }
    return true;
}

bool Decrypt::finalize(unsigned char *plaintext, int &plaintext_len)
{
    if(1 != EVP_DecryptFinal_ex(m_crypto->m_ctx, plaintext, &plaintext_len))
    {
        return false;
    }
    return true;
}
