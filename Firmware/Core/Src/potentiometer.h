#pragma once

typedef struct {
	ADC_HandleTypeDef* adc;
	uint32_t channel;
	double oldValue;
} Potentiometer;

void PotentiometerInit(Potentiometer* potentiometer) {
	potentiometer->oldValue = -1;
}

uint32_t PotentiometerRead(Potentiometer* potentiometer) {
	// Changing channel
	ADC_ChannelConfTypeDef channelConfig = {
		.Channel = potentiometer->channel,
		.Rank = 1,
		.SamplingTime = ADC_SAMPLETIME_28CYCLES_5,
	};

	HAL_ADC_ConfigChannel(potentiometer->adc, &channelConfig);

	// Reading value
	HAL_ADC_Start(potentiometer->adc);
	HAL_ADC_PollForConversion(potentiometer->adc, HAL_MAX_DELAY);
	uint32_t value = HAL_ADC_GetValue(potentiometer->adc);
	HAL_ADC_Stop(potentiometer->adc);

	return value;
}

uint32_t PotentiometerReadSmooth(Potentiometer* potentiometer, uint32_t min, uint32_t max) {
	const uint8_t deltaThreshold = 20;
	const double newValueFactor = 0.05;

	const double newValue = PotentiometerRead(potentiometer);

	if (potentiometer->oldValue < 0) {
		potentiometer->oldValue = newValue;
	}
	else {
		if (abs(newValue - potentiometer->oldValue) > deltaThreshold)
			potentiometer->oldValue = newValue * newValueFactor + potentiometer->oldValue * (1 - newValueFactor);
	}

	// Clamping
	if (potentiometer->oldValue < min)
		potentiometer->oldValue = min;
	else if (potentiometer->oldValue > max)
		potentiometer->oldValue = max;

	double ratio = (double) (potentiometer->oldValue - min) / (double) (max - min);

	return (uint32_t) (ratio * 4096.0f);
}
