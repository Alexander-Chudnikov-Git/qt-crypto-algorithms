#include "protocol.hpp"

namespace CRYPTO
{
Protocol::Protocol(const std::string& name) : m_protocol_name(name)
{}

const std::string& Protocol::getProtocolName() const
{
	return m_protocol_name;
}

void Protocol::setProtocolName(const std::string& name)
{
	m_protocol_name = name;
}
} // namespace CRYPTO
