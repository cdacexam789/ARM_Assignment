#include <vega/hal.h>
#include <vega/aries.h>
#include <stdio.h>
#include <string.h>

// UART buffer to store data from PuTTY
char buf[80] = {0};

// Variable to track LED state (0: Red, 1: Blue, 2: Green)
volatile int ledState = 0;

// UART Receive Complete Callback
void UART_RxCpltCallback(UART_Handle_t *huart) {
    char msg[100];
    sprintf(msg, "\r\nReceived: %s", buf);
    UART_Transmit(&huart0, msg, strlen(msg)); // Send received data back to PuTTY

    // Process UART commands to control LEDs
    if (strcmp(buf, "led red toggle\n") == 0) {
        GPIO_TogglePin(GPIOB, RGB_RED);
        UART_Transmit(&huart0, "\r\nRed LED toggled\n", 19);
    } else if (strcmp(buf, "led blue toggle\n") == 0) {
        GPIO_TogglePin(GPIOB, RGB_BLUE);
        UART_Transmit(&huart0, "\r\nBlue LED toggled\n", 20);
    } else if (strcmp(buf, "led green toggle\n") == 0) {
        GPIO_TogglePin(GPIOB, RGB_GREEN);
        UART_Transmit(&huart0, "\r\nGreen LED toggled\n", 21);
    } else {
        UART_Transmit(&huart0, "\r\nCommand not understood\n", 26);
    }
}

// GPIO External Interrupt Callback
void GPIO_EXTICallback(uint16_t pin) {
    if (pin == 3) { // Button press detected
        delayms(500); // Debounce delay

        // Toggle LED state based on current state
        switch (ledState) {
            case 0: // Toggle Red LED
                GPIO_TogglePin(GPIOB, RGB_RED);
                UART_Transmit(&huart0, "\r\nRed LED toggled by button\n", 27);
                break;

            case 1: // Toggle Blue LED
                GPIO_TogglePin(GPIOB, RGB_BLUE);
                UART_Transmit(&huart0, "\r\nBlue LED toggled by button\n", 28);
                break;

            case 2: // Toggle Green LED
                GPIO_TogglePin(GPIOB, RGB_GREEN);
                UART_Transmit(&huart0, "\r\nGreen LED toggled by button\n", 29);
                break;
        }
        
        // Move to the next LED state for the next press
        ledState = (ledState + 1) % 3;
    }
}

int main() {
    // Initialize UART
    UART_Init(&huart0);

    // Initialize LEDs
    GPIO_Init(GPIOB, RGB_RED, GPIO_OUT);
    GPIO_Init(GPIOB, RGB_BLUE, GPIO_OUT);
    GPIO_Init(GPIOB, RGB_GREEN, GPIO_OUT);
    RGB_RED_RST;
    RGB_BLUE_RST;
    RGB_GREEN_RST;

    // Initialize Button for external interrupt
    GPIO_Init(GPIOA, 3, GPIO_IN);
    GPIO_IT_Enable(3); // Enable external interrupt on button pin

    while (1) {
        UART_Receive_IT(&huart0, buf, 80); // Continuously receive data from PuTTY
        // Infinite loop - all action happens in the interrupt callbacks
    }

    return 0;
}

