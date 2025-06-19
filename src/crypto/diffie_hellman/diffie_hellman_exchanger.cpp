#include "diffie_hellman_exchanger.hpp"

namespace CRYPTO
{

void DiffieHellmanExchanger::steps()
{
	this->addStep(QObject::tr("diffie_hellman_step_generation").toStdString(), std::make_shared<GeneratePublicKeys>());
	this->addStep(QObject::tr("diffie_hellman_step_exchange").toStdString(), std::make_shared<ExchangePublicKeys>());
	this->addStep(QObject::tr("diffie_hellman_step_computation").toStdString(), std::make_shared<ComputeSharedSecret>());
}

} // namespace CRYPTO
