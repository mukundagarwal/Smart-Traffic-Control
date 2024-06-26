#include <LPC17xx.h>


#define ROAD_1_PIN (1 << 4)   // P0.4 for road sensor 1
#define ROAD_2_PIN (1 << 5)   // P0.5 for road sensor 2
#define ROAD_3_PIN (1 << 6)   // P0.6 for road sensor 3
#define ROAD_4_PIN (1 << 7)   // P0.7 for road sensor 4

#define LED_1_PIN (1 << 23)    // P1.23 for LED 1
#define LED_2_PIN (1 << 24)    // P1.24 for LED 2
#define LED_3_PIN (1 << 25)   // P1.25 for LED 3
#define LED_4_PIN (1 << 26)   // P1.26 for LED 4

unsigned int road_counters[4] = {0};
unsigned int temp=5;
unsigned int j,i;
unsigned int FIOPREV=0xFF;
unsigned int max_counter = 0;
unsigned int max_road = 0;


unsigned int array_dec[10]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};

unsigned int update_leds() {
    max_counter = 0;
    max_road = 0;
    
    for (j = 0; j < 4; j++) {
        if (road_counters[j] > max_counter) {
            max_counter = road_counters[j];
            max_road = j;
        }
    }
    
    switch(max_road) {
        case 0:
            LPC_GPIO1->FIOPIN = (LPC_GPIO1->FIOPIN & ~(LED_1_PIN | LED_2_PIN | LED_3_PIN | LED_4_PIN)) | LED_1_PIN;
	    temp=road_counters[0];
            road_counters[0] = 0;
            return temp;
        case 1:
            LPC_GPIO1->FIOPIN = (LPC_GPIO1->FIOPIN & ~(LED_1_PIN | LED_2_PIN | LED_3_PIN | LED_4_PIN)) | LED_2_PIN;
            temp=road_counters[1];
	    road_counters[1] = 0;
            return temp;
        case 2:
            LPC_GPIO1->FIOPIN = (LPC_GPIO1->FIOPIN & ~(LED_1_PIN | LED_2_PIN | LED_3_PIN | LED_4_PIN)) | LED_3_PIN;
            temp=road_counters[2];
	    road_counters[2] = 0;
            return temp;
        case 3:
            LPC_GPIO1->FIOPIN = (LPC_GPIO1->FIOPIN & ~(LED_1_PIN | LED_2_PIN | LED_3_PIN | LED_4_PIN)) | LED_4_PIN;
            temp=road_counters[3];
	    road_counters[3] = 0;
            return temp;
        default: return temp;
    }
}

void display(int num)
{
  LPC_GPIO0->FIOCLR=0xFFF<<15;
  LPC_GPIO0->FIOSET=num<<23
  LPC_GPIO0->FIOSET=array_dec[(road_counters[num])%10]<<15; 
}
int main() {
    // Configure P0.4 to P0.11 as GPIO
    LPC_PINCON->PINSEL0 &= ~(0xC0FFFF << 8);    // Clear bits 8-23 and 30-31bits
    LPC_PINCON->PINSEL0 |=  (0x000000 << 8);    // Set bits 8-23&31-30 to 00 (GPIO mode)
	
    LPC_PINCON->PINSEL1 &= ~(0x3FFFFF); //Clear bits 0 to 21
	LPC_PINCON->PINSEL1 |=  (0x000000); // set bits as 00(GPIO mode)

    LPC_PINCON->PINSEL3 &= ~(0xFF << 14);
    LPC_PINCON->PINSEL3 |=  (0x00 << 14);
    
    // Set P0.4 to P0.7 as inputs for road sensors
    LPC_GPIO0->FIODIR &= ~(ROAD_1_PIN | ROAD_2_PIN | ROAD_3_PIN | ROAD_4_PIN);
    //Pins for seven_segment;
    LPC_GPIO0->FIODIR |=0xFFF<<15;
    
    // Set P1.23 to P1.26 as outputs for LEDs
    LPC_GPIO1->FIODIR |= (LED_1_PIN | LED_2_PIN | LED_3_PIN | LED_4_PIN);


    while (1) {
			if(temp==0)
			{
				temp=5;
			}
       for(i=0;i<temp*100000000;i++)
      { // Check for postive edge transition of each road sensor
				
        if ((LPC_GPIO0->FIOPIN & ROAD_1_PIN)==1<<4 && (FIOPREV & ROAD_1_PIN)==0) {
						
            road_counters[0]++; // Increment road 1 counter on negative edge
            display(0);
        }
        if ((LPC_GPIO0->FIOPIN & ROAD_2_PIN)==1<<5 && (FIOPREV & ROAD_2_PIN)==0) {
					  
            road_counters[1]++; // Increment road 2 counter on negative edge
            display(1);
        }
        if ((LPC_GPIO0->FIOPIN & ROAD_3_PIN)==1<<6 && (FIOPREV & ROAD_3_PIN)==0) {
					  
            road_counters[2]++; // Increment road 3 counter on negative edge
            display(2);
        }
        if ((LPC_GPIO0->FIOPIN & ROAD_4_PIN)==1<<7 && (FIOPREV & ROAD_4_PIN)==0) {
					  
            road_counters[3]++; // Increment road 4 counter on negative edge
            display(3);
        }
         FIOPREV=LPC_GPIO0->FIOPIN;
      }
        // Update LEDs based on the maximum counter
       
	  temp=update_leds();
        
    }
}

