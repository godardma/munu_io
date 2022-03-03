#ifndef _DEF_MUNU_IO_CLIENT_UDP_H_
#define _DEF_MUNU_IO_CLIENT_UDP_H_

#include <munu_io/AsyncDevice.h>

namespace munu {

template <template <typename,typename>class AsyncDeviceT = AsyncDevice,
          typename TimeSourceT = std::chrono::system_clock>
class ClientUDP : public AsyncDeviceT<boost::asio::ip::udp::socket, TimeSourceT>
{
    public:

    using Socket   = boost::asio::ip::udp::socket;
    using BaseType = AsyncDeviceT<Socket, TimeSourceT>;

    using EndPoint = boost::asio::ip::udp::endpoint;

    public:
    
    ClientUDP(boost::asio::io_service& service) : BaseType(service) {}

    void open(const EndPoint& endpoint);
    void open(const std::string& ip, uint16_t port);
};

template <template<typename,typename>class D, typename T>
void ClientUDP<D,T>::open(const EndPoint& endPoint)
{
    this->device_.connect(endPoint);
}

template <template<typename,typename>class D, typename T>
void ClientUDP<D,T>::open(const std::string& ip, uint16_t port)
{
    this->open(EndPoint(boost::asio::ip::address_v4::from_string(ip), port));
}

}; //namespace munu

#endif //_DEF_MUNU_IO_CLIENT_UDP_H_

