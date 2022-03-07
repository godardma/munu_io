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
    using StreamBuffer = boost::asio::streambuf;

    protected:

    Device       device_;
    TimePoint    stamp_;
    StreamBuffer readBuffer_;

    template <typename StreamT>
    std::pair<bool,unsigned int> read_buffer_until(char delimiter, StreamT& dst);

    public:

    AsyncDevice(boost::asio::io_service& service);

    void read_callback(const AsyncHandler& handler,
                       const boost::system::error_code& err,
                       size_t readCount);
    void async_read(size_t count, char* dst, const AsyncHandler& handler);
    size_t read(size_t count, char* dst);
    
    template <typename StreamT>
    void async_read_until(char delimiter, StreamT* dst, const AsyncHandler& handler);
    template <typename StreamT>
    void read_until_callback(char delimiter,
                             StreamT* dst,
                             const AsyncHandler& handler,
                             const boost::system::error_code& err,
                             size_t readCount);

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


template <typename D,typename T> template <typename StreamT>
std::pair<bool,unsigned int> AsyncDevice<D,T>::read_buffer_until(char delimiter, StreamT& dst)
{
    std::istream is(&readBuffer_);
    unsigned int count = 0;
    char c = is.get();
    while(!is.eof()) {
        //std::cout << c << std::flush;
        dst << c;
        count++;
        if(c == delimiter)
            return std::make_pair(true, count);
        c = is.get();
    }

    return std::make_pair(false, count);
}

template <typename D,typename T> template <typename StreamT>
void AsyncDevice<D,T>::async_read_until(char delimiter,
                                        StreamT* dst,
                                        const AsyncHandler& handler)
{
    // First checking the read buffer (enough data might be already in there
    auto res = this->read_buffer_until(delimiter, *dst);
    if(res.first) {
        handler(boost::system::error_code(), res.second);
    }
    else {
        boost::asio::async_read_until(device_, readBuffer_, delimiter,
            boost::bind(&AsyncDevice<D,T>::read_until_callback<StreamT>, this,
                        delimiter, dst,
                        handler, _1, _2));
    }
}

template <typename D,typename T> template <typename StreamT>
void AsyncDevice<D,T>::read_until_callback(char delimiter, 
                                           StreamT* dst,
                                           const AsyncHandler& handler,
                                           const boost::system::error_code& err,
                                           size_t readCount)
{
    if(err) {
        handler(err, readCount);
        return;
    }
    auto res = this->read_buffer_until(delimiter, *dst);
    if(res.first) {
        // got data, calling handler
        handler(err, res.second);
    }
    else {
        // delimiter was not found in received data but no error was returned
        // by boost. This won't probably happen (I think...)
        handler(err, 0);
    }
}

}; //namespace munu

#endif //_DEF_MUNU_IO_ASYNC_DEVICE_H_
