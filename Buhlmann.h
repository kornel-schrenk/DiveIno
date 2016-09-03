#ifndef DiveDeco_h
#define DiveDeco_h

#include "Arduino.h"

#define COMPARTMENT_COUNT 16

enum ascendRates
{
	DESCEND = -1,
	ASCEND_OK = 0,
	ASCEND_SLOW = 1,
	ASCEND_NORMAL = 2,
	ASCEND_ATTENTION = 3,
	ASCEND_CRITICAL = 4,
	ASCEND_DANGER = 5,
};

typedef struct DiveData {
	float pressure; //The current pressure in milliBars
	unsigned int duration; //The duration of the dive in seconds
};

typedef struct DiveResult {
	float compartmentPartialPressures[COMPARTMENT_COUNT];
	float maxDepthInMeters;
	unsigned int durationInSeconds;
	int noFlyTimeInMinutes;
};

typedef struct DiveInfo {
	int ascendRate;
	bool decoNeeded;
	int minutesToDeco;
	int decoStopInMeters;
	int decoStopDurationInMinutes;
};

class Buhlmann {
public:
	Buhlmann(float minimumAircraftCabinPressure, float waterVapourPressureCorrection);

	void setSeaLevelAtmosphericPressure(float seaLevelAtmosphericPressure);
	void setNitrogenRateInGas(float nitrogenRateInGas);

	float calculateDepthFromPressure(float pressure);
	float calculateHydrostaticPressureFromDepth(float depth);

    void copyArray(float destination[], float source[]);
	float maxSearch(float array[], int size);
	float minSearch(float array[], int size);
	int maxSearch(int array[], int size);
	int minSearch(int array[], int size);

	DiveResult* surfaceInterval(long surfaceIntervalInMinutes, DiveResult* previousDiveResult);
    DiveResult* initializeCompartments();
    void startDive(DiveResult* previousDiveResult);
    DiveInfo progressDive(DiveData* diveData);
    DiveResult* stopDive();

    float calculateNitrogenPartialPressureInLung(float currentPressure);

private:
	float _seaLevelAtmosphericPressure;
	float _nitrogenRateInGas;
	float _minimumAircraftCabinPressure;
	float _waterVapourPressureCorrection;

	float _halfTimesNitrogen[COMPARTMENT_COUNT];
	float _aValuesNitrogen[COMPARTMENT_COUNT];
	float _bValuesNitrogen[COMPARTMENT_COUNT];

	float _compartmentCurrentPartialPressures[COMPARTMENT_COUNT];
	unsigned int _currentDiveDuration; //In seconds
	float _currentDepth;
	float _maxDepth;

	float getCompartmentHalfTimeInSeconds(int compartmentIndex);
	float getCompartmentPartialPressure(int compartmentIndex);
	void setCompartmentPartialPressure(int compartmentIndex, float partialPressure);

	float calculateCompartmentInertGasPartialPressure(float timeInSeconds, float halfTimeInSeconds, float currentInertGasPartialPressureInCompartment, float currentInertGasPartialPressureInLung);
	float getAscendToPartialPressureForCompartment(int compartmentIndex, float compartmentPartialPressure);
	bool isDecoNeeded(float ascendToPartialPressure);
	int getMinutesNeededTillDeco(int compartmentIndex, float currentPressure);
	int calculateMinutesRequiredToReachCertainPressure(float targetPressure);
	int calculateAscentRate(float timeSpentInLevelInSeconds, float previousDepthInMeter, float currentDepthInMeter);
};

#endif

