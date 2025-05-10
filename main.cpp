// main.cpp
#include <QCoreApplication>
#include <QDebug>
#include "mceliece.h"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    PQCryptoLite kem;

    PQCryptoLite::PublicKey pk;
    PQCryptoLite::PrivateKey sk;
    kem.keygen(pk, sk);
    qDebug() << "Key generation complete!";
 //qDebug() <<
             kem.printPublicKey(pk);
            // kem.printPrivateKey(sk);
    PQCryptoLite::SharedSecret senderSecret;
    PQCryptoLite::CipherText ct = kem.encapsulate(pk, senderSecret);
    qDebug() << "Encapsulation complete!";
    qDebug() << "Sender shared secret:" << senderSecret.key.toHex();

    PQCryptoLite::SharedSecret receiverSecret = kem.decapsulate(ct, sk);
    qDebug() << "Decapsulation complete!";
    qDebug() << "Receiver shared secret:" << receiverSecret.key.toHex();

    if (senderSecret.key == receiverSecret.key) {
        qDebug() << "✅ Shared secrets match!";
    } else {
        qDebug() << "❌ Shared secrets do NOT match!";
    }

    return 0;
}
