#include "plugin.hpp"


Plugin* pluginInstance;


void init(rack::Plugin *p) {
    pluginInstance = p;

	// Add modules here
    p->addModel(modelPitchMangler);
    p->addModel(modelTwistedVerb);
    p->addModel(modelHiVerb);
    p->addModel(modelMVerb);
}
