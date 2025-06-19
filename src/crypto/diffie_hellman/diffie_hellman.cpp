#include "diffie_hellman.hpp"

#include "settings_defaults.hpp"
#include "spdlog_wrapper.hpp"

#include <QDebug>
#include <QString>
#include <boost/multiprecision/miller_rabin.hpp>
#include <boost/random/independent_bits.hpp>

namespace CRYPTO
{
DiffieHellman::DiffieHellman(unsigned int prime_bits) : Protocol("Diffie Hellman"), m_prime_bits(prime_bits)
{}

void DiffieHellman::init()
{
	SPD_INFO_CLASS(UTILS::DEFAULTS::d_logger_dh, QObject::tr("diffie_hellamn_init"));
	generateParameters();
}

void DiffieHellman::generateParameters()
{
	boost::random::mt19937 rng(std::random_device {}());

	// Шаг 1: Генерация большого простого числа p
	m_p = generatePrime(m_prime_bits, rng);

	// Шаг 2: Выбор генератора g
	m_g = 5;
}

void DiffieHellman::generateKeys()
{
	boost::random::mt19937 rng(std::random_device {}());

	// Шаг 3: Генерация закрытого ключа `a`
	boost::multiprecision::uniform_int_distribution<BigInt> dist(2, m_p - 2);
	m_private_key = dist(rng);

	// Шаг 4: Вычисление открытого ключа A = g^a mod p
	m_public_key = boost::multiprecision::powm(m_g, m_private_key, m_p);
}

BigInt DiffieHellman::generatePrime(unsigned int bits, boost::random::mt19937& rng)
{
	boost::random::independent_bits_engine<boost::random::mt19937, 256, BigInt> bit_gen(rng);
	BigInt																		p;
	do
	{
		p = bit_gen();
		if (bits > 1)
		{
			p |= (BigInt(1) << (bits - 1));
		}
		p |= 1;
	} while (!boost::multiprecision::miller_rabin_test(p, 25, rng));
	return p;
}

void DiffieHellman::setOtherPartyPublicKey(const BigInt& key)
{
	m_other_party_public_key = key;
}

void DiffieHellman::setOtherPartyPrime(const BigInt& prime)
{
	m_p = prime;
	generateKeys();
}

BigInt DiffieHellman::getPublicKey() const
{
	return m_public_key;
}

BigInt DiffieHellman::getPrime()
{
	generateKeys();
	return m_p;
}

void DiffieHellman::generateSharedSecret()
{
	if (m_other_party_public_key == 0)
	{
		throw std::runtime_error("Открытый ключ другого участника не установлен.");
	}

	// Вычисление общего секрета: s = (B^a) mod p
	m_shared_secret = boost::multiprecision::powm(m_other_party_public_key, m_private_key, m_p);
}

QString DiffieHellman::encrypt(const QString& plaintext)
{
	if (m_shared_secret == 0)
	{
		SPD_ERROR_CLASS(UTILS::DEFAULTS::d_logger_dh, QObject::tr("diffie_hellamn_common_secret_not_set"));
	}

	// 1. Преобразуем открытый текст в байты
	QByteArray bytes = plaintext.toUtf8();

	// 2. Преобразуем байты в шестнадцатеричную строку.
	QString hex_plaintext = bytes.toHex();

	// 3. Преобразуем шестнадцатеричную строку в BigInt.
	BigInt message("0x" + hex_plaintext.toStdString());

	// 3. Выполняем XOR с общим ключом
	BigInt encrypted_int = message ^ m_shared_secret;

	return QString::fromStdString(encrypted_int.str(0, std::ios_base::hex));
}

QString DiffieHellman::decrypt(const QString& ciphertext)
{
	if (m_shared_secret == 0)
	{
		SPD_ERROR_CLASS(UTILS::DEFAULTS::d_logger_dh, QObject::tr("diffie_hellamn_common_secret_not_set"));
	}

	// 1. Преобразуем шестнадцатеричную строку в массив байт
	BigInt encrypted_int("0x" + ciphertext.toStdString());

	// 3. Выполняем обратную операцию XOR
	BigInt decrypted_int = encrypted_int ^ m_shared_secret;

	std::string hex_str = decrypted_int.str(0, std::ios_base::hex);

	if (hex_str.length() % 2 != 0)
	{
		hex_str.insert(0, "0");
	}

	QByteArray bytes = QByteArray::fromHex(QByteArray::fromStdString(hex_str));

	return QString::fromUtf8(bytes);
}
} // namespace CRYPTO
