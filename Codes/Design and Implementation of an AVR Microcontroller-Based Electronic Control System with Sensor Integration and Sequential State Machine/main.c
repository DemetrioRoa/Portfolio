#include <avr/io.h>
#define F_CPU 16000000UL
#include <avr/interrupt.h>
#include <util/delay.h>
#define TIEMPO 0.05
//ENTRADAS
#define botonesDDR DDRD
#define botonesPORT PORTD
#define botonesPIN PIND
#define boton0 PIND0
#define boton1 PIND1
#define boton2 PIND2
//Macro
#define boton0Read (!(botonesPIN & (1 << boton0))) //START
#define boton1Read (!(botonesPIN & (1 << boton1))) //RESET
#define boton2Read (!(botonesPIN & (1 << boton2))) //HOLD
//Variables
char letras[4];
char numbers[4];
volatile int contador = 0;
volatile int contador1s = 0;
volatile int valueADC = 0;
volatile int lastScaledValue = 0;
#define DISPLAY_DDRX_SEG_AF DDRB
#define DISPLAY_PORTX_SEG_AF PORTB
#define DISPLAY_DDRX_SEG_G DDRC
#define DISPLAY_PORTX_SEG_G PORTC
// SEG_A
#define DISPLAY_SEG_A PORTB0
#define DISPLAY_SEG_A_ON DISPLAY_PORTX_SEG_AF |= (1 << DISPLAY_SEG_A)
#define DISPLAY_SEG_A_OFF DISPLAY_PORTX_SEG_AF &= ~(1 << DISPLAY_SEG_A)
// SEG_B
#define DISPLAY_SEG_B PORTB1
#define DISPLAY_SEG_B_ON DISPLAY_PORTX_SEG_AF |= (1 << DISPLAY_SEG_B)
#define DISPLAY_SEG_B_OFF DISPLAY_PORTX_SEG_AF &= ~(1 << DISPLAY_SEG_B)
// SEG_C
#define DISPLAY_SEG_C PORTB2
#define DISPLAY_SEG_C_ON DISPLAY_PORTX_SEG_AF |= (1 << DISPLAY_SEG_C)
#define DISPLAY_SEG_C_OFF DISPLAY_PORTX_SEG_AF &= ~(1 << DISPLAY_SEG_C)
// SEG_D
#define DISPLAY_SEG_D PORTB3
#define DISPLAY_SEG_D_ON DISPLAY_PORTX_SEG_AF |= (1 << DISPLAY_SEG_D)
#define DISPLAY_SEG_D_OFF DISPLAY_PORTX_SEG_AF &= ~(1 << DISPLAY_SEG_D)
// SEG_E
#define DISPLAY_SEG_E PORTB4
#define DISPLAY_SEG_E_ON DISPLAY_PORTX_SEG_AF |= (1 << DISPLAY_SEG_E)
#define DISPLAY_SEG_E_OFF DISPLAY_PORTX_SEG_AF &= ~(1 << DISPLAY_SEG_E)
// SEG_F
#define DISPLAY_SEG_F PORTB5
#define DISPLAY_SEG_F_ON DISPLAY_PORTX_SEG_AF |= (1 << DISPLAY_SEG_F)
#define DISPLAY_SEG_F_OFF DISPLAY_PORTX_SEG_AF &= ~(1 << DISPLAY_SEG_F)
// SEG_G
#define DISPLAY_SEG_G PORTC0
#define DISPLAY_SEG_G_ON DISPLAY_PORTX_SEG_G |= (1 << DISPLAY_SEG_G)
#define DISPLAY_SEG_G_OFF DISPLAY_PORTX_SEG_G &= ~(1 << DISPLAY_SEG_G)
//barrido
#define BARRIDO_DDR DDRD
#define BARRIDO_PORT PORTD
#define BARRIDO_Millares PORTD4
#define BARRIDO_Centenas PORTD5
#define BARRIDO_Decenas PORTD6
#define BARRIDO_Unidades PORTD7
//Declaracion
void init_display(void);
void display_show_number(uint8_t numero);
void display_show_letter(char letra);
void init_barrido(void);
void barrido_show_word(char letra3, char letra2, char letra1, char letra0);
void barrido_show_numbers(int number);
void init_timer0(void);
void init_ports(void);
void init_ADC(void);
void init_INT0(void);

int map(int x, int in_min, int in_max, int out_min, int out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//Estados
enum State {IDLE, START, HOLD};
State state = IDLE;

void IDLE(void){
  // Lógica para el estado IDLE
            barrido_show_word('C', 'I', 'A', 'O');
            _delay_ms(3000);  // Esperar 3 segundos
            barrido_show_word('D', 'E', 'L', 'E');
            
}

void START(void){
  // Lógica para el estado START
         int scaledValue = map(valueADC, 0, 255, 0, 100);
          lastScaledValue = scaledValue;  // Store the current scaled value

          // Mostrar el valor escalado en el display
          barrido_show_numbers(lastScaledValue);

}

void HOLD(void){
  // Lógica para el estado IDLE
            barrido_show_word('S', 'T', 'O', 'P');
            barrido_show_numbers(lastScaledValue);  // Mostrar el último valor escalado en el display


                // Esperar un breve periodo para evitar cambios rápidos
                _delay_ms(50);
}

void init_ports(void) {
botonesDDR &= ~(1 << boton0);
botonesDDR &= ~(1 << boton1);
botonesDDR &= ~(1 << boton2);

}
void init_display(void)
{
DISPLAY_DDRX_SEG_AF |= (1<<DISPLAY_SEG_A);
DISPLAY_DDRX_SEG_AF |= (1<<DISPLAY_SEG_B);
DISPLAY_DDRX_SEG_AF |= (1<<DISPLAY_SEG_C);
DISPLAY_DDRX_SEG_AF |= (1<<DISPLAY_SEG_D);
DISPLAY_DDRX_SEG_AF |= (1<<DISPLAY_SEG_E);
DISPLAY_DDRX_SEG_AF |= (1<<DISPLAY_SEG_F);
DISPLAY_DDRX_SEG_G |= (1<<DISPLAY_SEG_G);
}
void display_show_number(uint8_t numero)
{
switch (numero)
{
case 0:
DISPLAY_SEG_A_ON;
DISPLAY_SEG_B_ON;
DISPLAY_SEG_C_ON;
DISPLAY_SEG_D_ON;
DISPLAY_SEG_E_ON;
DISPLAY_SEG_F_ON;
DISPLAY_SEG_G_OFF;
break;
case 1:
DISPLAY_SEG_A_OFF;
DISPLAY_SEG_B_ON;
DISPLAY_SEG_C_ON;
DISPLAY_SEG_D_OFF;
DISPLAY_SEG_E_OFF;
DISPLAY_SEG_F_OFF;
DISPLAY_SEG_G_OFF;
break;
case 2:
DISPLAY_SEG_A_ON;
DISPLAY_SEG_B_ON;
DISPLAY_SEG_C_OFF;
DISPLAY_SEG_D_ON;
DISPLAY_SEG_E_ON;
DISPLAY_SEG_F_OFF;
DISPLAY_SEG_G_ON;
break;
case 3:
DISPLAY_SEG_A_ON;
DISPLAY_SEG_B_ON;
DISPLAY_SEG_C_ON;
DISPLAY_SEG_D_ON;
DISPLAY_SEG_E_OFF;
DISPLAY_SEG_F_OFF;
DISPLAY_SEG_G_ON;
break;
case 4:
DISPLAY_SEG_A_OFF;
DISPLAY_SEG_B_ON;
DISPLAY_SEG_C_ON;
DISPLAY_SEG_D_OFF;
DISPLAY_SEG_E_OFF;
DISPLAY_SEG_F_ON;
DISPLAY_SEG_G_ON;
break;
case 5:
DISPLAY_SEG_A_ON;
DISPLAY_SEG_B_OFF;
DISPLAY_SEG_C_ON;
DISPLAY_SEG_D_ON;
DISPLAY_SEG_E_OFF;
DISPLAY_SEG_F_ON;
DISPLAY_SEG_G_ON;
break;
case 6:
DISPLAY_SEG_A_ON;
DISPLAY_SEG_B_OFF;
DISPLAY_SEG_C_ON;
DISPLAY_SEG_D_ON;
DISPLAY_SEG_E_ON;
DISPLAY_SEG_F_ON;
DISPLAY_SEG_G_ON;
break;
case 7:
DISPLAY_SEG_A_ON;
DISPLAY_SEG_B_ON;
DISPLAY_SEG_C_ON;
DISPLAY_SEG_D_OFF;
DISPLAY_SEG_E_OFF;
DISPLAY_SEG_F_OFF;
DISPLAY_SEG_G_OFF;
break;
case 8:
DISPLAY_SEG_A_ON;
DISPLAY_SEG_B_ON;
DISPLAY_SEG_C_ON;
DISPLAY_SEG_D_ON;
DISPLAY_SEG_E_ON;
DISPLAY_SEG_F_ON;
DISPLAY_SEG_G_ON;
break;
case 9:
DISPLAY_SEG_A_ON;
DISPLAY_SEG_B_ON;
DISPLAY_SEG_C_ON;
DISPLAY_SEG_D_ON;
DISPLAY_SEG_E_OFF;
DISPLAY_SEG_F_ON;
DISPLAY_SEG_G_ON;
break;
default:
DISPLAY_SEG_A_OFF;
DISPLAY_SEG_B_OFF;
DISPLAY_SEG_C_OFF;
DISPLAY_SEG_D_OFF;
DISPLAY_SEG_E_OFF;
DISPLAY_SEG_F_OFF;
DISPLAY_SEG_G_OFF;
break;
}
}
void display_show_letter(char letra)
{
switch (letra)
{
case 'C':
DISPLAY_SEG_A_OFF;
DISPLAY_SEG_B_ON;
DISPLAY_SEG_C_ON;
DISPLAY_SEG_D_OFF;
DISPLAY_SEG_E_ON;
DISPLAY_SEG_F_ON;
DISPLAY_SEG_G_ON;
break;
case 'I':
DISPLAY_SEG_A_OFF;
DISPLAY_SEG_B_OFF;
DISPLAY_SEG_C_OFF;
DISPLAY_SEG_D_OFF;
DISPLAY_SEG_E_ON;
DISPLAY_SEG_F_ON;
DISPLAY_SEG_G_OFF;
break;
case 'A':
DISPLAY_SEG_A_ON;
DISPLAY_SEG_B_ON;
DISPLAY_SEG_C_ON;
DISPLAY_SEG_D_OFF;
DISPLAY_SEG_E_ON;
DISPLAY_SEG_F_ON;
DISPLAY_SEG_G_ON;
break;
case 'D':
DISPLAY_SEG_A_ON;
DISPLAY_SEG_B_ON;
DISPLAY_SEG_C_ON;
DISPLAY_SEG_D_ON;
DISPLAY_SEG_E_ON;
DISPLAY_SEG_F_ON;
DISPLAY_SEG_G_OFF;
break;
case 'E':
DISPLAY_SEG_A_ON;
DISPLAY_SEG_B_OFF;
DISPLAY_SEG_C_OFF;
DISPLAY_SEG_D_ON;
DISPLAY_SEG_E_ON;
DISPLAY_SEG_F_ON;
DISPLAY_SEG_G_ON;
break;
case 'L':
DISPLAY_SEG_A_OFF;
DISPLAY_SEG_B_OFF;
DISPLAY_SEG_C_OFF;
DISPLAY_SEG_D_OFF;
DISPLAY_SEG_E_ON;
DISPLAY_SEG_F_OFF;
DISPLAY_SEG_G_ON;
break;
case 'S':
DISPLAY_SEG_A_ON;
DISPLAY_SEG_B_OFF;
DISPLAY_SEG_C_ON;
DISPLAY_SEG_D_ON;
DISPLAY_SEG_E_OFF;
DISPLAY_SEG_F_ON;
DISPLAY_SEG_G_ON;
break;
case 'T':
DISPLAY_SEG_A_OFF;
DISPLAY_SEG_B_OFF;
DISPLAY_SEG_C_OFF;
DISPLAY_SEG_D_ON;
DISPLAY_SEG_E_ON;
DISPLAY_SEG_F_ON;
DISPLAY_SEG_G_ON;
break;
case 'P':
DISPLAY_SEG_A_ON;
DISPLAY_SEG_B_ON;
DISPLAY_SEG_C_OFF;
DISPLAY_SEG_D_OFF;
DISPLAY_SEG_E_ON;
DISPLAY_SEG_F_ON;
DISPLAY_SEG_G_ON;
break;
case 'O':
DISPLAY_SEG_A_ON;
DISPLAY_SEG_B_ON;
DISPLAY_SEG_C_ON;
DISPLAY_SEG_D_ON;
DISPLAY_SEG_E_ON;
DISPLAY_SEG_F_ON;
DISPLAY_SEG_G_OFF;
break;
}
}
void init_barrido(void)
{
BARRIDO_DDR |= (1<<BARRIDO_Millares);
BARRIDO_DDR |= (1<<BARRIDO_Centenas);
BARRIDO_DDR |= (1<<BARRIDO_Decenas);
BARRIDO_DDR |= (1<<BARRIDO_Unidades);
}

void barrido_show_word(char letra3, char letra2, char letra1, char letra0)
{
letras[3] = letra3;
letras[2] = letra2;
letras[1] = letra1;
letras[0] = letra0;
for(int i=4, k=3 ;i<8;i++)
{
BARRIDO_PORT=~(1<<i);
display_show_letter(letras[k]);
_delay_ms(TIEMPO);
k--;
}
}
void barrido_show_numbers(int number)
{
numbers[3] = number/1000;
numbers[2] = (number%1000)/100;
numbers[1] = (number%100)/10;
numbers[0] = number%10;
for(int i=4, e=3 ;i<8;i++)
{
BARRIDO_PORT=~(1<<i);
display_show_number(numbers[e]);
_delay_ms(TIEMPO);
e--;
}
}
void init_INT0(void)
{
//Rising edge
EICRA |= (1<<ISC00);
EICRA |= (1<<ISC01);
//ENABLE
EIMSK |= (1<<INT0);
}
ISR(INT0_vect)
{
contador++;
}
//Timer init
void init_timer0(void){
//CTC
TCCR0A &= ~(1<<WGM00);
TCCR0A |= (1<<WGM01);
TCCR0B &= ~(1<<WGM02);
//Clock select -> Prescaler /8
TCCR0B |= (1<<CS02);
TCCR0B &= ~ (1<<CS01);
TCCR0B |= (1<<CS00);
//TOP
OCR0A = 156;
//Interrupt enable
TIMSK0 |= (1<<OCIE0A);
}
ISR(TIMER0_COMPA_vect) {
    contador++;

    if (contador > 100) {
        // Cap contador at 100
        contador = 0;
    }
  }

ISR(ADC_vect) {
    valueADC = ADCH; // Obtener el valor del ADC (de 0 a 255)
    ADCSRA |= (1 << ADSC); // Iniciar la próxima conversión
}

void init_ADC(void) {
    // Seleccionar ADC2 como fuente de entrada
  
 	ADMUX &= ~(1 << MUX0);
	ADMUX |= (1 << MUX1);
	ADMUX &= ~(1 << MUX2);
	ADMUX &= ~(1 << MUX3);

    // Establecer la referencia de voltaje a Vcc
    ADMUX &= ~(1 << REFS1);
    ADMUX |= (1 << REFS0);
    // Habilitar el ADC y la interrupción del ADC
    ADCSRA |= (1 << ADEN) | (1 << ADIE);
    // Establecer el prescaler del ADC a 128
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    // Iniciar la conversión del ADC
    ADCSRA |= (1 << ADSC);
}
