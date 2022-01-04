
#include <spdlog/spdlog.h>
#include <argparse/argparse.hpp>

int main(int argc, char *argv[]) {

    // configure spdlog
    spdlog::set_level(spdlog::level::info);
    spdlog::set_pattern("[%H:%M:%S:%e] [thread %t] %^[%l] %v%$");

    // new program
    argparse::ArgumentParser program("buffer_size");

    program.add_argument("buffer_size")
        .required()
        .scan<'i',int>();

    program.add_argument("buffer_count")
        .required()
        .scan<'i',int>();

    // parse arguments
    try {
        program.parse_args(argc, argv);
    }
    catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(1);
    }

    const int data_size = sizeof(float);

    const double bufferMB = ((double) data_size) * program.get<int>("buffer_size") / 1e6;
    const double totalMB = bufferMB * program.get<int>("buffer_count");

    spdlog::info("buffer size: {} MB, total size: {} MB", bufferMB, totalMB);

    return 0;
}