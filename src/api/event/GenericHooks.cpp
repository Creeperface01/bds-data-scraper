#include <modloader/statichook.h>
#include <bds/bedrock/ServerInstance.h>
#include "../Api.h"

using namespace bdsapi;

TInstanceHook(void, _ZN14ServerInstance6onTickEii, ServerInstance, int int0, int int1) {
//    API_INSTANCE->server->tick();
    original(this, int0, int1);
}
