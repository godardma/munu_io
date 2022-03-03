#include <iostream>
using namespace std;

#include <munu_io/AsyncService.h>
#include <munu_io/ClientTCP.h>
using namespace munu;

void read_callback(ClientTCP<>* client,
              std::vector<char>* buf,
              const boost::system::error_code& err, size_t readCount)
{
    cout << std::chrono::duration_cast<std::chrono::seconds>(
            client->timestamp().time_since_epoch()).count() << endl;
    //std::ostringstream oss;
    //for(auto c : *buf) {
    //    oss << c;
    //}
    //cout << oss.str();
    client->async_read(buf->size(), buf->data(),
                       boost::bind(read_callback, client, buf, _1, _2));
}

void write_callback(ClientTCP<>* client,
                    const boost::system::error_code& err, size_t writeCount)
{
    cout << "Wrote " << writeCount << " bytes.\n" << std::flush;
    client->async_write("test", boost::bind(write_callback, client, _1, _2));
}

int main()
{
    AsyncService service;
    ClientTCP client(*(service.io_service()));
    
    client.open("127.0.0.1", 28334);

    //std::vector<char> buf(32);
    std::vector<char> buf(115200 / 8);

    client.async_read(buf.size(), buf.data(),
                      boost::bind(read_callback, &client, &buf, _1, _2));
    //client.async_write("test", boost::bind(write_callback, &client, _1, _2));

    service.start();

    getchar();
    //while(1) {
    //    std::cout << "Got " << client.read(buf.size(), buf.data())
    //              << " bytes." << std::endl << std::flush;
    //    std::this_thread::sleep_for(100ms);
    //}

    service.stop();

    return 0;
}
