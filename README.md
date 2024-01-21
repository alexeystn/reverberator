### Digital Reverberator

|    MCU    | ADC | DAC |
|   :---:   |:---:|:---:|
| **STM32F411CEU6** <br> *Black Pill* | **PCM1808** <br> *CJMCU-1808* | **PCM5102A** <br> *GY-PCM5102* |
| B5 (SD)   | OUT |     |
| B3 (CK)   | BCK |     |
| A15 (WS)  | LRC |     |
| B10 (MCK) | SCK |     |
| B12 (WS)  |     | LCK |
| B13 (CK)  |     | BCK |
| B15 (SD)  |     | DIN |
| GND | FMY-MD1-MD0 |   | 

Power distribution diagram
|Input  |Converter|Consumer| 
| :---: |  :---:  |  :---: |
| 9V |     -    |   Mic pre-amp  |
|    | LDO 5.0V | MCU -> 3.3V ADC digital <br> Display | 
|    | LDO 5.0V | ADC analog | 
|    | LDO 3.3V | DAC | 
