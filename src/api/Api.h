#pragma once

#include <v8.h>
#include <libplatform/libplatform.h>
#include "server/Server.h"

using namespace std;
using namespace v8;

namespace bdsapi {

    class Server;

    class Api {
    private:
        void start();

    public:
        static const char *PREFIX;

        Isolate *isolate;
        Persistent<Context> pContext;
        unique_ptr<Platform> v8Platform;
        Server *server = nullptr;

        Api();

        void initApi();

        void shutdown();

        Isolate *getIsolate() {
            return isolate;
        }
    };

//    extern Api *API_INSTANCE;
}
