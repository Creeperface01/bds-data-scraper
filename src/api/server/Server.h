#ifndef BDS_API_SERVER_H
#define BDS_API_SERVER_H

#include "../Api.h"

namespace bdsapi {

    class Api;

    class Server {
    public:
        Api *pApi;

        Server(Api *pApi);

        void tick();

        void loadPlugins();
    };

}


#endif //BDS_API_SERVER_H
