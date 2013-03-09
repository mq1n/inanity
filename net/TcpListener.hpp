#ifndef ___INANITY_NET_TCP_LISTENER_HPP___
#define ___INANITY_NET_TCP_LISTENER_HPP___

#include "net.hpp"

BEGIN_INANITY_NET

/// Абстрактный класс серверного TCP-сокета.
class TcpListener : public Object
{
public:
	/// Закрыть сокет.
	virtual void Close() = 0;
};

END_INANITY_NET

#endif
