#ifndef RSA_HPP
#define RSA_HPP

#include "protocol.hpp"

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/random/mersenne_twister.hpp>

using BigInt = boost::multiprecision::cpp_int;

namespace CRYPTO
{
class RSA final : public Protocol
{
public:
	/**
	 * @brief          Конструирует объект протокола RSA.
	 * @param key_bits Желаемая битовая длина модуля n. Должна быть четным числом.
	 *                 Типичные безопасные значения: 2048 или 4096.
	 */
	explicit RSA(unsigned int key_bits = 2048);

	/**
	 * @brief Инициализирует объект RSA путем генерации открытого и закрытого ключей.
	 * Это должно быть вызвано перед любыми операциями шифрования или дешифрования.
	 */
	void init() override;

	/**
	 * @brief           Шифрует блок данных, используя открытый ключ.
	 * @param plaintext Данные для шифрования. Его размер, интерпретируемый как целое число, должен быть меньше модуля n.
	 * @return          Зашифрованные данные (шифротекст), дополненные до размера модуля.
	 */
	QString encrypt(const QString& plaintext) override;

	/**
	 * @brief            Расшифровывает блок данных, используя закрытый ключ.
	 * @param ciphertext Данные для расшифровки.
	 * @return           Исходные данные (открытый текст).
	 */
	QString decrypt(const QString& ciphertext) override;

private:
	/**
	 * @brief Генерирует полную пару ключей RSA (p, q, n, phi, e, d).
	 */
	void generateKeys();

	/**
	 * @brief      Генерирует случайное простое число определенной битовой длины.
	 * @param bits Желаемая битовая длина простого числа.
	 * @param rng  Используемый генератор случайных чисел.
	 * @return     Простое число длиной приблизительно 'bits'.
	 */
	BigInt generatePrime(unsigned int bits, boost::random::mt19937& rng);

	// Параметры RSA
	unsigned int m_key_bits;
	BigInt		 m_m; // Модуль m = p * q (часть открытого ключа)
	BigInt		 m_e; // Открытая экспонента e (часть открытого ключа)
	BigInt		 m_d; // Закрытая экспонента d (закрытый ключ)

	// Простые множители сохраняются для потенциального использования, но являются частью закрытого ключа
	BigInt m_p, m_q;
	BigInt m_phi; // Функция Эйлера: phi(n) = (p-1)*(q-1)
};
} // namespace CRYPTO

#endif // RSA_HPP
