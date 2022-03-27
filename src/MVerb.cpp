#include "plugin.hpp"

//    Copyright (c) 2010 Martin Eastwood
//  This code is distributed under the terms of the GNU General Public License

//  MVerb is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  at your option) any later version.
//
//  MVerb is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this MVerb.  If not, see <http://www.gnu.org/licenses/>.

#ifndef yyEMVERB_H
#define yyEMVERB_H

//forward declaration
template<typename T, long max_length> class mv_allpass;
template<typename T, long max_length> class mv_staticallpass4tap;
template<typename T, long max_length> class mv_staticdelayline;
template<typename T, long max_length> class mv_staticdelayline4tap;

template<typename T, long max_length> class mv_staticdelayline8tap;
template<typename T, long OverSampleCount> class mv_statevariable;

#define nyBUFFERSIZE 96000
//#define BUFFERSIZE 4096

#define nyMAXFEEDBACK 0.99

template<typename T>
class mv_reverb_original
{
public:
    mv_allpass<T, nyBUFFERSIZE> allpass[4];
    mv_staticallpass4tap<T, nyBUFFERSIZE> allpassFourTap[4];
    mv_statevariable<T,4> hipassFilter[2];
    mv_statevariable<T,4> lowpassFilter[2];
    mv_statevariable<T,4> damping[2];
    mv_staticdelayline<T, nyBUFFERSIZE> predelayline;
    mv_staticdelayline4tap<T, nyBUFFERSIZE> staticDelayLine[4];
    mv_staticdelayline4tap<T, nyBUFFERSIZE> staticDelayLine2[4];
    mv_staticdelayline8tap<T, nyBUFFERSIZE> earlyReflectionsDelayLine[2];
    T m_samplerate;
    T m_predelaytime;
    T m_mix;
    T m_earlymix;
    T m_gain;
    T m_lowpassfrequency;
    T m_highpassfrequency;
    T m_decay;
    T m_dampingfrequency;
    T m_density1;
    T m_density2;
    T m_roomsize;
    
    T m_previouslefttank;
    T m_previousrighttank;

public:

    mv_reverb_original(){
        hipassFilter[0].Type(hipassFilter[0].HIGHPASS);
        hipassFilter[1].Type(hipassFilter[1].HIGHPASS);

        m_samplerate = 44100.;
        m_predelaytime = 0.5 * 200 * (m_samplerate / 1000);
        m_roomsize = (0.95 * 0.5 * 0.5) + 0.05;
        m_density1 = (0.995f * 0.5) + 0.0045;
        m_decay = ((0.9995f * 0.5) + 0.004);
        m_density2 = m_decay + 0.15;
        reset();
    }

    ~mv_reverb_original(){
        //nowt to do here
    }
    
    void set_roomsize(T roomsize){
        m_roomsize = (0.95 * roomsize * roomsize) + 0.05;
//        allpassFourTap[0].zero();
//        allpassFourTap[1].zero();
//        allpassFourTap[2].zero();
//        allpassFourTap[3].zero();
        allpassFourTap[0].set_length(0.020 * m_samplerate * m_roomsize);
        allpassFourTap[1].set_length(0.060 * m_samplerate * m_roomsize);
        allpassFourTap[2].set_length(0.030 * m_samplerate * m_roomsize);
        allpassFourTap[3].set_length(0.089 * m_samplerate * m_roomsize);
        allpassFourTap[1].set_index(0,0.006 * m_samplerate * m_roomsize, 0.041 * m_samplerate * m_roomsize, 0);
        allpassFourTap[3].set_index(0,0.031 * m_samplerate * m_roomsize, 0.011 * m_samplerate * m_roomsize, 0);
//        staticDelayLine[0].zero();
//        staticDelayLine[1].zero();
//        staticDelayLine[2].zero();
//        staticDelayLine[3].zero();
        staticDelayLine[0].set_length(0.15 * m_samplerate * m_roomsize);
        staticDelayLine[1].set_length(0.12 * m_samplerate * m_roomsize);
        staticDelayLine[2].set_length(0.14 * m_samplerate * m_roomsize);
        staticDelayLine[3].set_length(0.11 * m_samplerate * m_roomsize);
        staticDelayLine[0].set_index(0, 0.067 * m_samplerate * m_roomsize, 0.011 * m_samplerate * m_roomsize , 0.121 * m_samplerate * m_roomsize);
        staticDelayLine[1].set_index(0, 0.036 * m_samplerate * m_roomsize, 0.089 * m_samplerate * m_roomsize , 0);
        staticDelayLine[2].set_index(0, 0.0089 * m_samplerate * m_roomsize, 0.099 * m_samplerate * m_roomsize , 0);
        staticDelayLine[3].set_index(0, 0.067 * m_samplerate * m_roomsize, 0.0041 * m_samplerate * m_roomsize , 0);
//        staticDelayLine2[0].zero();
//        staticDelayLine2[1].zero();
//        staticDelayLine2[2].zero();
//        staticDelayLine2[3].zero();
        staticDelayLine2[0].set_length(0.17 * m_samplerate * m_roomsize);
        staticDelayLine2[1].set_length(0.19 * m_samplerate * m_roomsize);
        staticDelayLine2[2].set_length(0.23 * m_samplerate * m_roomsize);
        staticDelayLine2[3].set_length(0.29 * m_samplerate * m_roomsize);
        staticDelayLine2[0].set_index(0.017 * m_samplerate * m_roomsize, 0.037 * m_samplerate * m_roomsize, 0.019 * m_samplerate * m_roomsize , 0.141 * m_samplerate * m_roomsize);
        staticDelayLine2[1].set_index(0.019 * m_samplerate * m_roomsize, 0.041 * m_samplerate * m_roomsize, 0.189 * m_samplerate * m_roomsize , 0.289 * m_samplerate * m_roomsize);
        staticDelayLine2[2].set_index(0.023 * m_samplerate * m_roomsize, 0.289 * m_samplerate * m_roomsize, 0.079 * m_samplerate * m_roomsize , 0.039 * m_samplerate * m_roomsize);
        staticDelayLine2[3].set_index(0.029 * m_samplerate * m_roomsize, 0.037 * m_samplerate * m_roomsize, 0.041 * m_samplerate * m_roomsize ,  0.043 * m_samplerate * m_roomsize);
    }
    
    inline void set_predelay(T predelay)
    {
        m_predelaytime = predelay * 200 * (m_samplerate / 1000);
        predelayline.set_length(m_predelaytime);
    }
    
    inline void set_mix(T mix)
    {
        m_mix = mix;
    }
    
    inline void set_earlymix(T earlymix)
    {
        m_earlymix = earlymix;
    }
    
    inline void set_gain(T gain)
    {
        m_gain = gain;
    }
    
    inline void set_lowpassfrequency(T frequency)
    {
        m_lowpassfrequency = ((frequency * m_samplerate * 0.49) + 10.);
        lowpassFilter[0].Frequency(m_lowpassfrequency);
        lowpassFilter[1].Frequency(m_lowpassfrequency);
    }
    
    inline void set_highpassfrequency(T frequency)
    {
        m_highpassfrequency = ((frequency * m_samplerate * 0.49) + 10.);
        hipassFilter[0].Frequency(m_highpassfrequency);
        hipassFilter[1].Frequency(m_highpassfrequency);
    }
    
    inline void set_decay(T decay)
    {
        m_decay = ((0.9995f * decay ) + 0.004);
        
        m_density2 = m_decay + 0.15;
        if (m_density2 > 0.5)
            m_density2 = 0.5;
        if (m_density2 < 0.25)
            m_density2 = 0.25;
        allpassFourTap[1].set_feedback(m_density2);
        allpassFourTap[3].set_feedback(m_density2);
    }
    
    inline void set_dampingfrequency(T dampingfrequency)
    {
        m_dampingfrequency = ((dampingfrequency * m_samplerate * 0.49) + 10.);;
        
        damping[0].Frequency(m_dampingfrequency);
        damping[1].Frequency(m_dampingfrequency);
    }
    
    inline void set_density(T density)
    {
        m_density1 = (0.995f * density) + 0.0045;;
        allpassFourTap[0].set_feedback(m_density1);
        allpassFourTap[2].set_feedback(m_density1);
    }
    
    void process_one(T inputL, T inputR, T *outputL, T *outputR){
        T left = inputL;
        T right = inputR;
        T bandwidthLeft = lowpassFilter[0](left) ;
        T bandwidthRight = lowpassFilter[1](right) ;
        T earlyReflectionsL = earlyReflectionsDelayLine[0] ( bandwidthLeft * 0.5 + bandwidthRight * 0.3 )
        + earlyReflectionsDelayLine[0].get_value_at_index(2) * 0.6
        + earlyReflectionsDelayLine[0].get_value_at_index(3) * 0.4
        + earlyReflectionsDelayLine[0].get_value_at_index(4) * 0.3
        + earlyReflectionsDelayLine[0].get_value_at_index(5) * 0.3
        + earlyReflectionsDelayLine[0].get_value_at_index(6) * 0.1
        + earlyReflectionsDelayLine[0].get_value_at_index(7) * 0.1
        + ( bandwidthLeft * 0.4 + bandwidthRight * 0.2 ) * 0.5 ;
        T earlyReflectionsR = earlyReflectionsDelayLine[1] ( bandwidthLeft * 0.3 + bandwidthRight * 0.5 )
        + earlyReflectionsDelayLine[1].get_value_at_index(2) * 0.6
        + earlyReflectionsDelayLine[1].get_value_at_index(3) * 0.4
        + earlyReflectionsDelayLine[1].get_value_at_index(4) * 0.3
        + earlyReflectionsDelayLine[1].get_value_at_index(5) * 0.3
        + earlyReflectionsDelayLine[1].get_value_at_index(6) * 0.1
        + earlyReflectionsDelayLine[1].get_value_at_index(7) * 0.1
        + ( bandwidthLeft * 0.2 + bandwidthRight * 0.4 ) * 0.5 ;
        T predelayMonoInput = predelayline(( bandwidthRight + bandwidthLeft ) * 0.5f);
        T smearedInput = predelayMonoInput;
        for(int j=0;j<4;j++)
            smearedInput = allpass[j] ( smearedInput );
        T leftTank = allpassFourTap[0] ( smearedInput + m_previousrighttank ) ;
        leftTank = staticDelayLine[0] (leftTank);
        leftTank = damping[0](leftTank);
        leftTank = allpassFourTap[1](leftTank);
        leftTank = staticDelayLine[1](leftTank);
        T rightTank = allpassFourTap[2] (smearedInput + m_previouslefttank) ;
        rightTank = staticDelayLine[2](rightTank);
        rightTank = damping[1] (rightTank);
        rightTank = allpassFourTap[3](rightTank);
        rightTank = staticDelayLine[3](rightTank);
        m_previouslefttank = leftTank * m_decay;
        m_previousrighttank = rightTank * m_decay;
        T accumulatorL = (0.6*staticDelayLine[2].get_value_at_index(1))
        +(0.6*staticDelayLine[2].get_value_at_index(2))
        -(0.6*allpassFourTap[3].get_value_at_index(1))
        +(0.6*staticDelayLine[3].get_value_at_index(1))
        -(0.6*staticDelayLine[0].get_value_at_index(1))
        -(0.6*allpassFourTap[1].get_value_at_index(1))
        -(0.6*staticDelayLine[1].get_value_at_index(1))
        ;
        T accumulatorR = (0.6*staticDelayLine[0].get_value_at_index(2))
        +(0.6*staticDelayLine[0].get_value_at_index(3))
        -(0.6*allpassFourTap[1].get_value_at_index(2))
        +(0.6*staticDelayLine[1].get_value_at_index(2))
        -(0.6*staticDelayLine[2].get_value_at_index(3))
        -(0.6*allpassFourTap[3].get_value_at_index(2))
        -(0.6*staticDelayLine[3].get_value_at_index(2))
        ;
        accumulatorL = ((accumulatorL * m_earlymix) + ((1 - m_earlymix) * earlyReflectionsL));
        accumulatorR = ((accumulatorR * m_earlymix) + ((1 - m_earlymix) * earlyReflectionsR));
        left = ( left + m_mix * ( accumulatorL - left ) ) * m_gain;
        right = ( right + m_mix * ( accumulatorR - right ) ) * m_gain;
        *outputL = left;
        *outputR = right;
    }
    

    void process_one_twisted(T inputL, T inputR, T *outputL, T *outputR){
        T left = inputL;
        T right = inputR;
        T bandwidthLeft = lowpassFilter[0](left) ;
        T bandwidthRight = lowpassFilter[1](right) ;
        T earlyReflectionsL = earlyReflectionsDelayLine[0] ( bandwidthLeft * 0.5 + bandwidthRight * 0.3 )
        + earlyReflectionsDelayLine[0].get_value_at_index(2) * 0.6
        + earlyReflectionsDelayLine[0].get_value_at_index(3) * 0.4
        + earlyReflectionsDelayLine[0].get_value_at_index(4) * 0.3
        + earlyReflectionsDelayLine[0].get_value_at_index(5) * 0.3
        + earlyReflectionsDelayLine[0].get_value_at_index(6) * 0.1
        + earlyReflectionsDelayLine[0].get_value_at_index(7) * 0.1
        + ( bandwidthLeft * 0.4 + bandwidthRight * 0.2 ) * 0.5 ;
        T earlyReflectionsR = earlyReflectionsDelayLine[1] ( bandwidthLeft * 0.3 + bandwidthRight * 0.5 )
        + earlyReflectionsDelayLine[1].get_value_at_index(2) * 0.6
        + earlyReflectionsDelayLine[1].get_value_at_index(3) * 0.4
        + earlyReflectionsDelayLine[1].get_value_at_index(4) * 0.3
        + earlyReflectionsDelayLine[1].get_value_at_index(5) * 0.3
        + earlyReflectionsDelayLine[1].get_value_at_index(6) * 0.1
        + earlyReflectionsDelayLine[1].get_value_at_index(7) * 0.1
        + ( bandwidthLeft * 0.2 + bandwidthRight * 0.4 ) * 0.5 ;
        T predelayMonoInput = predelayline(( bandwidthRight + bandwidthLeft ) * 0.5f);
        T smearedInput = predelayMonoInput;
        for(int j=0;j<4;j++)
            smearedInput = allpass[j] ( smearedInput );
        T leftTank = allpassFourTap[0] ( smearedInput + m_previousrighttank ) ;
        leftTank = staticDelayLine[0] (leftTank);
        leftTank = staticDelayLine2[0] (leftTank);
        leftTank = damping[0](leftTank);
        leftTank = allpassFourTap[1](leftTank);
        leftTank = staticDelayLine[1](leftTank);
        leftTank = staticDelayLine2[1](leftTank);
        T rightTank = allpassFourTap[2] (smearedInput + m_previouslefttank) ;
        rightTank = staticDelayLine[2](rightTank);
        rightTank = staticDelayLine2[2](rightTank);
        rightTank = damping[1] (rightTank);
        rightTank = allpassFourTap[3](rightTank);
        rightTank = staticDelayLine[3](rightTank);
        rightTank = staticDelayLine2[3](rightTank);
        m_previouslefttank = leftTank * m_decay;
        m_previousrighttank = rightTank * m_decay;
        T accumulatorL = (0.6*staticDelayLine[2].get_value_at_index(1))
        +(0.6*staticDelayLine[2].get_value_at_index(2))
        +(0.6*staticDelayLine2[2].get_value_at_index(2))
        -(0.6*allpassFourTap[3].get_value_at_index(1))
        +(0.6*staticDelayLine[3].get_value_at_index(1))
        +(0.6*staticDelayLine2[3].get_value_at_index(1))
        -(0.6*staticDelayLine[0].get_value_at_index(1))
        -(0.6*staticDelayLine2[0].get_value_at_index(1))
        -(0.6*allpassFourTap[1].get_value_at_index(1))
        -(0.6*staticDelayLine[1].get_value_at_index(1))
        -(0.6*staticDelayLine2[1].get_value_at_index(1));
        T accumulatorR = (0.6*staticDelayLine[0].get_value_at_index(2))
        +(0.6*staticDelayLine[0].get_value_at_index(3))
        +(0.6*staticDelayLine2[0].get_value_at_index(3))
        -(0.6*allpassFourTap[1].get_value_at_index(2))
        +(0.6*staticDelayLine[1].get_value_at_index(2))
        +(0.6*staticDelayLine2[1].get_value_at_index(2))
        -(0.6*staticDelayLine[2].get_value_at_index(3))
        -(0.6*staticDelayLine2[2].get_value_at_index(3))
        -(0.6*allpassFourTap[3].get_value_at_index(2))
        -(0.6*staticDelayLine[3].get_value_at_index(2))
        -(0.6*staticDelayLine2[3].get_value_at_index(2));
        accumulatorL = ((accumulatorL * m_earlymix) + ((1 - m_earlymix) * earlyReflectionsL));
        accumulatorR = ((accumulatorR * m_earlymix) + ((1 - m_earlymix) * earlyReflectionsR));
        left = ( left + m_mix * ( accumulatorL - left ) ) * m_gain;
        right = ( right + m_mix * ( accumulatorR - right ) ) * m_gain;
        *outputL = left;
        *outputR = right;
    }
    
    void process_one_shimmer(T inputL, T inputR, T *outputL, T *outputR){
        T left = inputL;
        T right = inputR;
        T bandwidthLeft = hipassFilter[0](left) ;
        T bandwidthRight = hipassFilter[1](right) ;
        T earlyReflectionsL = earlyReflectionsDelayLine[0] ( bandwidthLeft * 0.5 + bandwidthRight * 0.3 )
        + earlyReflectionsDelayLine[0].get_value_at_index(2) * 0.6
        + earlyReflectionsDelayLine[0].get_value_at_index(3) * 0.4
        + earlyReflectionsDelayLine[0].get_value_at_index(4) * 0.3
        + earlyReflectionsDelayLine[0].get_value_at_index(5) * 0.3
        + earlyReflectionsDelayLine[0].get_value_at_index(6) * 0.1
        + earlyReflectionsDelayLine[0].get_value_at_index(7) * 0.1
        + ( bandwidthLeft * 0.4 + bandwidthRight * 0.2 ) * 0.5 ;
        T earlyReflectionsR = earlyReflectionsDelayLine[1] ( bandwidthLeft * 0.3 + bandwidthRight * 0.5 )
        + earlyReflectionsDelayLine[1].get_value_at_index(2) * 0.6
        + earlyReflectionsDelayLine[1].get_value_at_index(3) * 0.4
        + earlyReflectionsDelayLine[1].get_value_at_index(4) * 0.3
        + earlyReflectionsDelayLine[1].get_value_at_index(5) * 0.3
        + earlyReflectionsDelayLine[1].get_value_at_index(6) * 0.1
        + earlyReflectionsDelayLine[1].get_value_at_index(7) * 0.1
        + ( bandwidthLeft * 0.2 + bandwidthRight * 0.4 ) * 0.5 ;
        T predelayMonoInput = predelayline(( bandwidthRight + bandwidthLeft ) * 0.5f);
        T smearedInput = predelayMonoInput;
        for(int j=0;j<4;j++)
            smearedInput = allpass[j] ( smearedInput );
        T leftTank = allpassFourTap[0] ( smearedInput + m_previousrighttank ) ;
        leftTank = staticDelayLine[0] (leftTank);
        leftTank = staticDelayLine2[0] (leftTank);
        leftTank = damping[0](leftTank);
        leftTank = allpassFourTap[1](leftTank);
        leftTank = staticDelayLine[1](leftTank);
        leftTank = staticDelayLine2[1](leftTank);
        T rightTank = allpassFourTap[2] (smearedInput + m_previouslefttank) ;
        rightTank = staticDelayLine[2](rightTank);
        rightTank = staticDelayLine2[2](rightTank);
        rightTank = damping[1] (rightTank);
        rightTank = allpassFourTap[3](rightTank);
        rightTank = staticDelayLine[3](rightTank);
        rightTank = staticDelayLine2[3](rightTank);
        m_previouslefttank = leftTank * m_decay;
        m_previousrighttank = rightTank * m_decay;
        T accumulatorL = (0.6*staticDelayLine[2].get_value_at_index(1))
        +(0.6*staticDelayLine[2].get_value_at_index(2))
        +(0.6*staticDelayLine2[2].get_value_at_index(2))
        -(0.6*allpassFourTap[3].get_value_at_index(1))
        +(0.6*staticDelayLine[3].get_value_at_index(1))
        +(0.6*staticDelayLine2[3].get_value_at_index(1))
        -(0.6*staticDelayLine[0].get_value_at_index(1))
        -(0.6*staticDelayLine2[0].get_value_at_index(1))
        -(0.6*allpassFourTap[1].get_value_at_index(1))
        -(0.6*staticDelayLine[1].get_value_at_index(1))
        -(0.6*staticDelayLine2[1].get_value_at_index(1));
        T accumulatorR = (0.6*staticDelayLine[0].get_value_at_index(2))
        +(0.6*staticDelayLine[0].get_value_at_index(3))
        +(0.6*staticDelayLine2[0].get_value_at_index(3))
        -(0.6*allpassFourTap[1].get_value_at_index(2))
        +(0.6*staticDelayLine[1].get_value_at_index(2))
        +(0.6*staticDelayLine2[1].get_value_at_index(2))
        -(0.6*staticDelayLine[2].get_value_at_index(3))
        -(0.6*staticDelayLine2[2].get_value_at_index(3))
        -(0.6*allpassFourTap[3].get_value_at_index(2))
        -(0.6*staticDelayLine[3].get_value_at_index(2))
        -(0.6*staticDelayLine2[3].get_value_at_index(2));
        accumulatorL = ((accumulatorL * m_earlymix) + ((1 - m_earlymix) * earlyReflectionsL));
        accumulatorR = ((accumulatorR * m_earlymix) + ((1 - m_earlymix) * earlyReflectionsR));
        left = ( left + m_mix * ( accumulatorL - left ) ) * m_gain;
        right = ( right + m_mix * ( accumulatorR - right ) ) * m_gain;
        *outputL = left;
        *outputR = right;
    }
    


    void reset(){

        hipassFilter[0].SetSampleRate (m_samplerate );
        hipassFilter[1].SetSampleRate (m_samplerate );
        hipassFilter[0].Reset();
        hipassFilter[1].Reset();

        lowpassFilter[0].SetSampleRate (m_samplerate );
        lowpassFilter[1].SetSampleRate (m_samplerate );
        lowpassFilter[0].Reset();
        lowpassFilter[1].Reset();

        damping[0].SetSampleRate (m_samplerate);
        damping[1].SetSampleRate (m_samplerate);
        damping[0].Reset();
        damping[1].Reset();
        predelayline.zero();
        predelayline.set_length(m_predelaytime);
        allpass[0].zero();
        allpass[1].zero();
        allpass[2].zero();
        allpass[3].zero();
        allpass[0].set_length (0.0048 * m_samplerate);
        allpass[1].set_length (0.0036 * m_samplerate);
        allpass[2].set_length (0.0127 * m_samplerate);
        allpass[3].set_length (0.0093 * m_samplerate);
        allpass[0].set_feedback (0.75);
        allpass[1].set_feedback (0.75);
        allpass[2].set_feedback (0.625);
        allpass[3].set_feedback (0.625);
        allpassFourTap[0].zero();
        allpassFourTap[1].zero();
        allpassFourTap[2].zero();
        allpassFourTap[3].zero();
        allpassFourTap[0].set_length(0.020 * m_samplerate * m_roomsize);
        allpassFourTap[1].set_length(0.060 * m_samplerate * m_roomsize);
        allpassFourTap[2].set_length(0.030 * m_samplerate * m_roomsize);
        allpassFourTap[3].set_length(0.089 * m_samplerate * m_roomsize);
        allpassFourTap[0].set_feedback(m_density1);
        allpassFourTap[1].set_feedback(m_density2);
        allpassFourTap[2].set_feedback(m_density1);
        allpassFourTap[3].set_feedback(m_density2);
        allpassFourTap[0].set_index(0,0,0,0);
        allpassFourTap[1].set_index(0,0.006 * m_samplerate * m_roomsize, 0.041 * m_samplerate * m_roomsize, 0);
        allpassFourTap[2].set_index(0,0,0,0);
        allpassFourTap[3].set_index(0,0.031 * m_samplerate * m_roomsize, 0.011 * m_samplerate * m_roomsize, 0);
        staticDelayLine[0].zero();
        staticDelayLine[1].zero();
        staticDelayLine[2].zero();
        staticDelayLine[3].zero();
        staticDelayLine[0].set_length(0.15 * m_samplerate * m_roomsize);
        staticDelayLine[1].set_length(0.12 * m_samplerate * m_roomsize);
        staticDelayLine[2].set_length(0.14 * m_samplerate * m_roomsize);
        staticDelayLine[3].set_length(0.11 * m_samplerate * m_roomsize);
        staticDelayLine[0].set_index(0, 0.067 * m_samplerate * m_roomsize, 0.011 * m_samplerate * m_roomsize , 0.121 * m_samplerate * m_roomsize);
        staticDelayLine[1].set_index(0, 0.036 * m_samplerate * m_roomsize, 0.089 * m_samplerate * m_roomsize , 0);
        staticDelayLine[2].set_index(0, 0.0089 * m_samplerate * m_roomsize, 0.099 * m_samplerate * m_roomsize , 0);
        staticDelayLine[3].set_index(0, 0.067 * m_samplerate * m_roomsize, 0.0041 * m_samplerate * m_roomsize , 0);
        staticDelayLine2[0].zero();
        staticDelayLine2[1].zero();
        staticDelayLine2[2].zero();
        staticDelayLine2[3].zero();
        staticDelayLine2[0].set_length(0.17 * m_samplerate * m_roomsize);
        staticDelayLine2[1].set_length(0.19 * m_samplerate * m_roomsize);
        staticDelayLine2[2].set_length(0.23 * m_samplerate * m_roomsize);
        staticDelayLine2[3].set_length(0.29 * m_samplerate * m_roomsize);
        staticDelayLine2[0].set_index(0.017 * m_samplerate * m_roomsize, 0.037 * m_samplerate * m_roomsize, 0.019 * m_samplerate * m_roomsize , 0.141 * m_samplerate * m_roomsize);
        staticDelayLine2[1].set_index(0.019 * m_samplerate * m_roomsize, 0.041 * m_samplerate * m_roomsize, 0.189 * m_samplerate * m_roomsize , 0.289 * m_samplerate * m_roomsize);
        staticDelayLine2[2].set_index(0.023 * m_samplerate * m_roomsize, 0.289 * m_samplerate * m_roomsize, 0.079 * m_samplerate * m_roomsize , 0.039 * m_samplerate * m_roomsize);
        staticDelayLine2[3].set_index(0.029 * m_samplerate * m_roomsize, 0.037 * m_samplerate * m_roomsize, 0.041 * m_samplerate * m_roomsize ,  0.043 * m_samplerate * m_roomsize);
        earlyReflectionsDelayLine[0].zero();
        earlyReflectionsDelayLine[1].zero();
        earlyReflectionsDelayLine[0].set_length(0.089 * m_samplerate);
        earlyReflectionsDelayLine[0].set_index (0, 0.0199*m_samplerate, 0.0219*m_samplerate, 0.0354*m_samplerate,0.0389*m_samplerate, 0.0414*m_samplerate, 0.0692*m_samplerate, 0);
        earlyReflectionsDelayLine[1].set_length(0.069 * m_samplerate);
        earlyReflectionsDelayLine[1].set_index (0, 0.0099*m_samplerate, 0.011*m_samplerate, 0.0182*m_samplerate,0.0189*m_samplerate, 0.0213*m_samplerate, 0.0431*m_samplerate, 0);
    }

    void setSampleRate(T sr){
        m_samplerate = sr;

        reset();
    }
};


template<typename T, long max_length>
class mv_allpass
{
private:
    T   buffer[max_length];
    int read_write_index;
    int length;
    T   feedback;
    
public:
    mv_allpass()
    {
        set_length(max_length - 1);
        zero();
        feedback = 0.5;
    }
    
    T operator()(T input)
    {
        T output = buffer[read_write_index] - input * feedback;
        buffer[read_write_index] = input + output * feedback;
        
        if(++read_write_index >= length)
            read_write_index = 0;
        
        return output;
    }
    
    void set_length(int in_length)
    {
        if(in_length >= max_length)
            in_length = max_length;
        
        if(in_length < 0 )
            in_length = 0;
        
        this->length = in_length;
    }
    
    void set_feedback(T in_feedback)
    {
        feedback = in_feedback;
    }
    
    void zero()
    {
        T       *buf = buffer;
        long    count = max_length;
        T       localvalue = 0.0;
        
        --buf;
        while(--count >= 0)
        {
            *++buf = localvalue;
        }
        
        read_write_index = 0;
    }
};

template<typename T, long max_length>
class mv_staticallpass4tap
{
private:
    T       buffer[max_length];
    long    write_index;
    long    read_index1, read_index2, read_index3, read_index4;
    long    length;
    T       feedback;

public:
    mv_staticallpass4tap()
    {
        write_index = 0;
        read_index1 = 0;
        read_index2 = 0;
        read_index3 = 0;
        read_index4 = 0;
        length = 0;
        feedback = 0.5;
        set_length(max_length - 1);
        zero();
    }

    T operator()(T input)
    {
        T output = buffer[read_index1] - input * feedback;
        buffer[write_index] = input + output * feedback;

        if(++write_index >= length)
            write_index = 0;
        
        if(++read_index1 >= length)
            read_index1 = 0;
        
        if(++read_index2 >= length)
            read_index2 = 0;
        
        if(++read_index3 >= length)
            read_index3 = 0;
        
        if(++read_index4 >= length)
            read_index4 = 0;

        return output;
    }

    void set_index(long in_index1_len, long in_index2_len, long in_index3_len, long in_index4_len)
    {
        read_index1 = write_index - in_index1_len;
        if(read_index1 < 0)
            read_index1 += length;
        
        read_index2 = write_index - in_index2_len;
        if(read_index2 < 0)
            read_index2 += length;
        
        read_index3 = write_index - in_index3_len;
        if(read_index3 < 0)
            read_index3 += length;
        
        read_index4 = write_index - in_index4_len;
        if(read_index4 < 0)
            read_index4 += length;
    }

    T get_value_at_index(long in_index)
    {
        switch(in_index)
        {
            case 0:
                return buffer[read_index1];
                break;
                
            case 1:
                return buffer[read_index2];
                break;
                
            case 2:
                return buffer[read_index3];
                break;
                
            case 3:
                return buffer[read_index4];
                break;
                
            default:
                return buffer[read_index1];
                break;
        }
    }

    void set_length(long in_length)
    {
        long pre_len = this->length;
        
        if(in_length >= max_length)
            in_length = max_length;
        
        if(in_length < 0)
            in_length = 0;
        

        this->length = in_length;
        
        long diff_len = in_length - pre_len;
        
        read_index1 += diff_len;
        if(read_index1 < 0)
            read_index1 += length;
        if(read_index1 > length)
            read_index1 -= length;
        
        read_index2 += diff_len;
        if(read_index2 < 0)
            read_index2 += length;
        if(read_index2 > length)
            read_index2 -= length;

        read_index3 += diff_len;
        if(read_index3 < 0)
            read_index3 += length;
        if(read_index3 > length)
            read_index3 -= length;

        read_index4 += diff_len;
        if(read_index4 < 0)
            read_index4 += length;
        if(read_index4 > length)
            read_index4 -= length;

//        read_index1 = write_index - this->length;
//        if(read_index1 < 0)
//            read_index1 += max_length;
//
//        read_index2 = write_index - this->length;
//        if(read_index2 < 0)
//            read_index2 += max_length;
//
//        read_index3 = write_index - this->length;
//        if(read_index3 < 0)
//            read_index3 += max_length;
//
//        read_index4 = write_index - this->length;
//        if(read_index4 < 0)
//            read_index4 += max_length;
        
    }

    void zero()
    {
        T       *buf = buffer;
        long    count = max_length;
        T       localvalue = 0.0;
        
        --buf;
        while(--count >= 0)
        {
            *++buf = localvalue;
        }
    }

    void set_feedback(T in_feedback)
    {
        feedback = in_feedback;
    }
};

template<typename T, long max_length>
class mv_staticdelayline
{
private:
    T       buffer[max_length];
    long    write_index = 0;
    long    read_index = 0;
    long    length;

public:
    mv_staticdelayline()
    {
        write_index = 0;
        read_index = 0;
        length = 0;
        set_length(max_length - 1);
        zero();
    }

    T operator()(T input)
    {
        if(read_index < 0)
            read_index = 0;
        
        if(write_index < 0)
            write_index = 0;
        
        T output = buffer[read_index];
        buffer[write_index] = input;
        
        if(++write_index >= length)
            write_index = 0;
        
        if(++read_index >= length)
            read_index = 0;
        
        return output;
        
    }

    void set_length (long in_length)
    {
        long pre_len = this->length;
        
        if(in_length >= max_length)
            in_length = max_length;
        
        if(in_length < 0)
            in_length = 0;
        
        
        this->length = in_length;
        
        long diff_len = in_length - pre_len;
        

        read_index += diff_len;
        if(read_index < 0)
            read_index += length;
        if(read_index > length)
            read_index -= length;

        //        read_index = write_index - length;
//        if(read_index < 0)
//            read_index += max_length;
    }

    void zero()
    {
        T       *buf = buffer;
        long    count = max_length;
        T       localvalue = 0.0;
        
        --buf;
        while(--count >= 0)
        {
            *++buf = localvalue;
        }
    }
};


template<typename T, long max_length>
class mv_staticdelayline4tap
{
private:
    T       buffer[max_length];
    long    write_index;
    long    read_index1, read_index2, read_index3, read_index4;
    long    length;

public:
    mv_staticdelayline4tap()
    {
        write_index = 0;
        read_index1 = 0;
        read_index2 = 0;
        read_index3 = 0;
        read_index4 = 0;
        length = 0;
        set_length(max_length - 1);
        zero();
    }

    //get ouput and iterate
    T operator()(T input)
    {
        T output = buffer[read_index1];
        buffer[write_index] = input;
        
        if(++write_index >= length)
            write_index = 0;
        
        if(++read_index1 >= length)
            read_index1 = 0;
        
        if(++read_index2 >= length)
            read_index2 = 0;
        
        if(++read_index3 >= length)
            read_index3 = 0;
        
        if(++read_index4 >= length)
            read_index4 = 0;
        
        return output;

    }

    void set_index(long in_index1_len, long in_index2_len, long in_index3_len, long in_index4_len)
    {
        read_index1 = write_index - in_index1_len;
        if(read_index1 < 0)
            read_index1 += length;
        
        read_index2 = write_index - in_index2_len;
        if(read_index2 < 0)
            read_index2 += length;
        
        read_index3 = write_index - in_index3_len;
        if(read_index3 < 0)
            read_index3 += length;
        
        read_index4 = write_index - in_index4_len;
        if(read_index4 < 0)
            read_index4 += length;
    }

    T get_value_at_index (long in_index)
    {
        switch (in_index)
        {
            case 0:
                return buffer[read_index1];
                break;
            case 1:
                return buffer[read_index2];
                break;
            case 2:
                return buffer[read_index3];
                break;
            case 3:
                return buffer[read_index4];
                break;
            default:
                return buffer[read_index1];
                break;
        }
    }


    void set_length(long in_length)
    {
        long pre_len = this->length;
        
        if(in_length >= max_length)
            in_length = max_length;
        
        if(in_length < 0)
            in_length = 0;
        
        
        this->length = in_length;
        
        long diff_len = in_length - pre_len;
        
        read_index1 += diff_len;
        if(read_index1 < 0)
            read_index1 += length;
        if(read_index1 > length)
            read_index1 -= length;
        
        read_index2 += diff_len;
        if(read_index2 < 0)
            read_index2 += length;
        if(read_index2 > length)
            read_index2 -= length;
        
        read_index3 += diff_len;
        if(read_index3 < 0)
            read_index3 += length;
        if(read_index3 > length)
            read_index3 -= length;
        
        read_index4 += diff_len;
        if(read_index4 < 0)
            read_index4 += length;
        if(read_index4 > length)
            read_index4 -= length;
        
        //        read_index1 = write_index - this->length;
        //        if(read_index1 < 0)
        //            read_index1 += max_length;
        //
        //        read_index2 = write_index - this->length;
        //        if(read_index2 < 0)
        //            read_index2 += max_length;
        //
        //        read_index3 = write_index - this->length;
        //        if(read_index3 < 0)
        //            read_index3 += max_length;
        //
        //        read_index4 = write_index - this->length;
        //        if(read_index4 < 0)
        //            read_index4 += max_length;
        
    }
    


    void zero()
    {
        T       *buf = buffer;
        long    count = max_length;
        T       localvalue = 0.0;
        
        --buf;
        while(--count >= 0)
        {
            *++buf = localvalue;
        }
    }
};

template<typename T, long max_length>
class mv_staticdelayline8tap
{
private:
    T       buffer[max_length];
    long    write_index;
    long    read_index1, read_index2, read_index3, read_index4;
    long    read_index5, read_index6, read_index7, read_index8;
    long    length;
    
public:
    mv_staticdelayline8tap()
    {
        write_index = 0;
        read_index1 = 0;
        read_index2 = 0;
        read_index3 = 0;
        read_index4 = 0;
        read_index5 = 0;
        read_index6 = 0;
        read_index7 = 0;
        read_index8 = 0;
        length = 0;
        set_length(max_length - 1);
        zero();
    }
    
    //get ouput and iterate
    T operator()(T input)
    {
        T output = buffer[read_index1];
        buffer[write_index] = input;
        
        if(++write_index >= length)
            write_index = 0;
        
        if(++read_index1 >= length)
            read_index1 = 0;
        
        if(++read_index2 >= length)
            read_index2 = 0;
        
        if(++read_index3 >= length)
            read_index3 = 0;
        
        if(++read_index4 >= length)
            read_index4 = 0;
        
        if(++read_index5 >= length)
            read_index5 = 0;
        
        if(++read_index6 >= length)
            read_index6 = 0;
        
        if(++read_index7 >= length)
            read_index7 = 0;
        
        if(++read_index8 >= length)
            read_index8 = 0;
        
        return output;
        
    }
    
    void set_index(long in_index1_len, long in_index2_len, long in_index3_len, long in_index4_len,
                   long in_index5_len, long in_index6_len, long in_index7_len, long in_index8_len
                   )
    {
        read_index1 = write_index - in_index1_len;
        if(read_index1 < 0)
            read_index1 += length;
        
        read_index2 = write_index - in_index2_len;
        if(read_index2 < 0)
            read_index2 += length;
        
        read_index3 = write_index - in_index3_len;
        if(read_index3 < 0)
            read_index3 += length;
        
        read_index4 = write_index - in_index4_len;
        if(read_index4 < 0)
            read_index4 += length;
        
        read_index5 = write_index - in_index5_len;
        if(read_index5 < 0)
            read_index5 += length;
        
        read_index6 = write_index - in_index6_len;
        if(read_index6 < 0)
            read_index6 += length;
        
        read_index7 = write_index - in_index7_len;
        if(read_index7 < 0)
            read_index7 += length;
        
        read_index8 = write_index - in_index8_len;
        if(read_index8 < 0)
            read_index8 += length;
        
    }
    
    T get_value_at_index (long in_index)
    {
        switch(in_index)
        {
            case 0:
                return buffer[read_index1];
                break;
            case 1:
                return buffer[read_index2];
                break;
            case 2:
                return buffer[read_index3];
                break;
            case 3:
                return buffer[read_index4];
                break;
            case 4:
                return buffer[read_index4];
                break;
            case 5:
                return buffer[read_index5];
                break;
            case 6:
                return buffer[read_index6];
                break;
            case 7:
                return buffer[read_index7];
                break;
            default:
                return buffer[read_index8];
                break;
        }
    }
    
    void set_length(long in_length)
    {
        long pre_len = this->length;
        
        if(in_length >= max_length)
            in_length = max_length;
        
        if(in_length < 0)
            in_length = 0;
        
        
        this->length = in_length;
        
        long diff_len = in_length - pre_len;
        
        read_index1 += diff_len;
        if(read_index1 < 0)
            read_index1 += length;
        if(read_index1 > length)
            read_index1 -= length;
        
        read_index2 += diff_len;
        if(read_index2 < 0)
            read_index2 += length;
        if(read_index2 > length)
            read_index2 -= length;
        
        read_index3 += diff_len;
        if(read_index3 < 0)
            read_index3 += length;
        if(read_index3 > length)
            read_index3 -= length;
        
        read_index4 += diff_len;
        if(read_index4 < 0)
            read_index4 += length;
        if(read_index4 > length)
            read_index4 -= length;
        
        read_index5 += diff_len;
        if(read_index5 < 0)
            read_index5 += length;
        if(read_index5 > length)
            read_index5 -= length;
        
        read_index6 += diff_len;
        if(read_index6 < 0)
            read_index6 += length;
        if(read_index6 > length)
            read_index6 -= length;
        
        read_index7 += diff_len;
        if(read_index7 < 0)
            read_index7 += length;
        if(read_index7 > length)
            read_index7 -= length;
        
        read_index8 += diff_len;
        if(read_index8 < 0)
            read_index8 += length;
        if(read_index8 > length)
            read_index8 -= length;
        
        //        read_index1 = write_index - this->length;
        //        if(read_index1 < 0)
        //            read_index1 += max_length;
        //
        //        read_index2 = write_index - this->length;
        //        if(read_index2 < 0)
        //            read_index2 += max_length;
        //
        //        read_index3 = write_index - this->length;
        //        if(read_index3 < 0)
        //            read_index3 += max_length;
        //
        //        read_index4 = write_index - this->length;
        //        if(read_index4 < 0)
        //            read_index4 += max_length;
        
    }
    
    
//    void set_length (long in_length)
//    {
//        if(in_length >= max_length)
//            in_length = max_length;
//
//        if(in_length < 0 )
//            in_length = 0;
//
//        this->length = in_length;
//
//
//        read_index1 = write_index - length;
//        if(read_index1 < 0)
//            read_index1 += max_length;
//
//        read_index2 = write_index - length;
//        if(read_index2 < 0)
//            read_index2 += max_length;
//
//        read_index3 = write_index - length;
//        if(read_index3 < 0)
//            read_index3 += max_length;
//
//        read_index4 = write_index - length;
//        if(read_index4 < 0)
//            read_index4 += max_length;
//
//        read_index5 = write_index - length;
//        if(read_index5 < 0)
//            read_index5 += max_length;
//
//        read_index6 = write_index - length;
//        if(read_index6 < 0)
//            read_index6 += max_length;
//
//        read_index7 = write_index - length;
//        if(read_index7 < 0)
//            read_index7 += max_length;
//
//        read_index8 = write_index - length;
//        if(read_index8 < 0)
//            read_index8 += max_length;
//
//    }
//
    
    void zero()
    {
        T       *buf = buffer;
        long    count = max_length;
        T       localvalue = 0.0;
        
        --buf;
        while(--count >= 0)
        {
            *++buf = localvalue;
        }
    }
};


//template<typename T, long max_length>
//class mv_staticdelayline8tap
//{
//private:
//    T buffer[max_length];
//    long index1, index2, index3, index4, index5, index6, index7, index8;
//    long Length;
//
//public:
//    mv_staticdelayline8tap()
//    {
//        set_length ( max_length - 1 );
//        zero();
//    }
//
//    //get ouput and iterate
//    T operator()(T input)
//    {
//        T output = buffer[index1];
//        buffer[index1++] = input;
//        if(index1 >= Length)
//            index1 = 0;
//        if(++index2 >= Length)
//            index2 = 0;
//        if(++index3 >= Length)
//            index3 = 0;
//        if(++index4 >= Length)
//            index4 = 0;
//        if(++index5 >= Length)
//            index5 = 0;
//        if(++index6 >= Length)
//            index6 = 0;
//        if(++index7 >= Length)
//            index7 = 0;
//        if(++index8 >= Length)
//            index8 = 0;
//        return output;
//
//    }
//
//    void set_index (long Index1, long Index2, long Index3, long Index4, long Index5, long Index6, long Index7, long Index8)
//    {
//        index1 = Index1;
//        index2 = Index2;
//        index3 = Index3;
//        index4 = Index4;
//        index5 = Index5;
//        index6 = Index6;
//        index7 = Index7;
//        index8 = Index8;
//    }
//
//
//    T get_value_at_index (long Index)
//    {
//        switch (Index)
//        {
//            case 0:
//                return buffer[index1];
//                break;
//            case 1:
//                return buffer[index2];
//                break;
//            case 2:
//                return buffer[index3];
//                break;
//            case 3:
//                return buffer[index4];
//                break;
//            case 4:
//                return buffer[index5];
//                break;
//            case 5:
//                return buffer[index6];
//                break;
//            case 6:
//                return buffer[index7];
//                break;
//            case 7:
//                return buffer[index8];
//                break;
//            default:
//                return buffer[index1];
//                break;
//        }
//    }
//
//
//    void set_length (long Length)
//    {
//       if( Length >= max_length )
//            Length = max_length;
//       if( Length < 0 )
//            Length = 0;
//
//        this->Length = Length;
//    }
//
//
//    void zero()
//    {
////        memset(buffer, 0, sizeof(buffer));
////        if(buffer)
//        {
//            T *buf = buffer;
//            long    count = max_length;
//            T    localvalue = 0.0;
//
//            --buf;
//            while(--count >= 0)
//            {
//                *++buf = localvalue;
//            }
//        }
//
//        index1 = index2  = index3 = index4 = index5 = index6 = index7 = index8 = 0;
//    }
//};

template<typename T, long OverSampleCount>
    class mv_statevariable
    {
    public:

        enum FilterType
        {
            LOWPASS,
            HIGHPASS,
            BANDPASS,
            NOTCH,
            FilterTypeCount
        };

    private:

        T sampleRate;
        T frequency;
        T q;
        T f;

        T low;
        T high;
        T band;
        T notch;

        T *out;

    public:
        mv_statevariable()
        {
            frequency = 1000.;
            sampleRate = 44100. * OverSampleCount;
            UpdateCoefficient();
            Resonance(0);
            Type(LOWPASS);
            Reset();
        }

        T operator()(T input)
        {
            for(unsigned long i = 0; i < OverSampleCount; i++)
            {
                low += f * band + 1e-25;
                high = input - low - q * band;
                band += f * high;
                notch = low + high;
            }
            return *out;
        }

        void Reset()
        {
            low = high = band = notch = 0;
        }

        void SetSampleRate(T sampleRate)
        {
            this->sampleRate = sampleRate * OverSampleCount;
            UpdateCoefficient();
        }

        void Frequency(T frequency)
        {
            this->frequency = frequency;
            UpdateCoefficient();
        }

        void Resonance(T resonance)
        {
            this->q = 2 - 2 * resonance;
        }

        void Type(int type)
        {
            switch(type)
            {
            case LOWPASS:
                out = &low;
                break;

            case HIGHPASS:
                out = &high;
                break;

            case BANDPASS:
                out = &band;
                break;

            case NOTCH:
                out = &notch;
                break;

            default:
                out = &low;
                break;
            }
        }

    private:
        void UpdateCoefficient()
        {
            f = 2. * sinf(3.141592654 * frequency / sampleRate);
        }
    };


#endif

struct MVerbModule : Module {
	enum ParamIds {
        MIX_PARAM,
        PREDELAY_PARAM,
        EARLYMIX_PARAM,
        ROOMSIZE_PARAM,
        DENSITY_PARAM,
        FILTERFREQUENCY_PARAM,
        DECAY_PARAM,
        DAMP_PARAM,
        GAIN_PARAM,
        FREEZE_BUTTON,
        
		NUM_PARAMS
	};
	enum InputIds {
        IN_INPUT_L,
        IN_INPUT_R,
		FREEZE_INPUT,
        
		NUM_INPUTS
	};
	enum OutputIds {
		OUTL_OUTPUT,
		OUTR_OUTPUT,
        
		NUM_OUTPUTS
	};
    
    enum LightIds {
        LI_FREEZE,
        
        NUM_LIGHTS
    };
    
    std::mutex creation_mutex;

//    mv_reverb_original<float>   *real_mverb;
    mv_reverb_original<float>   mverb;
    
//    gverb::gverb<4, 1000>       m_gverbs[2];
    
    float mix = 0.5;
    float predelay = 0.5;
    float earlymix = 0.5;
    float roomsize = 0.5;
    float density = 0.5;
    float filterfrequency = 0.5;
    float decay = 0.5;
    float dampingfrequency = 0.5;
    float gain = 0.5;

    long    sample_count = 0;
    
    float m_last_out_L;
    float m_last_out_R;

    bool freezed = false;
//    bool inited = false;

    dsp::SchmittTrigger buttonTrigger;
    float m_sample_rate;

    MVerbModule(){
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        
        configParam(MIX_PARAM, 0.0, 1.0, 0.5, "Mix Dry-Wet 0..1");
        configParam(PREDELAY_PARAM, 0.0, 1.0, 0.5, "PreDelay 0..1");
        configParam(EARLYMIX_PARAM, 0.0, 1.0, 0.5, "Early Mix 0..1");
        configParam(ROOMSIZE_PARAM, 0.0, 1.0, 0.6, "Roomsize 0.1");
        configParam(DENSITY_PARAM, 0.0, 1.0, 0.5, "Density 0..1");
        configParam(FILTERFREQUENCY_PARAM, 0.0, 1.0, 0.68, "Filter 0..1");
        configParam(DECAY_PARAM, 0.0, 1.0, 0.6, "Decay 0..1");
        configParam(DAMP_PARAM, 0.0, 1.0, 0.8, "Damp 0..1");
        configParam(GAIN_PARAM, 0.0, 1.0, 0.5, "Gain 0..1");
        configParam(FREEZE_BUTTON, 0.0, 1.0, 0.0, "Freeze");

        configBypass(IN_INPUT_L, OUTL_OUTPUT);
        configBypass(IN_INPUT_R, OUTR_OUTPUT);

        this->onReset();
        
    };

//    MVerbModule() :
////    real_mverb(new mv_reverb_original<float>()),
////    mverb(*real_mverb),
//    Module( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS )
//    {
//        //        real_mverb = new mv_reverb_original<float>();
//        //        mverb = *real_mverb;
//        this->onReset();
//    };
    
    virtual ~MVerbModule()
    {
//        if(real_mverb)
//            delete real_mverb;
    };
    
	void process(const ProcessArgs &args) override;

    void onSampleRateChange()  override{
        m_sample_rate = APP->engine->getSampleRate();
        mverb.setSampleRate(m_sample_rate);
    };

    void initializelocal(){
        m_sample_rate = APP->engine->getSampleRate();
        mverb.setSampleRate(m_sample_rate);
        mverb.reset();
        
        mverb.set_mix(mix);
        mverb.set_predelay(predelay);
        mverb.set_earlymix(earlymix);
        mverb.set_gain(gain * 2);
        
        mverb.set_roomsize(roomsize);
        
        mverb.set_density(density);
        
        mverb.set_lowpassfrequency(filterfrequency);
        
        
        mverb.set_decay(decay);
        
        mverb.set_dampingfrequency(dampingfrequency);
        
//        inited = true;
   };
    
    void onReset() override{
        this->initializelocal();
    };
    
};

void MVerbModule::process(const ProcessArgs &args)
{
    sample_count++;
    
    if(sample_count < 44100)
    {
        return;
    }
    
	float outL, outR;

	outL = outR = 0.0;

//    if(inited)
    {
        float old_mix = mix;
        float old_predelay = predelay;
        float old_earlymix = earlymix;
        float old_roomsize = roomsize;
        float old_density = density;
        float old_filterfrequency = filterfrequency;
        float old_decay = decay;
        float old_dampingfrequency = dampingfrequency;
        float old_gain = gain;

        float inL = 0.0, inR = 0.0;
        
        if(!freezed)
        {
            mix = params[MIX_PARAM].getValue();
            predelay = params[PREDELAY_PARAM].getValue();
            earlymix = params[EARLYMIX_PARAM].getValue();
            roomsize = params[ROOMSIZE_PARAM].getValue();
            density = params[DENSITY_PARAM].getValue();
            filterfrequency = params[FILTERFREQUENCY_PARAM].getValue();
            decay = params[DECAY_PARAM].getValue();
            dampingfrequency = params[DAMP_PARAM].getValue();
            gain = params[GAIN_PARAM].getValue();


            if(inputs[IN_INPUT_L].isConnected() && !inputs[IN_INPUT_R].isConnected())
            {
                inR = inL = inputs[IN_INPUT_L].getVoltageSum();
            }
            else if(!inputs[IN_INPUT_L].isConnected() && inputs[IN_INPUT_R].isConnected())
            {
                inR = inL = inputs[IN_INPUT_R].getVoltageSum();
            }
            else if(inputs[IN_INPUT_L].isConnected() && inputs[IN_INPUT_R].isConnected())
            {
                inL = inputs[IN_INPUT_L].getVoltageSum();
                inR = inputs[IN_INPUT_R].getVoltageSum();
            }

            if( old_mix != mix)
            {
                mverb.set_mix(mix);
            }
            
            if( old_predelay != predelay)
            {
                mverb.set_predelay(predelay);
            }
            
            if( old_earlymix != earlymix)
            {
                mverb.set_earlymix(earlymix);
            }

            if( old_roomsize != roomsize)
            {
                mverb.set_roomsize(roomsize);
            }
            
            if( old_density != density)
            {
                mverb.set_density(density);
            }
            
            if( old_filterfrequency != filterfrequency)
            {
                mverb.set_lowpassfrequency(filterfrequency);
            }
            
            if( old_decay != decay)
            {
                mverb.set_decay(decay);
            }
            
            if( old_dampingfrequency != dampingfrequency)
            {
                mverb.set_dampingfrequency(dampingfrequency);
            }
            
            if( old_gain != gain)
            {
                mverb.set_gain(gain * 2);
            }
        }
        
        if(buttonTrigger.process(params[FREEZE_BUTTON].getValue() + ((inputs[FREEZE_INPUT].getVoltage() > 1.0) ? 1.0 : 0.0)))
        {
            freezed = !freezed;
            
            float localdampingfrequency = dampingfrequency;
            if(freezed)
                localdampingfrequency = nyMAXFEEDBACK;
            
            mverb.set_dampingfrequency(localdampingfrequency);
            
            float localdecay = decay;
            if(freezed)
                localdecay = nyMAXFEEDBACK;
            
            mverb.set_decay(localdecay);
        }

    //
    //    if(inputs[FREEZE_INPUT].isConnected()) {
    //        freezed = inputs[FREEZE_INPUT].getVoltage() >= 1.0;
    //
    //        float localdampingfrequency = dampingfrequency;
    //        if(freezed)
    //            localdampingfrequency = MAXFEEDBACK;
    //
    //        mverb.set_dampingfrequency(localdampingfrequency);
    //
    //        float localdecay = decay;
    //        if(freezed)
    //            localdecay = MAXFEEDBACK;
    //
    //        mverb.set_decay(localdecay);
    //    } else {
    //        if(buttonTrigger.process(params[FREEZE_BUTTON].getValue()))
    //        {
    //            freezed = !freezed;
    //
    //            float localdampingfrequency = dampingfrequency;
    //            if(freezed)
    //                localdampingfrequency = MAXFEEDBACK;
    //
    //            mverb.set_dampingfrequency(localdampingfrequency);
    //
    //            float localdecay = decay;
    //            if(freezed)
    //                localdecay = MAXFEEDBACK;
    //
    //            mverb.set_decay(localdecay);
    //        }
    //    }

        
//        if(inputs[FREEZE_INPUT].isConnected()) {
//            freezed = inputs[FREEZE_INPUT].getVoltage() >= 1.0;
//            
//            float localdampingfrequency = dampingfrequency;
//            if(freezed)
//                localdampingfrequency = nyMAXFEEDBACK;
//            
//            mverb.set_dampingfrequency(localdampingfrequency);
//            
//            float localdecay = decay;
//            if(freezed)
//                localdecay = nyMAXFEEDBACK;
//            
//            mverb.set_decay(localdecay);
//        } else {
//            if(buttonTrigger.process(params[FREEZE_BUTTON].getValue()))
//            {
//                freezed = !freezed;
//                
//                float localdampingfrequency = dampingfrequency;
//                if(freezed)
//                    localdampingfrequency = nyMAXFEEDBACK;
//                
//                mverb.set_dampingfrequency(localdampingfrequency);
//                
//                float localdecay = decay;
//                if(freezed)
//                    localdecay = nyMAXFEEDBACK;
//                
//                mverb.set_decay(localdecay);
//            }
//        }
        
        lights[LI_FREEZE].value = freezed ? 1.0 : 0.0;

        if(!freezed)
        {
            mverb.process_one(inL, inR, &outL, &outR);
            
//            double yl1, yr1, yl2, yr2;
//
//            m_gverbs[0].process(inL * 0.1, &yl1, &yr1);
//            m_gverbs[1].process(inR * 0.1, &yl2, &yr2);
//
//            m_last_out_L = outL = (yl1 + yl2) * 5.0;
//            m_last_out_R = outR = (yr1 + yr2) * 5.0;
//
//            outL = inL + (inL - outL) * mix;
//            outR = inR + (inR - outR) * mix;
        }
        else
        {
            mverb.process_one(0.0, 0.0, &outL, &outR);
            
//            double yl1, yr1, yl2, yr2;
//            
//            m_gverbs[0].processfreezed(m_last_out_L * 0.1, &yl1, &yr1);
//            m_gverbs[1].processfreezed(m_last_out_R * 0.1, &yl2, &yr2);
//            
////            m_last_out_L = outL = (yl1 + yl2) * 5.0;
////            m_last_out_R = outR = (yr1 + yr2) * 5.0;
//            outL = (yl1 + yl2) * 5.0;
//            outR = (yr1 + yr2) * 5.0;
//
//            outL = (outL) * mix;
//            outR = (outR) * mix;

        }
    }
	outputs[OUTL_OUTPUT].setVoltage(outL);
	outputs[OUTR_OUTPUT].setVoltage(outR);
};



struct MVerbWidget : ModuleWidget
{
    MVerbWidget(Module *module)
    {
        setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/MVerb.svg")));
        box.size = Vec(15 * 6, 380);
        
        addInput(createInput<PJ301MPort>(Vec(12 - 1, 29 - 1), module, MVerbModule::IN_INPUT_L));
        addInput(createInput<PJ301MPort>(Vec(56 - 1, 29 - 1), module, MVerbModule::IN_INPUT_R));
        
        float      base = 75;
        float      left1 = 56 + 1;
        float      differential = 23;
        
        addParam(createParam<Davies1900hWhiteKnob20>(Vec(left1, base), module, MVerbModule::MIX_PARAM));
        addParam(createParam<Davies1900hWhiteKnob20>(Vec(left1, base + differential * 1), module, MVerbModule::PREDELAY_PARAM));
        addParam(createParam<Davies1900hWhiteKnob20>(Vec(left1, base + differential * 2), module, MVerbModule::EARLYMIX_PARAM));
        addParam(createParam<Davies1900hWhiteKnob20>(Vec(left1, base + differential * 3), module, MVerbModule::ROOMSIZE_PARAM));
        addParam(createParam<Davies1900hWhiteKnob20>(Vec(left1, base + differential * 4), module, MVerbModule::DENSITY_PARAM));
        addParam(createParam<Davies1900hWhiteKnob20>(Vec(left1, base + differential * 5), module, MVerbModule::FILTERFREQUENCY_PARAM));
        addParam(createParam<Davies1900hWhiteKnob20>(Vec(left1, base + differential * 6), module, MVerbModule::DECAY_PARAM));
        addParam(createParam<Davies1900hWhiteKnob20>(Vec(left1, base + differential * 7), module, MVerbModule::DAMP_PARAM));
        addParam(createParam<Davies1900hWhiteKnob20>(Vec(left1, base + differential * 8), module, MVerbModule::GAIN_PARAM));
        
        addInput(createInput<PJ301MPort>(Vec(left1 - 1 - 1, base + differential * 9 - 1 - 1), module, MVerbModule::FREEZE_INPUT));
        
        addParam(createParam<nyTapper12>(Vec(41, base + differential * 9 + 3), module, MVerbModule::FREEZE_BUTTON));
        addChild(createLight<SmallLight<GreenLight>>(Vec(41 + 3, base + differential * 9 + 6), module, MVerbModule::LI_FREEZE));

        addOutput(createOutput<PJ301MPort>(Vec(12 - 1, 314 - 1), module, MVerbModule::OUTL_OUTPUT));
        addOutput(createOutput<PJ301MPort>(Vec(56 - 1, 314 - 1), module, MVerbModule::OUTR_OUTPUT));
    }
};

Model *modelMVerb = createModel<MVerbModule, MVerbWidget>("MVerb");
