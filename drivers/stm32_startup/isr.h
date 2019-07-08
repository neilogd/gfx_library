// create a type that takes the irqn as a template parameter. 
// Then we can create specialisations for specific irqns
 
 
// create a type for the table that takes a list of template parameters and unpacks them
// it instantiates a template for each of the pack members which is a specialization of the irq handler template
//could ask the table type to take a range based on the values in the enum class
#pragma once
 
#include <functional>
#include <stdint.h>
extern uint32_t _estack;
 
__attribute__((section (".StackPointer")))
const auto StackPointer = &_estack;
 
//probably needs to be renamed and the values altered for clarity - doesn't necessarily have to be the index here
//because we're setting the indexes manually in the declaration of VectorTableTest
enum class VectorTableEntry
{
    Empty,
    Reset,
    NMI,
    Hardfault,
    SVC,
    PendSV,
    Systick,
    Watchdog,
    PVD,
    TAMPER,
    RTC,
    Flash,
    RCC,
    EXTI0,
    EXTI1,
    EXTI2,
    EXTI3,
    EXTI4,
    DMA1_Channel1,
    DMA1_Channel2,
    DMA1_Channel3,
    DMA1_Channel4,
    DMA1_Channel5,
    DMA1_Channel6,
    DMA1_Channel7,
    ADC1_2,
    CAN1_TX,
    CAN1_RX0,
    CAN1_RX1,
    CAN1_SCE,
    EXTI9_5,
    TIM1_BRK,
    TIM1_UP,
    TIM1_TRG_COM,
    TIM2,
    TIM3,
    TIM4,
    I2C1_EV,
    I2C1_ER,
    I2C2_EV,
    I2C2_ER,
    SPI1,
    SPI2,
    USART1,
    USART2,
    USART3,
    EXTI15_10,
    RTCAlarm,
    OTG_FS_WKUP,
    TIM5,
    SPI3,
    UART4,
    UART5,
    TIM6,
    TIM7,
    DMA2_Channel1,
    DMA2_Channel2,
    DMA2_Channel3,
    DMA2_Channel4,
    DMA2_Channel5,
    ETH,
    ETH_WKUP,
    CAN2_TX,
    CAN2_RX0,
    CAN2_RX1,
    CAN2_SCE,
    OTG_FS,

};
 
//possibly should change this to take a normal integer and do a cast?
//default specialization, should use an infinite loop as an empty handler rather than the nullptr default for now
  
template<VectorTableEntry Index>
struct IRQHandler
{
    static constexpr VectorTableEntry HANDLER = Index;
    static constexpr auto LambdaHandler = []()
    {
        VectorTableEntry handleIndex = Index;
        while(true)
        {
            // Unhandled interrupt.
            (*(int*)&handleIndex)++;
            (*(int*)&handleIndex)--;
        }
    };

    //static constexpr auto Value =nullptr;
    using constant_t = std::integral_constant<void(*)(), +LambdaHandler>;
};
 
template<>
struct IRQHandler<VectorTableEntry::Empty>
{
    using constant_t = std::integral_constant<void(*)(), nullptr>;
};
 
//all these specializations can be written in other source files
//perhaps these can actually inherit from std::integral constant.. hrm
 
// //specialization via a free function
// template<>
// struct IRQHandler<VectorTableEntry::ResetHandler_IRQn>
// {
//     using constant_t = std::integral_constant<void(*)(), ResetHandler_Impl>;
// };
 
// template<>
// struct IRQHandler<VectorTableEntry::SVC_IRQn>
// {
//     static constexpr auto Value = SVCHandler_Impl;
// };
 
//specialization via lambda
// template<>
// struct IRQHandler<VectorTableEntry::NMI_IRQn>
// {
//     //triggers implicit conversion to pointer
//     static constexpr auto Value = [](){};
//     using constant_t = std::integral_constant<void(*)(), +Value>;
// };
 
template <class... T> struct typelist 
{
     
};
 
 
template<VectorTableEntry ...T>
struct VectorTable 
{
    static constexpr std::array<void(*)(), sizeof...(T)> data{{IRQHandler<T>::constant_t::value...}};
};
 
//should be possible to write a handler as 
//a) static member function 
//b) normal member function with 'this' captured via lambda
//c) free function
//c) lambda
//none of these should require C linkage