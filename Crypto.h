#ifndef CRYPTO_H
#define CRYPTO_H


const unsigned char KEY256[] = "01234567890123456789012345678901";
const unsigned char IV256[] = "01234567890123456";

class Crypto;
class Encrypt
{
public:
    Encrypt(const unsigned char *key = KEY256, const unsigned char *iv = IV256);
    ~Encrypt();

    bool initialize();
    bool toCipher(unsigned char *plaintext, int plaintext_len, unsigned char *ciphertext, int &ciphertext_len);
    bool finalize(unsigned char *ciphertext, int &ciphertext_len);
    int calculateCipherLen(int plaintext_len) const;

private:
    Crypto *m_crypto;
};

class Decrypt
{
public:
    Decrypt(const unsigned char *key = KEY256, const unsigned char *iv = IV256);
    ~Decrypt();

    bool initialize();
    bool toPlain(unsigned char *ciphertext, int ciphertext_len, unsigned char *plaintext, int &plaintext_len);
    bool finalize(unsigned char *plaintext, int &plaintext_len);

private:
    Crypto *m_crypto;
};

#endif // CRYPTO_H
