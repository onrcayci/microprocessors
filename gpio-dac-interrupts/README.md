enable_mathjax = true
# GPIO and DAC Interrupts

General Purpose I/O (GPIO) and Digital to Analog Converter (DAC) practice using interrupt signals and timers, and Direct Memory Access (DMA). 

## Toggle LED using GPIO Interrupts

To toggle the LED, I used the interrupts from the GPIO button available on the STM32L475 discovery kit. I am detecting both the rising and the falling edges of the button to signal and interrupt, so that the LED is on when you press and hold the button. Otherwise, the LED is off.

## DAC using Interrupts

I am using a timer with a frequency of roughly 96 kHz to generate a sine wave using the DAC of the STM32L475 discovery kit. In order to make this work properly with the interrupts, I created an array that stores the sample from a sine wave over 2pi and a variable that keeps tracks of the indx of the next sample in the array.

## DAC using DMA

This is a lot simpler compared to DAC with Interrupts. After allowing external triggers for the DAC on STMCUBE32MX, I added a DMA request. I chose circular to loop over the array of samples from the sine wave and I chose the length of my variables as bytes since my array holds uint8_t variables.

## Putting it all together

The last part is about combining everything together. For this, I have created 3 sine waves with differents frequencies. I was able to change their frequencies by changing the number of samples I am using instead of changing the count of the timer. Every time the button is pressed, I am changing the sine wave to a different one using button interrupts and DMA.
