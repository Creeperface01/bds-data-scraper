#include <bds/classes.h>
#include <bds/bedrock/ServerInstance.h>
#include <bds/bedrock/level/Level.h>
#include <bds/bedrock/level/ServerLevel.h>
#include <bds/bedrock/block/unmapped/BlockPalette.h>
#include <bds/bedrock/block/unmapped/BlockTypeRegistry.h>
#include <bds/bedrock/block/BlockLegacy.h>
#include <bds/unmapped/Block.h>
#include <bds/bedrock/block/VanillaBlockTypes.h>
#include <bds/bedrock/block/VanillaBlocks.h>
#include <bds/bedrock/item/VanillaItems.h>
#include <bds/bedrock/item/DiggerItem.h>
#include <bds/bedrock/item/WeaponItem.h>
#include <bds/bedrock/util/Color.h>
#include <bds/bedrock/util/Vec3.h>
#include <bds/unmapped/Material.h>
#include <modloader/statichook.h>
#include <modloader/log.h>
#include <unordered_map>
#include <vector>
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <bits/stdc++.h>
#include <set>
#include <memory>
#include <typeinfo>
#include <math.h>

using json = nlohmann::json;
using namespace std;

void initItems();

string getTool(BlockLegacy *block);

string getMinTier(BlockLegacy *block);

bool init = false;

void genBlockStates(vector<string> &blocks) {
    sort(blocks.begin(), blocks.end(), [](string const &a, string const &b) { return a < b; });

    auto out = ofstream("block_categories/block_states.txt", ofstream::out | ofstream::trunc);

    for (auto &b : blocks) {
        out << "public static final BlockState  " << b << " = BlockPalette.INSTANCE.getDefaultState(BlockTypes." << b
            << ");" << endl;
    }

    out.close();
}

void genBlockTypes(vector<string> &blocks) {
    sort(blocks.begin(), blocks.end(), [](string const &a, string const &b) { return a < b; });

    auto out = ofstream("block_categories/block_types.txt", ofstream::out | ofstream::trunc);

    for (auto &b : blocks) {
        string lower = b;
        for (auto &c: lower) c = tolower(c);

        out << "public static final Identifier " << b << " = Identifier.fromString(\"" << lower << "\");" << endl;
    }

    out.close();
}

void saveFile(string const &name, vector<string> &blocks) {
    sort(blocks.begin(), blocks.end(), [](string const &a, string const &b) { return a < b; });

    auto out = ofstream("block_categories/" + name + ".txt", ofstream::out | ofstream::trunc);

    for (string &b : blocks) {

        out << b << endl;
    }

    out.close();
}

string jsonBool(bool b) {
    return b ? "true" : "false";
}

string jsonFloat(float f) {
    stringstream s;
    s << f;

    float i;
    float fract = modff(f, &i);

    if(fract != 0) {
        s << "f";
    }

    return s.str();
}

extern "C" void modloader_on_server_start(ServerInstance *serverInstance) {
//    auto api = new Api();
//    api->initApi();
//    modloader::Log::verbose("START", "test 0");
//    BlockPalette* palette = BlockPalette::initFromBlockDefinitions();
    modloader::Log::verbose("START", "START");

    initItems();
//    cout << getTool(&*VanillaBlockTypes::mStone) << endl;


    vector<string> identifiers;

    vector<string> solid;
    vector<string> flammable;
    vector<string> neverBuildable;
    vector<string> alwaysDestroyable;
    vector<string> replacable;
    vector<string> liquid;
    vector<string> superHot;

    unordered_map<string, float> transparency;

    stringstream jsonBuilder;
    stringstream hardnessBuilder;

    hardnessBuilder << "{";
    jsonBuilder << "["; //start array
    map<int, vector<string>> materialTypes;

    bool first = true;

    auto nevim = materialTypes.insert(1);

    cout << "stone type: " << typeid(VanillaBlocks::mStone).name() << endl;
    cout << "stone legacy: " << typeid(VanillaBlocks::mStone->mLegacyBlock).name() << endl;
    cout << "stone legacy material: " << typeid(VanillaBlocks::mStone->mLegacyBlock->getMaterial()).name() << endl;
    cout << "stone legacy 2: " << typeid((*(&VanillaBlocks::mStone))->mLegacyBlock->getMaterial()).name() << endl;

    VanillaBlockTypes::mStone->getMaterial();
    cout << "0" << endl;
    const Block *stone = VanillaBlocks::mStone;
    cout << "1" << endl;
    VanillaBlocks::mStone->mLegacyBlock->getMaterial();
    cout << "2" << endl;
    stone->mLegacyBlock->getMaterial();
    cout << "3" << endl;

    auto firstBlock = &VanillaBlocks::mStone;
    int blocksCount = (0x0000000010B84D90u - 0x0000000010B83C50u) >> 3u;

    for (int i = 0; i <= blocksCount; ++i) {
        if (i == 210 || i == 229) {
            continue;
        }
        try {
//        const Block *stone = VanillaBlocks::mStone;
            const Block *block = *(firstBlock + i);
            cout << "block pointer: " << block << endl;
//        cout << "stone pointer: " << &VanillaBlocks::mStone << endl;
            cout << "block - " << i << endl;
            WeakPtr<BlockLegacy> legacy = block->mLegacyBlock;
//        cout << "stone legacy type: " << typeid(&stone->mLegacyBlock).name() << endl;
//        cout << "legacy type: " << typeid(&legacy).name() << endl;
//        stone->mLegacyBlock->getMaterial();
//        cout << "material" << endl;

            string name = block->getFullName().substr(10);
            for (auto &c: name) c = toupper(c);

            cout << name << endl;

            Material *m = legacy->getMaterial();

            if (!first) {
                jsonBuilder << ",";
                hardnessBuilder << ",";
            } else {
                first = false;
            }

            jsonBuilder << "{"; //start object

            jsonBuilder << R"("name": ")" << name << "\",";
//                jsonBuilder << "\"gamma\": " << block.mGamma << ",";
//                jsonBuilder << "\"thickness\": " << block.mThickness << ",";
            jsonBuilder << "\"fallable\": " << jsonBool(legacy->mCanSlide) << ",";
            jsonBuilder << "\"friction\": " << legacy->mFriction << ",";
            jsonBuilder << "\"hardness\": " << legacy->getDestroySpeed() << ",";
            jsonBuilder << "\"resistance\": " << legacy->mExplosionResistance << ",";
//                jsonBuilder << "\"can_break_from_falling\": " << jsonBool(block.mCanBeBrokenFromFalling) << ",";
            jsonBuilder << "\"solid\": " << jsonBool(legacy->isSolid()) << ",";
            jsonBuilder << "\"pushesOutItems\": " << jsonBool(legacy->mPushesOutItems) << ",";
            jsonBuilder << "\"translucency\": " << legacy->mTransluency << ",";
            jsonBuilder << "\"burnChance\": " << legacy->mFlameOdds << ",";
            jsonBuilder << "\"burnAbility\": " << legacy->mBurnOdds << ",";
//                jsonBuilder << "\"is_mob_piece\": " << block.misMobPiece << ",";
//                jsonBuilder << "\"can_propagate_brightness\": " << jsonBool(block.mCanPropagateBrightness) << ",";
            jsonBuilder << "\"filterLight\": " << (int) legacy->mLightBlock << ",";
//                jsonBuilder << "\"vanilla\": " << jsonBool(block.mIsVanilla) << ",";
            jsonBuilder << "\"experimental\": " << jsonBool(legacy->mExperimental) << ",";
            jsonBuilder << "\"flammable\": " << jsonBool(m->isFlammable()) << ",";
            jsonBuilder << "\"replaceable\": " << jsonBool(m->isReplaceable()) << ",";
            jsonBuilder << "\"emitLight\": " << (int) legacy->mLightEmission << ",";
            jsonBuilder << "\"diggable\": " << jsonBool(!block->isUnbreakable()) << ",";
            jsonBuilder << "\"powerSource\": " << jsonBool(block->isSignalSource()) << ",";
            jsonBuilder << "\"breakFalling\": " << jsonBool(block->breaksFallingBlocks()) << ",";
            jsonBuilder << "\"blockWater\": " << jsonBool(block->isWaterBlocking()) << ",";
            jsonBuilder << "\"canBeSilkTouched\": " << jsonBool(block->canBeSilkTouched()) << ",";
            jsonBuilder << "\"blockSolid\": " << jsonBool(block->isSolidBlockingBlock()) << ",";
            jsonBuilder << "\"blockMotion\": " << jsonBool(block->isMotionBlockingBlock()) << ",";
//            jsonBuilder << "\"empty\": " << jsonBool(block->isEmpty()) << ",";
            jsonBuilder << "\"comparatorSignal\": " << jsonBool(block->hasComparatorSignal()) << ",";
            jsonBuilder << "\"pushUpFalling\": " << jsonBool(block->pushesUpFallingBlocks()) << ",";
            jsonBuilder << "\"waterlogFlowing\": " << jsonBool(!block->waterSpreadCausesSpawn()) << ",";
            jsonBuilder << "\"waterlogSolid\": " << jsonBool(block->canContainLiquid()) << ",";
            jsonBuilder << "\"color\": " << block->getColor() << ",";

            AABB* bb = &legacy->mVisualShape;
            jsonBuilder << "\"aabb\": \"new SimpleAxisAlignedBB(" << jsonFloat(bb->min.x) << "," << jsonFloat(bb->min.y) << "," << jsonFloat(bb->min.z) << "," << jsonFloat(bb->max.x) << "," << jsonFloat(bb->max.y) << "," << jsonFloat(bb->max.z) << ")\",";

            auto tool = getTool(&*legacy);

            if (tool != "NONE") {
                jsonBuilder << R"("targetTool": ")" << "ToolTypes." << tool << "\"";
            }

//                jsonBuilder << R"("tierType": ")" << getMinTier(&block) << "\"";


            jsonBuilder << "}"; //end object

            hardnessBuilder << "\"" << name << "\": " << legacy->getDestroySpeed();
        } catch (std::exception& e) {
            std::cerr << "Exception caught : " << e.what() << std::endl;
        }
    }

//    BlockTypeRegistry::forEachBlock(
//            [&solid, &flammable, &neverBuildable, &alwaysDestroyable, &replacable, &liquid, &superHot, &identifiers, &transparency, &jsonBuilder, &hardnessBuilder, &first, &materialTypes](
//                    BlockLegacy const &block) {
//                auto m = block.getMaterial();
//                auto name = (*block.getFullName()).substr(10);
//                for (auto &c: name) c = toupper(c);
//
//                vector<string> *typeBlocks;
//                if (materialTypes.count(m->getType())) {
//                    typeBlocks = &materialTypes[m->getType()];
//                } else {
//                    typeBlocks = new vector<string>();
//                    materialTypes.insert({m->getType(), *typeBlocks});
//                }
//
//                typeBlocks->push_back(name);
//
//                identifiers.push_back(name);
//
//                if (m->isSolid()) {
//                    solid.push_back(name);
//                }
//
//                if (m->isFlammable()) {
//                    flammable.push_back(name);
//                }
//
//                if (m->isNeverBuildable()) {
//                    neverBuildable.push_back(name);
//                }
//
//                if (m->isAlwaysDestroyable()) {
//                    alwaysDestroyable.push_back(name);
//                }
//
//                if (m->isReplaceable()) {
//                    replacable.push_back(name);
//                }
//
//                if (m->isLiquid()) {
//                    liquid.push_back(name);
//                }
//
//                if (m->isSuperHot()) {
//                    superHot.push_back(name);
//                }
//
////                cout << name << ": " << m->getTranslucency() << endl;
//                transparency.insert({name, m->getTranslucency()});
//
//                if (!first) {
//                    jsonBuilder << ",";
//                    hardnessBuilder << ",";
//                } else {
//                    first = false;
//                }
//
//                jsonBuilder << "{"; //start object
//
//                const char *bp = (char *) &block;
//
//                jsonBuilder << R"("name": ")" << name << "\",";
////                jsonBuilder << "\"gamma\": " << block.mGamma << ",";
////                jsonBuilder << "\"thickness\": " << block.mThickness << ",";
//                jsonBuilder << "\"fallable\": " << jsonBool(block.mCanSlide) << ",";
//                jsonBuilder << "\"friction\": " << block.mFriction << ",";
//                jsonBuilder << "\"hardness\": " << block.getDestroySpeed() << ",";
//                jsonBuilder << "\"resistance\": " << block.mExplosionResistance << ",";
////                jsonBuilder << "\"can_break_from_falling\": " << jsonBool(block.mCanBeBrokenFromFalling) << ",";
//                jsonBuilder << "\"solid\": " << jsonBool(block.isSolid()) << ",";
//                jsonBuilder << "\"pushesOutItems\": " << jsonBool(block.mPushesOutItems) << ",";
//                jsonBuilder << "\"translucency\": " << block.mTransluency << ",";
//                jsonBuilder << "\"burnChance\": " << block.mFlameOdds << ",";
//                jsonBuilder << "\"burnAbility\": " << block.mBurnOdds << ",";
////                jsonBuilder << "\"is_mob_piece\": " << block.misMobPiece << ",";
////                jsonBuilder << "\"can_propagate_brightness\": " << jsonBool(block.mCanPropagateBrightness) << ",";
//                jsonBuilder << "\"filterLight\": " << (int) block.mLightBlock << ",";
////                jsonBuilder << "\"vanilla\": " << jsonBool(block.mIsVanilla) << ",";
//                jsonBuilder << "\"experimental\": " << jsonBool(block.mExperimental) << ",";
//                jsonBuilder << "\"flammable\": " << jsonBool(m->isFlammable()) << ",";
//                jsonBuilder << "\"replaceable\": " << jsonBool(m->isReplaceable()) << ",";
//                jsonBuilder << "\"emitLight\": " << (int) block.mLightEmission << ",";
//
//                auto tool = getTool(&block);
//
//                if (tool != "NONE") {
//                    jsonBuilder << R"("targetTool": ")" << "ToolTypes." << tool << "\"";
//                }
//
////                jsonBuilder << R"("tierType": ")" << getMinTier(&block) << "\"";
//
//
//                jsonBuilder << "}"; //end object
//
//                hardnessBuilder << "\"" << name << "\": " << block.getDestroySpeed();
//
//                return true;
//            });

    jsonBuilder << "]"; //end array
    hardnessBuilder << "}";

    auto out = ofstream("block_categories/info.json", ofstream::out | ofstream::trunc);

    out << jsonBuilder.rdbuf();

    out.close();

    out = ofstream("block_categories/hardness.json", ofstream::out | ofstream::trunc);

    out << hardnessBuilder.rdbuf();

    out.close();

    saveFile("solid", solid);
    saveFile("flammable", flammable);
    saveFile("never_buildable", neverBuildable);
    saveFile("always_destroyable", alwaysDestroyable);
    saveFile("replacable", replacable);
    saveFile("liquid", liquid);
    saveFile("superhot", superHot);
    saveFile("identifiers", identifiers);

    genBlockTypes(identifiers);
    genBlockStates(identifiers);

    stringstream material;
    material << "{";
    first = true;

    for (auto &materialType : materialTypes) {
        if (first) {
            first = false;
        } else {
            material << ",";
        }
        material << "\"" << materialType.first << "\":[";

        for (int i = 0; i < materialType.second.size(); i++) {
            auto name = materialType.second[i];

            if (i > 0) {
                material << ",";
            }

            material << "\"" + name + "\"";
        }

        material << "]";
    }

    material << "}";


//    BlockPalette::initFromBlockDefinitions();
}

DiggerItem *axes[6];
DiggerItem *pickAxes[6];

void initItems() {
    axes[0] = dynamic_cast<DiggerItem *>(&(*VanillaItems::mHatchet_wood));
    axes[1] = dynamic_cast<DiggerItem *>(&(*VanillaItems::mHatchet_stone));
    axes[2] = dynamic_cast<DiggerItem *>(&(*VanillaItems::mHatchet_iron));
    axes[3] = dynamic_cast<DiggerItem *>(&(*VanillaItems::mHatchet_gold));
    axes[4] = dynamic_cast<DiggerItem *>(&(*VanillaItems::mHatchet_diamond));
    axes[5] = dynamic_cast<DiggerItem *>(&(*VanillaItems::mHatchet_netherite));

    pickAxes[0] = dynamic_cast<DiggerItem *>(&(*VanillaItems::mPickAxe_wood));
    pickAxes[1] = dynamic_cast<DiggerItem *>(&(*VanillaItems::mPickAxe_stone));
    pickAxes[2] = dynamic_cast<DiggerItem *>(&(*VanillaItems::mPickAxe_iron));
    pickAxes[3] = dynamic_cast<DiggerItem *>(&(*VanillaItems::mPickAxe_gold));
    pickAxes[4] = dynamic_cast<DiggerItem *>(&(*VanillaItems::mPickAxe_diamond));
    pickAxes[5] = dynamic_cast<DiggerItem *>(&(*VanillaItems::mPickAxe_netherite));
}

string getTool(BlockLegacy *block) {
    auto material = block->getMaterial();

    if (material->isType(MaterialType::Metal)
        || material->isType(MaterialType::Stone)
        || material->isType(MaterialType::StoneDecoration)
        || material->isType(MaterialType::PackedIce)) {
        return "PICKAXE";
    }

    if (material->isType(MaterialType::Wood)
        || material->isType(MaterialType::Plant)
        || material->isType(MaterialType::ReplaceablePlant)) {
        return "AXE";
    }

    if (*block == *VanillaBlockTypes::mWeb
        || *block == *VanillaBlockTypes::mLeaves
        || *block == *VanillaBlockTypes::mLeaves2
        || *block == *VanillaBlockTypes::mWool) {
        return "SHEARS";
    }

    if (*block == *VanillaBlockTypes::mWeb
        || *block == *VanillaBlockTypes::mBambooBlock) {
        return "SWORD";
    }

    for (auto &axe : axes) {
        if (axe->m_bBlocks.count(block)) {
            return "AXE";
        }
    }

    for (auto &axe : pickAxes) {
        if (axe->m_bBlocks.count(block)) {
            return "PICKAXE";
        }
    }

    return "NONE";
}

string getMinTier(BlockLegacy *block) {
    int tier = -1;

    for (int i = 0; i < 6; i++) {
        auto axe = axes[i];
        if (axe->m_bBlocks.count(block)) {
            tier = i;
            break;
        }
    }

    if (tier == -1) {
        for (int i = 0; i < 6; i++) {
            auto axe = pickAxes[i];
            if (axe->m_bBlocks.count(block)) {
                tier = i;
                break;
            }
        }
    }

    switch (tier) {
        case 0:
            return "WOOD";
        case 1:
            return "STONE";
        case 2:
            return "IRON";
        case 3:
            return "GOLD";
        case 4:
            return "DIAMOND";
        case 5:
            return "NETHERITE";
        default:
            return "NONE";
    }
}

//TInstanceHook(bool,
//              _ZN11ServerLevel10initializeERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEERK13LevelSettingsP9LevelDataPS6_,
//              ServerLevel, std::string const &levelName, LevelSettings const& settings, LevelData * data,
//              std::string const *levelId) {
//    bool r = original(this, levelName, settings, data, levelId);
//
//    BlockPalette *palette = this->getBlockPalette();
//
//    modloader::Log::verbose("TEST", "test 2");
//
//    palette->clear();
//
//    modloader::Log::verbose("TEST", "test 3");
//
//    const BlockLegacy *block = palette->getBlockLegacy("stone");
//    Material *material = block->getMaterial();
//
//    modloader::Log::verbose("TEST", "test");
//
//    if (material->isSolid()) {
//        modloader::Log::verbose("TEST", "solid");
//    }
//
//    if (material->isLiquid()) {
//        modloader::Log::verbose("TEST", "liquid");
//    }
//
//    return r;
//}

//TInstanceHook(void, _ZN5Level4tickEv, Level) {
//    if(!init) {
//
//
//        init = true;
//    }
//    original(this);
//}