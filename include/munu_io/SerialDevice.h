#ifndef _DEF_MUNU_DRIVER_SERIAL_DEVICE_H_
#define _DEF_MUNU_DRIVER_SERIAL_DEVICE_H_

#include <boost/asio.hpp>

#include <munu_io/AsyncDeviceWritable.h>

namespace munu {

/**
 * This class handler low level serial communication with the USBL device.
 *
 * It handles the coding and decoding of binary messages (encoding, checksum)
 * as well as the serial port itself (configuration and reception).
 */
template <template <typename,typename>class AsyncDeviceT = AsyncDeviceWritable,
          typename TimeSourceT = std::chrono::system_clock>
class SerialDevice : public AsyncDeviceT<boost::asio::serial_port, TimeSourceT>
{
    public:

    using Serial   = boost::asio::serial_port;
    using BaseType = AsyncDeviceT<boost::asio::serial_port, TimeSourceT>;

    struct Parity {
        static constexpr const auto None = Serial::parity::none;
        static constexpr const auto Odd  = Serial::parity::odd;
        static constexpr const auto Even = Serial::parity::even;
    };

    struct FlowControl {
        static constexpr const auto None = Serial::flow_control::none;
        static constexpr const auto Soft = Serial::flow_control::software;
        static constexpr const auto Hard = Serial::flow_control::hardware;
    };

    struct StopBits {
        static constexpr const auto One  = Serial::stop_bits::one;
        static constexpr const auto Half = Serial::stop_bits::onepointfive;
        static constexpr const auto Two  = Serial::stop_bits::two;
    };

    enum FlushType {
        FlushReceive = TCIFLUSH,
        FlushSend    = TCOFLUSH,
        FlushBoth    = TCIOFLUSH
    };

    public:

    SerialDevice(boost::asio::io_service& service) : BaseType(service) {}

    void open(const std::string& device,
              unsigned int baudrate              = 115200,
              unsigned int cSize                 = 8,
              Serial::parity::type parity        = Parity::None,
              Serial::flow_control::type flowCtl = FlowControl::None,
              Serial::stop_bits::type stopBits   = StopBits::One);

	boost::system::error_code flush(FlushType flushType = FlushBoth);

    void set_baudrate(unsigned int baudrate);
    void set_charsize(unsigned int cSize);
    void set_parity(Serial::parity::type parity);
    void set_flowcontrol(Serial::flow_control::type flowCtl);
    void set_stopbits(Serial::stop_bits::type stopBits);
};

template <template<typename,typename>class D, typename T>
void SerialDevice<D,T>::open(const std::string& device,
          unsigned int baudrate,
          unsigned int cSize,
          Serial::parity::type parity,
          Serial::flow_control::type flowCtl,
          Serial::stop_bits::type stopBits)
{
    this->device_.open(device);

    this->set_baudrate(baudrate);
    this->set_charsize(cSize);
    this->set_parity(parity);
    this->set_flowcontrol(flowCtl);
    this->set_stopbits(stopBits);

    this->flush();
}

template <template<typename,typename>class D, typename T>
boost::system::error_code SerialDevice<D,T>::flush(FlushType flushType)
{
    if(::tcflush(this->device_.lowest_layer().native_handle(), flushType) == 0) {
        return boost::system::error_code();
    }
    else {
        return boost::system::error_code(errno,
            boost::asio::error::get_system_category());
    }
}

template <template<typename,typename>class D, typename T>
void SerialDevice<D,T>::set_baudrate(unsigned int baudrate)
{
    this->device_.set_option(Serial::baud_rate(baudrate));
}

template <template<typename,typename>class D, typename T>
void SerialDevice<D,T>::set_charsize(unsigned int cSize)
{
    this->device_.set_option(Serial::character_size(cSize));
}

template <template<typename,typename>class D, typename T>
void SerialDevice<D,T>::set_parity(Serial::parity::type parity)
{
    this->device_.set_option(Serial::parity(parity));
}

template <template<typename,typename>class D, typename T>
void SerialDevice<D,T>::set_flowcontrol(Serial::flow_control::type flowCtl)
{
    this->device_.set_option(Serial::flow_control(flowCtl));
}

template <template<typename,typename>class D, typename T>
void SerialDevice<D,T>::set_stopbits(Serial::stop_bits::type stopBits)
{
    this->device_.set_option(Serial::stop_bits(stopBits));
}

}; //namespace munu

#endif //_DEF_MUNU_DRIVER_SERIAL_DEVICE_H_


