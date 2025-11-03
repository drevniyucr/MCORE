///*
// * mcore_system.cpp
// *
// *  Created on: Jul 13, 2025
// *      Author: doomt
// */
//
//#include "mcore_system.hpp"
//
//
//
//volatile uint32_t myTick = 0;   // счетчик миллисекунд
//
//// Обработчик прерывания SysTick
//extern "C" void SysTick_Handler(void) {
//    myTick += 1;   // каждый тик увеличиваем счетчик
//}
//
//
//void delay_ms(uint32_t ms) {
//    uint32_t start = myTick;
//    while ((myTick - start) < ms) {
//        // ждем
//    }
//}
//
//uint32_t get_tick(void){
//	return myTick;
//}
