#pragma once
 
#include "isr.h"
 
extern "C" void ResetHandler();
 
template<>
struct IRQHandler<VectorTableEntry::Reset>
{
    using constant_t =  std::integral_constant<void(*)(), ResetHandler>;
};

// concrete implementation of vector table for this chip
//eventually split off into separate files or whatever per chip
__attribute__((section (".VectorTable"),used))
 constexpr auto static Table = VectorTable
    <
    VectorTableEntry::Reset,
    VectorTableEntry::NMI,
    VectorTableEntry::Hardfault,
    VectorTableEntry::Empty,
    VectorTableEntry::Empty,
    VectorTableEntry::Empty,
    VectorTableEntry::Empty,
    VectorTableEntry::SVC,
    VectorTableEntry::Empty,
    VectorTableEntry::PendSV,
    VectorTableEntry::Systick,
    VectorTableEntry::Watchdog,
    VectorTableEntry::PVD,
    VectorTableEntry::TAMPER,
    VectorTableEntry::RTC,
    VectorTableEntry::Flash,
    VectorTableEntry::RCC,
    VectorTableEntry::EXTI0,
    VectorTableEntry::EXTI1,
    VectorTableEntry::EXTI2,
    VectorTableEntry::EXTI3,
    VectorTableEntry::EXTI4,
    VectorTableEntry::DMA1_Channel1,
    VectorTableEntry::DMA1_Channel2,
    VectorTableEntry::DMA1_Channel3,
    VectorTableEntry::DMA1_Channel4,
    VectorTableEntry::DMA1_Channel5,
    VectorTableEntry::DMA1_Channel6,
    VectorTableEntry::DMA1_Channel7,
    VectorTableEntry::ADC1_2,
    VectorTableEntry::CAN1_TX,
    VectorTableEntry::CAN1_RX0,
    VectorTableEntry::CAN1_RX1,
    VectorTableEntry::CAN1_SCE,
    VectorTableEntry::EXTI9_5,
    VectorTableEntry::TIM1_BRK,
    VectorTableEntry::TIM1_UP,
    VectorTableEntry::TIM1_TRG_COM,
    VectorTableEntry::TIM2,
    VectorTableEntry::TIM3,
    VectorTableEntry::TIM4,
    VectorTableEntry::I2C1_EV,
    VectorTableEntry::I2C1_ER,
    VectorTableEntry::I2C2_EV,
    VectorTableEntry::I2C2_ER,
    VectorTableEntry::SPI1,
    VectorTableEntry::SPI2,
    VectorTableEntry::USART1,
    VectorTableEntry::USART2,
    VectorTableEntry::USART3,
    VectorTableEntry::EXTI15_10,
    VectorTableEntry::RTCAlarm,
    VectorTableEntry::OTG_FS_WKUP,
    VectorTableEntry::Empty,
    VectorTableEntry::TIM5,
    VectorTableEntry::SPI3,
    VectorTableEntry::UART4,
    VectorTableEntry::UART5,
    VectorTableEntry::TIM6,
    VectorTableEntry::TIM7,
    VectorTableEntry::DMA2_Channel1,
    VectorTableEntry::DMA2_Channel2,
    VectorTableEntry::DMA2_Channel3,
    VectorTableEntry::DMA2_Channel4,
    VectorTableEntry::DMA2_Channel5,
    VectorTableEntry::ETH,
    VectorTableEntry::ETH_WKUP,
    VectorTableEntry::CAN2_TX,
    VectorTableEntry::CAN2_RX0,
    VectorTableEntry::CAN2_RX1,
    VectorTableEntry::CAN2_SCE,
    VectorTableEntry::OTG_FS
    >().data;