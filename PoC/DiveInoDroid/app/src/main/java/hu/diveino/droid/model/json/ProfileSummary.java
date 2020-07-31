package hu.diveino.droid.model.json;

public class ProfileSummary {

    private Integer diveDuration = 0;
    private Double maxDepth = 0.0;
    private Double minTemperature = 0.0;
    private Double oxygenPercentage = 21.0;
    private String diveDate = "";
    private String diveTime = "";

    public Integer getDiveDuration() {
        return diveDuration;
    }

    public void setDiveDuration(Integer diveDuration) {
        this.diveDuration = diveDuration;
    }

    public Double getMaxDepth() {
        return maxDepth;
    }

    public void setMaxDepth(Double maxDepth) {
        this.maxDepth = maxDepth;
    }

    public Double getMinTemperature() {
        return minTemperature;
    }

    public void setMinTemperature(Double minTemperature) {
        this.minTemperature = minTemperature;
    }

    public Double getOxygenPercentage() {
        return oxygenPercentage;
    }

    public void setOxygenPercentage(Double oxigenPercentage) {
        this.oxygenPercentage = oxigenPercentage;
    }

    public String getDiveDate() {
        return diveDate;
    }

    public void setDiveDate(String diveDate) {
        this.diveDate = diveDate;
    }

    public String getDiveTime() {
        return diveTime;
    }

    public void setDiveTime(String diveTime) {
        this.diveTime = diveTime;
    }
}
