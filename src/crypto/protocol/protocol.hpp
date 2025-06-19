#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include <QByteArray>
#include <string>

namespace CRYPTO
{
class Protocol
{
public:
	Protocol() = default;
	explicit Protocol(const std::string& name);
	virtual ~Protocol() = 0;

public:
	virtual void init() = 0;

	virtual QString encrypt(const QString& plaintext) = 0;
	virtual QString decrypt(const QString& plaintext) = 0;

	const std::string& getProtocolName() const;

	void setProtocolName(const std::string& name);

private:
	std::string m_protocol_name;
};

inline Protocol::~Protocol() = default;

} // namespace CRYPTO

#endif // PROTOCOL_HPP
