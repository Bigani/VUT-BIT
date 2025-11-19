/* Header file with all the essential definitions for a given type of MCU */
#include "MK60D10.h"


/* Macros for bit-level registers manipulation */
#define GPIO_PIN_MASK	0x1Fu
#define GPIO_PIN(x)		(((1)<<(x & GPIO_PIN_MASK)))
#define PIT_IRQn	22

#define BTN_SW2 0x400 // Port E, bit 10
#define BTN_SW3 0x1000 // Port E, bit 12
#define BTN_SW4 0x8000000 // Port E, bit 27
#define BTN_SW5 0x4000000 // Port E, bit 26

#define R0 (0x1 << 26)
#define R1 (0x1 << 24)
#define R2 (0x1 << 9)
#define R3 (0x1 << 25)
#define R4 (0x1 << 28)
#define R5 (0x1 << 7)
#define R6 (0x1 << 27)
#define R7 (0x1 << 29)

unsigned int snake[4];
unsigned int segm = 0;
unsigned int col = 3;	//start pos head
unsigned int row = 3;	//start pos head
int count = 0;
int a = 0; 	// Direction



void SystemConfig();

void column_select(unsigned int col_num);

void row_select(unsigned int row_num);

void snakeInit();

void move();

void delay(uint64_t bound);

void LPTMR0Init();

void LPTMR0_IRQHandler(void);

void PORTE_IRQHandler(void);


int main(void)
{
	SystemConfig();
	snakeInit();
	LPTMR0Init();
    for (;;) {}
    return 0;
}


void delay(uint64_t bound) {
	for (uint64_t i=0; i < bound; i++) { __NOP(); }
}

void PORTE_IRQHandler(void){

	delay (10000);
	if (PORTE->ISFR & BTN_SW5 && ((GPIOE_PDIR & BTN_SW5) == 0)){	/*	UP	*/
		if((a % 2) == 1) a = 2;
	}
	else if (PORTE->ISFR & BTN_SW3 && ((GPIOE_PDIR & BTN_SW3) == 0)){	/*	DOWN	*/
		if((a % 2) == 1) a = 0;
	}
	else if (PORTE->ISFR & BTN_SW4 && ((GPIOE_PDIR & BTN_SW4) == 0)){	/*	LEFT	*/
		if((a % 2) == 0) a = 3;
	}
	else if (PORTE->ISFR & BTN_SW2 && ((GPIOE_PDIR & BTN_SW2) == 0)){	/*	RIGHT	*/
		if((a % 2) == 0) a = 1;
	}
	PORTE->ISFR = BTN_SW5 | BTN_SW3 | BTN_SW4 | BTN_SW2;
}

void LPTMR0Init(){
    SIM_SCGC5 |= SIM_SCGC5_LPTIMER_MASK;
    LPTMR0_CSR &= ~LPTMR_CSR_TEN_MASK;
    LPTMR0_PSR = ( LPTMR_PSR_PRESCALE(0)
                 | LPTMR_PSR_PBYP_MASK
                 | LPTMR_PSR_PCS(1)) ;
    LPTMR0_CMR = 0x1;
    LPTMR0_CSR =(  LPTMR_CSR_TCF_MASK
                 | LPTMR_CSR_TIE_MASK
                );
    NVIC_EnableIRQ(LPTMR0_IRQn);
    LPTMR0_CSR |= LPTMR_CSR_TEN_MASK;   // Turn ON LPTMR0 and start counting
}

void LPTMR0_IRQHandler(void){
    LPTMR0_CSR |=  LPTMR_CSR_TCF_MASK;
    if(count == 150){
    	move();
    	count=0;
    }

    if (segm == 4) {
    		segm = 0;
	}
	PTA->PDOR = snake[segm];
	segm++;
	count++;
}

void SystemConfig() {

	MCG->C4 |= (MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0x01));
	SIM->CLKDIV1 |= SIM_CLKDIV1_OUTDIV1(0x00);

	SIM->SCGC5 = SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTE_MASK;


	/*	Buttons  */
    PORTE->PCR[10] = ( PORT_PCR_ISF(0x01)
                | PORT_PCR_IRQC(0x0A)
                | PORT_PCR_MUX(0x01)
                | PORT_PCR_PE(0x01)
                | PORT_PCR_PS(0x01));
    PORTE->PCR[12] = ( PORT_PCR_ISF(0x01)
                | PORT_PCR_IRQC(0x0A)
                | PORT_PCR_MUX(0x01)
                | PORT_PCR_PE(0x01)
                | PORT_PCR_PS(0x01));
    PORTE->PCR[27] = ( PORT_PCR_ISF(0x01)
                | PORT_PCR_IRQC(0x0A)
                | PORT_PCR_MUX(0x01)
                | PORT_PCR_PE(0x01)
                | PORT_PCR_PS(0x01));
    PORTE->PCR[26] = ( PORT_PCR_ISF(0x01)
                | PORT_PCR_IRQC(0x0A)
                | PORT_PCR_MUX(0x01)
                | PORT_PCR_PE(0x01)
                | PORT_PCR_PS(0x01));

    NVIC_ClearPendingIRQ(PORTE_IRQn);
    NVIC_EnableIRQ(PORTE_IRQn);

	/* Set corresponding PTA pins (column activators of 74HC154) for GPIO functionality */
	PORTA->PCR[8] = ( 0|PORT_PCR_MUX(0x01) );  // A0
	PORTA->PCR[10] = ( 0|PORT_PCR_MUX(0x01) ); // A1
	PORTA->PCR[6] = ( 0|PORT_PCR_MUX(0x01) );  // A2
	PORTA->PCR[11] = ( 0|PORT_PCR_MUX(0x01) ); // A3

	/* Set corresponding PTA pins (rows selectors of 74HC154) for GPIO functionality */
	PORTA->PCR[26] = ( 0|PORT_PCR_MUX(0x01) );  // R0
	PORTA->PCR[24] = ( 0|PORT_PCR_MUX(0x01) );  // R1
	PORTA->PCR[9] = ( 0|PORT_PCR_MUX(0x01) );   // R2
	PORTA->PCR[25] = ( 0|PORT_PCR_MUX(0x01) );  // R3
	PORTA->PCR[28] = ( 0|PORT_PCR_MUX(0x01) );  // R4
	PORTA->PCR[7] = ( 0|PORT_PCR_MUX(0x01) );   // R5
	PORTA->PCR[27] = ( 0|PORT_PCR_MUX(0x01) );  // R6
	PORTA->PCR[29] = ( 0|PORT_PCR_MUX(0x01) );  // R7

	/* Set corresponding PTE pins (output enable of 74HC154) for GPIO functionality */
	PORTE->PCR[28] = ( 0|PORT_PCR_MUX(0x01) ); // #EN

	/* Change corresponding PTA port pins as outputs */
	PTA->PDDR = GPIO_PDDR_PDD(0x3F000FC0);

	/* Change corresponding PTE port pins as outputs */
	PTE->PDDR = GPIO_PDDR_PDD( GPIO_PIN(28) );
}

void column_select(unsigned int col_num)
{
	unsigned i, result, col_sel[4];

	for (i =0; i<4; i++) {
		result = col_num / 2;	  // Whole-number division of the input number
		col_sel[i] = col_num % 2;
		col_num = result;

		switch(i) {

			// Selection signal A0
		    case 0:
				((col_sel[i]) == 0) ? (PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(8))) : (PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(8)));
				break;

			// Selection signal A1
			case 1:
				((col_sel[i]) == 0) ? (PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(10))) : (PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(10)));
				break;

			// Selection signal A2
			case 2:
				((col_sel[i]) == 0) ? (PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(6))) : (PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(6)));
				break;

			// Selection signal A3
			case 3:
				((col_sel[i]) == 0) ? (PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(11))) : (PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(11)));
				break;

			// Otherwise nothing to do...
			default:
				break;
		}
	}
}

void row_select(unsigned int row_num){
	switch(row_num){
		case 0:
			PTA->PDOR |= R0;
			break;
		case 1:
			PTA->PDOR |= R1;
			break;
		case 2:
			PTA->PDOR |= R2;
			break;
		case 3:
			PTA->PDOR |= R3;
			break;
		case 4:
			PTA->PDOR |= R4;
			break;
		case 5:
			PTA->PDOR |= R5;
			break;
		case 6:
			PTA->PDOR |= R6;
			break;
		case 7:
			PTA->PDOR |= R7;
			break;
		default:
			break;
	}
}

void snakeInit(){
	for (int i=0; i<4; i++){
		snake[3-i] = 0x0;
		column_select(i);
		PTA->PDOR |= R3;
		snake[3-i] = PTA->PDOR;
	}
}

void move(){
	PTA->PDOR = GPIO_PDOR_PDO(0x0);
	for(int i=2; i>=0;i--){
		snake[i+1] = snake[i];
	}

	if (a == 0) col++;
	else if (a == 1) row--;
	else if (a == 2) col--;
	else if (a == 3) row++;

	if (col == 16) col = 0;
	else if (col == -1) {col = 15;}
	if (row == 8) row = 0;
	else if (row == -1) {row = 7;}

	column_select(col);
	row_select(row);
	snake[0] = PTA->PDOR;
}

