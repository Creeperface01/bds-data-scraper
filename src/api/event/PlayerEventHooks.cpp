#include <modloader/statichook.h>
#include <modloader/log.h>
#include <bds/bedrock/actor/ServerPlayer.h>
#include <bds/bedrock/network/packet/Packet.h>
#include <bds/bedrock/network/packet/TextPacket.h>
#include <bds/bedrock/network/packet/PlayStatusPacket.h>
#include <bds/bedrock/network/ServerNetworkHandler.h>
#include <bds/unmapped/NetworkIdentifier.h>
#include <string>
#include "../Api.h"

using namespace modloader;
using namespace std;
using namespace bdsapi;

//outgoing packets
TInstanceHook(void, _ZNK12ServerPlayer17sendNetworkPacketER6Packet, ServerPlayer, Packet &packet) {
    if (auto pk = dynamic_cast<TextPacket *>(&packet)) {

    }

    if (auto pk = dynamic_cast<PlayStatusPacket *>(&packet)) {

    }

    original(this, packet);
}

TInstanceHook(void, _ZN20ServerNetworkHandler6handleERK17NetworkIdentifierRK10TextPacket, ServerNetworkHandler,
              NetworkIdentifier const &identifier, TextPacket const &packet) {

    string text = "chat text: " + string(((char *) (&packet + 72)));
    Log::info(Api::PREFIX, text.c_str());
    original(this, identifier, packet);
}