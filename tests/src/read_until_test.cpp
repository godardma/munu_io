#include <iostream>
using namespace std;

#include <munu_io/AsyncService.h>
#include <munu_io/AsyncDevice.h>
#include <munu_io/SerialDevice.h>
#include <munu_io/ClientTCP.h>
using namespace munu;

using DeviceType = SerialDevice<>;
//using DeviceType = ClientTCP<>;

void read_callback(DeviceType* device,
                   const boost::system::error_code& err,
                   const std::string& data)
{
    cout << std::chrono::duration_cast<std::chrono::seconds>(
            device->timestamp().time_since_epoch()).count() << endl;
    cout << data.size() << " : " << data << endl << endl << flush;
    device->async_read_until('\n', boost::bind(read_callback, device, _1, _2));
}

int main()
{
    AsyncService service;
    DeviceType device(*(service.io_service()));
    
    device.open("/dev/ttyACM0", 115200);
    //device.open("127.0.0.1", 28334);
    
    std::ostringstream oss;
    device.async_read_until('\n', boost::bind(read_callback, &device, _1, _2));

    service.start();

    getchar();

    service.stop();

    return 0;
}
