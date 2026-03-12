#pragma once

#include <stdint.h>
#include "mcore_regs.hpp"
#include "mcore_system.hpp"
#include "mcore_conf.hpp"


enum class AdcClkPresc : uint8_t{
    DIV2 = 0b00,
    DIV4 = 0b01,
    DIV6 = 0b10,
    DIV8 = 0b11
};

enum class AdcResolution : uint8_t{
    RES_12B = 0b00,
    RES_10B = 0b01,
    RES_8B  = 0b10,
    RES_6B  = 0b11
};

enum class AdcDataAlign : uint32_t{
    RIGHT = 0b0,
    LEFT  = 0b1
};

enum class AdcExtTrigConvSrc : uint8_t{
    T1_CH1             = 0b0000, 
    T1_CH2             = 0b0001,   
    T1_CH3             = 0b0010,   
    T2_CH2             = 0b0011, 
    T5_TRGO            = 0b0100,   
    T4_CH4             = 0b0101,    
    T3_CH4             = 0b0110,  
    T8_TRGO            = 0b0111,  
    T8_TRGO2           = 0b1000,  
    T1_TRGO            = 0b1001, 
    T1_TRGO2           = 0b1010, 
    T2_TRGO            = 0b1011, 
    T4_TRGO            = 0b1100,
    T6_TRGO            = 0b1101,
    EXT_IT11           = 0b1111, 
    ADC_SOFTWARE_START,         
};

enum class AdcExtTrigConvEdge :uint32_t{
    NONE           = 0b00,
    RISING         = 0b01,
    FALLING        = 0b10,
    RISING_FALLING = 0b11
};

enum class AdcDiscMum : uint8_t{
    _1CH = 0, 
    _2CH = 1, 
    _3CH = 2, 
    _4CH = 3, 
    _5CH = 4, 
    _6CH = 5, 
    _7CH = 6, 
    _8CH = 7
};

enum class AdcEOCSel : uint8_t{
    SEQ_CONV    = 0b0,
    SINGLE_CONV = 0b1,
};

enum class AdcWDGCMode : uint32_t{
    SINGLE_REG     = ADC1::_CR1::AWDSGL::BitMsk | ADC1::_CR1::AWDEN::BitMsk,
    SINGLE_INJEC   = ADC1::_CR1::AWDSGL::BitMsk | ADC1::_CR1::JAWDEN::BitMsk,
    SINGLE_REINJEC = ADC1::_CR1::AWDSGL::BitMsk | ADC1::_CR1::AWDEN::BitMsk | ADC1::_CR1::JAWDEN::BitMsk,
    ALL_REG        = ADC1::_CR1::AWDEN::BitMsk,
    ALL_INJEC      = ADC1::_CR1::JAWDEN::BitMsk,
    ALL_REINJEC    = ADC1::_CR1::AWDEN::BitMsk  | ADC1::_CR1::JAWDEN::BitMsk,
    NONE = 0 
};

enum class AdcChnl : uint8_t{
    ADC_CHANNEL_0  = 0,
    ADC_CHANNEL_1  = 1,
    ADC_CHANNEL_2  = 2,
    ADC_CHANNEL_3  = 3,
    ADC_CHANNEL_4  = 4,
    ADC_CHANNEL_5  = 5,
    ADC_CHANNEL_6  = 6,
    ADC_CHANNEL_7  = 7,
    ADC_CHANNEL_8  = 8,
    ADC_CHANNEL_9  = 9,
    ADC_CHANNEL_10 = 10,
    ADC_CHANNEL_11 = 11,
    ADC_CHANNEL_12 = 12,
    ADC_CHANNEL_13 = 13,
    ADC_CHANNEL_14 = 14,
    ADC_CHANNEL_15 = 15,
    ADC_CHANNEL_16 = 16,
    ADC_CHANNEL_17 = 17,
    ADC_CHANNEL_18 = 18
};

enum class AdcChnlRank : uint8_t{
    NONE        = 0,
    ADC_RANK_1  = 1,
    ADC_RANK_2  = 2,
    ADC_RANK_3  = 3,
    ADC_RANK_4  = 4,
    ADC_RANK_5  = 5,
    ADC_RANK_6  = 6,
    ADC_RANK_7  = 7,
    ADC_RANK_8  = 8,
    ADC_RANK_9  = 9,
    ADC_RANK_10 = 10,
    ADC_RANK_11 = 11,
    ADC_RANK_12 = 12,
    ADC_RANK_13 = 13,
    ADC_RANK_14 = 14,
    ADC_RANK_15 = 15,
    ADC_RANK_16 = 16
};

enum class AdcSampleTime : uint32_t{
    _3CYCLES,
    _15CYCLES,
    _28CYCLES,
    _56CYCLES,
    _84CYCLES,
    _112CYCLES,
    _144CYCLES,
    _480CYCLES
};

struct AdcHwConfig {
    AdcExtTrigConvEdge ExternalTrigConvEdge = AdcExtTrigConvEdge::NONE;
    AdcExtTrigConvSrc ExternalTrigConv = AdcExtTrigConvSrc::ADC_SOFTWARE_START;
    AdcClkPresc ClockPrescaler = AdcClkPresc::DIV8;
    AdcResolution Resolution = AdcResolution::RES_12B;
    AdcDataAlign DataAlign = AdcDataAlign::RIGHT;
    AdcEOCSel EOCSelection = AdcEOCSel::SINGLE_CONV;
    uint8_t NbrOfConversion = 1;
    AdcDiscMum NbrOfDiscConversion = AdcDiscMum::_1CH;
    bool ScanConvModeEnable = false;
    bool DMAContinuousRequests = false;
    bool ContinuousConvModeEnable = false;
    bool DiscontinuousConvModeEnable = false;
};

struct AdcChannel {
    AdcSampleTime SamplingTime = AdcSampleTime::_480CYCLES;
    AdcChnlRank Rank = AdcChnlRank::NONE;
    bool TempSens_Vref_EN = false;
    bool VbatEN = false;
};

struct AdcWDGConfig {
    AdcWDGCMode WatchdogMode =AdcWDGCMode::NONE;
    uint16_t HighThreshold = 0;
    uint16_t LowThreshold  = 0;
    AdcChnl Channel = AdcChnl::ADC_CHANNEL_0;
    bool ITMode = false;
};

template<typename ADC> inline void ADC_ClkEnable();
template<> inline void ADC_ClkEnable<ADC1>() { RCC::_APB2ENR::ADC1EN ::set(); }
template<> inline void ADC_ClkEnable<ADC2>() { RCC::_APB2ENR::ADC2EN::set(); }
template<> inline void ADC_ClkEnable<ADC3>() { RCC::_APB2ENR::ADC3EN::set(); }



template<typename ADCx>
struct AdcTraits;

template<>
struct AdcTraits<ADC1>
{
    static constexpr bool hasTempSensor = true;
    static constexpr bool hasVbat = true;
};

template<>
struct AdcTraits<ADC2>
{
    static constexpr bool hasTempSensor = false;
    static constexpr bool hasVbat = false;
};

template<>
struct AdcTraits<ADC3>
{
    static constexpr bool hasTempSensor = false;
    static constexpr bool hasVbat = false;
};

template <typename ADCx, auto cfg>
void ADC_Config(){
    static_assert((cfg.NbrOfConversion >= 1 && cfg.NbrOfConversion <= 16),  
        "NbrOfConversion must be 1..16");
    
    ADC_ClkEnable<ADCx>();
    //1 clk presc
    ADC_Common::_CCR::ADCPRE::write(static_cast<uint32_t>(cfg.ClockPrescaler));
    //2 adc scan
    ADC1::_CR1::OVRIE::set();
    ADCx::_CR1::SCAN::write(cfg.ScanConvModeEnable ? 1u : 0u);
    //3 adc resolution
    ADCx::_CR1::RES::write(static_cast<uint32_t>(cfg.Resolution));
    //5 adc data aligh
    ADCx::_CR2::ALIGN::write(static_cast<uint32_t>(cfg.DataAlign));

    ADCx::_CR2::EXTSEL::write(0);
    ADCx::_CR2::EXTEN::write(0);
    if constexpr(cfg.ExternalTrigConv != AdcExtTrigConvSrc::ADC_SOFTWARE_START){
        //5 adc external trigger src
        ADCx::_CR2::EXTSEL::write(static_cast<uint32_t>(cfg.ExternalTrigConv));
        //5 adc external trigger polarity
        ADCx::_CR2::EXTEN::write(static_cast<uint32_t>(cfg.ExternalTrigConvEdge));
    }
    
    ADCx::_CR2::CONT::write(cfg.ContinuousConvModeEnable ? 1u : 0u);
    ADCx::_CR1::DISCEN::clear();
    if constexpr(cfg.DiscontinuousConvModeEnable != false){
        ADCx::_CR1::DISCEN::set();
        ADCx::_CR1::DISCNUM::write(static_cast<uint32_t>(cfg.NbrOfDiscConversion));
     }

    ADCx::_SQR1::L::write(static_cast<uint32_t>(cfg.NbrOfConversion-1));
    ADCx::_CR2::DDS::write(cfg.DMAContinuousRequests ? 1 : 0);
    ADCx::_CR2::EOCS::write(static_cast<uint32_t>(cfg.EOCSelection));
}


template <typename ADCx, auto cfg>
void ADC_AWDConfig(){
    
    ADCx::_CR1::AWDIE::write(cfg.ITMode ? 1 : 0);
   
    ADCx::_CR1::clearMask( ADCx::_CR1::AWDSGL::BitMsk
                         | ADCx::_CR1::JAWDEN::BitMsk
                         | ADCx::_CR1::AWDEN::BitMsk);

    ADCx::_CR1::setMask(static_cast<uint32_t>(cfg.WatchdogMode));
    
    static_assert(cfg.HighThreshold <= 4095 && cfg.HighThreshold >= 0);
    ADCx::_HTR::HT::write(static_cast<uint32_t>(cfg.HighThreshold));
    
    static_assert(cfg.LowThreshold <= 4095 && cfg.LowThreshold >= 0);
    ADCx::_LTR::LT::write(static_cast<uint32_t>(cfg.LowThreshold));

    ADCx::_CR1::AWDCH::write(static_cast<uint32_t>(cfg.Channel));
}

template <typename ADCx, auto cfg, AdcChnl ch>
void ADC_ChannelConfig()
{
    uint32_t shft,mask;

    using traits = AdcTraits<ADCx>;

    if constexpr(ch > AdcChnl::ADC_CHANNEL_9){//ch 10-18
        shft = (static_cast<uint32_t>(ch) - 10) * 3;
        mask = static_cast<uint32_t>(cfg.SamplingTime) << shft;
        ADCx::_SMPR1::clearMask(0b111u << shft);
        ADCx::_SMPR1::setMask(mask);
    }else{//ch 1-9
        shft = (static_cast<uint32_t>(ch)) * 3;
        mask = static_cast<uint32_t>(cfg.SamplingTime) << shft;
        ADCx::_SMPR2::clearMask(0b111u << shft);
        ADCx::_SMPR2::setMask(mask);    
    }

    if constexpr(cfg.Rank != AdcChnlRank::NONE){
        if constexpr(cfg.Rank < AdcChnlRank::ADC_RANK_7){
            shft = (static_cast<uint32_t>(cfg.Rank)-1) * 5;
            mask = static_cast<uint32_t>(ch) << shft;
            ADCx::_SQR3::clearMask(0b11111u << shft);
            ADCx::_SQR3::setMask(mask);
        }
        else if constexpr(cfg.Rank < AdcChnlRank::ADC_RANK_13){
            shft = (static_cast<uint32_t>(cfg.Rank) - 7) * 5;
            mask = static_cast<uint32_t>(ch) << shft;
            ADCx::_SQR2::clearMask(0b11111u << shft);
            ADCx::_SQR2::setMask(mask);
        }
        else{// 16
            shft = (static_cast<uint32_t>(cfg.Rank) - 13) * 5;
            mask = static_cast<uint32_t>(ch) << shft;
            ADCx::_SQR1::clearMask(0b11111u << shft);
            ADCx::_SQR1::setMask(mask);
        }
    }
   if constexpr(traits::hasTempSensor || traits::hasVbat){
        ADC_Common::_CCR::TSVREFE::clear();
        ADC_Common::_CCR::VBATE::clear();
        
        if(cfg.VbatEN) ADC_Common::_CCR::VBATE::set();
        if(cfg.TempSens_Vref_EN){ADC_Common::_CCR::TSVREFE::set();
            delay_us(10);//force_delay
        }
    }
}



template<typename ADCx>
static inline void ADC_Start_IT(){

	ADCx::_CR2::ADON::set();
	delay_us(3);
    ADCx::_SR::clearMask(
        ADCx::_SR::EOC::BitMsk
      | ADCx::_SR::OVR::BitMsk
    );
    
    ADCx::_CR1::setMask(
        ADCx::_CR1::EOCIE::BitMsk
    |   ADCx::_CR1::OVRIE::BitMsk
    );
	ADCx::_CR2::SWSTART::set();
}

template<typename ADCx>
static inline void ADC_Start(){

	ADCx::_CR2::ADON::set();
	delay_us(3);
    ADCx::_SR::clearMask(
        ADCx::_SR::EOC::BitMsk
      | ADCx::_SR::OVR::BitMsk
    );
	ADCx::_CR2::SWSTART::set();
}

template<typename ADCx>
static inline bool ADC_Ready(){return ADCx::_SR::EOC::read();}

template<typename ADCx>
static inline uint32_t ADC_Read(){return ADCx::_DR::DATA::read();}

template<typename ADCx>
inline void ADC_Stop(){ADCx::_CR2::SWSTART::clear();}

template<typename ADCx>
inline bool ADC_IsEnabled(){return ADCx::_CR2::ADON::read();}
