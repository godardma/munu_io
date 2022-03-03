#ifndef _DEF_MUNU_IO_ASYNC_DEVICE_H_
#define _DEF_MUNU_IO_ASYNC_DEVICE_H_

#include <memory>
#include <chrono>
#include <type_traits>

#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

namespace munu {

template <typename DeviceT,
          typename TimeSourceT = std::chrono::system_clock>
class AsyncDevice
{
    public:

    using Device = DeviceT;
    using TimeSource = TimeSourceT;
    using TimePoint  = typename std::invoke_result<decltype(&TimeSource::now)>::type;
    
    using AsyncHandler = boost::function<void(const boost::system::error_code&, size_t)>;

    protected:

    Device    device_;
    TimePoint stamp_;

    public:

    AsyncDevice(boost::asio::io_service& service);

    void read_callback(const AsyncHandler& handler,
                       const boost::system::error_code& err,
                       size_t readCount);
    void async_read(size_t count, char* dst, const AsyncHandler& handler);
    size_t read(size_t count, char* dst);

    TimePoint timestamp() const { return stamp_; }
};

template <typename D,typename T>
AsyncDevice<D,T>::AsyncDevice(boost::asio::io_service& service) :
    device_(service),
    stamp_(TimeSource::now())
{}

/**
 * This method only serves to record current time when the user callback is
 * called.
 */
template <typename D,typename T>
void AsyncDevice<D,T>::read_callback(const AsyncHandler& handler,
                                     const boost::system::error_code& err,
                                     size_t readCount)
{
    stamp_ = TimeSource::now();
    handler(err, readCount);
}

template <typename D,typename T>
void AsyncDevice<D,T>::async_read(size_t count, char* dst,
                                  const AsyncHandler& handler)
{
    boost::asio::async_read(device_, boost::asio::buffer(dst, count),
        boost::bind(&AsyncDevice<D,T>::read_callback, this, handler, _1, _2));
}

template <typename D, typename T>
size_t AsyncDevice<D,T>::read(size_t count, char* dst)
{
    throw std::runtime_error("Not debugged");
    std::future<size_t> f = boost::asio::async_read(device_, boost::asio::buffer(dst, count),
                                                    boost::asio::use_future);
    return f.get(); // will wait
}

}; //namespace munu

#endif //_DEF_MUNU_IO_ASYNC_DEVICE_H_
