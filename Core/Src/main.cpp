
#include "mcore.hpp"
#include "main.hpp"
#include <cstdio>

void GPIO_Init(void);
void RCC_Init(void);
// void TIM2_Init(void);
// void TIM3_Init(void);

uint8_t test_data[] = "LMAO LMAO LMAO LMAO LMAO LMAO LMAO";

int main(void)
{  
	SystemInit();
	uint32_t start = DWT::_CYCCNT::read();
	RCC_Init();
    MPU_Config();
   SCB_EnableICache();
   // SCB_EnableDCache(); 
    GPIO_Init();
    if (ETH_Init() != ETH_Status::OK) {Error_Handler();}
    NET_TCP_Init();
    NVIC_API::EnableIRQGlobal();
    uint32_t elapsed = DWT::_CYCCNT::read() - start;
	print("Spend %lu cycles for init\n", elapsed);
	(void)start;
	(void)elapsed;
	
	uint32_t tickstart = get_tick();
	uint32_t tickstart1 = get_tick();
	
	while (true)
    {
        ETH_RxWorker();
        if ((get_tick() - tickstart) > 5000) {
        	tickstart = get_tick();
        	if (tcp_clients[9].state == tcp_state_t::TCP_ESTABLISHED){
        	NET_TCP_SendUser(&tcp_clients[9], test_data,sizeof(test_data));
        	}
        }
		if((get_tick()- tickstart1) > 100) {
			NET_TCP_Timers();
			tickstart1 = get_tick();
		}
}
}

void RCC_Init(void) {
	constexpr const ClockConfig cfg = {
			.PLLM = 4,              // MCO(8Mhz)/8=2
			.PLLN = 216,            // 2*216 = 432
			.PLLP = PLL_P::Div2,    // 432/2=216MHz SYSCKL MAX
			.AHBDiv = AHBPrescaler::Div1,    // 216MHz HCKL MAX
			.APB1Div = APBPrescaler::Div4,    //216/4=54 MHz APB1 MAX
			.APB2Div = APBPrescaler::Div2,    //216/2=108 MHz APB2 MAX
			.FLASHLatency = FLASH_Latency::WS7,
			.useHSE = true,
			.useHSEBypass =true,
			.useSysTick = true
};

	enablePowerInterface();
	SetVoltageScale<VoltageScale::Scale1>();

	if (OSCInit<cfg>() != RCCStatus::OK ||
		OverDriveInit() != RCCStatus::OK||
		RCC_ClockInit<cfg>() != RCCStatus::OK)
	{
		Error_Handler();
	}

};

void GPIO_Init(void) {

	RCC_GPIO_ALLEN();

	GPIO_Config GPIO_ConfigStruct = {};
	uint32_t pin_mask;

	/**ETH GPIO Configuration
	 PC1     ------> ETH_MDC
	 PA1     ------> ETH_REF_CLK
	 PA2     ------> ETH_MDIO
	 PA7     ------> ETH_CRS_DV
	 PC4     ------> ETH_RXD0
	 PC5     ------> ETH_RXD1
	 PB13     ------> ETH_TXD1
	 PG11     ------> ETH_TX_EN
	 PG13     ------> ETH_TXD0
	 */

	GPIO_ConfigStruct.mode = Mode::Alt;
	GPIO_ConfigStruct.otype = OType::PP;
	GPIO_ConfigStruct.pull = Pull::None;
	GPIO_ConfigStruct.speed = Speed::VeryHigh;
	GPIO_ConfigStruct.af = AF::AF11;
	pin_mask = (1U << 1) | (1U << 2) | (1U << 7); //(PA1,2,7)
	GPIO_ConfigGroupPin<GPIOA>(GPIO_ConfigStruct, pin_mask);

	GPIO_ConfigPin<GPIOB>(GPIO_ConfigStruct, 13); //PB13

	pin_mask = (1U << 1) | (1U << 4) | (1U << 5); //PC1,4,5
	GPIO_ConfigGroupPin<GPIOC>(GPIO_ConfigStruct, pin_mask);

	pin_mask = (1U << 11) | (1U << 13); //PG11,13
	GPIO_ConfigGroupPin<GPIOG>(GPIO_ConfigStruct, pin_mask);

	/**TIM GPIO Configuration
	 PB10     ------> TIM2_CH3
	 PA3     ------> TIM2_CH4
	 PA6     ------> TIM3_CH1
	 PC7     ------> TIM3_CH2
	 PB0     ------> TIM3_CH3
	 PB1     ------> TIM3_CH4**/

	GPIO_ConfigStruct.pull = Pull::PullUp;
	GPIO_ConfigStruct.af = AF::AF1; // TIM2
	GPIO_ConfigPin<GPIOB>(GPIO_ConfigStruct, 10); // PB10

	GPIO_ConfigPin<GPIOA>(GPIO_ConfigStruct, 3); // PA3

	GPIO_ConfigStruct.af = AF::AF2; // TIM3
	GPIO_ConfigPin<GPIOA>(GPIO_ConfigStruct, 6); //PA6

	GPIO_ConfigPin<GPIOC>(GPIO_ConfigStruct, 7); //PC7

	pin_mask = (1U << 0) | (1U << 1); //PB0,1
	GPIO_ConfigGroupPin<GPIOB>(GPIO_ConfigStruct, pin_mask);
}

void Error_Handler(void) {
	while (1) {
		// error processing
	}
}
