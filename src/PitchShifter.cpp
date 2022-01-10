/**
 The Synthesis ToolKit in C++ (STK)

Copyright (c) 1995--2017 Perry R. Cook and Gary P. Scavone

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

Any person wishing to distribute modifications to the Software is
asked to send the modifications to the original developer so that they
can be incorporated into the canonical version.  This is, however, not
a binding provision of this license.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**/

#include "plugin.hpp"
#include <iomanip>

#define k_ps_size  2048
template<long max_length>
class f_delay
{
public:
    float       m_buffer[max_length + 1];
    long        m_write_index;
    long        m_read_index;
    float       m_fractional_part;

    f_delay()
    {
        m_write_index = 0;
        set_length(max_length);
        zero();
    }
    
    f_delay(float in_length)
    {
        m_write_index = 0;
        set_length(in_length);
        zero();
    }
    
    float operator()(float input)
    {
        float   output;
        float   temp;
        
        temp = m_buffer[m_read_index];
        output = temp + (m_buffer[m_read_index + 1] - temp) * m_fractional_part;

        m_read_index++;
        if(m_read_index >= max_length)
            m_read_index = 0;
        
        m_write_index++;
        if(m_write_index >= max_length)
        {
            m_buffer[m_write_index] = input;
            m_write_index = 0;
        }
        m_buffer[m_write_index] = input;

        return output;
    }
    
    void set_length(float in_length)
    {
        if(in_length >= max_length)
            in_length = max_length;
        
        if(in_length < 1)
            in_length = 1;
        
        {
            float m_read_index_float = m_write_index - in_length;
            if(m_read_index_float < 0)
                m_read_index_float += max_length;
            
            m_read_index = m_read_index_float; // int index
            m_fractional_part = m_read_index_float - m_read_index;
        }
    }
    
    void zero()
    {
        float   *buf = m_buffer;
        long    count = max_length;
        
        --buf;
        while(--count >= 0)
        {
            *++buf = 0.0f;
        }
    }
};

template<long max_length>
class nf_delay
{
public:
    float       m_buffer[max_length];
    long        m_write_index;
    long        m_read_index;

    nf_delay()
    {
        m_write_index = 0;
        set_length(max_length);
        zero();
    }
    
    nf_delay(float in_length)
    {
        m_write_index = 0;
        set_length(in_length);
        zero();
    }
    
    float operator()(float input)
    {
        float   output;
        
        output = m_buffer[m_read_index];

        m_read_index++;
        if(m_read_index >= max_length)
            m_read_index = 0;
        
        m_write_index++;
        if(m_write_index >= max_length)
        {
            m_write_index = 0;
        }
        m_buffer[m_write_index] = input;

        return output;
    }
    
    void set_length(long in_length)
    {
        if(in_length >= max_length)
            in_length = max_length;
        
        if(in_length < 1)
            in_length = 1;
        
        {
            m_read_index = in_length; // int index

            m_write_index = m_read_index - in_length;
            if(m_write_index < 0)
                m_write_index += max_length;
        }
    }
    
    void zero()
    {
        float   *buf = m_buffer;
        long    count = max_length;
        
        --buf;
        while(--count >= 0)
        {
            *++buf = 0.0f;
        }
    }
};

class pitch_shifter //based on STK
{
public:
    const float                         k_max_delay = k_ps_size;
    f_delay<k_ps_size>                  m_delay_line[2];
    nf_delay<k_ps_size>                 m_indelay_line[1];
    float                               m_computed_delay_length[2];
    float                               m_envelope[2];
    float                               m_shift_rate;
    float                               m_mix = 0.5;
    unsigned long                       m_delay_length;
    unsigned long                       m_half_length;
    float                               m_last_out = 0.0;

    float                               m_sample_rate = 44100.0;

    pitch_shifter()
    {
        m_delay_length = k_max_delay - 24;
        m_half_length = m_delay_length / 2;
        m_computed_delay_length[0] = 12;
        m_computed_delay_length[1] = k_max_delay / 2;
        
        m_delay_line[0].set_length(m_computed_delay_length[0]);
        m_delay_line[1].set_length(m_computed_delay_length[1]);
        m_indelay_line[0].set_length(m_computed_delay_length[1]);
        
        m_mix = 0.5;
        m_shift_rate = 1.0;

        set_sample_rate(44100.);
    }
    
    virtual ~pitch_shifter()
    {
    }
    
    void set_shift(float in_shift)
    {
        if(in_shift < 1.0)
        {
            m_shift_rate = 1.0 - in_shift;
        }
        else if (in_shift > 1.0 )
        {
            m_shift_rate = 1.0 - in_shift;
        }
        else
        {
            m_shift_rate = 0.0;
            m_computed_delay_length[0] = m_half_length + 12;
        }
    }
    
    void set_mix(float in_mix)
    {
        m_mix = in_mix;
    }
    
    void zero()
    {
        m_delay_line[0].zero();
        m_delay_line[1].zero();
        m_indelay_line[0].zero();

        
        m_last_out = 0.0;
    }

    void set_sample_rate(float in_sr)
    {
        m_sample_rate = in_sr;
    }
    
    //get ouput and iterate
    float operator()(float input)
    {
        // Calculate the two delay length values, keeping them within the
        // range 12 to maxDelay-12.
        m_computed_delay_length[0] += m_shift_rate;
        while(m_computed_delay_length[0] > k_max_delay - 12)
            m_computed_delay_length[0] -= m_delay_length;
        
        while(m_computed_delay_length[0] < 12)
            m_computed_delay_length[0] += m_delay_length;
        
        m_computed_delay_length[1] = m_computed_delay_length[0] + m_half_length;
        while(m_computed_delay_length[1] > k_max_delay - 12)
            m_computed_delay_length[1] -= m_delay_length;
        
        while(m_computed_delay_length[1] < 12)
            m_computed_delay_length[1] += m_delay_length;
        
        // Set the new delay line lengths.
        m_delay_line[0].set_length(m_computed_delay_length[0]);
        m_delay_line[1].set_length(m_computed_delay_length[1]);
        
//        m_indelay_line[0].set_length(m_computed_delay_length[1]);

        // Calculate a triangular envelope.
        m_envelope[1] = fabs((m_computed_delay_length[0] - m_half_length + 12 ) * (1.0 / (m_half_length + 12)));
        m_envelope[0] = 1.0 - m_envelope[1];
        
        // Delay input and apply envelope.
        m_last_out =  m_envelope[0] * m_delay_line[0](input);
        m_last_out += m_envelope[1] * m_delay_line[1](input);
        
        input = m_indelay_line[0](input);
        // Compute effect mix and output.
        m_last_out = input + (m_last_out - input) * m_mix;
        
        return m_last_out;
    }
};

struct PitchMangler : Module
{
	enum ParamIds {
        
        PA_PITCH_CV_VCA,
        
        PA_DRY_WET,
        PA_BYPASS_TRIG,

        NUM_PARAMS
	};
    

	enum InputIds {
        IN_L,
        IN_R,
        
        
        IN_PITCH_CV,
        IN_BYPASS_TRIG,

		NUM_INPUTS
	};
    
	enum OutputIds {
        OUT_L,
        OUT_R,
        

		NUM_OUTPUTS
	};
    
    enum LightIds {
        LI_BYPASS_TRIG,

        NUM_LIGHTS
    };
    
    float                   m_sr;
    pitch_shifter           ps[2];
    
    float                   m_pitch_deviation = 1.0;
    float                   m_last_pitch_deviation = 0.0;
    float                   m_applied_pitch_deviation = 1.0;
    
    dsp::SchmittTrigger     PA_BYPASS_TRIG_Trigger;
    bool                    m_bypass = false;
    
    PitchMangler()
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        
        this->onReset();
        
        configParam(PA_PITCH_CV_VCA, 0.0, 2.0, 1.0);
        configParam(PA_DRY_WET, 0.0, 1.0, 0.5);
        configParam(PA_BYPASS_TRIG, 0.0, 1.0, 0.0);
        
        configBypass(IN_L, OUT_L);
        configBypass(IN_R, OUT_R);
    };
    
    ~PitchMangler()
    {
        this->destroy_all();
    };
    
    void process(const ProcessArgs &args) override;

    void onSampleRateChange() override
    {
        m_sr = APP->engine->getSampleRate();
        
        ps[0].set_sample_rate(m_sr);
        ps[1].set_sample_rate(m_sr);
    };

    void destroy_all()
    {
    };

	void initializelocal()
    {
        m_sr = APP->engine->getSampleRate();

        m_pitch_deviation = 1.0;
        ps[0].set_sample_rate(m_sr);
        ps[1].set_sample_rate(m_sr);
        
        ps[0].set_mix(1.0);
        ps[1].set_mix(1.0);

	};
    

    void onReset() override
    {
        this->initializelocal();
    }

    json_t *dataToJson()  override
    {
        json_t *rootJ = json_object();

        json_object_set_new(rootJ, "m_bypass", json_boolean(m_bypass));
        json_object_set_new(rootJ, "m_pitch_deviation", json_real(m_pitch_deviation));

        return rootJ;
    }
    
    void dataFromJson(json_t *rootJ)  override
    {
        json_t *idJ = NULL;

        idJ = json_object_get(rootJ, "m_bypass");
        if(idJ && json_is_boolean(idJ))
        {
            m_bypass = json_is_true(idJ);
        }

        idJ = json_object_get(rootJ, "m_pitch_deviation");
        if(idJ && json_is_real(idJ))
        {
            m_pitch_deviation = json_number_value(idJ);
        }
    }
    
    void customUpdate(long update_field_tag)
    {
        if(update_field_tag == 0) // update next time nothing to do
        {
//            m_applied_delay_time = m_delay_time * m_delay_time_multiplier;
//
//            m_applied_delay_time = clamp(m_applied_delay_time, 0.001f, m_max_len_delay);
        }
        
        if(update_field_tag == 1) // update next time nothing to do
        {
//            m_applied_voltage_offset = m_voltage_offset;
        }
        
        if(update_field_tag == 2) //
        {
//            m_applied_voltage_gain = m_voltage_gain;
        }
        
        if(update_field_tag == 3) //
        {
//            m_howmany_bits_values = int(pow(2, m_howmany_bits));
        }
        
        if(update_field_tag == 4) //
        {
//            this->recompute_tables();
        }
    }

};

void PitchMangler::process(const ProcessArgs &args)
{
    if(PA_BYPASS_TRIG_Trigger.process(params[PA_BYPASS_TRIG].getValue() || inputs[IN_BYPASS_TRIG].getVoltage()))
    {
        m_bypass = !m_bypass;
    }
    lights[LI_BYPASS_TRIG].value = m_bypass ? 1.0f : 0.0f;


    bool recompute_ps = false;
    
    if(m_last_pitch_deviation != m_pitch_deviation)
    {
        m_last_pitch_deviation = m_pitch_deviation;
        recompute_ps = true;
    }
    
    float pitch_cv = m_applied_pitch_deviation = m_last_pitch_deviation;
    
    if(inputs[IN_PITCH_CV].isConnected())
    {
        pitch_cv = pitch_cv + inputs[IN_PITCH_CV].getVoltage() * 0.2 * params[PA_PITCH_CV_VCA].getValue();
        pitch_cv = clamp(pitch_cv, 0.2f, 4.0f);
        recompute_ps = true;
        m_applied_pitch_deviation = pitch_cv;
    }
    
    if(recompute_ps)
    {
        ps[0].set_shift(m_applied_pitch_deviation);
        ps[1].set_shift(m_applied_pitch_deviation);
    }
    
    if(m_bypass)
    {
        outputs[OUT_L].setVoltage(inputs[IN_L].getVoltage());
        outputs[OUT_R].setVoltage(inputs[IN_R].getVoltage());
    }
    else
    {
        if(inputs[IN_L].isConnected())
        {
            float in_val = inputs[IN_L].getVoltage();
            ps[0].m_mix = params[PA_DRY_WET].getValue();
            float out_val = ps[0](in_val);
//            outputs[OUT_L].setVoltage(in_val + (out_val - in_val) * params[PA_DRY_WET].getValue());
            outputs[OUT_L].setVoltage(out_val);
        }
        else
        {
            outputs[OUT_L].setVoltage( 0.f);
        }
        
        if(inputs[IN_R].isConnected())
        {
            float in_val = inputs[IN_R].getVoltage();
            ps[1].m_mix = params[PA_DRY_WET].getValue();
            float out_val = ps[1](in_val);
//            outputs[OUT_R].setVoltage(in_val + (out_val - in_val) * params[PA_DRY_WET].getValue());
            outputs[OUT_R].setVoltage(out_val);
        }
        else
        {
            outputs[OUT_R].setVoltage( 0.f);
        }
    }
};

struct pm_gainvalue_clickable : TransparentWidget
{
    float   *value = NULL;
    float    max_value;
    float    min_value;
    float    default_value;
    float    advancer = 0.0001;
    PitchMangler  *module = NULL;
    long     updatefield = 2;
    bool     editable = true;
    
    std::shared_ptr<Font> font;
    
    pm_gainvalue_clickable()
    {
    };
    
    float           start_x = 0.0;
    float           start_y = 0.0;
    
    float           running_x = 0.0;
    float           running_y = 0.0;
    
    float           clipped_x = 0.0;
    float           clipped_y = 0.0;
    
    float           start_value = 0.0;
    
//    Vec global2parent(Vec in_global_vec)
//    {
//        //        in_global_vec.x = in_global_vec.x - (this->parent->box.pos.x + this->parent->box.size.x);
//        //        in_global_vec.y = in_global_vec.y - (this->parent->box.pos.y + this->parent->box.size.y);
//
//        in_global_vec.x = in_global_vec.x - (this->parent->box.pos.x);
//        in_global_vec.y = in_global_vec.y - (this->parent->box.pos.y);
//
//        return in_global_vec;
//    }
//
//    Vec parent2local(Vec in_parent_vec)
//    {
//        //        in_global_vec.x = in_global_vec.x - (this->parent->box.pos.x + this->parent->box.size.x);
//        //        in_global_vec.y = in_global_vec.y - (this->parent->box.pos.y + this->parent->box.size.y);
//
//        in_parent_vec.x = in_parent_vec.x - (this->box.pos.x);
//        in_parent_vec.y = in_parent_vec.y - (this->box.pos.y);
//
//        return in_parent_vec;
//    }
    
    Vec global2parent(Vec in_global_vec, Widget *wi)
    {
        if(wi != NULL && wi->parent != NULL)
            return in_global_vec.minus(wi->parent->box.pos);
            
        return in_global_vec;
    }

    Vec parent2local(Vec in_parent_vec, Widget *wi)
    {
        if(wi != NULL && wi->parent != NULL)
            return in_parent_vec.minus(wi->box.pos);

        return in_parent_vec;
    }


    Vec global2local(Vec in_global_vec, Widget *wi){
        return parent2local(global2parent(in_global_vec, wi), wi);
    }


    void onDragStart(const DragStartEvent &e) override
    {
        if(editable)
        {
            start_x = APP->scene->rack->getMousePos().x;
            start_y = APP->scene->rack->getMousePos().y;
            start_value = *value;
            
            Vec     parentXY = global2local(Vec(start_x, start_y), this);
            float   localx = this->box.size.x - parentXY.x;
            
            if(localx < 24)
            {
                advancer = 0.01;
            }
            else if(localx < 36)
            {
                advancer = 0.1;
            }
            else if(localx < 48)
            {
                advancer = 1.;
            }
            //            else if(localx < 60)
            //            {
            //                advancer = 10;
            //            }
            //            else if(localx < 72)
            //            {
            //                advancer = 100;
            //            }
            //            else if(localx < 84)
            //            {
            //                advancer = 500;
            //            }
            else
            {
                advancer = 1.;
            }
            e.consume(this);
       }
    }
    
    void onDragMove(const DragMoveEvent &e) override
    {
        if(editable)
        {
            running_x = APP->scene->rack->getMousePos().x;
            running_y = APP->scene->rack->getMousePos().y;
            //        *value = start_value - ((running_y - start_y) / 10) * advancer;
            *value = start_value - (int(running_y) - int(start_y)) * 0.1 * advancer;
            if(*value >= max_value)
            *value = max_value;
            
            if(*value <= min_value)
            *value = min_value;
            
            if(editable)
            {
                if(module != NULL)
                {
                    module->customUpdate(updatefield);
                }
            }
        }
    }
    
    void onDragEnd(const DragEndEvent &e) override
    {
        if(editable)
        {
            if(module != NULL)
            {
                module->customUpdate(updatefield);
            }
        }
    }
    
    void onButton(const ButtonEvent &e) override {
        if(editable)
        {
            // GLFW_PRESS GLFW_RELEASE GLFW_MOUSE_BUTTON_LEFT GLFW_MOUSE_BUTTON_RIGHT
            if(e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_RIGHT)
            {
                *value = default_value;
                e.consume(this);
            }
            if(e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_LEFT)
            {
                e.consume(this);
            }
            
            if(e.action == GLFW_RELEASE)
            {
                e.consume(this);
            }
        }
    }

//    void onMouseDown(EventMouseDown &e) override
//    {
//        if(editable)
//        {
//            //if(!module->m_interpolators_active)
//            {
//                if (e.button == 1) {
//                    *value = default_value;
//                    
//                    e.consumed = true;
//                    e.target = this;
//                }
//                
//                if(e.button == 0)
//                {
//                    e.consumed = true;
//                    e.target = this;
//                }
//            }
//        }
//    }
//    void onMouseUp(EventMouseUp &e) override
//    {
//        if(editable)
//        {
//            if(e.button == 0)
//            {
//                //            started_dragging = false;
//                //            fresh_start = false;
//                e.consumed = true;
//            }
//        }
//    }
    
    void drawLayer(const DrawArgs& args, int layer) override
    {
        nvgScissor(args.vg, RECT_ARGS(args.clipBox));

        if(layer == 1)
        {
            drawX(args);
        }
        Widget::drawLayer(args, layer);
        nvgResetScissor(args.vg);
    }

    void drawX(const DrawArgs& args)
    {
        NVGcontext *vg = args.vg;

        Rect b = Rect(Vec(0, 0), box.size);
        nvgScissor(vg, b.pos.x, b.pos.y, b.size.x, b.size.y);
        
        NVGcolor backgroundColor = nvgRGB(0, 0, 0);
        nvgBeginPath(vg);
        nvgRoundedRect(vg, 0.0, 0.0, box.size.x, box.size.y, 2.0);
        nvgFillColor(vg, backgroundColor);
        nvgFill(vg);
        
        font = APP->window->loadFont(asset::plugin(pluginInstance, "res/LEDCalculator.ttf"));

        if(font)
        {
            float ftsize = 15;
            nvgFontSize(vg, ftsize);
            nvgFontFaceId(vg, font->handle);
            nvgTextLetterSpacing(vg, 1);
            
            std::string  display_string;
            
            std::ostringstream ss;
            float local_value = value ? *value : 0;

            //if(local_value >= 0.)
            //            ss << "  ";
            
            int precision = 2;
            //        if(local_value >= 99.9999 || local_value <= -99.9999)
            //            precision = 2;
            //        else if(local_value >= 9.9999 || local_value <= -9.9999)
            //            precision = 3;
            //        if(local_value >= 100.)
            //            precision = 0;
            
            //        ss << std::setw(5) << std::setprecision(precision) << std::fixed << local_value;
            ss << std::setprecision(precision) << std::fixed;
            
            if(local_value <= -10.)
                ss << " ";
            else if(local_value < 0.)
                ss << "  ";
            else if(local_value >= 10.)
                ss << " ";
            else if(local_value >= 0.)
            ss << "  ";
            
            ss << local_value;
            //        ss << "V";
            
            display_string.assign(ss.str());
            Vec textPos = Vec(-9, 14);
            
            NVGcolor textColor;
            textColor = nvgRGB(0x00, 0xFF, 0x00);
            if(editable)
            textColor = nvgRGB(0xff, 0, 0x00);
            nvgFillColor(vg, textColor);
            nvgText(vg, textPos.x, textPos.y, display_string.c_str(), NULL);
        }
        
        nvgResetScissor(vg);
    }
};

struct PitchManglerWidget : ModuleWidget {
    
    PitchManglerWidget(Module *module)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/PitchShifter.svg")));

        box.size = Vec(15 * 7, 380);
        
        float baseX = 4.5;
        float advX = 24;
        float baseY = 30;
        float advY = 23;

        addInput(createInput<PJ301MPort>(Vec(baseX + advX * 0 + 1, baseY + advY * 0 + 1 - 8 + 33), module, PitchMangler::IN_L));
        addInput(createInput<PJ301MPort>(Vec(baseX + advX * 3 + 1, baseY + advY * 0 + 1 - 8 + 33), module, PitchMangler::IN_R));

        {
            pm_gainvalue_clickable *wgc = NULL;
            wgc = new pm_gainvalue_clickable();
            wgc->module = (PitchMangler*)module;
            if(module)
                wgc->value = &(wgc->module->m_pitch_deviation);
            wgc->updatefield = 1;
            wgc->max_value = 4.f;
            wgc->min_value = 0.2f;
            wgc->default_value = 1.f;
            wgc->box.pos = Vec(baseX + advX * 2, baseY + advY * 1 + 3 + 5 + 60 + 2);
            wgc->box.size = Vec(advX * 2, 18);
            addChild(wgc);

            wgc = NULL;
            wgc = new pm_gainvalue_clickable();
            wgc->module = (PitchMangler*)module;
            if(module)
                wgc->value = &(wgc->module->m_applied_pitch_deviation);
            wgc->editable = false;
            wgc->box.pos = Vec(baseX + advX * 2, baseY + advY * 2 + 3 - 4 + 5 + 60 + 6);
            wgc->box.size = Vec(advX * 2, 18);
            addChild(wgc);
        }
        
        addInput(createInput<PJ301MPort>(Vec(baseX + advX * 0 + 3 - 1, baseY + advY * 1 + 3 + 5 + 60), module, PitchMangler::IN_PITCH_CV));
        addParam(createParam<Davies1900hWhiteKnob20>(Vec(baseX + advX * 0 + 3 + 1, baseY + advY * 2 + 3 - 4 + 5 + 60 + 5), module, PitchMangler::PA_PITCH_CV_VCA));

        addParam(createParam<nyTapper16>(Vec(baseX + advX * 0 + 15 - 2 + 17 - 4, baseY + advY * 11 + 4 - 8 + 11 - 6 - 10 + 1 + 2), module, PitchMangler::PA_BYPASS_TRIG));
        addChild(createLight<SmallLight<YellowLight>>(Vec(baseX + advX * 0 + 15 + 3 - 2 + 17 + 2 - 4, baseY + advY * 11 + 4 - 8 + 11 + 3 - 6 - 10 + 2 + 1 + 2), module, PitchMangler::LI_BYPASS_TRIG));
        addInput(createInput<PJ301MPort>(Vec(baseX + advX * 0 + 15 + 17 - 2 + 17 - 1 + 4, baseY + advY * 11 + 4 - 8 + 11 - 1 - 6 - 10 - 1 + 2), module, PitchMangler::IN_BYPASS_TRIG));

        

        addParam(createParam<Davies1900hWhiteKnob20>(Vec(baseX + advX * 2 - 10 - 1, baseY + advY * 14 + 6 + 2 - 12 - 45 - 1 + 8), module, PitchMangler::PA_DRY_WET));

        addOutput(createOutput<PJ301MPort>(Vec(baseX + advX * 0 + 1 + 2 - 1, baseY + advY * 14 + 12 + 1 - 21 - 20 - 1), module, PitchMangler::OUT_L));
        addOutput(createOutput<PJ301MPort>(Vec(baseX + advX * 3 + 1 - 1 - 1, baseY + advY * 14 + 12 + 1 - 21 - 20 - 1), module, PitchMangler::OUT_R));

   }

};

Model *modelPitchMangler = createModel<PitchMangler, PitchManglerWidget>("PitchShifter");
