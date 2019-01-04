#include "LittleUtils.hpp"
#include <vector>
#include <map>

#define NUM_TELEPORT_INPUTS 8

struct TeleportInModule;

struct Teleport : Module {
	bool status; //TODO: necessary?
	std::string label;
	Teleport(int numParams, int numInputs, int numOutputs, int numLights = 0):
		Module(numParams, numInputs, numOutputs, numLights) {
			status = false;
		}

	//static unsigned int sync //TODO: allow multiple inputs with same name?

	// this static map is used for keeping track of all existing Teleport instances
	// TODO: rename
	// TODO: static std::map<std::string, Input*> sources; <-- access inputs directly in map, good idea? does this prevent mixing? do we want multiple inputs with the same label?
	static std::map<std::string, TeleportInModule*> sources; //TODO: replace with set, custom comparator that compares labels.
	static std::string lastInsertedKey; // this is used to assign the label of an output initially

	void addSource(TeleportInModule *t);
	inline bool sourceExists(std::string lbl) {
		return sources.find(lbl) != sources.end();
	}
};

std::map<std::string, TeleportInModule*> Teleport::sources = {};
std::string Teleport::lastInsertedKey = "";
