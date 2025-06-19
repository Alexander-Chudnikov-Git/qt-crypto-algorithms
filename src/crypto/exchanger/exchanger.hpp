#ifndef EXCHANGER_HPP
#define EXCHANGER_HPP

#include "protocol.hpp"

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace CRYPTO
{
class ExchangerStep
{
public:
	virtual ~ExchangerStep() = default;

	virtual bool enter([[maybe_unused]] Protocol& protocol_a, [[maybe_unused]] Protocol& protocol_b)   = 0;
	virtual bool execute([[maybe_unused]] Protocol& protocol_a, [[maybe_unused]] Protocol& protocol_b) = 0;
	virtual bool exit([[maybe_unused]] Protocol& protocol_a, [[maybe_unused]] Protocol& protocol_b)	   = 0;
};

class Exchanger
{
public:
	Exchanger();

	void		 init(std::shared_ptr<Protocol> protocol_a, std::shared_ptr<Protocol> protocol_b);
	virtual void steps() = 0;
	std::tuple<bool, std::shared_ptr<Protocol>, std::shared_ptr<Protocol>> perform();
	void																   reset();

public:
	void addStep(const std::string& name, std::shared_ptr<ExchangerStep> step);
	void changeStep(const std::string& name);
	void nextStep();

private:
	std::map<std::string, std::shared_ptr<ExchangerStep>> m_steps;
	std::shared_ptr<ExchangerStep>						  m_current_step;
	std::vector<std::string>							  m_step_order;
	size_t												  m_current_index;

	std::shared_ptr<Protocol> m_protocol_a;
	std::shared_ptr<Protocol> m_protocol_b;
};
} // namespace CRYPTO

#endif // EXCHANGER_HPP
