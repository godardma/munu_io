#ifndef _DEF_MUNU_TRIGGER_ASYNC_SERVICE_H_
#define _DEF_MUNU_TRIGGER_ASYNC_SERVICE_H_

#include <iostream>
#include <thread>
#include <memory>

#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/asio.hpp>
#include <boost/bind.hpp>

namespace munu {

/**
 * This is a simple class to launch a boost asio service in a background
 * thread.
 */
class AsyncService
{
    public:

    using IoService    = boost::asio::io_service;
    using IoServicePtr = std::shared_ptr<IoService>;

    protected:
    
    IoServicePtr service_;
    std::thread  thread_;
    bool         isRunning_;

    public:

    AsyncService() :
        service_(std::make_shared<IoService>()),
        isRunning_(false)
    {}
    ~AsyncService() { this->stop(); }

    IoServicePtr io_service() { return service_;   }
    bool is_running() const   { return isRunning_; }

    void start();
    void stop();
};

inline void AsyncService::start()
{
    if(this->is_running()) return;
    std::cout << "starting" << std::endl;

    if(service_->stopped())
        service_->reset();

    thread_ = std::thread(boost::bind(&boost::asio::io_service::run, service_));
    if(!thread_.joinable())
        throw std::runtime_error("Failed to start AsyncService");

    isRunning_ = true;
}

inline void AsyncService::stop()
{
    if(!this->is_running()) return;

    std::cout << "stopping" << std::endl;
    
    service_->stop();
    thread_.join();
    if(thread_.joinable())
        throw std::runtime_error("Failed to stop AsyncService");

    isRunning_ = false;

    std::cout << "stopped" << std::endl;
}

}; //namespace munu

#endif //_DEF_SEATRAC_DRIVER_ASYNC_SERVICE_H_
