#pragma once
#include <rack.hpp>
#include <mutex>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <memory>

using namespace rack;

// Declare the Plugin, defined in plugin.cpp
extern Plugin* pluginInstance;

// Declare each Model, defined in each module source file
extern Model* modelPitchMangler;
extern Model* modelTwistedVerb;
extern Model* modelHiVerb;
extern Model* modelMVerb;


struct nyTapper12 : app::SvgSwitch {
    nyTapper12() {
        momentary = true;
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/nyTapperUp12.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/nyTapperDown12.svg")));

        sw->wrap();
        box.size = sw->box.size;
    }
};

struct nyTapper16 : app::SvgSwitch {
    nyTapper16() {
        momentary = true;
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/nyTapperUp16.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/nyTapperDown16.svg")));

        sw->wrap();
        box.size = sw->box.size;
    }
};

struct Davies1900hWhiteKnob20 : Davies1900hKnob {
    Davies1900hWhiteKnob20() {
        setSvg(Svg::load(asset::plugin(pluginInstance, "res/Davies1900hWhite20.svg")));
        bg->setSvg(Svg::load(asset::plugin(pluginInstance, "res/Davies1900hWhite20_bg.svg")));
    }
};

//
//struct localRoundKnob : RoundKnob {
//    bool    randomizable = true;
//    widget::SvgWidget* fg;
//
//    localRoundKnob() {
//        fg = new widget::SvgWidget;
//        fb->addChildAbove(fg, tw);
//    }
//};
//
//struct jojoWhiteKnobRed20 : localRoundKnob {
//    jojoWhiteKnobRed20() {
//        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/jojoWhiteKnob20fg.svg")));
//        bg->setSvg(Svg::load(asset::plugin(pluginInstance, "res/jojoWhiteKnob20bg.svg")));
//    }
//};
//
