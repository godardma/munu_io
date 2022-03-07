#ifndef _DEF_MUNU_IO_ASYNC_DEVICE_WRITABLE_H_
#define _DEF_MUNU_IO_ASYNC_DEVICE_WRITABLE_H_

#include <munu_io/AsyncDevice.h>

namespace munu {

template <typename DeviceT,
          typename TimeSourceT = std::chrono::system_clock>
class AsyncDeviceWritable : public AsyncDevice<DeviceT, TimeSourceT>
{
    public:

    using BaseType      = AsyncDevice<DeviceT, TimeSourceT>;
    using Device        = typename BaseType::Device;
    using WriteCallback = boost::function<void(const boost::system::error_code&, size_t)>;

    public:

    AsyncDeviceWritable(boost::asio::io_service& service) : BaseType(service) {}

    void async_write(size_t count, const char* data, const WriteCallback& handler);
    void async_write(const std::string& data, const WriteCallback& handler);
    
    // synchronous operation
    size_t write(size_t count, const char* data);
    size_t write(const std::string& data);
};

template <typename D, typename T>
void AsyncDeviceWritable<D,T>::async_write(size_t count,
                                           const char* data,
                                           const WriteCallback& handler)
{
    // Check if wrte completed
    boost::asio::async_write(this->device_, boost::asio::buffer(data, count), handler);
}

template <typename D, typename T>
void AsyncDeviceWritable<D,T>::async_write(const std::string& data,
                                           const WriteCallback& handler)
{
    this->async_write(data.size(), data.c_str(), handler);
}

template <typename D, typename T>
size_t AsyncDeviceWritable<D,T>::write(size_t count, const char* data)
{
    // Check if wrte completed
    return boost::asio::write(this->device_, boost::asio::buffer(data, count));
}

template <typename D, typename T>
size_t AsyncDeviceWritable<D,T>::write(const std::string& data)
{
    return this->write(data.size(), data.c_str());
}


}; //namespace munu


#endif //_DEF_MUNU_IO_ASYNC_DEVICE_WRITABLE_H_
