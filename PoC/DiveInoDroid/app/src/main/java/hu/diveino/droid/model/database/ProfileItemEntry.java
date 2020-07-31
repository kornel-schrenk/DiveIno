package hu.diveino.droid.model.database;

public class ProfileItemEntry {

    private Double depth;           //In meters
    private Double pressure;        //In millibar
    private Integer duration;       //In seconds
    private Double temperature;     //In celsius degrees

    public ProfileItemEntry(Double depth, Double pressure, Integer duration, Double temperature) {
        this.depth = depth;
        this.pressure = pressure;
        this.duration = duration;
        this.temperature = temperature;
    }

    public Double getDepth() {
        return depth;
    }

    public Double getPressure() {
        return pressure;
    }

    public Integer getDuration() {
        return duration;
    }

    public Double getTemperature() {
        return temperature;
    }
}
