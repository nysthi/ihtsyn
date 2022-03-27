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

#ifndef EMVERB_H
#define EMVERB_H

//forward declaration
template<typename T, int maxLength> class mv_allpass;
template<typename T, int maxLength> class mv_staticallpass4tap;
template<typename T, int maxLength> class mv_staticdelayline;
template<typename T, int maxLength> class mv_staticdelayline4tap;

template<typename T, int maxLength> class mv_staticdelayline8tap;
template<typename T, int OverSampleCount> class mv_statevariable;

//#define BUFFERSIZE 96000
#define BUFFERSIZE 4096

#define MAXFEEDBACK 0.99

template<typename T>
class mv_reverb_twisted
{
public:
    mv_allpass<T, BUFFERSIZE> allpass[4];
    mv_staticallpass4tap<T, BUFFERSIZE> allpassFourTap[4];
    mv_statevariable<T,4> hipassFilter[2];
    mv_statevariable<T,4> lowpassFilter[2];
    mv_statevariable<T,4> damping[2];
    mv_staticdelayline<T, BUFFERSIZE> predelayline;
    mv_staticdelayline4tap<T, BUFFERSIZE> staticDelayLine[4];
    mv_staticdelayline4tap<T, BUFFERSIZE> staticDelayLine2[4];
    mv_staticdelayline8tap<T, BUFFERSIZE> earlyReflectionsDelayLine[2];
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

    mv_reverb_twisted(){
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

    ~mv_reverb_twisted(){
        //nowt to do here
    }
    
    void set_roomsizeCLEAN(T roomsize){
        
        for(int ix = 0; ix < 4; ix++)
        {
            allpass[ix].Clear();
            allpassFourTap[ix].Clear();
            staticDelayLine[ix].Clear();
            staticDelayLine2[ix].Clear();
        }
        predelayline.Clear();
        for(int ix = 0; ix < 2; ix++)
        {
            earlyReflectionsDelayLine[ix].Clear();
//            hipassFilter[ix].Clear();
//            lowpassFilter[ix].Clear();
//            damping[ix].Clear();
        }
//        mv_statevariable<T,4> hipassFilter[2];
//        mv_statevariable<T,4> lowpassFilter[2];
//        mv_statevariable<T,4> damping[2];
//        mv_staticdelayline<T, BUFFERSIZE> predelayline;
//        mv_staticdelayline4tap<T, BUFFERSIZE> staticDelayLine[4];
//        mv_staticdelayline4tap<T, BUFFERSIZE> staticDelayLine2[4];
//        mv_staticdelayline8tap<T, BUFFERSIZE> earlyReflectionsDelayLine[2];

        m_roomsize = (0.95 * roomsize * roomsize) + 0.05;
                allpassFourTap[0].Clear();
                allpassFourTap[1].Clear();
                allpassFourTap[2].Clear();
                allpassFourTap[3].Clear();
        allpassFourTap[0].SetLength(0.020 * m_samplerate * m_roomsize);
        allpassFourTap[1].SetLength(0.060 * m_samplerate * m_roomsize);
        allpassFourTap[2].SetLength(0.030 * m_samplerate * m_roomsize);
        allpassFourTap[3].SetLength(0.089 * m_samplerate * m_roomsize);
        allpassFourTap[1].SetIndex(0,0.006 * m_samplerate * m_roomsize, 0.041 * m_samplerate * m_roomsize, 0);
        allpassFourTap[3].SetIndex(0,0.031 * m_samplerate * m_roomsize, 0.011 * m_samplerate * m_roomsize, 0);
                staticDelayLine[0].Clear();
                staticDelayLine[1].Clear();
                staticDelayLine[2].Clear();
                staticDelayLine[3].Clear();
        staticDelayLine[0].SetLength(0.15 * m_samplerate * m_roomsize);
        staticDelayLine[1].SetLength(0.12 * m_samplerate * m_roomsize);
        staticDelayLine[2].SetLength(0.14 * m_samplerate * m_roomsize);
        staticDelayLine[3].SetLength(0.11 * m_samplerate * m_roomsize);
        staticDelayLine[0].SetIndex(0, 0.067 * m_samplerate * m_roomsize, 0.011 * m_samplerate * m_roomsize , 0.121 * m_samplerate * m_roomsize);
        staticDelayLine[1].SetIndex(0, 0.036 * m_samplerate * m_roomsize, 0.089 * m_samplerate * m_roomsize , 0);
        staticDelayLine[2].SetIndex(0, 0.0089 * m_samplerate * m_roomsize, 0.099 * m_samplerate * m_roomsize , 0);
        staticDelayLine[3].SetIndex(0, 0.067 * m_samplerate * m_roomsize, 0.0041 * m_samplerate * m_roomsize , 0);
                staticDelayLine2[0].Clear();
                staticDelayLine2[1].Clear();
                staticDelayLine2[2].Clear();
                staticDelayLine2[3].Clear();
        staticDelayLine2[0].SetLength(0.17 * m_samplerate * m_roomsize);
        staticDelayLine2[1].SetLength(0.19 * m_samplerate * m_roomsize);
        staticDelayLine2[2].SetLength(0.23 * m_samplerate * m_roomsize);
        staticDelayLine2[3].SetLength(0.29 * m_samplerate * m_roomsize);
        staticDelayLine2[0].SetIndex(0.017 * m_samplerate * m_roomsize, 0.037 * m_samplerate * m_roomsize, 0.019 * m_samplerate * m_roomsize , 0.141 * m_samplerate * m_roomsize);
        staticDelayLine2[1].SetIndex(0.019 * m_samplerate * m_roomsize, 0.041 * m_samplerate * m_roomsize, 0.189 * m_samplerate * m_roomsize , 0.289 * m_samplerate * m_roomsize);
        staticDelayLine2[2].SetIndex(0.023 * m_samplerate * m_roomsize, 0.289 * m_samplerate * m_roomsize, 0.079 * m_samplerate * m_roomsize , 0.039 * m_samplerate * m_roomsize);
        staticDelayLine2[3].SetIndex(0.029 * m_samplerate * m_roomsize, 0.037 * m_samplerate * m_roomsize, 0.041 * m_samplerate * m_roomsize ,  0.043 * m_samplerate * m_roomsize);
    }
    
    void set_roomsize(T roomsize){
        m_roomsize = (0.95 * roomsize * roomsize) + 0.05;
        //        allpassFourTap[0].Clear();
        //        allpassFourTap[1].Clear();
        //        allpassFourTap[2].Clear();
        //        allpassFourTap[3].Clear();
        allpassFourTap[0].SetLength(0.020 * m_samplerate * m_roomsize);
        allpassFourTap[1].SetLength(0.060 * m_samplerate * m_roomsize);
        allpassFourTap[2].SetLength(0.030 * m_samplerate * m_roomsize);
        allpassFourTap[3].SetLength(0.089 * m_samplerate * m_roomsize);
        allpassFourTap[1].SetIndex(0,0.006 * m_samplerate * m_roomsize, 0.041 * m_samplerate * m_roomsize, 0);
        allpassFourTap[3].SetIndex(0,0.031 * m_samplerate * m_roomsize, 0.011 * m_samplerate * m_roomsize, 0);
        //        staticDelayLine[0].Clear();
        //        staticDelayLine[1].Clear();
        //        staticDelayLine[2].Clear();
        //        staticDelayLine[3].Clear();
        staticDelayLine[0].SetLength(0.15 * m_samplerate * m_roomsize);
        staticDelayLine[1].SetLength(0.12 * m_samplerate * m_roomsize);
        staticDelayLine[2].SetLength(0.14 * m_samplerate * m_roomsize);
        staticDelayLine[3].SetLength(0.11 * m_samplerate * m_roomsize);
        staticDelayLine[0].SetIndex(0, 0.067 * m_samplerate * m_roomsize, 0.011 * m_samplerate * m_roomsize , 0.121 * m_samplerate * m_roomsize);
        staticDelayLine[1].SetIndex(0, 0.036 * m_samplerate * m_roomsize, 0.089 * m_samplerate * m_roomsize , 0);
        staticDelayLine[2].SetIndex(0, 0.0089 * m_samplerate * m_roomsize, 0.099 * m_samplerate * m_roomsize , 0);
        staticDelayLine[3].SetIndex(0, 0.067 * m_samplerate * m_roomsize, 0.0041 * m_samplerate * m_roomsize , 0);
        //        staticDelayLine2[0].Clear();
        //        staticDelayLine2[1].Clear();
        //        staticDelayLine2[2].Clear();
        //        staticDelayLine2[3].Clear();
        staticDelayLine2[0].SetLength(0.17 * m_samplerate * m_roomsize);
        staticDelayLine2[1].SetLength(0.19 * m_samplerate * m_roomsize);
        staticDelayLine2[2].SetLength(0.23 * m_samplerate * m_roomsize);
        staticDelayLine2[3].SetLength(0.29 * m_samplerate * m_roomsize);
        staticDelayLine2[0].SetIndex(0.017 * m_samplerate * m_roomsize, 0.037 * m_samplerate * m_roomsize, 0.019 * m_samplerate * m_roomsize , 0.141 * m_samplerate * m_roomsize);
        staticDelayLine2[1].SetIndex(0.019 * m_samplerate * m_roomsize, 0.041 * m_samplerate * m_roomsize, 0.189 * m_samplerate * m_roomsize , 0.289 * m_samplerate * m_roomsize);
        staticDelayLine2[2].SetIndex(0.023 * m_samplerate * m_roomsize, 0.289 * m_samplerate * m_roomsize, 0.079 * m_samplerate * m_roomsize , 0.039 * m_samplerate * m_roomsize);
        staticDelayLine2[3].SetIndex(0.029 * m_samplerate * m_roomsize, 0.037 * m_samplerate * m_roomsize, 0.041 * m_samplerate * m_roomsize ,  0.043 * m_samplerate * m_roomsize);
    }
    
    inline void set_predelay(T predelay)
    {
        m_predelaytime = predelay * 200 * (m_samplerate / 1000);
        predelayline.SetLength(m_predelaytime);
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
        allpassFourTap[1].SetFeedback(m_density2);
        allpassFourTap[3].SetFeedback(m_density2);
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
        allpassFourTap[0].SetFeedback(m_density1);
        allpassFourTap[2].SetFeedback(m_density1);
    }
    
    void process_one(T inputL, T inputR, T *outputL, T *outputR){
        T left = inputL;
        T right = inputR;
        T bandwidthLeft = lowpassFilter[0](left) ;
        T bandwidthRight = lowpassFilter[1](right) ;
        T earlyReflectionsL = earlyReflectionsDelayLine[0] ( bandwidthLeft * 0.5 + bandwidthRight * 0.3 )
        + earlyReflectionsDelayLine[0].GetIndex(2) * 0.6
        + earlyReflectionsDelayLine[0].GetIndex(3) * 0.4
        + earlyReflectionsDelayLine[0].GetIndex(4) * 0.3
        + earlyReflectionsDelayLine[0].GetIndex(5) * 0.3
        + earlyReflectionsDelayLine[0].GetIndex(6) * 0.1
        + earlyReflectionsDelayLine[0].GetIndex(7) * 0.1
        + ( bandwidthLeft * 0.4 + bandwidthRight * 0.2 ) * 0.5 ;
        T earlyReflectionsR = earlyReflectionsDelayLine[1] ( bandwidthLeft * 0.3 + bandwidthRight * 0.5 )
        + earlyReflectionsDelayLine[1].GetIndex(2) * 0.6
        + earlyReflectionsDelayLine[1].GetIndex(3) * 0.4
        + earlyReflectionsDelayLine[1].GetIndex(4) * 0.3
        + earlyReflectionsDelayLine[1].GetIndex(5) * 0.3
        + earlyReflectionsDelayLine[1].GetIndex(6) * 0.1
        + earlyReflectionsDelayLine[1].GetIndex(7) * 0.1
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
        T accumulatorL = (0.6*staticDelayLine[2].GetIndex(1))
        +(0.6*staticDelayLine[2].GetIndex(2))
        -(0.6*allpassFourTap[3].GetIndex(1))
        +(0.6*staticDelayLine[3].GetIndex(1))
        -(0.6*staticDelayLine[0].GetIndex(1))
        -(0.6*allpassFourTap[1].GetIndex(1))
        -(0.6*staticDelayLine[1].GetIndex(1))
        ;
        T accumulatorR = (0.6*staticDelayLine[0].GetIndex(2))
        +(0.6*staticDelayLine[0].GetIndex(3))
        -(0.6*allpassFourTap[1].GetIndex(2))
        +(0.6*staticDelayLine[1].GetIndex(2))
        -(0.6*staticDelayLine[2].GetIndex(3))
        -(0.6*allpassFourTap[3].GetIndex(2))
        -(0.6*staticDelayLine[3].GetIndex(2))
        ;
//        accumulatorL = ((accumulatorL * m_earlymix) + ((1 - m_earlymix) * earlyReflectionsL));
//        accumulatorR = ((accumulatorR * m_earlymix) + ((1 - m_earlymix) * earlyReflectionsR));
        accumulatorL = earlyReflectionsL + (accumulatorL - earlyReflectionsL) * m_earlymix;
        accumulatorR = earlyReflectionsR + (accumulatorR - earlyReflectionsR) * m_earlymix;
        left = left + (accumulatorL - left) * m_mix;
        right = right + (accumulatorR - right ) * m_mix;
        left *= m_gain;
        right *= m_gain;
        *outputL = left;
        *outputR = right;
    }
    

    void process_one_twisted(T inputL, T inputR, T *outputL, T *outputR){
        T left = inputL;
        T right = inputR;
        T bandwidthLeft = lowpassFilter[0](left) ;
        T bandwidthRight = lowpassFilter[1](right) ;
        T earlyReflectionsL = earlyReflectionsDelayLine[0] ( bandwidthLeft * 0.5 + bandwidthRight * 0.3 )
        + earlyReflectionsDelayLine[0].GetIndex(2) * 0.6
        + earlyReflectionsDelayLine[0].GetIndex(3) * 0.4
        + earlyReflectionsDelayLine[0].GetIndex(4) * 0.3
        + earlyReflectionsDelayLine[0].GetIndex(5) * 0.3
        + earlyReflectionsDelayLine[0].GetIndex(6) * 0.1
        + earlyReflectionsDelayLine[0].GetIndex(7) * 0.1
        + ( bandwidthLeft * 0.4 + bandwidthRight * 0.2 ) * 0.5 ;
        T earlyReflectionsR = earlyReflectionsDelayLine[1] ( bandwidthLeft * 0.3 + bandwidthRight * 0.5 )
        + earlyReflectionsDelayLine[1].GetIndex(2) * 0.6
        + earlyReflectionsDelayLine[1].GetIndex(3) * 0.4
        + earlyReflectionsDelayLine[1].GetIndex(4) * 0.3
        + earlyReflectionsDelayLine[1].GetIndex(5) * 0.3
        + earlyReflectionsDelayLine[1].GetIndex(6) * 0.1
        + earlyReflectionsDelayLine[1].GetIndex(7) * 0.1
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
        T accumulatorL = (0.6*staticDelayLine[2].GetIndex(1))
        +(0.6*staticDelayLine[2].GetIndex(2))
        +(0.6*staticDelayLine2[2].GetIndex(2))
        -(0.6*allpassFourTap[3].GetIndex(1))
        +(0.6*staticDelayLine[3].GetIndex(1))
        +(0.6*staticDelayLine2[3].GetIndex(1))
        -(0.6*staticDelayLine[0].GetIndex(1))
        -(0.6*staticDelayLine2[0].GetIndex(1))
        -(0.6*allpassFourTap[1].GetIndex(1))
        -(0.6*staticDelayLine[1].GetIndex(1))
        -(0.6*staticDelayLine2[1].GetIndex(1));
        T accumulatorR = (0.6*staticDelayLine[0].GetIndex(2))
        +(0.6*staticDelayLine[0].GetIndex(3))
        +(0.6*staticDelayLine2[0].GetIndex(3))
        -(0.6*allpassFourTap[1].GetIndex(2))
        +(0.6*staticDelayLine[1].GetIndex(2))
        +(0.6*staticDelayLine2[1].GetIndex(2))
        -(0.6*staticDelayLine[2].GetIndex(3))
        -(0.6*staticDelayLine2[2].GetIndex(3))
        -(0.6*allpassFourTap[3].GetIndex(2))
        -(0.6*staticDelayLine[3].GetIndex(2))
        -(0.6*staticDelayLine2[3].GetIndex(2));
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
        + earlyReflectionsDelayLine[0].GetIndex(2) * 0.6
        + earlyReflectionsDelayLine[0].GetIndex(3) * 0.4
        + earlyReflectionsDelayLine[0].GetIndex(4) * 0.3
        + earlyReflectionsDelayLine[0].GetIndex(5) * 0.3
        + earlyReflectionsDelayLine[0].GetIndex(6) * 0.1
        + earlyReflectionsDelayLine[0].GetIndex(7) * 0.1
        + ( bandwidthLeft * 0.4 + bandwidthRight * 0.2 ) * 0.5 ;
        T earlyReflectionsR = earlyReflectionsDelayLine[1] ( bandwidthLeft * 0.3 + bandwidthRight * 0.5 )
        + earlyReflectionsDelayLine[1].GetIndex(2) * 0.6
        + earlyReflectionsDelayLine[1].GetIndex(3) * 0.4
        + earlyReflectionsDelayLine[1].GetIndex(4) * 0.3
        + earlyReflectionsDelayLine[1].GetIndex(5) * 0.3
        + earlyReflectionsDelayLine[1].GetIndex(6) * 0.1
        + earlyReflectionsDelayLine[1].GetIndex(7) * 0.1
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
        T accumulatorL = (0.6*staticDelayLine[2].GetIndex(1))
        +(0.6*staticDelayLine[2].GetIndex(2))
        +(0.6*staticDelayLine2[2].GetIndex(2))
        -(0.6*allpassFourTap[3].GetIndex(1))
        +(0.6*staticDelayLine[3].GetIndex(1))
        +(0.6*staticDelayLine2[3].GetIndex(1))
        -(0.6*staticDelayLine[0].GetIndex(1))
        -(0.6*staticDelayLine2[0].GetIndex(1))
        -(0.6*allpassFourTap[1].GetIndex(1))
        -(0.6*staticDelayLine[1].GetIndex(1))
        -(0.6*staticDelayLine2[1].GetIndex(1));
        T accumulatorR = (0.6*staticDelayLine[0].GetIndex(2))
        +(0.6*staticDelayLine[0].GetIndex(3))
        +(0.6*staticDelayLine2[0].GetIndex(3))
        -(0.6*allpassFourTap[1].GetIndex(2))
        +(0.6*staticDelayLine[1].GetIndex(2))
        +(0.6*staticDelayLine2[1].GetIndex(2))
        -(0.6*staticDelayLine[2].GetIndex(3))
        -(0.6*staticDelayLine2[2].GetIndex(3))
        -(0.6*allpassFourTap[3].GetIndex(2))
        -(0.6*staticDelayLine[3].GetIndex(2))
        -(0.6*staticDelayLine2[3].GetIndex(2));
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
        predelayline.Clear();
        predelayline.SetLength(m_predelaytime);
        allpass[0].Clear();
        allpass[1].Clear();
        allpass[2].Clear();
        allpass[3].Clear();
        allpass[0].SetLength (0.0048 * m_samplerate);
        allpass[1].SetLength (0.0036 * m_samplerate);
        allpass[2].SetLength (0.0127 * m_samplerate);
        allpass[3].SetLength (0.0093 * m_samplerate);
        allpass[0].SetFeedback (0.75);
        allpass[1].SetFeedback (0.75);
        allpass[2].SetFeedback (0.625);
        allpass[3].SetFeedback (0.625);
        allpassFourTap[0].Clear();
        allpassFourTap[1].Clear();
        allpassFourTap[2].Clear();
        allpassFourTap[3].Clear();
        allpassFourTap[0].SetLength(0.020 * m_samplerate * m_roomsize);
        allpassFourTap[1].SetLength(0.060 * m_samplerate * m_roomsize);
        allpassFourTap[2].SetLength(0.030 * m_samplerate * m_roomsize);
        allpassFourTap[3].SetLength(0.089 * m_samplerate * m_roomsize);
        allpassFourTap[0].SetFeedback(m_density1);
        allpassFourTap[1].SetFeedback(m_density2);
        allpassFourTap[2].SetFeedback(m_density1);
        allpassFourTap[3].SetFeedback(m_density2);
        allpassFourTap[0].SetIndex(0,0,0,0);
        allpassFourTap[1].SetIndex(0,0.006 * m_samplerate * m_roomsize, 0.041 * m_samplerate * m_roomsize, 0);
        allpassFourTap[2].SetIndex(0,0,0,0);
        allpassFourTap[3].SetIndex(0,0.031 * m_samplerate * m_roomsize, 0.011 * m_samplerate * m_roomsize, 0);
        staticDelayLine[0].Clear();
        staticDelayLine[1].Clear();
        staticDelayLine[2].Clear();
        staticDelayLine[3].Clear();
        staticDelayLine[0].SetLength(0.15 * m_samplerate * m_roomsize);
        staticDelayLine[1].SetLength(0.12 * m_samplerate * m_roomsize);
        staticDelayLine[2].SetLength(0.14 * m_samplerate * m_roomsize);
        staticDelayLine[3].SetLength(0.11 * m_samplerate * m_roomsize);
        staticDelayLine[0].SetIndex(0, 0.067 * m_samplerate * m_roomsize, 0.011 * m_samplerate * m_roomsize , 0.121 * m_samplerate * m_roomsize);
        staticDelayLine[1].SetIndex(0, 0.036 * m_samplerate * m_roomsize, 0.089 * m_samplerate * m_roomsize , 0);
        staticDelayLine[2].SetIndex(0, 0.0089 * m_samplerate * m_roomsize, 0.099 * m_samplerate * m_roomsize , 0);
        staticDelayLine[3].SetIndex(0, 0.067 * m_samplerate * m_roomsize, 0.0041 * m_samplerate * m_roomsize , 0);
        staticDelayLine2[0].Clear();
        staticDelayLine2[1].Clear();
        staticDelayLine2[2].Clear();
        staticDelayLine2[3].Clear();
        staticDelayLine2[0].SetLength(0.17 * m_samplerate * m_roomsize);
        staticDelayLine2[1].SetLength(0.19 * m_samplerate * m_roomsize);
        staticDelayLine2[2].SetLength(0.23 * m_samplerate * m_roomsize);
        staticDelayLine2[3].SetLength(0.29 * m_samplerate * m_roomsize);
        staticDelayLine2[0].SetIndex(0.017 * m_samplerate * m_roomsize, 0.037 * m_samplerate * m_roomsize, 0.019 * m_samplerate * m_roomsize , 0.141 * m_samplerate * m_roomsize);
        staticDelayLine2[1].SetIndex(0.019 * m_samplerate * m_roomsize, 0.041 * m_samplerate * m_roomsize, 0.189 * m_samplerate * m_roomsize , 0.289 * m_samplerate * m_roomsize);
        staticDelayLine2[2].SetIndex(0.023 * m_samplerate * m_roomsize, 0.289 * m_samplerate * m_roomsize, 0.079 * m_samplerate * m_roomsize , 0.039 * m_samplerate * m_roomsize);
        staticDelayLine2[3].SetIndex(0.029 * m_samplerate * m_roomsize, 0.037 * m_samplerate * m_roomsize, 0.041 * m_samplerate * m_roomsize ,  0.043 * m_samplerate * m_roomsize);
        earlyReflectionsDelayLine[0].Clear();
        earlyReflectionsDelayLine[1].Clear();
        earlyReflectionsDelayLine[0].SetLength(0.089 * m_samplerate);
        earlyReflectionsDelayLine[0].SetIndex (0, 0.0199*m_samplerate, 0.0219*m_samplerate, 0.0354*m_samplerate,0.0389*m_samplerate, 0.0414*m_samplerate, 0.0692*m_samplerate, 0);
        earlyReflectionsDelayLine[1].SetLength(0.069 * m_samplerate);
        earlyReflectionsDelayLine[1].SetIndex (0, 0.0099*m_samplerate, 0.011*m_samplerate, 0.0182*m_samplerate,0.0189*m_samplerate, 0.0213*m_samplerate, 0.0431*m_samplerate, 0);
    }

    void setSampleRate(T sr){
        m_samplerate = sr;

        reset();
    }
};



template<typename T, int maxLength>
class mv_allpass
{
private:
    T buffer[maxLength];
    int index;
    int Length;
    T Feedback;

public:
    mv_allpass()
    {
        SetLength ( maxLength - 1 );
        Clear();
        Feedback = 0.5;
    }

    T operator()(T input)
    {
        T output;
        T bufout;
        bufout = buffer[index];
        T temp = input * -Feedback;
        output = bufout + temp;
        buffer[index] = input + ((bufout+temp)*Feedback);
        if(++index>=Length) index = 0;
        return output;
    }

    void SetLength (int Length)
    {
       if( Length >= maxLength )
            Length = maxLength;
       if( Length < 0 )
            Length = 0;

        this->Length = Length;
    }

    void SetFeedback(T feedback)
    {
        Feedback = feedback;
    }

    void Clear()
    {
//        memset(buffer, 0, sizeof(buffer));
//        index = 0;
        
//        if(buffer)
        {
            T *buf = buffer;
            long    count = maxLength;
            T    localvalue = 0.0;
            
            --buf;
            while(--count >= 0)
            {
                *++buf = localvalue;
            }
        }
        index = 0;

    }

    int GetLength() const
    {
        return Length;
    }
};

template<typename T, int maxLength>
class mv_staticallpass4tap
{
private:
    T buffer[maxLength];
    int index1, index2, index3, index4;
    int Length;
    T Feedback;

public:
    mv_staticallpass4tap()
    {
        SetLength ( maxLength - 1 );
        Clear();
        Feedback = 0.5;
    }

    T operator()(T input)
    {
        T output;
        T bufout;

        bufout = buffer[index1];
        T temp = input * -Feedback;
        output = bufout + temp;
        buffer[index1] = input + ((bufout+temp)*Feedback);

        if(++index1>=Length)
            index1 = 0;
        if(++index2 >= Length)
            index2 = 0;
        if(++index3 >= Length)
            index3 = 0;
        if(++index4 >= Length)
            index4 = 0;

        return output;

    }

    void SetIndex (int Index1, int Index2, int Index3, int Index4)
    {
        index1 = Index1;
        index2 = Index2;
        index3 = Index3;
        index4 = Index4;
    }

    T GetIndex (int Index)
    {
        switch (Index)
        {
            case 0:
                return buffer[index1];
                break;
            case 1:
                return buffer[index2];
                break;
            case 2:
                return buffer[index3];
                break;
            case 3:
                return buffer[index4];
                break;
            default:
                return buffer[index1];
                break;
        }
    }

    void SetLength (int Length)
    {
       if( Length >= maxLength )
            Length = maxLength;
       if( Length < 0 )
            Length = 0;

        this->Length = Length;
    }


    void Clear()
    {
//        memset(buffer, 0, sizeof(buffer));
//        if(buffer)
        {
            T *buf = buffer;
            long    count = maxLength;
            T    localvalue = 0.0;
            
            --buf;
            while(--count >= 0)
            {
                *++buf = localvalue;
            }
        }

        index1 = index2  = index3 = index4 = 0;
    }

    void SetFeedback(T feedback)
    {
        Feedback = feedback;
    }


    int GetLength() const
    {
        return Length;
    }
};

template<typename T, int maxLength>
class mv_staticdelayline
{
private:
    T buffer[maxLength];
    int index;
    int Length;
    T Feedback;

public:
    mv_staticdelayline()
    {
        SetLength ( maxLength - 1 );
        Clear();
    }

    T operator()(T input)
    {
        T output = buffer[index];
        buffer[index++] = input;
        if(index >= Length)
            index = 0;
        return output;

    }

    void SetLength (int Length)
    {
       if( Length >= maxLength )
            Length = maxLength;
       if( Length < 0 )
            Length = 0;

        this->Length = Length;
    }

    void Clear()
    {
//        memset(buffer, 0, sizeof(buffer));
//        if(buffer)
        {
            T *buf = buffer;
            long    count = maxLength;
            T    localvalue = 0.0;
            
            --buf;
            while(--count >= 0)
            {
                *++buf = localvalue;
            }
        }

        index = 0;
    }

    int GetLength() const
    {
        return Length;
    }
};


template<typename T, int maxLength>
class mv_staticdelayline4tap
{
private:
    T buffer[maxLength];
    int index1, index2, index3, index4;
    int Length;
    T Feedback;

public:
    mv_staticdelayline4tap()
    {
        SetLength ( maxLength - 1 );
        Clear();
    }

    //get ouput and iterate
    T operator()(T input)
    {
        T output = buffer[index1];
        buffer[index1++] = input;
        if(index1 >= Length)
            index1 = 0;
        if(++index2 >= Length)
            index2 = 0;
        if(++index3 >= Length)
            index3 = 0;
        if(++index4 >= Length)
            index4 = 0;
        return output;

    }

    void SetIndex (int Index1, int Index2, int Index3, int Index4)
    {
        index1 = Index1;
        index2 = Index2;
        index3 = Index3;
        index4 = Index4;
    }


    T GetIndex (int Index)
    {
        switch (Index)
        {
            case 0:
                return buffer[index1];
                break;
            case 1:
                return buffer[index2];
                break;
            case 2:
                return buffer[index3];
                break;
            case 3:
                return buffer[index4];
                break;
            default:
                return buffer[index1];
                break;
        }
    }


    void SetLength (int Length)
    {
       if( Length >= maxLength )
            Length = maxLength;
       if( Length < 0 )
            Length = 0;

        this->Length = Length;
    }


    void Clear()
    {
//        memset(buffer, 0, sizeof(buffer));
//        if(buffer)
        {
            T *buf = buffer;
            long    count = maxLength;
            T    localvalue = 0.0;
            
            --buf;
            while(--count >= 0)
            {
                *++buf = localvalue;
            }
        }

        index1 = index2  = index3 = index4 = 0;
    }


    int GetLength() const
    {
        return Length;
    }
};


template<typename T, int maxLength>
class mv_staticdelayline8tap
{
private:
    T buffer[maxLength];
    int index1, index2, index3, index4, index5, index6, index7, index8;
    int Length;
    T Feedback;

public:
    mv_staticdelayline8tap()
    {
        SetLength ( maxLength - 1 );
        Clear();
    }

    //get ouput and iterate
    T operator()(T input)
    {
        T output = buffer[index1];
        buffer[index1++] = input;
        if(index1 >= Length)
            index1 = 0;
        if(++index2 >= Length)
            index2 = 0;
        if(++index3 >= Length)
            index3 = 0;
        if(++index4 >= Length)
            index4 = 0;
        if(++index5 >= Length)
            index5 = 0;
        if(++index6 >= Length)
            index6 = 0;
        if(++index7 >= Length)
            index7 = 0;
        if(++index8 >= Length)
            index8 = 0;
        return output;

    }

    void SetIndex (int Index1, int Index2, int Index3, int Index4, int Index5, int Index6, int Index7, int Index8)
    {
        index1 = Index1;
        index2 = Index2;
        index3 = Index3;
        index4 = Index4;
        index5 = Index5;
        index6 = Index6;
        index7 = Index7;
        index8 = Index8;
    }


    T GetIndex (int Index)
    {
        switch (Index)
        {
            case 0:
                return buffer[index1];
                break;
            case 1:
                return buffer[index2];
                break;
            case 2:
                return buffer[index3];
                break;
            case 3:
                return buffer[index4];
                break;
            case 4:
                return buffer[index5];
                break;
            case 5:
                return buffer[index6];
                break;
            case 6:
                return buffer[index7];
                break;
            case 7:
                return buffer[index8];
                break;
            default:
                return buffer[index1];
                break;
        }
    }


    void SetLength (int Length)
    {
       if( Length >= maxLength )
            Length = maxLength;
       if( Length < 0 )
            Length = 0;

        this->Length = Length;
    }


    void Clear()
    {
//        memset(buffer, 0, sizeof(buffer));
//        if(buffer)
        {
            T *buf = buffer;
            long    count = maxLength;
            T    localvalue = 0.0;
            
            --buf;
            while(--count >= 0)
            {
                *++buf = localvalue;
            }
        }

        index1 = index2  = index3 = index4 = index5 = index6 = index7 = index8 = 0;
    }


    int GetLength() const
    {
        return Length;
    }
};

template<typename T, int OverSampleCount>
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
            sampleRate = 44100. * OverSampleCount;
            frequency = 1000.;
            UpdateCoefficient();
            Resonance(0);
            Type(LOWPASS);
            Reset();
        }

        T operator()(T input)
        {
            for(unsigned int i = 0; i < OverSampleCount; i++)
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



struct TwistedVerbModule : Module {
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
        IN_MIX_MODULATOR,
        IN_DENSITY_MODULATOR,
        IN_BANDWIDTH_MODULATOR,
        IN_DECAY_MODULATOR,
        IN_DAMP_MODULATOR,
        IN_GAIN_MODULATOR,
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


    mv_reverb_twisted<float>   mverb;
    
    float mix = 0.5;
    float predelay = 0.5;
    float earlymix = 0.5;
    float roomsize = 0.5;
    float density = 0.5;
    float filterfrequency = 0.5;
    float decay = 0.5;
    float dampingfrequency = 0.5;
    float gain = 0.5;

    bool freezed = false;

    int countertostart = 0;
    int k_countertostart_max;
    bool start_checked = false;
    
    dsp::SchmittTrigger buttonTrigger;
    
    float m_sample_rate;

	TwistedVerbModule() {
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
        
//        mverb.set_roomsize(roomsize);
        mverb.set_roomsizeCLEAN(roomsize);

        mverb.set_density(density);
        
        mverb.set_lowpassfrequency(filterfrequency);
        
        mverb.set_decay(decay);
        
        mverb.set_dampingfrequency(dampingfrequency);
        
        mverb.reset();

        
        k_countertostart_max = int(m_sample_rate);

	};

    void onReset() override{
        this->initializelocal();
    };
    
    json_t *dataToJson() override{
        json_t *rootJ = json_object();
        
//        std::ostringstream ss;
//        
//        ss.str("");
//        ss.clear();
//        ss << "m_direction";
//        json_object_set_new(rootJ, ss.str().c_str(), json_boolean(m_direction == e_up));
//        
//        for(int ix = 0; ix < k_simpler_buddy_stage_count; ix++)
//        {
//            
//            ss.str("");
//            ss.clear();
//            ss << "m_x10" << ix;
//            json_object_set_new(rootJ, ss.str().c_str(), json_boolean(m_x10[ix]));
//            
//            ss.str("");
//            ss.clear();
//            ss << "m_step_active" << ix;
//            json_object_set_new(rootJ, ss.str().c_str(), json_boolean(m_step_active[ix]));
//        }
        
        return rootJ;
    }
    
    void dataFromJson(json_t *rootJ) override{
//        json_t *idJ = NULL;
        
        //       mverb.set_roomsizeCLEAN(roomsize);
        
       
//        std::ostringstream ss;
//        
//        ss.str("");
//        ss.clear();
//        ss << "m_direction";
//        
//        idJ = json_object_get(rootJ, ss.str().c_str());
//        if(idJ && json_is_boolean(idJ))
//        {
//            m_direction = json_is_true(idJ) ? e_up : e_down;
//        }
//        
//        // get id
//        for(int ix = 0; ix < k_simpler_buddy_stage_count; ix++)
//        {
//            ss.str("");
//            ss.clear();
//            ss << "m_x10" << ix;
//            
//            idJ = json_object_get(rootJ, ss.str().c_str());
//            if(idJ && json_is_boolean(idJ))
//            {
//                m_x10[ix] = json_is_true(idJ);
//            }
//            
//            ss.str("");
//            ss.clear();
//            ss << "m_step_active" << ix;
//            
//            idJ = json_object_get(rootJ, ss.str().c_str());
//            if(idJ && json_is_boolean(idJ))
//            {
//                m_step_active[ix] = json_is_true(idJ);
//            }
//        }
    }

};


void TwistedVerbModule::process(const ProcessArgs &args) {
    
    if(!start_checked)
    {
        countertostart++;
        
        if(countertostart < k_countertostart_max)
        {
            return;
        }
        else
        {
            start_checked = true;
        }
    }
    
	float outL, outR;

	outL = outR = 0.0;

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
            inR = inL = clamp(inputs[IN_INPUT_L].getVoltageSum() * .1, -1.f, 1.f);
        }
        else if(!inputs[IN_INPUT_L].isConnected() && inputs[IN_INPUT_R].isConnected())
        {
            inR = inL = clamp(inputs[IN_INPUT_R].getVoltageSum() * .1, -1.f, 1.f);
        }
        else if(inputs[IN_INPUT_L].isConnected() && inputs[IN_INPUT_R].isConnected())
        {
            inL = clamp(inputs[IN_INPUT_L].getVoltageSum() * .1, -1.f, 1.f);
            inR = clamp(inputs[IN_INPUT_R].getVoltageSum() * .1, -1.f, 1.f);
        }
        
        if(inputs[IN_MIX_MODULATOR].isConnected())
        {
            mix = clamp(inputs[IN_MIX_MODULATOR].getVoltage() * .1, 0.f, 1.f);
        }
        
        if(inputs[IN_DENSITY_MODULATOR].isConnected())
        {
            density = clamp(inputs[IN_DENSITY_MODULATOR].getVoltage() * .1, 0.f, 1.f);
        }
        
        if(inputs[IN_BANDWIDTH_MODULATOR].isConnected())
        {
            filterfrequency = clamp(inputs[IN_BANDWIDTH_MODULATOR].getVoltage() * .1, 0.f, 1.f);
        }
        
        if(inputs[IN_DECAY_MODULATOR].isConnected())
        {
            decay = clamp(inputs[IN_DECAY_MODULATOR].getVoltage() * .1, 0.f, 1.f);
        }
        
        if(inputs[IN_DAMP_MODULATOR].isConnected())
        {
            dampingfrequency = clamp(inputs[IN_DAMP_MODULATOR].getVoltage() * .1, 0.f, 1.f);
        }
        
        if(inputs[IN_GAIN_MODULATOR].isConnected())
        {
            gain = clamp(inputs[IN_GAIN_MODULATOR].getVoltage() * .1, 0.f, 1.f);
        }
        
        
        if(old_mix != mix)
        {
            mverb.set_mix(mix);
        }
        
        if(old_predelay != predelay)
        {
            mverb.set_predelay(predelay);
        }
        
        if(old_earlymix != earlymix)
        {
            mverb.set_earlymix(earlymix);
        }

        if(old_roomsize != roomsize)
        {
            mverb.set_roomsize(roomsize);
        }
        
        if(old_density != density)
        {
            mverb.set_density(density);
        }
        
        if(old_filterfrequency != filterfrequency)
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
            localdampingfrequency = MAXFEEDBACK;
        
        mverb.set_dampingfrequency(localdampingfrequency);
        
        float localdecay = decay;
        if(freezed)
            localdecay = MAXFEEDBACK;
        
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
    
    lights[LI_FREEZE].value = freezed ? 1.0 : 0.0;

    if(!freezed)
    {
        mverb.process_one_twisted(inL, inR, &outL, &outR);
    }
    else
    {
        mverb.process_one_twisted(0.0, 0.0, &outL, &outR);
    }
	outputs[OUTL_OUTPUT].setVoltage(outL * 10.);
	outputs[OUTR_OUTPUT].setVoltage(outR * 10.);
};



struct TwistedVerbWidget : ModuleWidget
{
    TwistedVerbWidget(Module *module)
    {
        setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/TwistedMVerb.svg")));

        box.size = Vec(15 * 7, 380);

        addInput(createInput<PJ301MPort>(Vec(12 - 1, 29 - 1), module, TwistedVerbModule::IN_INPUT_L));
        addInput(createInput<PJ301MPort>(Vec(71 - 1, 29 - 1), module, TwistedVerbModule::IN_INPUT_R));

        float      base = 75;
        float      left1 = 49;
        float      left2 = 71;
        float      differential = 23;
        
        addParam(createParam<Davies1900hWhiteKnob20>(Vec(left1, base), module, TwistedVerbModule::MIX_PARAM));
        addInput(createInput<PJ301MPort>(Vec(left2 - 1, base - 2 - 1), module, TwistedVerbModule::IN_MIX_MODULATOR));

        addParam(createParam<Davies1900hWhiteKnob20>(Vec(left1, base + differential * 1), module, TwistedVerbModule::PREDELAY_PARAM));
        addParam(createParam<Davies1900hWhiteKnob20>(Vec(left1, base + differential * 2), module, TwistedVerbModule::EARLYMIX_PARAM));
        addParam(createParam<Davies1900hWhiteKnob20>(Vec(left1, base + differential * 3), module, TwistedVerbModule::ROOMSIZE_PARAM));

        addParam(createParam<Davies1900hWhiteKnob20>(Vec(left1, base + differential * 4), module, TwistedVerbModule::DENSITY_PARAM));
        addInput(createInput<PJ301MPort>(Vec(left2 - 1, base + differential * 4 - 2 - 1), module, TwistedVerbModule::IN_DENSITY_MODULATOR));

        addParam(createParam<Davies1900hWhiteKnob20>(Vec(left1, base + differential * 5), module, TwistedVerbModule::FILTERFREQUENCY_PARAM));
        addInput(createInput<PJ301MPort>(Vec(left2 - 1, base + differential * 5 - 2 - 1), module, TwistedVerbModule::IN_BANDWIDTH_MODULATOR));
        
        addParam(createParam<Davies1900hWhiteKnob20>(Vec(left1, base + differential * 6), module, TwistedVerbModule::DECAY_PARAM));
        addInput(createInput<PJ301MPort>(Vec(left2 - 1, base + differential * 6 - 2 - 1), module, TwistedVerbModule::IN_DECAY_MODULATOR));
        
        addParam(createParam<Davies1900hWhiteKnob20>(Vec(left1, base + differential * 7), module, TwistedVerbModule::DAMP_PARAM));
        addInput(createInput<PJ301MPort>(Vec(left2 - 1, base + differential * 7 - 2 - 1), module, TwistedVerbModule::IN_DAMP_MODULATOR));
        
        addParam(createParam<Davies1900hWhiteKnob20>(Vec(left1, base + differential * 8), module, TwistedVerbModule::GAIN_PARAM));
        addInput(createInput<PJ301MPort>(Vec(left2 - 1, base + differential * 8 - 2 - 1), module, TwistedVerbModule::IN_GAIN_MODULATOR));
        
        addInput(createInput<PJ301MPort>(Vec(left2 - 1, base + differential * 9 - 2 - 1), module, TwistedVerbModule::FREEZE_INPUT));
        addParam(createParam<nyTapper12>(Vec(left1 + 3, base + differential * 9 + 3), module, TwistedVerbModule::FREEZE_BUTTON));
        addChild(createLight<SmallLight<GreenLight>>(Vec(left1 + 3 + 3, base + differential * 9 + 3 + 3), module, TwistedVerbModule::LI_FREEZE));

        addOutput(createOutput<PJ301MPort>(Vec(12 - 1, 313 - 1), module, TwistedVerbModule::OUTL_OUTPUT));
        addOutput(createOutput<PJ301MPort>(Vec(71 - 1, 313 - 1), module, TwistedVerbModule::OUTR_OUTPUT));
    }
};

Model *modelTwistedVerb = createModel<TwistedVerbModule, TwistedVerbWidget>("TwistedMVerb");


#define HIVERBEXTRAGAIN  1.5

struct HiVerbModule : Module {
    enum ParamIds {
        MIX_PARAM,
        PREDELAY_PARAM,
        EARLYMIX_PARAM,
        ROOMSIZE_PARAM,
        DENSITY_PARAM,
        HIGHPASSFREQUENCY_PARAM,
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
    


    mv_reverb_twisted<float>   mverb;
    
    float mix = 0.5;
    float predelay = 0.5;
    float earlymix = 0.5;
    float roomsize = 0.5;
    float density = 0.5;
    float highpassfrequency = 0.5;
    float decay = 0.5;
    float dampingfrequency = 0.5;
    float gain = 0.5;

    bool freezed = false;

    dsp::SchmittTrigger buttonTrigger;
    float m_sample_rate;

    HiVerbModule(){

        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        
        configParam(MIX_PARAM, 0.0, 1.0, 0.5, "Mix Dry-Wet 0..1");
        configParam(PREDELAY_PARAM, 0.0, 1.0, 0.5, "PreDelay 0..1");
        configParam(EARLYMIX_PARAM, 0.0, 1.0, 0.5, "Early Mix 0..1");
        configParam(ROOMSIZE_PARAM, 0.0, 1.0, 0.6, "Roomsize 0.1");
        configParam(DENSITY_PARAM, 0.0, 1.0, 0.5, "Density 0..1");
        configParam(HIGHPASSFREQUENCY_PARAM, 0.0, 1.0, 0.68, "Filter 0..1");
        configParam(DECAY_PARAM, 0.0, 1.0, 0.6, "Decay 0..1");
        configParam(DAMP_PARAM, 0.0, 1.0, 0.8, "Damp 0..1");
        configParam(GAIN_PARAM, 0.0, 1.0, 0.5, "Gain 0..1");
        configParam(FREEZE_BUTTON, 0.0, 1.0, 0.0, "Freeze");

        configBypass(IN_INPUT_L, OUTL_OUTPUT);
        configBypass(IN_INPUT_R, OUTR_OUTPUT);

        this->onReset();
    };

    void process(const ProcessArgs &args) override;

    void onSampleRateChange() override{
        m_sample_rate = APP->engine->getSampleRate();
        mverb.setSampleRate(m_sample_rate);
    }

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
        
        mverb.set_highpassfrequency(highpassfrequency);
        
        mverb.set_decay(decay);
        
        mverb.set_dampingfrequency(dampingfrequency);
    };

    void onReset() override{
        initializelocal();
    }

};

void HiVerbModule::process(const ProcessArgs &args)
{
    float outL, outR;

    outL = outR = 0.0;

    float old_mix = mix;
    float old_predelay = predelay;
    float old_earlymix = earlymix;
    float old_roomsize = roomsize;
    float old_density = density;
    float old_highpassfrequency = highpassfrequency;
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
        highpassfrequency = params[HIGHPASSFREQUENCY_PARAM].getValue();
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
        
        if( old_highpassfrequency != highpassfrequency)
        {
            mverb.set_highpassfrequency(highpassfrequency);
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
            mverb.set_gain(gain * 2 * HIVERBEXTRAGAIN);
        }
    }
    
    if(buttonTrigger.process(params[FREEZE_BUTTON].getValue() + ((inputs[FREEZE_INPUT].getVoltage() > 1.0) ? 1.0 : 0.0)))
    {
        freezed = !freezed;
        
        float localdampingfrequency = dampingfrequency;
        if(freezed)
            localdampingfrequency = MAXFEEDBACK;
        
        mverb.set_dampingfrequency(localdampingfrequency);
        
        float localdecay = decay;
        if(freezed)
            localdecay = MAXFEEDBACK;
        
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

    lights[LI_FREEZE].value = freezed ? 1.0 : 0.0;

    if(!freezed)
    {
        mverb.process_one_shimmer(inL, inR, &outL, &outR);
    }
    else
    {
        mverb.process_one_shimmer(0.0, 0.0, &outL, &outR);
    }
    outputs[OUTL_OUTPUT].setVoltage(outL);
    outputs[OUTR_OUTPUT].setVoltage(outR);
};



struct HiVerbWidget : ModuleWidget
{
    HiVerbWidget(Module *module)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/HiVerb.svg")));

        box.size = Vec(15 * 6, 380);
        
        addInput(createInput<PJ301MPort>(Vec(12 - 1, 29 - 1), module, HiVerbModule::IN_INPUT_L));
        addInput(createInput<PJ301MPort>(Vec(56 - 1, 29 - 1), module, HiVerbModule::IN_INPUT_R));
        
        float      base = 75;
        float      left1 = 56 + 1;
        float      differential = 23;
        
        addParam(createParam<Davies1900hWhiteKnob20>(Vec(left1, base), module, HiVerbModule::MIX_PARAM));
        addParam(createParam<Davies1900hWhiteKnob20>(Vec(left1, base + differential * 1), module, HiVerbModule::PREDELAY_PARAM));
        addParam(createParam<Davies1900hWhiteKnob20>(Vec(left1, base + differential * 2), module, HiVerbModule::EARLYMIX_PARAM));
        addParam(createParam<Davies1900hWhiteKnob20>(Vec(left1, base + differential * 3), module, HiVerbModule::ROOMSIZE_PARAM));
        addParam(createParam<Davies1900hWhiteKnob20>(Vec(left1, base + differential * 4), module, HiVerbModule::DENSITY_PARAM));
        addParam(createParam<Davies1900hWhiteKnob20>(Vec(left1, base + differential * 5), module, HiVerbModule::HIGHPASSFREQUENCY_PARAM));
        addParam(createParam<Davies1900hWhiteKnob20>(Vec(left1, base + differential * 6), module, HiVerbModule::DECAY_PARAM));
        addParam(createParam<Davies1900hWhiteKnob20>(Vec(left1, base + differential * 7), module, HiVerbModule::DAMP_PARAM));
        addParam(createParam<Davies1900hWhiteKnob20>(Vec(left1, base + differential * 8), module, HiVerbModule::GAIN_PARAM));
        
        addInput(createInput<PJ301MPort>(Vec(left1 - 1 - 1, base + differential * 9 - 1 - 1), module, HiVerbModule::FREEZE_INPUT));
        
        addParam(createParam<nyTapper12>(Vec(41, base + differential * 9 + 3), module, HiVerbModule::FREEZE_BUTTON));
        addChild(createLight<SmallLight<GreenLight>>(Vec(41 + 3, base + differential * 9 + 6), module, HiVerbModule::LI_FREEZE));

        addOutput(createOutput<PJ301MPort>(Vec(12 - 1, 314 - 1), module, HiVerbModule::OUTL_OUTPUT));
        addOutput(createOutput<PJ301MPort>(Vec(56 - 1, 314 - 1), module, HiVerbModule::OUTR_OUTPUT));
    }
};


Model *modelHiVerb = createModel<HiVerbModule, HiVerbWidget>("HiVerb");
