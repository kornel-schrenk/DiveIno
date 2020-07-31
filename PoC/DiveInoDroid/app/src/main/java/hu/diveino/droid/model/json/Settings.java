package hu.diveino.droid.model.json;

public class Settings {

    private Double seaLevelPressure = 1013.2;
    private Double oxygenPercentage = 0.21;
    private Boolean soundEnabled = Boolean.TRUE;
    private Boolean isImperialUnits = Boolean.FALSE;

    public Double getSeaLevelPressure() {
        return seaLevelPressure;
    }

    public void setSeaLevelPressure(Double seaLevelPressure) {
        this.seaLevelPressure = seaLevelPressure;
    }

    public Double getOxygenPercentage() {
        return oxygenPercentage;
    }

    public void setOxygenPercentage(Double oxygenPercentage) {
        this.oxygenPercentage = oxygenPercentage;
    }

    public Boolean getSoundEnabled() {
        return soundEnabled;
    }

    public void setSoundEnabled(Boolean soundEnabled) {
        this.soundEnabled = soundEnabled;
    }

    public Boolean getImperialUnits() {
        return isImperialUnits;
    }

    public void setImperialUnits(Boolean imperialUnits) {
        isImperialUnits = imperialUnits;
    }
}
