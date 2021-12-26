#include <iostream>
#include "spdlog/spdlog.h"
#include "cli/cli.h"
#include "cli/clilocalsession.h"
#include "cli/filehistorystorage.h"
#include "cli/loopscheduler.h"
#include "common/server/RouteServer.h"
#include <chrono>
#include <thread>

Route::RouteServer* server;

BOOL WINAPI CtrlHandler(DWORD ctrlType) {

    switch (ctrlType) {

        case CTRL_C_EVENT: {
            spdlog::info("quitting...");

            // stop the server
            server->stop();

            // close the server
            server->close();

            // delete engine object
            delete server;

            return FALSE;
        }
        default: {
            spdlog::error("nope!");
            return FALSE;
        }
    }
}

int main() {
    std::cout << "Hello, World!" << std::endl;

    // create cmd handler
    cli::CmdHandler cmd;

    // create main menu
    auto rootMenu = std::make_unique<cli::Menu>("route");

    // insert basic command to the root menu
    rootMenu->Insert(
            "hello",
            [](std::ostream& out){ out << "Hello, world\n"; },
            "Print hello world" );

    // server start/stop commands
    rootMenu->Insert(
            "stop",
            [](std::ostream& out) {
                switch (server->getState()) {

                    case IDLE:
                        spdlog::error("server is not running!");
                        return;
                    case STOPPING:
                        spdlog::error("server is shutting down!");
                        return;
                    case STARTING:
                        spdlog::warn("not implemented!");
                        spdlog::error("wait for server to start first!");
                        return;

                    case RUNNING:
                        break;
                }

                server->stop();

                // close the server
                server->close();
            },
            "stop the server");
    rootMenu->Insert(
            "start",
            [](std::ostream& out) {
                switch (server->getState()) {

                    case RUNNING:
                        spdlog::error("server is running!");
                        return;
                    case STOPPING:
                        spdlog::error("wait for server to shut down!");
                        return;
                    case STARTING:
                        spdlog::error("server is starting!");
                        return;

                    case IDLE:
                        break;
                }

                // open server
                server->open();

                // start server
                server->start();
            },
            "start the server");

    // create a cli instance
    cli::Cli cli( std::move(rootMenu), std::make_unique<cli::FileHistoryStorage>(".cli"));

    // global exit action
    cli.ExitAction( [](auto& out){ out << "Goodbye and thanks for all the fish.\n"; } );
    // std exception custom handler
    cli.StdExceptionHandler(
            [](std::ostream& out, const std::string& cmd, const std::exception& e)
            {
                out << "Exception caught in cli handler: "
                    << e.what()
                    << " handling command: "
                    << cmd
                    << ".\n";
            }
    );

    // create the scheduler
    cli::LoopScheduler scheduler;

    // create a local terminal session
    cli::CliLocalTerminalSession localSession(cli, scheduler, std::cout, 200);

    // define an exit action
    localSession.ExitAction(
            [&scheduler](auto& out) // session exit action
            {
                out << "Closing App...\n";
                scheduler.Stop();
            }
    );


    spdlog::set_level(spdlog::level::debug);
    spdlog::set_pattern("[%H:%M:%S:%e] [thread %t] %^[%l] %v%$");

    // handle ctrl+c
    if (!SetConsoleCtrlHandler(CtrlHandler, TRUE)) {
        spdlog::error("couldn't create Control Handler!");
        return 1;
    }

    // create route server instance
    server = new Route::RouteServer();

    // start the scheduler
    scheduler.Run();

    // close server
    server->close();

    return 0;
}
