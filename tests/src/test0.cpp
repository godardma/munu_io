#include <iostream>
using namespace std;

#include <munu_io/AsyncService.h>
#include <munu_io/AsyncDevice.h>
#include <munu_io/SerialDevice.h>
#include <munu_io/ClientTCP.h>
using namespace munu;

//using DeviceType = SerialDevice<>;
using DeviceType = ClientTCP<>;

void read_callback(DeviceType* device,
              std::vector<char>* buf,
              const boost::system::error_code& err, size_t readCount)
{
    cout << std::chrono::duration_cast<std::chrono::seconds>(
            device->timestamp().time_since_epoch()).count() << endl;
    //std::ostringstream oss;
    //for(auto c : *buf) {
    //    oss << c;
    //}
    //cout << oss.str();
    device->async_read(buf->size(), buf->data(),
                       boost::bind(read_callback, device, buf, _1, _2));
}

void write_callback(DeviceType* device,
                    const boost::system::error_code& err, size_t writeCount)
{
    cout << "Wrote " << writeCount << " bytes.\n" << std::flush;
    device->async_write("test", boost::bind(write_callback, device, _1, _2));
}

int main()
{
    AsyncService service;
    DeviceType device(*(service.io_service()));
    
    //device.open("/dev/ttyACM0", 115200);
    device.open("127.0.0.1", 28334);

    //std::vector<char> buf(32);
    std::vector<char> buf(115200 / 8);

    device.async_read(buf.size(), buf.data(),
                      boost::bind(read_callback, &device, &buf, _1, _2));
    //device.async_write("test", boost::bind(write_callback, &device, _1, _2));

    service.start();

    getchar();
    //while(1) {
    //    std::cout << "Got " << device.read(buf.size(), buf.data())
    //              << " bytes." << std::endl << std::flush;
    //    std::this_thread::sleep_for(100ms);
    //}

    service.stop();

    return 0;
}
