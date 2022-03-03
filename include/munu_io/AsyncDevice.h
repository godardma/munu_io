#ifndef _DEF_MUNU_IO_ASYNC_DEVICE_H_
#define _DEF_MUNU_IO_ASYNC_DEVICE_H_

#include <memory>

#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/asio.hpp>
#include <boost/bind.hpp>

namespace munu {

template <typename DeviceT>
class AsyncDevice
{
    public:

    // using Ptr      = std::shared_ptr<AsyncDevice>;
    // using ConstPtr = std::shared_ptr<const AsyncDevice>;

    using Device = DeviceT;

    protected:
    
    Device device_;

    public:
    
    AsyncDevice(boost::asio::io_service& service) : device_(service) {}
    
    template <typename Handler>
    void async_read(size_t count, char* dst, Handler handler);

    size_t read(size_t count, char* dst);
};

template <typename D> template <typename Handler>
void AsyncDevice<D>::async_read(size_t count, char* dst, Handler handler)
{
    boost::asio::async_read(device_, boost::asio::buffer(dst, count), handler);
}

template <typename D>
size_t AsyncDevice<D>::read(size_t count, char* dst)
{
    throw std::runtime_error("Not debugged");
    std::future<size_t> f = boost::asio::async_read(device_, boost::asio::buffer(dst, count),
                                                    boost::asio::use_future);
    return f.get(); // will wait
}

}; //namespace munu

#endif //_DEF_MUNU_IO_ASYNC_DEVICE_H_
