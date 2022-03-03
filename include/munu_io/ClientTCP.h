#ifndef _DEF_MUNU_IO_CLIENT_TCP_H_
#define _DEF_MUNU_IO_CLIENT_TCP_H_

#include <munu_io/AsyncDeviceWritable.h>

namespace munu {

template <template <typename,typename>class AsyncDeviceT = AsyncDeviceWritable,
          typename TimeSourceT = std::chrono::system_clock>
class ClientTCP : public AsyncDeviceT<boost::asio::ip::tcp::socket, TimeSourceT>
{
    public:

    using Socket   = boost::asio::ip::tcp::socket;
    using BaseType = AsyncDeviceT<Socket, TimeSourceT>;

    using EndPoint = boost::asio::ip::tcp::endpoint;

    public:
    
    ClientTCP(boost::asio::io_service& service) : BaseType(service) {}

    void open(const std::string& ip, uint16_t port);
};

template <template<typename,typename>class D, typename T>
void ClientTCP<D,T>::open(const std::string& ip, uint16_t port)
{
    this->device_.connect(EndPoint(boost::asio::ip::address_v4::from_string(ip), port));
}

}; //namespace munu

#endif //_DEF_MUNU_IO_CLIENT_TCP_H_
