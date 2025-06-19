#include "rsa.hpp"

#include <QByteArray>
#include <QString>
#include <boost/multiprecision/integer.hpp>
#include <boost/multiprecision/miller_rabin.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <chrono>
#include <stdexcept>

namespace CRYPTO
{

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

RSA::RSA(unsigned int key_bits) : Protocol("RSA"), m_key_bits(key_bits)
{
	if (key_bits % 2 != 0 || key_bits < 1)
	{
		throw std::invalid_argument("Key bits must be an even number and at least 1.");
	}
	init();
}

void RSA::init()
{
	generateKeys();
}

void RSA::generateKeys()
{
	// 1. Инициализация генератора случайных чисел
	boost::random::mt19937 rng(std::chrono::high_resolution_clock::now().time_since_epoch().count());

	// 2. Генерация двух различных простых чисел p и q
	unsigned int prime_bits = m_key_bits / 2;
	do
	{
		m_p = generatePrime(prime_bits, rng);
		m_q = generatePrime(prime_bits, rng);
	} while (m_p == m_q);

	// 3. Вычисление модуля n и функции Эйлера phi(n)
	m_m	  = m_p * m_q;
	m_phi = (m_p - 1) * (m_q - 1);

	// 4. Выбор открытой экспоненты e
	// Часто используется 65537, так как это простое число Ферма, что ускоряет шифрование.
	m_e = 65537;
	// Убедимся, что gcd(e, phi) == 1. Если нет, нужно выбрать другое e.
	// Для больших случайных p и q это условие почти всегда выполняется.
	if (boost::multiprecision::gcd(m_e, m_phi) != 1)
	{
		// В маловероятном случае, если 65537 не подходит, генерируем ключи заново
		generateKeys();
		return;
	}

	// 5. Вычисление закрытой экспоненты d (d * e ≡ 1 (mod phi))
	m_d = inverse(m_e, m_phi);
}

BigInt RSA::generatePrime(unsigned int bits, boost::random::mt19937& rng)
{
	// Задаем диапазон для генерации числа с нужным количеством бит
	BigInt											lower_bound = BigInt(1) << (bits - 1);
	BigInt											upper_bound = (BigInt(1) << bits) - 1;
	boost::random::uniform_int_distribution<BigInt> dist(lower_bound, upper_bound);

	BigInt candidate;
	while (true)
	{
		candidate = dist(rng);
		// Убедимся, что число нечетное
		if (candidate % 2 == 0)
		{
			candidate++;
		}
		// Проверяем на простоту с помощью теста Миллера-Рабина
		// 25 итераций дают очень высокую вероятность того, что число простое
		if (boost::multiprecision::miller_rabin_test(candidate, 25))
		{
			return candidate;
		}
	}
}

QString RSA::encrypt(const QString& plaintext)
{
	if (m_m == 0 || m_e == 0)
	{
		throw std::runtime_error("RSA is not initialized. Call init() before encryption.");
	}

	// 1. Преобразуем открытый текст в байты
	QByteArray bytes = plaintext.toUtf8();

	// 2. Преобразуем байты в шестнадцатеричную строку.
	QString hex_plaintext = bytes.toHex();

	// 3. Преобразуем шестнадцатеричную строку в BigInt.
	BigInt message("0x" + hex_plaintext.toStdString());

	// 4. Проверяем, что сообщение меньше модуля n
	if (message >= m_m)
	{
		throw std::runtime_error("Message is too large for the current key size.");
	}

	// 5. Шифруем: c = m^e mod n
	BigInt ciphertext = boost::multiprecision::powm(message, m_e, m_m);

	// 6. Преобразуем зашифрованное число в строку в шестнадцатеричном формате для передачи
	return QString::fromStdString(ciphertext.str(0, std::ios_base::hex));
}

QString RSA::decrypt(const QString& ciphertext)
{
	if (m_m == 0 || m_d == 0)
	{
		throw std::runtime_error("RSA is not initialized. Call init() before decryption.");
	}

	// 1. Преобразуем шифротекст (шестнадцатеричная строка) в большое число (BigInt)
	BigInt encrypted_message("0x" + ciphertext.toStdString());

	// 2. Расшифровываем: m = c^d mod n
	BigInt decrypted_message = boost::multiprecision::powm(encrypted_message, m_d, m_m);

	// 3. Преобразуем расшифрованное число обратно в шестнадцатеричную строку
	std::string hex_str = decrypted_message.str(0, std::ios_base::hex);

	// 4. Если в результате преобразования получилась строка
	// с нечетной длиной, значит был утерян ведущий ноль (например, 0x0F -> "f").
	// Его необходимо восстановить.
	if (hex_str.length() % 2 != 0)
	{
		hex_str.insert(0, "0");
	}

	// 5. Преобразуем шестнадцатеричную строку обратно в QByteArray
	QByteArray bytes = QByteArray::fromHex(QByteArray::fromStdString(hex_str));

	// 6. Создаем QString из байтов в кодировке UTF-8. Теперь это корректно.
	return QString::fromUtf8(bytes);
}

} // namespace CRYPTO
