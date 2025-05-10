// pqcrypto_light.h - Simplified, working McEliece-like KEM (for educational/demo use only)
// WARNING: This is not secure! For demonstration purposes only.

#ifndef PQCRYPTO_LIGHT_H
#define PQCRYPTO_LIGHT_H

#include <QtCore>
#include <array>
#include <random>
#include <bitset>

class PQCryptoLite {
public:
    static constexpr int K = 128; // message bits
    static constexpr int N = 256; // codeword length (simple code)

    using BitVec = std::bitset<N>;

    struct PublicKey {
        std::array<BitVec, K> G; // Generator matrix
    };

    struct PrivateKey {
        std::bitset<N> permutation; // Permuted positions (identity here)
        PublicKey pk; // Store public key for decoding
    };

    struct CipherText {
        BitVec c;
    };

    struct SharedSecret {
        QByteArray key;
    };

    PQCryptoLite() {
        std::random_device rd;
        rng.seed(rd());
    }

    void keygen(PublicKey &pk, PrivateKey &sk) {
        for (int i = 0; i < K; ++i) {
            pk.G[i].reset();
            pk.G[i].set(i); // Identity generator (trivial code)
        }
       // sk.permutation.reset();
        for (int i = 0; i < N; ++i) sk.permutation.set(i); // Identity permutation
    }
#include <QVector>
#include <QRandomGenerator>

// Convert bitset to QVector<bool>, shuffle, and convert back
void shuffleBitset(std::bitset<256>& bits) {
    QVector<bool> bitVector;
    for (int i = 0; i < 256; ++i) {
        bitVector.append(bits[i]);
    }

    std::shuffle(bitVector.begin(), bitVector.end(), *QRandomGenerator::global());

    for (int i = 0; i < 256; ++i) {
        bits[i] = bitVector[i];
    }
}

    void keygen3(PublicKey &pk, PrivateKey &sk) {
        std::uniform_int_distribution<int> bitdist(0, 1);  // Declare here

        for (int i = 0; i < K; ++i) {
            pk.G[i].reset();
            for (int j = 0; j < N; ++j) {
                pk.G[i][j] = bitdist(rng);
            }

            // Optional: shuffle bits in each row (not typical in McEliece, but possible for experimentation)
            shuffleBitset(pk.G[i]);
        }

        sk.pk = pk;
    }


    CipherText encapsulate(const PublicKey &pk, SharedSecret &ss) {
        BitVec message = randomMessage();
        BitVec c = encode(pk, message);
        c ^= simpleErrorPattern(); // Add correctable errors

        ss.key = hash(message);
        return { c };
    }

    SharedSecret decapsulate(const CipherText &ct, const PrivateKey &sk) {
        BitVec corrected = simpleErrorCorrection(ct.c);
        BitVec message;
        for (int i = 0; i < K; ++i)
            message[i] = corrected[i];

        return { hash(message) };
    }

    void printPublicKey(const PublicKey &pk) {
        qDebug() << "Public Key (Generator Matrix G):";
        for (int i = 0; i < K; ++i) {
            // Convert bitset to a QString and then print it
            QString str = QString::fromStdString(pk.G[i].to_string());
            qDebug() << str;
        }
    }

    // Function to print the private key
    void printPrivateKey(const PrivateKey &sk) {
     qDebug() << "Private Key (Permutation):" ;
        for (int i = 0; i < N; ++i) {
     qDebug()  << sk.permutation[i] << " ";
        }
        printPublicKey(sk.pk); // Print the associated public key
    }

    // Function to print the ciphertext
    void printCipherText(const CipherText &ct) {
 //    qDebug()  << "Ciphertext (c): " << ct.c;
    }

    // Function to print shared secret
    void printSharedSecret(const SharedSecret &ss) {
    qDebug()  << "Shared Secret: " << ss.key.toHex() ;
    }

private:
    std::mt19937 rng;

    BitVec randomMessage() {
        BitVec m;
        std::uniform_int_distribution<int> dist(0, 1);
        for (int i = 0; i < K; ++i)
            m[i] = dist(rng);
        return m;
    }

    BitVec encode(const PublicKey &pk, const BitVec &m) {
        BitVec c;
        for (int i = 0; i < K; ++i)
            if (m[i]) c ^= pk.G[i];
        return c;
    }

    BitVec simpleErrorPattern() {
        BitVec e;
        e.set(N - 1); // Flip last bit only for simple correction
        return e;
    }

    BitVec simpleErrorCorrection(const BitVec &c) {
        BitVec corrected = c;
        corrected.flip(N - 1); // Reverse the bit flipped
        return corrected;
    }

    QByteArray hash(const BitVec &b) {
        QByteArray raw;
        for (int i = 0; i < N; i += 8) {
            uint8_t byte = 0;
            for (int j = 0; j < 8 && i + j < N; ++j) {
                if (b[i + j]) byte |= (1 << j);
            }
            raw.append(byte);
        }
        return QCryptographicHash::hash(raw, QCryptographicHash::Sha256);
    }
};

#endif // PQCRYPTO_LIGHT_H
