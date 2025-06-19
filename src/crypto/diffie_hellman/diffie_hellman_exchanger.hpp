#ifndef DIFFIE_HELLMAN_EXCHANGER_HPP
#define DIFFIE_HELLMAN_EXCHANGER_HPP

#include "diffie_hellman.hpp"
#include "exchanger.hpp"
#include "settings_defaults.hpp"
#include "spdlog_wrapper.hpp"

namespace CRYPTO
{
// ШАГ 1: Генерация открытых ключей
class GeneratePublicKeys : public ExchangerStep
{
public:
	bool enter(Protocol&, Protocol&) override
	{
		SPD_INFO_CLASS(UTILS::DEFAULTS::d_logger_dh, QObject::tr("diffie_hellman_open_key_generation_enter"));
		return true;
	}

	bool execute(Protocol& protocol_a, Protocol& protocol_b) override
	{
		SPD_INFO_CLASS(UTILS::DEFAULTS::d_logger_dh, QObject::tr("diffie_hellman_open_key_generation_execute"));

		auto* dh_a = dynamic_cast<DiffieHellman*>(&protocol_a);
		auto* dh_b = dynamic_cast<DiffieHellman*>(&protocol_b);

		if (!dh_a || !dh_b)
		{
			return false;
		}

		dh_a->setOtherPartyPrime(dh_b->getPrime());

		return true;
	}

	bool exit(Protocol&, Protocol&) override
	{
		SPD_INFO_CLASS(UTILS::DEFAULTS::d_logger_dh, QObject::tr("diffie_hellman_open_key_generation_exit"));
		return true;
	}
};

// ШАГ 2: Обмен открытыми ключами
class ExchangePublicKeys : public ExchangerStep
{
public:
	bool enter(Protocol&, Protocol&) override
	{
		SPD_INFO_CLASS(UTILS::DEFAULTS::d_logger_dh, QObject::tr("diffie_hellman_open_key_exchange_enter"));
		return true;
	}

	bool execute(Protocol& protocol_a, Protocol& protocol_b) override
	{
		SPD_INFO_CLASS(UTILS::DEFAULTS::d_logger_dh, QObject::tr("diffie_hellman_open_key_exchange_execute"));

		auto* dh_a = dynamic_cast<DiffieHellman*>(&protocol_a);
		auto* dh_b = dynamic_cast<DiffieHellman*>(&protocol_b);

		if (!dh_a || !dh_b)
		{
			return false;
		}

		// Имитация обмена: A получает ключ B, B получает ключ A
		dh_a->setOtherPartyPublicKey(dh_b->getPublicKey());
		dh_b->setOtherPartyPublicKey(dh_a->getPublicKey());

		return true;
	}

	bool exit(Protocol&, Protocol&) override
	{
		SPD_INFO_CLASS(UTILS::DEFAULTS::d_logger_dh, QObject::tr("diffie_hellman_open_key_exchange_exit"));
		return true;
	}
};

// ШАГ 3: Вычисление общего секрета
class ComputeSharedSecret : public ExchangerStep
{
public:
	bool enter(Protocol&, Protocol&) override
	{
		SPD_INFO_CLASS(UTILS::DEFAULTS::d_logger_dh, QObject::tr("diffie_hellman_shared_secret_calculation_enter"));
		return true;
	}

	bool execute(Protocol& protocol_a, Protocol& protocol_b) override
	{
		SPD_INFO_CLASS(UTILS::DEFAULTS::d_logger_dh, QObject::tr("diffie_hellman_shared_secret_calculation_execute"));
		auto* dh_a = dynamic_cast<DiffieHellman*>(&protocol_a);
		auto* dh_b = dynamic_cast<DiffieHellman*>(&protocol_b);

		if (!dh_a || !dh_b)
			return false;

		// Каждая сторона независимо вычисляет общий секрет
		dh_a->generateSharedSecret();
		dh_b->generateSharedSecret();
		return true;
	}

	bool exit(Protocol&, Protocol&) override
	{
		SPD_INFO_CLASS(UTILS::DEFAULTS::d_logger_dh, QObject::tr("diffie_hellman_shared_secret_calculation_exit"));
		return true;
	}
};

// Конкретный Exchanger для протокола Диффи-Хеллмана
class DiffieHellmanExchanger : public Exchanger
{
public:
	void steps() override;
};

} // namespace CRYPTO

#endif // DIFFIE_HELLMAN_EXCHANGER_HPP
