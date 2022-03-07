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
                   std::ostringstream* oss,
                   const boost::system::error_code& err, size_t readCount)
{
    cout << readCount << " "
         << std::chrono::duration_cast<std::chrono::seconds>(
            device->timestamp().time_since_epoch()).count() << endl;
    cout << oss->str().size() << " : " << oss->str() << endl << endl << flush;
    oss->str("");
    oss->clear();
    device->async_read_until('\n', oss, boost::bind(read_callback, device, oss, _1, _2));
}

int main()
{
    AsyncService service;
    DeviceType device(*(service.io_service()));
    
    device.open("/dev/ttyACM0", 115200);
    //device.open("127.0.0.1", 28334);
    
    std::ostringstream oss;
    device.async_read_until('\n', &oss, boost::bind(read_callback, &device, &oss, _1, _2));

    service.start();

    getchar();

    service.stop();

    return 0;
}
