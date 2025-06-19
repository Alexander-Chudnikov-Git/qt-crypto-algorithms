#include "elgamal.hpp"

#include <QByteArray>
#include <QStringList> // Необходимо для разделения шифртекста
#include <boost/multiprecision/miller_rabin.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <chrono>
#include <stdexcept>

namespace
{
// Помещаем функцию `inverse` в анонимное пространство имен, чтобы
// сделать её видимой только внутри этого файла (internal linkage).
// Это предотвращает ошибки линковки, если такая же функция есть в rsa.cpp.
BigInt inverse(const BigInt& a, const BigInt& n)
{
	BigInt t = 0, new_t = 1;
	BigInt r = n, new_r = a;

	while (new_r != 0)
	{
		BigInt quotient = r / new_r;

		BigInt temp_t = t;
		t			  = new_t;
		new_t		  = temp_t - quotient * new_t;

		BigInt temp_r = r;
		r			  = new_r;
		new_r		  = temp_r - quotient * new_r;
	}

	if (r > 1)
	{
		throw std::runtime_error("a is not invertible");
	}
	if (t < 0)
	{
		t = t + n;
	}
	return t;
}
} // namespace

namespace CRYPTO
{

ElGamal::ElGamal(unsigned int key_bits) : Protocol("El Gamal"), m_key_bits(key_bits)
{
	// Для Эль-Гамаля также требуются ключи достаточной длины.
	if (key_bits < 1)
	{
		throw std::invalid_argument("Key bits must be at least 1 for ElGamal for adequate security.");
	}
	init();
}

void ElGamal::init()
{
	generateParameters();
	generateKeys();
}

void ElGamal::generateParameters()
{
	boost::random::mt19937 rng(std::chrono::high_resolution_clock::now().time_since_epoch().count());

	// 1. Генерируем большое простое число p нужной битовой длины.
	m_p = generatePrime(m_key_bits, rng);

	// 2. Выбираем генератор g.
	// Для простоты реализации часто выбирают небольшое число (например, 2 или 5).
	// В криптографически стойких системах выбор g является более сложной задачей
	// (он должен быть порождающим элементом большой циклической подгруппы).
	// Для демонстрационных целей g = 2 является приемлемым выбором.
	m_g = 2;
}

void ElGamal::generateKeys()
{
	boost::random::mt19937 rng(std::chrono::high_resolution_clock::now().time_since_epoch().count());

	// 1. Выбираем случайный секретный ключ d
	// в диапазоне 1 < d < p-1. Используем p-1, так как размер подгруппы
	// может быть p-1 (если p - безопасное простое).
	boost::random::uniform_int_distribution<BigInt> dist(2, m_p - 2);
	m_d = dist(rng);

	// 2. Вычисляем открытый ключ e: e = g^d mod p.
	m_e = boost::multiprecision::powm(m_g, m_d, m_p);
}

BigInt ElGamal::generatePrime(unsigned int bits, boost::random::mt19937& rng)
{
	// Эта функция аналогична реализации в RSA.
	BigInt											lower_bound = BigInt(1) << (bits - 1);
	BigInt											upper_bound = (BigInt(1) << bits) - 1;
	boost::random::uniform_int_distribution<BigInt> dist(lower_bound, upper_bound);

	BigInt candidate;
	while (true)
	{
		candidate = dist(rng);
		if (candidate % 2 == 0)
		{
			candidate++;
		}
		// Тест Миллера-Рабина с 25 итерациями дает очень высокую
		// вероятность того, что сгенерированное число является простым.
		if (boost::multiprecision::miller_rabin_test(candidate, 25))
		{
			return candidate;
		}
	}
}

QString ElGamal::encrypt(const QString& plaintext)
{
	if (m_p == 0 || m_g == 0 || m_e == 0)
	{
		throw std::runtime_error("ElGamal is not initialized. Call init() before encryption.");
	}

	// 1. Преобразуем открытый текст в BigInt s (через hex, как в RSA).
	QByteArray bytes		 = plaintext.toUtf8();
	QString	   hex_plaintext = bytes.toHex();
	BigInt	   s("0x" + hex_plaintext.toStdString());

	if (s >= m_p)
	{
		throw std::runtime_error("Message is too large for the current key size.");
	}

	// 2. Выбираем случайное сессионное (эфемерное) число k: 1 < k < p-1.
	boost::random::mt19937							rng(std::chrono::high_resolution_clock::now().time_since_epoch().count());
	boost::random::uniform_int_distribution<BigInt> dist(2, m_p - 2);
	BigInt											k = dist(rng);

	// 3. Вычисляем первую компоненту шифртекста: r = g^k mod p.
	BigInt r = boost::multiprecision::powm(m_g, k, m_p);

	// 4. Вычисляем вторую компоненту шифртекста: c = s * e^k mod p.
	BigInt e_k = boost::multiprecision::powm(m_e, k, m_p);
	BigInt c   = (s * e_k) % m_p;

	// 5. Формируем шифртекст: пара (r, c) в виде hex-строк, разделенных пробелом.
	QString r_hex = QString::fromStdString(r.str(0, std::ios_base::hex));
	QString c_hex = QString::fromStdString(c.str(0, std::ios_base::hex));

	return r_hex + " " + c_hex;
}

QString ElGamal::decrypt(const QString& ciphertext)
{
	if (m_p == 0 || m_d == 0)
	{
		throw std::runtime_error("ElGamal is not initialized or private key is missing.");
	}

	// 1. Разделяем шифртекст на две hex-строки (r и c).
	QStringList parts = ciphertext.split(' ');
	if (parts.size() != 2)
	{
		throw std::runtime_error("Invalid ciphertext format for ElGamal. Expected 'r_hex c_hex'.");
	}

	// 2. Преобразуем hex-строки r и c обратно в BigInt.
	BigInt r("0x" + parts[0].toStdString());
	BigInt c("0x" + parts[1].toStdString());

	// 3. Расшифровываем: s = c * (r^d)^-1 mod p.
	//    Это эквивалентно s = c * r^(p-1-d) mod p, но вычисление через
	//    обратный элемент более прямолинейно.

	// Вычисляем r^d mod p
	BigInt r_d = boost::multiprecision::powm(r, m_d, m_p);

	// Находим мультипликативное обратное для r^d по модулю p
	BigInt r_d_inv = inverse(r_d, m_p);

	// Находим исходное сообщение s
	BigInt decrypted_message = (c * r_d_inv) % m_p;

	// 4. Преобразуем расшифрованное число обратно в QString (аналогично RSA::decrypt).
	std::string hex_str = decrypted_message.str(0, std::ios_base::hex);
	if (hex_str.length() % 2 != 0)
	{
		hex_str.insert(0, "0"); // Восстанавливаем потерянный ведущий ноль
	}

	QByteArray bytes = QByteArray::fromHex(QByteArray::fromStdString(hex_str));
	return QString::fromUtf8(bytes);
}

} // namespace CRYPTO
