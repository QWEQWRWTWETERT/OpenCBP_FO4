#include "SimObj.h"
#include "f4se/NiNodes.h"
#include "f4se/GameForms.h"
#include "f4se/GameRTTI.h"
#include "log.h"


enum femaleBodyType {CBBE_BODY, FG_BODY};
// Note we don't ref count the nodes becasue it's ignored when the Actor is deleted, and calling Release after that can corrupt memory
const char *leftScrotumName_BT2 = "Penis_Balls_CBP_01";
const char *rightScrotumName_BT2 = "Penis_Balls_CBP_02";

std::unordered_map<const char*, std::string> configMap;

std::vector<const char *> femaleBones;

SimObj::SimObj(Actor *actor, config_t &config)
	: things(4){
	id = actor->formID;
}

SimObj::~SimObj() {
}


bool SimObj::bind(Actor *actor, std::vector<const char *>& boneNames, config_t &config)
{
	logger.error("bind\n");


	auto loadedData = actor->unkF0;
	if (loadedData && loadedData->rootNode) {
		bound = true;

		things.clear();
		for (const char * &b : boneNames) {
			BSFixedString cs(b);
			auto bone = loadedData->rootNode->GetObjectByName(&cs);
			if (!bone) {
				logger.info("Failed to find Bone %s for actor %08x\n", b, actor->formID);
			} else {
				logger.info("Doing Bone %s for actor %08x\n", b, actor->formID);
				things.emplace(b, Thing(bone, cs));
			}
		}
		updateConfig(config);
		return  true;
	}
	return false;
}

bool SimObj::actorValid(Actor *actor) {
	if (actor->flags & TESForm::kFlag_IsDeleted)
		return false;
	if (actor && actor->unkF0 && actor->unkF0->rootNode)
		return true;
	return false;
}


void SimObj::update(Actor *actor) {
	if (!bound)
		return;
	//logger.error("update\n");
	for (auto &t : things) {
		t.second.update(actor);
	}
	//logger.error("end SimObj update\n");
}

bool SimObj::updateConfig(config_t & config) {
	for (auto &t : things) {
		std::string &section = configMap[t.first];
		//logger.info("updateConfig in SimObj: %s, %s\n", section, t.first);
		auto &centry = config[section];
		t.second.updateConfig(centry);
	}
	return true;
}

