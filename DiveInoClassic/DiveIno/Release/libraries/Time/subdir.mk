################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
C:\Users\Kornel_Schrenk\Documents\Arduino\libraries\Time\DateStrings.cpp \
C:\Users\Kornel_Schrenk\Documents\Arduino\libraries\Time\Time.cpp 

LINK_OBJ += \
.\libraries\Time\DateStrings.cpp.o \
.\libraries\Time\Time.cpp.o 

CPP_DEPS += \
.\libraries\Time\DateStrings.cpp.d \
.\libraries\Time\Time.cpp.d 


# Each subdirectory must supply rules for building sources it contributes
libraries\Time\DateStrings.cpp.o: C:\Users\Kornel_Schrenk\Documents\Arduino\libraries\Time\DateStrings.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"C:\Users\Kornel_Schrenk\Tools\sloeber\/arduinoPlugin/packages/arduino/tools/avr-gcc/4.9.2-atmel3.5.4-arduino2/bin/avr-g++" -c -g -Os -Wall -Wextra -std=gnu++11 -fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -flto -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=10802 -DARDUINO_AVR_MEGA2560 -DARDUINO_ARCH_AVR   -I"C:\Users\Kornel_Schrenk\Tools\sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.6.21\cores\arduino" -I"C:\Users\Kornel_Schrenk\Tools\sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.6.21\variants\mega" -I"C:\Users\Kornel_Schrenk\Documents\Arduino\libraries\ArduinoJson" -I"C:\Users\Kornel_Schrenk\Documents\Arduino\libraries\DS1307RTC" -I"C:\Users\Kornel_Schrenk\Documents\Arduino\libraries\SdFat" -I"C:\Users\Kornel_Schrenk\Documents\Arduino\libraries\SdFat\src" -I"C:\Users\Kornel_Schrenk\Tools\sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.6.21\libraries\SPI" -I"C:\Users\Kornel_Schrenk\Documents\Arduino\libraries\UTFT_SdRaw" -I"C:\Users\Kornel_Schrenk\Tools\sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.6.21\libraries\Wire" -I"C:\Users\Kornel_Schrenk\Documents\Arduino\libraries\UTFT" -I"C:\Users\Kornel_Schrenk\Documents\Arduino\libraries\MS5803_14" -I"C:\Users\Kornel_Schrenk\Documents\Arduino\libraries\Time" -I"C:\Users\Kornel_Schrenk\Documents\Arduino\libraries\Adafruit_BluefruitLE_nRF51" -I"C:\Users\Kornel_Schrenk\Documents\Arduino\libraries\Adafruit_BluefruitLE_nRF51\utility" -I"C:\Users\Kornel_Schrenk\Documents\Arduino\libraries\Arduino-IRremote-Due" -I"C:\Users\Kornel_Schrenk\Documents\Arduino\libraries\MAX17043" -I"C:\Users\Kornel_Schrenk\Documents\Arduino\libraries\TimerFreeTone" -I"C:\Users\Kornel_Schrenk\Tools\sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.6.21\libraries\SoftwareSerial" -I"C:\Users\Kornel_Schrenk\Tools\sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.6.21\libraries\SoftwareSerial\src" -I"C:\Users\Kornel_Schrenk\Tools\sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.6.21\libraries\SPI\src" -I"C:\Users\Kornel_Schrenk\Tools\sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.6.21\libraries\Wire\src" -I"C:\Users\Kornel_Schrenk\Documents\Arduino\libraries\ArduinoJson\src" -I"C:\Users\Kornel_Schrenk\Documents\Arduino\libraries\UTFT_SdRaw\src" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"
	@echo 'Finished building: $<'
	@echo ' '

libraries\Time\Time.cpp.o: C:\Users\Kornel_Schrenk\Documents\Arduino\libraries\Time\Time.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"C:\Users\Kornel_Schrenk\Tools\sloeber\/arduinoPlugin/packages/arduino/tools/avr-gcc/4.9.2-atmel3.5.4-arduino2/bin/avr-g++" -c -g -Os -Wall -Wextra -std=gnu++11 -fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -flto -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=10802 -DARDUINO_AVR_MEGA2560 -DARDUINO_ARCH_AVR   -I"C:\Users\Kornel_Schrenk\Tools\sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.6.21\cores\arduino" -I"C:\Users\Kornel_Schrenk\Tools\sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.6.21\variants\mega" -I"C:\Users\Kornel_Schrenk\Documents\Arduino\libraries\ArduinoJson" -I"C:\Users\Kornel_Schrenk\Documents\Arduino\libraries\DS1307RTC" -I"C:\Users\Kornel_Schrenk\Documents\Arduino\libraries\SdFat" -I"C:\Users\Kornel_Schrenk\Documents\Arduino\libraries\SdFat\src" -I"C:\Users\Kornel_Schrenk\Tools\sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.6.21\libraries\SPI" -I"C:\Users\Kornel_Schrenk\Documents\Arduino\libraries\UTFT_SdRaw" -I"C:\Users\Kornel_Schrenk\Tools\sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.6.21\libraries\Wire" -I"C:\Users\Kornel_Schrenk\Documents\Arduino\libraries\UTFT" -I"C:\Users\Kornel_Schrenk\Documents\Arduino\libraries\MS5803_14" -I"C:\Users\Kornel_Schrenk\Documents\Arduino\libraries\Time" -I"C:\Users\Kornel_Schrenk\Documents\Arduino\libraries\Adafruit_BluefruitLE_nRF51" -I"C:\Users\Kornel_Schrenk\Documents\Arduino\libraries\Adafruit_BluefruitLE_nRF51\utility" -I"C:\Users\Kornel_Schrenk\Documents\Arduino\libraries\Arduino-IRremote-Due" -I"C:\Users\Kornel_Schrenk\Documents\Arduino\libraries\MAX17043" -I"C:\Users\Kornel_Schrenk\Documents\Arduino\libraries\TimerFreeTone" -I"C:\Users\Kornel_Schrenk\Tools\sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.6.21\libraries\SoftwareSerial" -I"C:\Users\Kornel_Schrenk\Tools\sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.6.21\libraries\SoftwareSerial\src" -I"C:\Users\Kornel_Schrenk\Tools\sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.6.21\libraries\SPI\src" -I"C:\Users\Kornel_Schrenk\Tools\sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.6.21\libraries\Wire\src" -I"C:\Users\Kornel_Schrenk\Documents\Arduino\libraries\ArduinoJson\src" -I"C:\Users\Kornel_Schrenk\Documents\Arduino\libraries\UTFT_SdRaw\src" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"
	@echo 'Finished building: $<'
	@echo ' '


