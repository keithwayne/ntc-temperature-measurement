#include "debug.h"
#include <math.h>
#include "OLED.h"


/* Global define */


/* Global Variable */
u16 AdcBuf[10];
float Adc_Val = 0.0f;
float Temp = 0.0f;

float value_to_temp_table[] = {
    // 30.0℃ ~ 30.9℃ (索引0~9)
    158.00f, 185.70f, 213.40f, 241.10f, 268.80f,
    296.50f, 324.20f, 351.90f, 379.60f, 407.30f,
    
    // 31.0℃ ~ 31.9℃ (索引10~19)
    435.00f, 462.70f, 490.40f, 518.10f, 545.80f,
    573.50f, 601.20f, 628.90f, 656.60f, 684.30f,
    
    // 32.0℃ ~ 32.9℃ (索引20~29)
    712.00f, 739.60f, 767.20f, 794.80f, 822.40f,
    850.00f, 877.60f, 905.20f, 932.80f, 960.40f,
    
    // 33.0℃ ~ 33.9℃ (索引30~39)
    988.00f, 1015.60f, 1043.20f, 1070.80f, 1098.40f,
    1126.00f, 1153.60f, 1181.20f, 1208.80f, 1236.40f,
    
    // 34.0℃ ~ 34.9℃ (索引40~49)
    1264.00f, 1291.50f, 1319.00f, 1346.50f, 1374.00f,
    1401.50f, 1429.00f, 1456.50f, 1484.00f, 1511.50f,
    
    // 35.0℃ ~ 35.9℃ (索引50~59)
    1539.00f, 1566.40f, 1593.80f, 1621.20f, 1648.60f,
    1676.00f, 1703.40f, 1730.80f, 1758.20f, 1785.60f,
    
    // 36.0℃ ~ 36.9℃ (索引60~69)
    1813.00f, 1840.30f, 1867.60f, 1894.90f, 1922.20f,
    1949.50f, 1976.80f, 2004.10f, 2031.40f, 2058.70f,
    
    // 37.0℃ ~ 37.9℃ (索引70~79)
    2086.00f, 2113.10f, 2140.20f, 2167.30f, 2194.40f,
    2221.50f, 2248.60f, 2275.70f, 2302.80f, 2329.90f,
    
    // 38.0℃ ~ 38.9℃ (索引80~89)
    2357.00f, 2383.90f, 2410.80f, 2437.70f, 2464.60f,
    2491.50f, 2518.40f, 2545.30f, 2572.20f, 2599.10f,
    
    // 39.0℃ ~ 39.9℃ (索引90~99)
    2626.00f, 2652.70f, 2679.40f, 2706.10f, 2732.80f,
    2759.50f, 2786.20f, 2812.90f, 2839.60f, 2866.30f,
    
    // 40.0℃ ~ 40.9℃ (索引100~109)
    2893.00f, 2919.50f, 2946.00f, 2972.50f, 2999.00f,
    3025.50f, 3052.00f, 3078.50f, 3105.00f, 3131.50f,
    
    // 41.0℃ ~ 41.9℃ (索引110~119)
    3158.00f, 3184.30f, 3210.60f, 3236.90f, 3263.20f,
    3289.50f, 3315.80f, 3342.10f, 3368.40f, 3394.70f,
    
    // 42.0℃ ~ 42.9℃ (索引120~129)
    3421.00f, 3447.00f, 3473.00f, 3499.00f, 3525.00f,
    3551.00f, 3577.00f, 3603.00f, 3629.00f, 3655.00f,
    
    // 43.0℃ ~ 43.9℃ (索引130~139)
    3681.00f, 3706.80f, 3732.60f, 3758.40f, 3784.20f,
    3810.00f, 3835.80f, 3861.60f, 3887.40f, 3913.20f,
    
    // 44.0℃ (索引140)
    3939.00f
};

// 数组长度宏定义（自动计算，避免手动计数出错）
#define TABLE_LEN (sizeof(value_to_temp_table) / sizeof(value_to_temp_table[0]))

/**
 * 核心函数：根据输入数值，查表找最接近的项，返回对应0.1℃精度的温度
 * @param target_value 输入的待匹配数值
 * @return 匹配到的温度（30.0~44.0℃，步长0.1℃）；超出范围返回-1.0f（无效值）
 */
float get_closest_temp(float target_value) {
    // 1. 边界校验：数值超出表范围，直接返回无效值
    if (target_value < value_to_temp_table[0] || target_value > value_to_temp_table[TABLE_LEN-1]) {
        return -1.0f;
    }

    // 2. 初始化：最小差值设为极大值，最接近索引设为0
    float min_diff = INFINITY;
    int closest_index = 0;

    // 3. 遍历数组，找与目标数值差值最小的项
    for (int i = 0; i < TABLE_LEN; i++) {
        // 计算当前项与目标值的绝对差值
        float current_diff = fabs(target_value - value_to_temp_table[i]);
        // 找到更小的差值，更新最小差值和索引
        if (current_diff < min_diff) {
            min_diff = current_diff;
            closest_index = i;
        }
    }

    // 4. 根据索引换算温度：30.0℃ + 索引×0.1℃（核心映射关系）
    float closest_temp = 30.0f + closest_index * 0.1f;
    return closest_temp;
}

/*********************************************************************
 * @fn      ADC_Function_Init
 *
 * @brief   Initializes ADC collection.
 *
 * @return  none
 */
void ADC_Function_Init(void)
{
    ADC_InitTypeDef  ADC_InitStructure = {0};
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOC, ENABLE);
    RCC_PB2PeriphClockCmd(RCC_PB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    ADC_DeInit(ADC1);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_CyclesMode7);
    ADC_ExternalTrigConvCmd(ADC1, ENABLE);

    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    ADC_BufferCmd(ADC1, DISABLE);    //disable buffer
}

/*********************************************************************
 * @fn      EXTI_Event_Init
 *
 * @brief   Initializes EXTI.
 *
 * @return  none
 */
void TIM1_PWM_In(u16 arr, u16 psc, u16 ccp)
{
    GPIO_InitTypeDef        GPIO_InitStructure = {0};
    TIM_OCInitTypeDef       TIM_OCInitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};

    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOC| RCC_PB2Periph_TIM1, ENABLE);
    RCC_PB2PeriphClockCmd(RCC_PB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_PartialRemap2_TIM1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = ccp;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);

    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM1, ENABLE);
    TIM_Cmd(TIM1, ENABLE);
}

/*********************************************************************
 * @fn      DMA_Tx_Init
 *
 * @brief   Initializes the DMAy Channelx configuration.
 *
 * @param   DMA_CHx - x can be 1 to 7.
 *          ppadr - Peripheral base address.
 *          memadr - Memory base address.
 *          bufsize - DMA channel buffer size.
 *
 * @return  none
 */
void DMA_Tx_Init(DMA_Channel_TypeDef *DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize)
{
    DMA_InitTypeDef DMA_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA_CHx);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;
    DMA_InitStructure.DMA_MemoryBaseAddr = memadr;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = bufsize;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA_CHx, &DMA_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE); // 启用传输完成中断
}

/*********************************************************************
 * @fn      OPA1_Init
 *
 * @brief   Initializes OPA collection.
 *
 * @return  none
 */
void OPA1_Init( void )
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    OPA_InitTypeDef  OPA_InitStructure = {0};

    RCC_PB2PeriphClockCmd( RCC_PB2Periph_GPIOA, ENABLE);
    RCC_PB2PeriphClockCmd( RCC_PB2Periph_GPIOD, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    OPA_InitStructure.Mode = OUT_IO_OUT0;
    OPA_InitStructure.PSEL = CHP0;
    OPA_InitStructure.NSEL = CHN0;
    OPA_InitStructure.OPA_HS = HS_ON;
    OPA_Init(&OPA_InitStructure);
    OPA_Cmd(ENABLE);

}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    SystemCoreClockUpdate();
    Delay_Init();

    FLASH_UserOptionByteConfig(OB_IWDG_SW,  OB_STDBY_NoRST, OB_RST_EN_DT12ms, OB_PowerON_Start_Mode_BOOT);

    OLED_Init();
    Delay_Ms(500);
    
    OPA_Unlock();
    OPA1_Init();
    ADC_Function_Init();
    DMA_Tx_Init(DMA1_Channel1, (u32)&ADC1->RDATAR, (u32)AdcBuf, 10);
    DMA_Cmd(DMA1_Channel1, ENABLE);
    TIM1_PWM_In(4800,100,2400);

    while(1)
    {
        OLED_ShowNum(0,0, Adc_Val * 10, 5, 24, 1);
        Temp = get_closest_temp(Adc_Val);
        if(Temp <= 32.0 || Temp >= 42.0)
        {
            OLED_ShowString(0, 24, " NA ", 24, 1);
        }
        else
        {
            OLED_ShowFloat(0, 24, Temp, 4, 1);
        }
        OLED_Refresh();
        Delay_Ms(500);
    }
}

void DMA1_Channel1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void DMA1_Channel1_IRQHandler(void)
{
    u32 Adc_Sum = 0;
    if(DMA_GetITStatus(DMA1_IT_TC1)){
        for(int i=0; i<10; i++)
        {
            Adc_Sum += AdcBuf[i];
        }
        Adc_Val = (float)Adc_Sum / 10;
        DMA_ClearITPendingBit(DMA1_IT_TC1); // 清除中断标志
    }
}