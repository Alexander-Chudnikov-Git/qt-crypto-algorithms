#ifndef DIFFIE_HELLMAN_HPP
#define DIFFIE_HELLMAN_HPP

#include "protocol.hpp"

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/random/mersenne_twister.hpp>

// Используем псевдоним для больших чисел для удобства
using BigInt = boost::multiprecision::cpp_int;

namespace CRYPTO
{
/**
 * @brief Реализация протокола обмена ключами Диффи-Хеллмана.
 *
 * Этот протокол позволяет двум сторонам (например, Алисе и Бобу)
 * установить общий секретный ключ через незащищенный канал связи.
 * Безопасность основана на сложности вычисления дискретного логарифма.
 */
class DiffieHellman final : public Protocol
{
public:
	/**
	 * @brief           Конструктор объекта протокола Diffie-Hellman.
	 * @param name      Имя участника (например, "Алиса" или "Боб").
	 * @param prime_bits Желаемая битовая длина простого модуля p.
	 *                   Безопасные значения обычно начинаются с 2048.
	 *                   Для демонстрации используются меньшие значения.
	 */
	explicit DiffieHellman(unsigned int prime_bits = 512);

	/**
	 * @brief Инициализирует участника, генерируя публичные параметры (p, g)
	 *        и личную пару ключей (закрытый и открытый).
	 */
	void init() override;

	/**
	 * @brief           "Шифрует" сообщение, используя установленный общий секрет.
	 *
	 * @details         Примечание: Диффи-Хеллман - это протокол обмена ключами,
	 *                  а не шифрования. Этот метод просто демонстрирует, что
	 *                  общий ключ был успешно создан, используя его для
	 *                  простого преобразования (XOR).
	 * @param plaintext Открытый текст для шифрования.
	 * @return          Зашифрованные данные в виде шестнадцатеричной строки.
	 */
	QString encrypt(const QString& plaintext) override;

	/**
	 * @brief            "Расшифровывает" сообщение, используя общий секрет.
	 * @param ciphertext Зашифрованные данные в виде шестнадцатеричной строки.
	 * @return           Исходный открытый текст.
	 */
	QString decrypt(const QString& ciphertext) override;

	// --- Методы, специфичные для протокола Диффи-Хеллмана ---

	/**
	 * @brief Вычисляет общий секретный ключ.
	 * Формула: s = (B^a) mod p, где B - открытый ключ другого участника,
	 * a - наш закрытый ключ.
	 */
	void generateSharedSecret();

	// Геттеры и сеттеры для процесса обмена
	BigInt getPrime();
	BigInt getPublicKey() const;

	void setOtherPartyPublicKey(const BigInt& key);
	void setOtherPartyPrime(const BigInt& prime);

private:
	/**
	 * @brief Генерирует все необходимые параметры: p, g, закрытый и открытый ключи.
	 */
	void generateParameters();
	void generateKeys();

	/**
	 * @brief      Генерирует случайное простое число указанной битовой длины.
	 * @param bits Битовая длина простого числа.
	 * @param rng  Генератор случайных чисел.
	 * @return     Большое простое число.
	 */
	BigInt generatePrime(unsigned int bits, boost::random::mt19937& rng);

private:
	// --- Параметры и ключи Диффи-Хеллмана ---
	unsigned int m_prime_bits;

	// Публичные параметры, общие для обеих сторон
	BigInt m_p; // Простое число-модуль `p`
	BigInt m_g; // Первообразный корень (генератор) `g`

	// Ключи этого участника
	BigInt m_private_key; // Закрытый ключ (секретное число `a` или `b`)
	BigInt m_public_key;  // Открытый ключ (A = g^a mod p или B = g^b mod p)

	// Данные, полученные от другого участника
	BigInt m_other_party_public_key; // Открытый ключ другого участника (A или B)

	// Финальный результат
	BigInt m_shared_secret; // Общий секретный ключ `s = (B^a) mod p = (A^b) mod p`
};

} // namespace CRYPTO

#endif // DIFFIE_HELLMAN_HPP
