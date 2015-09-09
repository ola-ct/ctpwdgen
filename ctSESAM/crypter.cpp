/*

    Copyright (c) 2015 Oliver Lau <ola@ct.de>, Heise Medien GmbH & Co. KG

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <QDebug>
#include <random>
#include <string>
#include "sha.h"
#include "ccm.h"
#include "misc.h"
#include "securebytearray.h"
#include "pbkdf2.h"
#include "crypter.h"
#include "util.h"
#include "global.h"


const int Crypter::SaltSize = 32;
const int Crypter::AESKeySize = 256 / 8;
const int Crypter::DomainIterations = 32768;
const int Crypter::KGKIterations = 1024;
const int Crypter::KGKSize = 64;
const int Crypter::AESBlockSize = CryptoPP::AES::BLOCKSIZE;
const int Crypter::CryptDataSize = Crypter::SaltSize + Crypter::AESBlockSize + Crypter::KGKSize;


/*!
 * \brief gRandomDevice
 *
 * The random number generator used by `Crypter`.
 *
 * `std::random_device` is a uniformly-distributed integer random number generator
 * that produces non-deterministic random numbers.
 * `std::random_device` may be implemented in terms of an implementation-defined
 * pseudo-random number engine if a non-deterministic source (e.g. a hardware device)
 * is not available to the implementation.
 * In this case each `std::random_device` object may generate the same number sequence.
 *
 */
std::random_device gRandomDevice;


/*!
 * \brief Crypter::encode
 *
 * This function encrypts a block of data with the given encryption key.
 *
 * The encryption key itself is encrypted with the given key and IV which themselves are generated from the master password.
 *
 * \param key An AES key generated from the user's master password.
 * \param IV AES initialization vector. A randomly generated byte sequence of `Crypter::AESBlockSize` length.
 * \param salt A salt of `Crypter::SaltSize` length in bytes.
 * \param KGK Key generation key. A randomly generated byte sequence of `Crypter::KGKSize` length.
 * \param data The data to be encrypted.
 * \param compress If `true`, data will be compressed before encryption.
 * \return Block of binary data with the following structure:
 *
 * Bytes   | Description
 * ------- | ---------------------------------------------------------------------------
 *       1 | Format flag (must be 0x01)
 *      32 | Salt (randomly generated)
 *     112 | Encrypted key generation key
 *       n | Encrypted data
 *
 */
QByteArray Crypter::encode(const SecureByteArray &key,
                           const SecureByteArray &IV,
                           const QByteArray &salt,
                           const SecureByteArray &KGK,
                           const QByteArray &data,
                           bool compress)
{
  const QByteArray &salt2 = randomBytes(SaltSize);
  const QByteArray &IV2 = randomBytes(AESBlockSize);
  const SecureByteArray &KGK2 = salt2 + IV2 + KGK;
  const QByteArray &encryptedKGK = encrypt(key, IV, KGK2, CryptoPP::StreamTransformationFilter::NO_PADDING);
  const SecureByteArray &blobKey = Crypter::makeKeyFromPassword(KGK, salt2);
  const QByteArray &baPlain = compress ? qCompress(data, 9) : data;
  const QByteArray &baCipher = encrypt(blobKey, IV2, baPlain, CryptoPP::StreamTransformationFilter::PKCS_PADDING);
  const QByteArray &formatFlag = QByteArray(int(1), static_cast<char>(AES256EncryptedMasterkeyFormat));
  return formatFlag + salt + encryptedKGK + baCipher;
}

/*!
 * \brief Crypter::decode
 * \param masterPassword The user's master password.
 * \param cipher The data to be decrypted.
 * \param uncompress If `true`, data will be uncompressed after encryption.
 * \param KGK Key generation key. A randomly generated byte sequence of `Crypter::AESKeySize` length.
 * \return The decrypted payload (without format flag and other header data) contained in `cipher`.
 */
QByteArray Crypter::decode(const SecureByteArray &masterPassword,
                           QByteArray cipher,
                           bool uncompress,
                           __out SecureByteArray &KGK)
{
  FormatFlags formatFlag = static_cast<FormatFlags>(cipher.at(0));
  if (formatFlag != AES256EncryptedMasterkeyFormat)
    return QByteArray();
  const QByteArray &salt = QByteArray(cipher.constData() + sizeof(char), SaltSize);
  const SecureByteArray &encryptedKGK = SecureByteArray(cipher.constData() + sizeof(char) + SaltSize, CryptDataSize);
  SecureByteArray key, IV;
  Crypter::makeKeyAndIVFromPassword(masterPassword, salt, key, IV);
  QByteArray baKGK = decrypt(key, IV, encryptedKGK, CryptoPP::StreamTransformationFilter::NO_PADDING);
  const QByteArray salt2(baKGK.constData(), SaltSize);
  const QByteArray IV2(baKGK.constData() + SaltSize, AESBlockSize);
  KGK = SecureByteArray(baKGK.constData() + SaltSize + AESBlockSize, KGKSize);
  const SecureByteArray &blobKey = Crypter::makeKeyFromPassword(KGK, salt2);
  const QByteArray &plain = decrypt(blobKey, IV2, cipher.mid(+ sizeof(char) + SaltSize + CryptDataSize), CryptoPP::StreamTransformationFilter::PKCS_PADDING);
  return uncompress ? qUncompress(plain) : plain;
}


/*!
 * \brief Crypter::encrypt
 *
 * AES-CBC encrypts a block of data.
 *
 * \param key The key to be used for encryption.
 * \param IV The initialization vector used to initialize AES.
 * \param plain The block of data to be encrypted.
 * \param padding A flag telling what kind of padding should be used. `CryptoPP::StreamTransformationFilter::PKCS_PADDING` means that PKCS#7 padding should be used (see RFC 5652). `CryptoPP::StreamTransformationFilter::NO_PADDING` means that no padding should be used (only applicable if `plain` length is a multiple of `Crypter::AESBlockSize`.
 * \return Encrypted block of data.
 */
QByteArray Crypter::encrypt(const SecureByteArray &key, const SecureByteArray &IV, const QByteArray &plain, CryptoPP::StreamTransformationFilter::BlockPaddingScheme padding)
{
  const std::string sPlain(plain.constData(), plain.size());
  std::string sCipher;
  CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption enc;
  enc.SetKeyWithIV(reinterpret_cast<const byte*>(key.constData()), key.size(), reinterpret_cast<const byte*>(IV.constData()));
  CryptoPP::ArraySource s(
        sPlain,
        true,
        new CryptoPP::StreamTransformationFilter(
          enc,
          new CryptoPP::StringSink(sCipher),
          padding
          )
        );
  Q_UNUSED(s); // just to please the compiler
  return QByteArray(sCipher.c_str(), sCipher.length());
}


/*!
 * \brief Crypter::encrypt
 *
 * AES-CBC decrypts a block of data.
 *
 * \param key The key to be used for decryption.
 * \param IV The initialization vector used to initialize AES.
 * \param cipher The block of data to be decrypted.
 * \param padding A flag telling what kind of padding should be used. `CryptoPP::StreamTransformationFilter::PKCS_PADDING` means that PKCS#7 padding should be used (see RFC 5652). `CryptoPP::StreamTransformationFilter::NO_PADDING` means that no padding should be used (only applicable if the length of the resulting plaintext data is a multiple of `Crypter::AESBlockSize`.
 * \return Decrypted block of data.
 */
SecureByteArray Crypter::decrypt(const SecureByteArray &key, const SecureByteArray &IV, const QByteArray &cipher, CryptoPP::StreamTransformationFilter::BlockPaddingScheme padding)
{
  const std::string sCipher(cipher.constData(), cipher.size());
  std::string sPlain;
  CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption dec;
  dec.SetKeyWithIV(reinterpret_cast<const byte*>(key.constData()), key.size(), reinterpret_cast<const byte*>(IV.constData()));
  CryptoPP::ArraySource s(
        sCipher,
        true,
        new CryptoPP::StreamTransformationFilter(
          dec,
          new CryptoPP::StringSink(sPlain),
          padding
          )
        );
  Q_UNUSED(s); // just to please the compiler
  return SecureByteArray(sPlain.c_str(), sPlain.length());
}


/*!
 * \brief Crypter::randomBytes
 *
 * Create a `QByteArray` filled with `size` randomly generated bytes. The sequence is uniformly distributed in the interval [0, 255].
 *
 * \param size So many bytes should be generated.
 * \return A `QByteArray` with `size` randomly generated bytes.
 */
QByteArray Crypter::randomBytes(const int size)
{
  QByteArray buf(size, static_cast<char>(0));
  for (int i = 0; i < size; ++i)
    buf[i] = static_cast<char>(gRandomDevice());
  return buf;
}


/*!
 * \brief Crypter::makeKeyFromPassword
 *
 * Generates a 256 bit key suitable for AES produced by PBKDF2.
 * PBKDF2 is called with the master password, the salt and an iteration count of `KGKIterations`.
 *
 * \param masterPassword The master password from which PBKDF2 should generate the key.
 * \param salt A salt used for PBKDF2.
 * \return A `SecureByteArray` containing a `AESKeySize` long SHA-256 hash generated via PBKDF2 parametrized with `masterPassword`, `salt` and `KGKIterations`.
 */
SecureByteArray Crypter::makeKeyFromPassword(const SecureByteArray &masterPassword, const QByteArray &salt)
{
  PBKDF2 pbkdf2(masterPassword, salt, KGKIterations, QCryptographicHash::Sha256);
  return pbkdf2.derivedKey(AESKeySize);
}


/*!
 * \brief Crypter::makeKeyAndIVFromPassword
 *
 * Generates a 256 bit key and 128 bit initialization vector from a 384 bit hash produced by PBKDF2.
 * PBKDF2 is called with the master password, the salt and an iteration count of `DomainIterations`.
 *
 * \param masterPassword The master password from which PBKDF2 should generate the key and IV.
 * \param salt A salt used for PBKDF2.
 * \param key A reference to a `SecureByteArray` object to which the generated key should be assigned.
 * \param IV A reference to a `SecureByteArray` object to which the generated IV should be assigned.
 */
void Crypter::makeKeyAndIVFromPassword(const SecureByteArray &masterPassword, const QByteArray &salt, SecureByteArray &key, SecureByteArray &IV)
{
  PBKDF2 pbkdf2(masterPassword, salt, DomainIterations, QCryptographicHash::Sha384);
  const SecureByteArray &hash = pbkdf2.derivedKey();
  key = hash.mid(0, AESKeySize);
  IV = hash.mid(AESKeySize, AESBlockSize);
}
