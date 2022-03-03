#ifndef _DEF_MUNU_IO_ASYNC_DEVICE_WRITABLE_H_
#define _DEF_MUNU_IO_ASYNC_DEVICE_WRITABLE_H_

#include <munu_io/AsyncDevice.h>

namespace munu {

template <typename DeviceT,
          typename TimeSourceT = std::chrono::system_clock>
class AsyncDeviceWritable : public AsyncDevice<DeviceT, TimeSourceT>
{
    public:

    using BaseType     = AsyncDevice<DeviceT, TimeSourceT>;
    using Device       = typename BaseType::Device;
    using AsyncHandler = typename BaseType::AsyncHandler;

    public:

    AsyncDeviceWritable(boost::asio::io_service& service) : BaseType(service) {}

    void async_write(size_t count, const char* data, const AsyncHandler& handler);
    void async_write(const std::string& data, const AsyncHandler& handler);
};

template <typename D, typename T>
void AsyncDeviceWritable<D,T>::async_write(size_t count,
                                           const char* data,
                                           const AsyncHandler& handler)
{
    // Check if wrte completed
    boost::asio::async_write(this->device_, boost::asio::buffer(data, count), handler);
}

template <typename D, typename T>
void AsyncDeviceWritable<D,T>::async_write(const std::string& data,
                                           const AsyncHandler& handler)
{
    this->async_write(data.size(), data.c_str(), handler);
}

}; //namespace munu


#endif //_DEF_MUNU_IO_ASYNC_DEVICE_WRITABLE_H_
