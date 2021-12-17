#ifndef ROUTE_SUITE_PIPEREQUEST_H
#define ROUTE_SUITE_PIPEREQUEST_H

#include "pipes/PipeClient.h"
#include "types.h"
#include "utils.h"

#define ValidateTransaction(func) { \
 STATUS stat = func;                 \
 if (stat != STATUS_OK) {                     \
    CRT_CTX(ValidateTransaction, "transaction error!"); \
    return stat;\
 }}\


#define ValidateSize() \
ReadTransaction(packetSize, int); \
if (packetSize != size()) {       \
 CRT_CTX(ValidateSize, "wrong packet size!"); \
 return STATUS_WRONG_PACKET_SIZE;\
}\

#define ReadTransaction(var, type) ValidateTransaction(client->read(&(var), sizeof(type)))

#define WriteTransaction(var, type) ValidateTransaction(client->write(&(var), sizeof(type)))

namespace Route {

    struct PipeRequest {

        enum RequestType {
            CLIENT_OPEN = 1,
            CLIENT_CLOSE = 2,
            OPEN_CONFIG = 3,
            ENGINE_START,
            ENGINE_STOP,
        };

        RequestType type;
        int packetSize;

        PipeRequest() : type(RequestType(0)), packetSize(0) {}
        PipeRequest(RequestType type) : type(type), packetSize(0) {}

        virtual ~PipeRequest() = default;

        virtual STATUS read(PipeClient* client) {
            return client->read(&type, sizeof(RequestType));
        }

        virtual STATUS write(PipeClient* client) {
            return STATUS_ERROR;
        }

        virtual STATUS write(PipeClient* client, int size) {
            this->packetSize = size;
            ValidateTransaction(client->write(&type, sizeof(RequestType)));
            return client->write(&size, sizeof(int));
        }

        virtual int size() {
            return 0;
        }

    };

    struct PipeResult {

        int result;

        PipeResult(): result(-1) {}
        PipeResult(int result) : result(result) {}
        virtual ~PipeResult() = default;

        virtual STATUS read(PipeClient* client) {
            return client->read(&result, sizeof(int));
        }

        virtual STATUS write(PipeClient* client) {
            return client->write(&result, sizeof(int));
        }

    };

    struct ClientOpenRequest : public PipeRequest {

        char name[CLIENT_NAME_SIZE + 1];
        int pid;

        ClientOpenRequest(): pid(0) {
            // set name to nothing
            memset(name, 0, sizeof(name));
        }

        ClientOpenRequest(const char* client_name, int client_pid) : PipeRequest(PipeRequest::CLIENT_OPEN) {
            memset(name, 0, sizeof(name));
            snprintf(name, sizeof(name), "%s", client_name);
            pid = client_pid;
        }

        STATUS read(PipeClient* client) override {
            ValidateSize();
            ReadTransaction(pid, int);
            ReadTransaction(name, name);
            return STATUS_OK;
        }

        STATUS write(PipeClient* client) override {
            ValidateTransaction(PipeRequest::write(client, size()));
            WriteTransaction(pid, int);
            WriteTransaction(name, name);
            return STATUS_OK;
        }

        int size() override {
            return sizeof(int) + sizeof(name);
        }

    };

    struct ClientOpenResult : public PipeResult {

        int referenceNumber;

        ClientOpenResult(): referenceNumber(-1) {}

        ClientOpenResult(int ref): referenceNumber(ref) {}

        STATUS read(PipeClient* client) override {
            ValidateTransaction(PipeResult::read(client));
            ReadTransaction(referenceNumber, int);
            return STATUS_OK;
        }

        STATUS write(PipeClient* client) override {
            ValidateTransaction(PipeResult::write(client));
            WriteTransaction(referenceNumber, int);
            return STATUS_OK;
        }

    };

    struct ClientCloseRequest : public PipeRequest {

        int ref;

        ClientCloseRequest(): ref(-1) {
        }

        explicit ClientCloseRequest(const int ref) : PipeRequest(PipeRequest::CLIENT_CLOSE), ref(ref) {

        }

        STATUS read(PipeClient* client) override {
            ValidateSize();
            ReadTransaction(ref, int);
            return STATUS_OK;
        }

        STATUS write(PipeClient* client) override {
            ValidateTransaction(PipeRequest::write(client, size()));
            WriteTransaction(ref, int);
            return STATUS_OK;
        }

        int size() override {
            return sizeof(int) + sizeof(ref);
        }

    };

    struct OpenConfigRequest : public PipeRequest {

        int ref;

        OpenConfigRequest(): ref(-1) {
        }

        explicit OpenConfigRequest(const int ref) : PipeRequest(PipeRequest::OPEN_CONFIG), ref(ref) {

        }

        STATUS read(PipeClient* client) override {
            ValidateSize();
            ReadTransaction(ref, int);
            return STATUS_OK;
        }

        STATUS write(PipeClient* client) override {
            ValidateTransaction(PipeRequest::write(client, size()));
            WriteTransaction(ref, int);
            return STATUS_OK;
        }

        int size() override {
            return sizeof(int) + sizeof(ref);
        }

    };

}


#endif //ROUTE_SUITE_PIPEREQUEST_H
