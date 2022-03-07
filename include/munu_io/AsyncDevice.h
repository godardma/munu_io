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
    //using TimePoint  = typename std::invoke_result<decltype(&TimeSource::now)>::type;
    using TimePoint  = typename TimeSource::time_point;
    
    using ReadCallback      = boost::function<void(const boost::system::error_code&, size_t)>;
    using ReadUntilCallback = boost::function<void(const boost::system::error_code&,
                                                   const std::string&)>;
    using StreamBuffer      = boost::asio::streambuf;

    protected:

    Device             device_;
    TimePoint          stamp_;
    StreamBuffer       recvBuffer_;
    std::ostringstream outputBuffer_;

    virtual void data_received(unsigned int count) { stamp_ = TimeSource::now(); }
    virtual void data_flushed(unsigned int count)  {}

    bool read_buffer_until(char delimiter);

    public:

    AsyncDevice(boost::asio::io_service& service);
    
    // These read a fixed number of characters
    void read_callback(const ReadCallback& handler,
                       const boost::system::error_code& err,
                       size_t readCount);
    void async_read(size_t count, char* dst, const ReadCallback& handler);
    size_t read(size_t count, char* dst);
    
    // These read until finding a delimiter (std::string only)
    void async_read_until(char delimiter, const ReadUntilCallback& handler);
    void read_until_callback(char delimiter,
                             const ReadUntilCallback& handler,
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
void AsyncDevice<D,T>::read_callback(const ReadCallback& handler,
                                     const boost::system::error_code& err,
                                     size_t readCount)
{
    this->data_received(readCount);
    handler(err, readCount);
}

template <typename D,typename T>
void AsyncDevice<D,T>::async_read(size_t count, char* dst,
                                  const ReadCallback& handler)
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


template <typename D,typename T>
bool AsyncDevice<D,T>::read_buffer_until(char delimiter)
{
    std::istream is(&recvBuffer_);
    unsigned int count = 0;
    char c = is.get();
    while(!is.eof()) {
        outputBuffer_ << c;
        count++;
        if(c == delimiter)
            return true;
        c = is.get();
    }

    return false;
}

template <typename D,typename T>
void AsyncDevice<D,T>::async_read_until(char delimiter,
                                        const ReadUntilCallback& handler)
{
    // First checking the read buffer (enough data might be already in there
    outputBuffer_.str("");
    outputBuffer_.clear();
    if(this->read_buffer_until(delimiter)) {
        this->data_flushed(outputBuffer_.str().size());
        handler(boost::system::error_code(), outputBuffer_.str());
    }
    else {
        boost::asio::async_read_until(device_, recvBuffer_, delimiter,
            boost::bind(&AsyncDevice<D,T>::read_until_callback, this,
                        delimiter, handler, _1, _2));
    }
}

template <typename D,typename T>
void AsyncDevice<D,T>::read_until_callback(char delimiter, 
                                           const ReadUntilCallback& handler,
                                           const boost::system::error_code& err,
                                           size_t readCount)
{
    if(err) {
        handler(err, std::string());
        return;
    }
    if(readCount > 0) {
        this->data_received(recvBuffer_.size());
    }
    if(this->read_buffer_until(delimiter)) {
        // got data, calling handler
        this->data_flushed(outputBuffer_.str().size());
        handler(err, outputBuffer_.str());
    }
    else {
        // delimiter was not found in received data but no error was returned
        // by boost. This won't probably happen (I think...)
        handler(err, std::string());
    }
}

}; //namespace munu

#endif //_DEF_MUNU_IO_ASYNC_DEVICE_H_
