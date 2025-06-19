#ifndef ELGAMAL_HPP
#define ELGAMAL_HPP

#include "protocol.hpp" // Для наследования от базового класса Protocol

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/random/mersenne_twister.hpp>

// Определяем псевдоним для типа больших целых чисел, как в rsa.hpp
using BigInt = boost::multiprecision::cpp_int;

namespace CRYPTO
{

/**
 * @brief Реализация асимметричной криптосистемы Эль-Гамаля.
 *
 * Эта схема основана на сложности задачи дискретного логарифмирования.
 * Является вероятностным алгоритмом шифрования.
 */
class ElGamal final : public Protocol
{
public:
	/**
	 * @brief           Конструирует объект протокола Эль-Гамаля.
	 * @param key_bits  Желаемая битовая длина простого модуля p.
	 *                  Типичные безопасные значения: 2048 или 3072.
	 */
	explicit ElGamal(unsigned int key_bits = 2048);

	/**
	 * @brief Инициализирует объект Эль-Гамаля путем генерации публичных параметров,
	 * а также открытого и закрытого ключей. Этот метод должен быть вызван
	 * перед любыми операциями шифрования или дешифрования.
	 */
	void init() override;

	/**
	 * @brief           Шифрует блок данных, используя открытый ключ получателя.
	 * @param plaintext Данные для шифрования. Его размер, интерпретируемый как
	 *                  целое число, должен быть меньше модуля p.
	 * @return          Зашифрованные данные (шифротекст) в виде пары чисел (r, c),
	 *                  представленных как hex-строки, разделенные пробелом.
	 */
	QString encrypt(const QString& plaintext) override;

	/**
	 * @brief            Расшифровывает блок данных, используя закрытый ключ.
	 * @param ciphertext Данные для расшифровки в формате "r_hex c_hex".
	 * @return           Исходные данные (открытый текст).
	 */
	QString decrypt(const QString& ciphertext) override;

private:
	/**
	 * @brief Генерирует публичные параметры (p, g) для системы Эль-Гамаля.
	 */
	void generateParameters();

	/**
	 * @brief Генерирует ключевую пару (закрытый ключ d, открытый ключ e).
	 */
	void generateKeys();

	/**
	 * @brief      Генерирует случайное простое число определенной битовой длины.
	 * @param bits Желаемая битовая длина простого числа.
	 * @param rng  Используемый генератор случайных чисел.
	 * @return     Простое число длиной приблизительно 'bits'.
	 */
	BigInt generatePrime(unsigned int bits, boost::random::mt19937& rng);

	// --- Параметры Эль-Гамаля ---

	unsigned int m_key_bits; // Длина ключа в битах

	// Публичные параметры, общие для всех
	BigInt m_p; // Большое простое число (модуль)
	BigInt m_g; // Порождающий элемент (генератор)

	// Ключи
	BigInt m_d; // Закрытый ключ (секретное число, 1 < d < p-1)
	BigInt m_e; // Открытый ключ (e = g^d mod p)
};

} // namespace CRYPTO

#endif // ELGAMAL_HPP