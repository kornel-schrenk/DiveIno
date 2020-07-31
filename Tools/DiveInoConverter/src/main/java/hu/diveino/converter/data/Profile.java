package hu.diveino.converter.data;

public class Profile {

    private double pressureInMillibar;
	private double depthInMeter;
    private double temperatureInCelsius;
    private int durationInSeconds;
    
    public Profile() {
    	this.pressureInMillibar = 0.0;
    	this.depthInMeter = 0.0;
    	this.temperatureInCelsius = 0.0;
        this.durationInSeconds = 0;   	
    }

    public Profile(double pressure, double depth, double temerature, int duration) {
        this.pressureInMillibar = pressure;
        this.depthInMeter = depth;
        this.temperatureInCelsius = temerature;
        this.durationInSeconds = duration;
    }

    public double getPressure() {
        return pressureInMillibar;
    }

    public void setPressure(double pressureInMillibar) {
		this.pressureInMillibar = pressureInMillibar;
	}
    
    public double getDepth() {
        return depthInMeter;
    }
    
	public void setDepth(double depthInMeter) {
		this.depthInMeter = depthInMeter;
	}

    public double getTemperature() {
        return temperatureInCelsius;
    }

	public void setTemperature(double temperatureInCelsius) {
		this.temperatureInCelsius = temperatureInCelsius;
	}
    
    public int getDuration() {
        return durationInSeconds;
    }

	public void setDuration(int durationInSeconds) {
		this.durationInSeconds = durationInSeconds;
	}
    
}
