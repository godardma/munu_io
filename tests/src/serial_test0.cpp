#include <iostream>
using namespace std;

#include <munu_io/AsyncService.h>
#include <munu_io/AsyncDevice.h>
#include <munu_io/SerialDevice.h>
using namespace munu;

void read_callback(SerialDevice<>* serial,
              std::vector<char>* buf,
              const boost::system::error_code& err, size_t readCount)
{
    cout << std::chrono::duration_cast<std::chrono::seconds>(
            serial->timestamp().time_since_epoch()).count() << endl;
    //std::ostringstream oss;
    //for(auto c : *buf) {
    //    oss << c;
    //}
    //cout << oss.str();
    serial->async_read(buf->size(), buf->data(),
                       boost::bind(read_callback, serial, buf, _1, _2));
}

void write_callback(SerialDevice<>* serial,
                    const boost::system::error_code& err, size_t writeCount)
{
    cout << "Wrote " << writeCount << " bytes.\n" << std::flush;
    serial->async_write("test", boost::bind(write_callback, serial, _1, _2));
}

int main()
{
    AsyncService service;
    SerialDevice serial(*(service.io_service()));
    
    serial.open("/dev/ttyACM0", 115200);

    //std::vector<char> buf(32);
    std::vector<char> buf(115200 / 8);

    serial.async_read(buf.size(), buf.data(),
                      boost::bind(read_callback, &serial, &buf, _1, _2));
    serial.async_write("test", boost::bind(write_callback, &serial, _1, _2));

    service.start();

    getchar();
    //while(1) {
    //    std::cout << "Got " << serial.read(buf.size(), buf.data())
    //              << " bytes." << std::endl << std::flush;
    //    std::this_thread::sleep_for(100ms);
    //}

    service.stop();

    return 0;
}
