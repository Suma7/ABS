#include <avr/io.h>
#include<util/delay.h>
#include<avr/interrupt.h>

#define SET_BIT(PORT,PIN) PORT|=(1<<PIN)

#define CLR_BIT(PORT,PIN) PORT&=~(1<<PIN)

void initADC();
uint16_t readADC(uint8_t ch);

volatile int Brake_pedal = 0;
volatile uint16_t Engine_switch=0;
volatile int Obstacle=0;

int main(void)
{
    CLR_BIT(DDRB,PB0);
    SET_BIT(PORTB,PB0);
    CLR_BIT(DDRD,PD2);
    SET_BIT(PORTD,PD2);
    CLR_BIT(DDRD,PD3);
    SET_BIT(PORTD,PD3);
    SET_BIT(EICRA,ISC10);
    SET_BIT(EIMSK,INT1);
    SET_BIT(EICRA,ISC00);
    SET_BIT(EIMSK,INT0);
    SET_BIT(PCICR,PCIE0);
    SET_BIT(PCMSK0,PCINT0);
    sei();
    initADC();
    CLR_BIT(DDRC,PC0);
    CLR_BIT(DDRC,PC2);
    SET_BIT(DDRB,5);
    uint16_t Speed=0;
    uint16_t Rpm=0;

    while(1)
    {
        uint16_t Speed=0;
    uint16_t Rpm=0;
        Speed = readADC(0);
        _delay_ms(200);
        Rpm = readADC(2);
        _delay_ms(200);

        if(Engine_switch)
        {
            //ABS
            if((Obstacle==1)&& (Brake_pedal ==1) && (Speed > 40) && (Rpm >200))
                {
                 SET_BIT(PORTB,PB5);
                 _delay_ms(200);
                }
              else
                {

                 CLR_BIT(PORTB,PB5);
                 _delay_ms(200);
                }

            }
            else
                {

                 CLR_BIT(PORTB,PB5);
                }
     }
     return 0;
}
ISR(INT0_vect)
{
    if(!(PIND&(1<<PD2)))
        Brake_pedal = 1;
    else
        Brake_pedal = 0;
}
ISR(INT1_vect)
{
    if(!(PIND&(1<<PD3)))
        Engine_switch = 1;
    else
        Engine_switch= 0;
}
ISR(PCINT0_vect)

{

   /*if(!(PIND&(1<<PB0)))
        Obstacle = 1;
    else
        Obstacle= 0;*/
        Obstacle=!Obstacle;

}



void initADC()
{
    ADMUX=(1<<REFS0);
    ADCSRA=(1<<ADEN)|(7<<ADPS0);
}
uint16_t readADC(uint8_t ch)
{
    ADMUX&=0xf8;
    ch=ch&0b00000111;
    ADMUX|=ch;

    ADCSRA|=(1<<ADSC);
    while(!(ADCSRA&(1<<ADIF)));
    ADCSRA|=(1<<ADIF);
    uint16_t TEMP=0;
    TEMP|=(ADCH<<8);
    TEMP|=(ADCL);
    //ADC=TEMP;
    return(TEMP);
}
