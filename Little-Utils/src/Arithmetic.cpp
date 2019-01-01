#include "LittleUtils.hpp"
#include "dsp/digital.hpp"
#include "Widgets.hpp"

#include <algorithm> // std::replace

struct Arithmetic : Module {
	enum ParamIds {
		CLIP_ENABLE_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		X_INPUT,
		Y_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		ADD_OUTPUT,
		SUB_OUTPUT,
		MUL_OUTPUT,
		DIV_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		CLIP_ENABLE_LIGHT,
		NUM_LIGHTS
	};

	// output this instead of NaN (when e.g. dividing by zero)
	float valid_div_value = 0.f;

	Arithmetic() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}

	void step() override;


	// For more advanced Module features, read Rack's engine.hpp header file
	// - toJson, fromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu

};

void Arithmetic::step() {
	bool clip = params[CLIP_ENABLE_PARAM].value > 0.5f;
	auto xin = inputs[X_INPUT];
	auto yin = inputs[Y_INPUT];
	float a = xin.value + yin.value;
	float s = xin.value - yin.value;
	float m = (xin.active ? xin.value : 1.f) * (yin.active ? yin.value : 1.f);
	outputs[ADD_OUTPUT].value = clip ? clamp(a, -10.f, 10.f) : a;
	outputs[SUB_OUTPUT].value = clip ? clamp(s, -10.f, 10.f) : s;
	outputs[MUL_OUTPUT].value = clip ? clamp(m, -10.f, 10.f) : m;

	if(yin.active) {
		float d = (xin.active ? xin.value : 1.f) / yin.value;
		valid_div_value = isfinite(d) ? d : valid_div_value;
		if(clip) valid_div_value = clamp(valid_div_value, -10.f, 10.f);
		outputs[DIV_OUTPUT].value = valid_div_value;
	} else {
		outputs[DIV_OUTPUT].value = clip ? clamp(xin.value, -10.f, 10.f) : xin.value;
	}

	lights[CLIP_ENABLE_LIGHT].setBrightnessSmooth(clip);
}

struct ArithmeticWidget : ModuleWidget {
	Arithmetic *module;

	ArithmeticWidget(Arithmetic *module) : ModuleWidget(module) {
		this->module = module;
		setPanel(SVG::load(assetPlugin(plugin, "res/Arithmetic.svg")));

		addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<PJ301MPort>(Vec(22.5,  46), module, Arithmetic::X_INPUT));
		addInput(createInputCentered<PJ301MPort>(Vec(22.5, 89), module, Arithmetic::Y_INPUT));
		addOutput(createOutputCentered<PJ301MPort>(Vec(22.5, 136), module, Arithmetic::ADD_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(Vec(22.5, 186), module, Arithmetic::SUB_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(Vec(22.5, 236), module, Arithmetic::MUL_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(Vec(22.5, 286), module, Arithmetic::DIV_OUTPUT));

		addParam(createParamCentered<ToggleLEDButton>(Vec(22.5, 315), module, Arithmetic::CLIP_ENABLE_PARAM, 0.f, 1.f, 0.f));

		addChild(createLightCentered<MediumLight<GreenLight>>(Vec(22.5, 315), module, Arithmetic::CLIP_ENABLE_LIGHT));
	}

};


// Specify the Module and ModuleWidget subclass, human-readable
// author name for categorization per plugin, module slug (should never
// change), human-readable module name, and any number of tags
// (found in `include/tags.hpp`) separated by commas.
Model *modelArithmetic = Model::create<Arithmetic, ArithmeticWidget>("Little Utils", "Arithmetic", "Arithmetic", UTILITY_TAG);