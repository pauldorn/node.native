#include <iostream>
#include <native/native.h>
using namespace native;

// usage: (executable)  src_file  dest_file

int main(int argc, char** argv) {
    if(argc < 3) {
        std::cout << "usage: " << argv[0] << " SRC_FILE DEST_FILE" << std::endl;
        return 1;
    }

    fs::open("./webserver", fs::read_only, 0600, [] (fs::file_handle fd, error e){
        long * bytesRead = new long(0);
        if (e) {
            std::cout << "Error reading file" << e;
            exit(1);
        } else {
            std::cout << "Reading file...\n";
            auto readHandlerShared = std::make_shared<std::function<void(std::string str, error e)>>();
            std::function<void(std::string str, error e)> readHandler = [=] (std::string str, error e) mutable {
                    std::cout << "Got data " << str.length() << "\n";
                    if (str.length() > 0) {
                        *bytesRead += str.length();
                        std::cout << "Bytes read: " << *bytesRead << "\n";
                        if (*readHandlerShared) {
                           // *readHandlerShared(std::string("TEST"), error(5));
                            fs::read(fd, 1024, *bytesRead, *readHandlerShared);
                        } else {
                            std::cout << "Read handler is invalid\n";
                        }
                    } else {
                        // Done reading file:
                        delete bytesRead;
                        readHandlerShared.reset();
                    }
            };
            *readHandlerShared = readHandler;
            std::cout << "Test read handler in parent function...\n";
            if(readHandler) {
                fs::read(fd, 1024, 0, readHandler);
            } else {
                std::cout << "Read handler is invalid\n";
            }

        }
        std::cout << "Exited function\n";
    });

    return run();
}
