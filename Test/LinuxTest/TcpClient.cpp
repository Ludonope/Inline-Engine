#include "TcpClient.hpp"
#include "Util.hpp"
#include "Init.hpp"

namespace inl::net::tcp
{
	TcpClient::TcpClient(const std::string &ip, int port) :
		ip(ip), port(port)
	{
		initialize(ip, port);
	}

	TcpClient::TcpClient(const SOCKET & socket)
	{
		this->soc = socket;
	}

	TcpClient::~TcpClient()
	{
		//disconnect
		freeaddrinfo(result);
		util::Delete(result);
	}

	bool TcpClient::Connect(const std::string & ip, int port)
	{
		initialize(ip, port);
		if (connected = (connect(soc, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR))
			closesocket(soc);
		freeaddrinfo(result);
		return connected;
	}

	bool TcpClient::DataAvailable(int &size)
	{
		unsigned long ulong;
		return ioctlsocket(soc, FIONREAD, &ulong) == NO_ERROR && (size = ulong) > 0;
	}

	bool TcpClient::ReceiveMessage(NetworkMessage & message)
	{
		NetworkBuffer buf = receive_buffer();
		if (buf.Valid)
		{
			message = NetworkMessage::DecodeMessage(buf);
			return true;
		}
		return false;
	}

	bool TcpClient::SendMessage(const NetworkMessage & message)
	{
		NetworkBuffer buf = NetworkMessage::EncodeMessage(message);
		return buf.Valid ? send_buffer(buf) : false;
	}

	bool TcpClient::initialize(const std::string &ip, int port)
	{
		if (!win32::Initialized)
			win32::Initialize();
		if (util::ValidIPv4Addr(ip) || port == 0)
			return false;
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		if (getaddrinfo(ip.c_str(), util::IntToStr(port), &hints, &result) != 0)
		{
			return false;
		}

		if ((soc = socket(result->ai_family, result->ai_socktype, result->ai_protocol)) == INVALID_SOCKET)
		{
			freeaddrinfo(result);
			return false;
		}

		unsigned long nNoBlock = 1;
		return ioctlsocket(soc, FIONBIO, &nNoBlock) == NO_ERROR;
	}

	NetworkBuffer TcpClient::receive_buffer()
	{
		NetworkBuffer buffer;

		int temp;
		if (DataAvailable(temp) && temp > sizeof(int))
		{
			char *header = new char[sizeof(int)]();
			if (recv(soc, header, sizeof(int), 0) != sizeof(int))
				return NetworkBuffer();
			try
			{
				buffer.BodySize = std::stoi(header);
			}
			catch (std::exception)
			{
				// header var doesnt contain int
			}
		}
		else
			return NetworkBuffer();

		buffer.Body = new char[buffer.BodySize]();
		int received_bytes = recv(soc, buffer.Body, buffer.BodySize, 0);
		if (received_bytes == SOCKET_ERROR || received_bytes != buffer.BodySize || WSAGetLastError() != 0)
			return NetworkBuffer();
		buffer.Valid = true; // we presume its valid
		return buffer;
	}

	bool TcpClient::send_buffer(const NetworkBuffer & net_buffer)
	{
		if (!net_buffer.Valid)
			return false;
		char *bytes = strcat(util::IntToStr(net_buffer.BodySize), net_buffer.Body);
		int length = strlen(bytes);
		int bytes_sent = send(soc, bytes, length, 0);
		// do some sort of checking and maybe log it somewhere, we need a logger
		return bytes_sent != SOCKET_ERROR || bytes_sent != length || WSAGetLastError() != 0;
	}
}