#include "Arduino.h"

#include "Buhlmann.h"

Buhlmann::Buhlmann(float minimumAircraftCabinPressure, float waterVapourPressureCorrection) {
	_waterVapourPressureCorrection = waterVapourPressureCorrection;

	//Coefficients of the ZH-L16C-GF algorithm
	_halfTimesNitrogen[0] = 4.0;
	_halfTimesNitrogen[1] = 8.0;
	_halfTimesNitrogen[2] = 12.5;
	_halfTimesNitrogen[3] = 18.5;
	_halfTimesNitrogen[4] = 27.0;
	_halfTimesNitrogen[5] = 38.3;
	_halfTimesNitrogen[6] = 54.3;
	_halfTimesNitrogen[7] = 77.0;
	_halfTimesNitrogen[8] = 109.0;
	_halfTimesNitrogen[9] = 146.0;
	_halfTimesNitrogen[10] = 187.0;
	_halfTimesNitrogen[11] = 239.0;
	_halfTimesNitrogen[12] = 305.0;
	_halfTimesNitrogen[13] = 390.0;
	_halfTimesNitrogen[14] = 498.0;
	_halfTimesNitrogen[15] = 635.0;

	_aValuesNitrogen[0] = 1.2599;
	_aValuesNitrogen[1] = 1.0000;
	_aValuesNitrogen[2] = 0.8618;
	_aValuesNitrogen[3] = 0.7562;
	_aValuesNitrogen[4] = 0.6200;
	_aValuesNitrogen[5] = 0.5043;
	_aValuesNitrogen[6] = 0.4410;
	_aValuesNitrogen[7] = 0.4000;
	_aValuesNitrogen[8] = 0.3750;
	_aValuesNitrogen[9] = 0.3500;
	_aValuesNitrogen[10] = 0.3295;
	_aValuesNitrogen[11] = 0.3065;
	_aValuesNitrogen[12] = 0.2835;
	_aValuesNitrogen[13] = 0.2610;
	_aValuesNitrogen[14] = 0.2480;
	_aValuesNitrogen[15] = 0.2327;

	_bValuesNitrogen[0] = 0.5050;
	_bValuesNitrogen[1] = 0.6514;
	_bValuesNitrogen[2] = 0.7222;
	_bValuesNitrogen[3] = 0.7825;
	_bValuesNitrogen[4] = 0.8126;
	_bValuesNitrogen[5] = 0.8434;
	_bValuesNitrogen[6] = 0.8693;
	_bValuesNitrogen[7] = 0.8910;
	_bValuesNitrogen[8] = 0.9092;
	_bValuesNitrogen[9] = 0.9222;
	_bValuesNitrogen[10] = 0.9319;
	_bValuesNitrogen[11] = 0.9403;
	_bValuesNitrogen[12] = 0.9477;
	_bValuesNitrogen[13] = 0.9544;
	_bValuesNitrogen[14] = 0.9602;
	_bValuesNitrogen[15] = 0.9653;

	_currentDiveDuration = 0;
	_currentDepth = 0;
	_maxDepth = 0;
}

/////////////////////
// Utility methods //
/////////////////////

float Buhlmann::calculateDepthFromPressure(float pressure) {
	float depthInMeter = 0;
	if (pressure > _seaLevelAtmosphericPressure) {
		depthInMeter = (pressure - _seaLevelAtmosphericPressure) / (9.80665 * 10.25);
	}
	return depthInMeter;
}

float Buhlmann::calculateHydrostaticPressureFromDepth(float depth) {
	return _seaLevelAtmosphericPressure + (9.80665 * 10.25 * depth * 100);
}

float Buhlmann::maxSearch(float array[], int size) {
	float max = array[0];
	for (int i=1; i < size; i++) {
		if (max < array[i]) {
			max = array[i];
		}
	}
	return max;
}

int Buhlmann::minSearch(int array[], int size) {
	int min = array[0];
	for (int i=1; i<size; i++) {
		if (min > array[i]) {
			min = array[i];
		}
	}
	return min;
}

/////////////////////////
// Getters and setters //
/////////////////////////

float Buhlmann::getCompartmentHalfTimeInSeconds(int compartmentIndex) {
	return _halfTimesNitrogen[compartmentIndex] * 60;
}

float Buhlmann::getCompartmentPartialPressure(int compartmentIndex) {
	return _compartmentCurrentPartialPressures[compartmentIndex];
}

void Buhlmann::setCompartmentPartialPressure(int compartmentIndex, float partialPressure) {
	_compartmentCurrentPartialPressures[compartmentIndex] = partialPressure;
}

void Buhlmann::setSeaLevelAtmosphericPressure(float seaLevelAtmosphericPressure) {
	_seaLevelAtmosphericPressure = seaLevelAtmosphericPressure;
}

void Buhlmann::setNitrogenRateInGas(float nitrogenRateInGas) {
	_nitrogenRateInGas = nitrogenRateInGas;
}

/////////////////////////////////
// Calculation related methods //
/////////////////////////////////

float Buhlmann::calculateNitrogenPartialPressureInLung(float currentPressure) {
	return _nitrogenRateInGas * (currentPressure - _waterVapourPressureCorrection);
}

float Buhlmann::calculateCompartmentInertGasPartialPressure(long timeInSeconds, float halfTimeInSeconds, float currentInertGasPartialPressureInCompartment, float currentInertGasPartialPressureInLung) {
	return currentInertGasPartialPressureInCompartment + (currentInertGasPartialPressureInLung - currentInertGasPartialPressureInCompartment) * (1 - pow(2, (-timeInSeconds/halfTimeInSeconds)));
}

float Buhlmann::getAscendToPartialPressureForCompartment(int compartmentIndex, float compartmentPartialPressure) {
	return (compartmentPartialPressure * _bValuesNitrogen[compartmentIndex]) - (1000 * _aValuesNitrogen[compartmentIndex] * _bValuesNitrogen[compartmentIndex]);
}

bool Buhlmann::isDecoNeeded(float ascendToPartialPressure) {
	return ascendToPartialPressure > _seaLevelAtmosphericPressure;
}

int Buhlmann::getMinutesNeededTillDeco(int compartmentIndex, float currentPressure) {
	int minutes = 0;
	float compartmentPartialPressure;
	bool decoNeeded = false;

	while (!decoNeeded && minutes < 99) {
		minutes = minutes + 1;
		compartmentPartialPressure = calculateCompartmentInertGasPartialPressure(
				minutes * 60,
				getCompartmentHalfTimeInSeconds(compartmentIndex),
				getCompartmentPartialPressure(compartmentIndex),
				calculateNitrogenPartialPressureInLung(currentPressure));
		decoNeeded = isDecoNeeded(getAscendToPartialPressureForCompartment(compartmentIndex, compartmentPartialPressure));
	}
	return minutes;
}

int Buhlmann::calculateMinutesRequiredToReachCertainPressure(float targetPressure) {
	float ascentCeilingArray[COMPARTMENT_COUNT];
	int requiredMinutes = 0;
	bool stopTimeCalculation = false;

	while (!stopTimeCalculation) {
		requiredMinutes = requiredMinutes +1;

		for (int i=0; i < COMPARTMENT_COUNT; i++) {
			//Calculate and store the partial pressure for the current compartment
			//Calculate the ascendTo partial pressure for the compartment and store it in the ascent ceiling array
			ascentCeilingArray[i] = getAscendToPartialPressureForCompartment(i,
					calculateCompartmentInertGasPartialPressure(
						requiredMinutes * 60,
						getCompartmentHalfTimeInSeconds(i),
						getCompartmentPartialPressure(i),
						calculateNitrogenPartialPressureInLung(targetPressure)));
		}

		//All Ascent Ceiling has to be lower than the Aircraft Cabin Pressure
		float maximum = maxSearch(ascentCeilingArray, COMPARTMENT_COUNT);
		if (maximum < targetPressure) {
			stopTimeCalculation = true;
		}
	}
	return requiredMinutes;
}

int Buhlmann::calculateAscentRate(float timeSpentInLevelInSeconds, float previousDepthInMeter, float currentDepthInMeter) {
	if (previousDepthInMeter > currentDepthInMeter) {

		float ascendInMeter = previousDepthInMeter - currentDepthInMeter;
		float rate = ascendInMeter / timeSpentInLevelInSeconds;

		float threshold;
		if (currentDepthInMeter <= 10) {
			//We are above 10 meters
			threshold = 0.1666666666666667;
		} else if (20 > currentDepthInMeter && currentDepthInMeter > 10) {
			//We are between 10 and 20 meters
			threshold = 0.2;
		} else {
			//We are below 18 meters
			threshold = 0.3;
		}

		if (rate <= (0.6 * threshold)) {
			return ASCEND_OK;
		} else if ((0.6 * threshold) < threshold && threshold <= (0.8 * threshold)) {
			return ASCEND_SLOW;
		} else if ((0.8 * threshold) < threshold && threshold <= (0.95 * threshold)) {
			return ASCEND_NORMAL;
		} else if ((0.95 * threshold) < threshold && threshold <= (1.05 * threshold)) {
			return ASCEND_ATTENTION;
		} else if ((1.05 * threshold) < threshold && threshold <= (1.2 * threshold)) {
			return ASCEND_CRITICAL;
		} else {
			return ASCEND_DANGER;
		}
	} else {
		return DESCEND;
	}
}

///////////////////
// Dive Progress //
///////////////////

DiveResult* Buhlmann::surfaceInterval(long surfaceIntervalInMinutes, DiveResult* previousDiveResult) {

	//Set the given compartment pressure data
    for (byte j = 0; j < COMPARTMENT_COUNT; j++) {
    	_compartmentCurrentPartialPressures[j] = previousDiveResult->compartmentPartialPressures[j];
    }

    float initialPartialPressureWithoutDive = calculateNitrogenPartialPressureInLung(_seaLevelAtmosphericPressure);

	//For each compartment calculate the new partial pressures after spent X amount of time on the surface
	for (int i=0; i < COMPARTMENT_COUNT; i++) {
		setCompartmentPartialPressure(i, calculateCompartmentInertGasPartialPressure(
				surfaceIntervalInMinutes * 60,
				getCompartmentHalfTimeInSeconds(i),
				getCompartmentPartialPressure(i),
				initialPartialPressureWithoutDive));
	}

	//If the partial pressure of the compartment becomes less than the no-dive initial partial pressure - set it as the initial partial pressure
	for (int i=0; i < COMPARTMENT_COUNT; i++) {
		if (getCompartmentPartialPressure(i) < initialPartialPressureWithoutDive) {
			setCompartmentPartialPressure(i, initialPartialPressureWithoutDive);
		}
	}

	DiveResult* diveResult = new DiveResult;
    for (byte j = 0; j < COMPARTMENT_COUNT; j++) {
    	diveResult->compartmentPartialPressures[j] = _compartmentCurrentPartialPressures[j];
    }
    diveResult->maxDepthInMeters = 0;
	diveResult->durationInSeconds = surfaceIntervalInMinutes * 60;
	if (previousDiveResult->noFlyTimeInMinutes > surfaceIntervalInMinutes) {
		diveResult->noFlyTimeInMinutes = previousDiveResult->noFlyTimeInMinutes - surfaceIntervalInMinutes;
	} else {
		diveResult->noFlyTimeInMinutes = 0;
	}
	//Copy over surface time and deco information
	diveResult->wasDecoDive = previousDiveResult->wasDecoDive;
	diveResult->previousDiveDateTimestamp = previousDiveResult->previousDiveDateTimestamp;
	return diveResult;
}

DiveResult* Buhlmann::initializeCompartments() {

	Serial.println("Compartment initialization - START");

    DiveResult* diveResult = new DiveResult;
    diveResult->maxDepthInMeters = 0;
    diveResult->durationInSeconds = 0;
    diveResult->noFlyTimeInMinutes = 0;

    //Initialize the compartments with the initial partial pressure value - it assumes that the inert gases were cleared off from the compartments
    for (int i=0; i < COMPARTMENT_COUNT; i++) {
        diveResult->compartmentPartialPressures[i] = calculateNitrogenPartialPressureInLung(_seaLevelAtmosphericPressure);
    }

    Serial.println(F("Compartment initialization - DONE"));
    Serial.println("");

    return diveResult;
}

void Buhlmann::startDive(DiveResult* previousDiveResult, unsigned long diveStartTimestamp) {
	_previousDiveResult = previousDiveResult;
	_diveStartTimestamp = diveStartTimestamp;

	//Set dive duration calculation to zero
    _currentDiveDuration = 0;

    //Set the deco indicator to false
    _wasDecoDive = false;

    //Reset maximum depth
    _maxDepth = 0;

    //Initialize the compartments based on the previous compartment partial pressure values
    for (byte j = 0; j < COMPARTMENT_COUNT; j++) {
    	_compartmentCurrentPartialPressures[j] = previousDiveResult->compartmentPartialPressures[j];
    }
}

DiveInfo Buhlmann::progressDive(float currentPressure, unsigned int duration) {

	DiveInfo diveInfo;

    unsigned int timeSpentInLevel = duration - _currentDiveDuration; //In seconds
    if (timeSpentInLevel > 0) {

        //Calculate ascent - descent depth here
        int ascendRate = calculateAscentRate(timeSpentInLevel, _currentDepth, calculateDepthFromPressure(currentPressure));

        _currentDiveDuration = _currentDiveDuration + timeSpentInLevel;
        _currentDepth = calculateDepthFromPressure(currentPressure);

        //Calculate the maximum depth
        if (_maxDepth < _currentDepth) {
            _maxDepth = _currentDepth;
        }

        diveInfo.ascendRate = ascendRate;

        bool isDecoNeededArray[COMPARTMENT_COUNT];
        int minutesNeededToDecoArray[COMPARTMENT_COUNT];
        float ascentCeilingArray[COMPARTMENT_COUNT];

        for (int i=0; i < COMPARTMENT_COUNT; i++) {
            //Calculate and store the partial pressure for the current compartment
            setCompartmentPartialPressure(i, calculateCompartmentInertGasPartialPressure(
                    timeSpentInLevel,
                    getCompartmentHalfTimeInSeconds(i),
                    getCompartmentPartialPressure(i),
                    calculateNitrogenPartialPressureInLung(currentPressure)));

            //Calculate the ascendTo partial pressure for the compartment and store it in the ascent ceiling array
            ascentCeilingArray[i] = getAscendToPartialPressureForCompartment(i, getCompartmentPartialPressure(i));

            isDecoNeededArray[i] = isDecoNeeded(ascentCeilingArray[i]);
            if (!isDecoNeededArray[i]) {
                //Calculate minutes can be spent in the current depth
                minutesNeededToDecoArray[i] = getMinutesNeededTillDeco(i, currentPressure);

                Serial.print(i);
                Serial.print(F(" - ppN2: "));
                Serial.print(getCompartmentPartialPressure(i), 2);
                Serial.print(F(" mb | ac ppN2: "));
                Serial.print(ascentCeilingArray[i], 2);
                Serial.print(F(" mb | no deco: "));
                Serial.println(minutesNeededToDecoArray[i]);
            } else {
                minutesNeededToDecoArray[i] = 0;
                //Calculate first deco stop
                float firstDecoStop = calculateDepthFromPressure(ascentCeilingArray[i]);
                Serial.print(i);
                Serial.print(F(" - ppN2: "));
                Serial.print(getCompartmentPartialPressure(i), 2);
                Serial.print(F(" mb | ac ppN2: "));
                Serial.print(ascentCeilingArray[i], 2);
                Serial.print(F(" mb | DECO: "));
                Serial.print(firstDecoStop);
                Serial.print(F(" m "));
                Serial.print(ascentCeilingArray[i]);
                Serial.println(F(" mb"));
            }
        }

        //Calculate if DECO is needed for all compartments
        bool decoNeeded = false;
        for (int i=0; i < COMPARTMENT_COUNT; i++) {
            if (isDecoNeededArray[i]) {
                decoNeeded = true;
                break;
            }
        }

        diveInfo.decoNeeded = decoNeeded;
        if (!decoNeeded) {
        	int minutesToDeco = minSearch(minutesNeededToDecoArray, COMPARTMENT_COUNT);
        	diveInfo.minutesToDeco = minutesToDeco;
            Serial.print(F("NO DECO | Min to deco: "));
            Serial.println(minutesToDeco);
        } else {
        	//The dive became a DECO dive
        	_wasDecoDive = true;

            //Find the deepest ascent ceiling (the biggest pressure)
            float ascentCeiling = maxSearch(ascentCeilingArray, COMPARTMENT_COUNT);

            //Calculate the first deco stop in 3 meter ranges
            float decoStop = calculateDepthFromPressure(ascentCeiling);
            int newDecoStop =  (((int)decoStop/3)+1)*3;

            //Calculate the duration of the decompression stop

            //We have to ascent to the ascentCeiling and calculate how many times we have to spend there

            float nextDecoPressure;
            int nextDecoStopDepth = newDecoStop - 3;
            if (nextDecoStopDepth <= 0) {
                nextDecoPressure = _seaLevelAtmosphericPressure;
            } else {
                nextDecoPressure = calculateHydrostaticPressureFromDepth(nextDecoStopDepth) / 100;
            }
            int decoStopDurationInMinutes = calculateMinutesRequiredToReachCertainPressure(nextDecoPressure);

            diveInfo.decoStopInMeters = newDecoStop;
            diveInfo.decoStopDurationInMinutes = decoStopDurationInMinutes;

            Serial.print(F("DECO "));
            Serial.print(newDecoStop);
            Serial.print(F(" m "));
            Serial.print(decoStopDurationInMinutes);
            Serial.print(F(" min"));
        }
        Serial.println("");
    }
    return diveInfo;
}

DiveResult* Buhlmann::stopDive(unsigned long diveStopTimestamp) {

	Serial.println(F("Buhmann - Stop Dive"));

	long desaturationTimeInMinutes = calculateDesaturationTime(1.02);
    long noFlyTimeInMinutes = calculateNoFlyTime(desaturationTimeInMinutes);

    DiveResult* diveResult = new DiveResult;
    for (byte j = 0; j < COMPARTMENT_COUNT; j++) {
    	diveResult->compartmentPartialPressures[j] = _compartmentCurrentPartialPressures[j];
    }
    diveResult->maxDepthInMeters = _maxDepth;
    diveResult->durationInSeconds = _currentDiveDuration;
    diveResult->noFlyTimeInMinutes = noFlyTimeInMinutes;
    diveResult->wasDecoDive = _wasDecoDive;
    diveResult->previousDiveDateTimestamp = diveStopTimestamp;

    return diveResult;
}

/**
 * Calculates the maximum time until each compartment partial pressure returns back to the initial value adjusted by the given percentage.
 */
long Buhlmann::calculateDesaturationTime(float limitPercentage) {

	float initialPartialPressure = calculateNitrogenPartialPressureInLung(_seaLevelAtmosphericPressure);
	float desatPartialPressureLimit = initialPartialPressure * limitPercentage;

	long desaturationTimeInMinutes = 1;
	for (int i=0; i < COMPARTMENT_COUNT; i++) {
		long timeInMinutes = 1;

		float currentPartialPressure = getCompartmentPartialPressure(i);
		Serial.print(i);
		Serial.print(F(" compartment partial pressure: "));
		Serial.println(currentPartialPressure);

		while (desatPartialPressureLimit < calculateCompartmentInertGasPartialPressure(timeInMinutes * 60,
				getCompartmentHalfTimeInSeconds(i), currentPartialPressure, initialPartialPressure)) {
			timeInMinutes++;
		}

		Serial.print(F(">> desaturation time: "));
		Serial.println(timeInMinutes);

		//Maximum search to find the maximum desaturation time
		if (desaturationTimeInMinutes < timeInMinutes) {
			desaturationTimeInMinutes = timeInMinutes;
		}
	}

	Serial.print(F("Desaturation time: "));
	Serial.println(desaturationTimeInMinutes);

	return desaturationTimeInMinutes;
}

/**
 * Definitions:
 * ************
 * Repetitive dive: The end of the previous dive was within 24 hours of the start of the current dive.
 * No-deco dive: There was no decompression stop calculated during the dive.
 *
 * Calculation rules:
 * ******************
 *
 * Non-repetitive AND No-deco dive:
 *     Desaturation time >  12 hours => No Fly time = Desaturation time
 *     Desaturation time <= 12 hours => No Fly time = 12 hours
 *
 * Repetitive OR Decompression dive:
 *     Desaturation time >  24 hours => No Fly time = Desaturation time
 *     Desaturation time <= 24 hours => No Fly time = 24 hours
 *
 * @param desaturationTimeInMinutes
 * @return The calculated No Fly time
 */
long Buhlmann::calculateNoFlyTime(long desaturationTimeInMinutes) {

	bool isRepetitiveDive = false;
	bool isDecoDive = false;

	//Test if the previous dive ended within 24 hours(1 day) = 24 × 60 × 60 = 86400 seconds
	if ((_diveStartTimestamp - _previousDiveResult->previousDiveDateTimestamp) < 86400) {
		isRepetitiveDive = true;
	}

	Serial.print("Was DECO dive? ");
	Serial.println(_wasDecoDive);
	Serial.print("Previous dive was DECO dive? ");
	Serial.println(_previousDiveResult->wasDecoDive);

	//Test if this or the previous dive was a decompression dive
	if (_wasDecoDive || _previousDiveResult->wasDecoDive) {
		isDecoDive = true;
	}
	long noFlyTimeInMinutes = 1;
	if (isRepetitiveDive || isDecoDive) {
		if (desaturationTimeInMinutes > 1440) { //24 hours
			noFlyTimeInMinutes = desaturationTimeInMinutes;
		} else {
			noFlyTimeInMinutes = 1440;
		}
	} else {
		if (desaturationTimeInMinutes > 720) { //12 hours
			noFlyTimeInMinutes = desaturationTimeInMinutes;
		} else {
			noFlyTimeInMinutes = 720;
		}
	}

	Serial.print(F("No Fly time: "));
	Serial.print(noFlyTimeInMinutes);
	Serial.print(F(", Is deco? "));
	Serial.print(isDecoDive);
	Serial.print(F(", Is repetitive? "));
	Serial.println(isRepetitiveDive);

	return noFlyTimeInMinutes;
}
