package hu.diveino.droid.model.json;

public class Logbook {

    private Integer numberOfDives = 0;
    private Integer loggedDiveHours = 0;
    private Integer loggedDiveMinutes = 0;
    private Double maxDepth = 0.0;
    private String lastDiveDate = "";
    private String lastDiveTime = "";
    private Integer numberOfStoredProfiles = 0;

    public Integer getNumberOfDives() {
        return numberOfDives;
    }

    public Integer getLoggedDiveHours() {
        return loggedDiveHours;
    }

    public Integer getLoggedDiveMinutes() {
        return loggedDiveMinutes;
    }

    public Double getMaxDepth() {
        return maxDepth;
    }

    public String getLastDiveDate() {
        return lastDiveDate;
    }

    public String getLastDiveTime() {
        return lastDiveTime;
    }

    public Integer getNumberOfStoredProfiles() {
        return numberOfStoredProfiles;
    }

    public void setNumberOfDives(Integer numberOfDives) {
        this.numberOfDives = numberOfDives;
    }

    public void setLoggedDiveHours(Integer loggedDiveHours) {
        this.loggedDiveHours = loggedDiveHours;
    }

    public void setLoggedDiveMinutes(Integer loggedDiveMinutes) {
        this.loggedDiveMinutes = loggedDiveMinutes;
    }

    public void setMaxDepth(Double maxDepth) {
        this.maxDepth = maxDepth;
    }

    public void setLastDiveDate(String lastDiveDate) {
        this.lastDiveDate = lastDiveDate;
    }

    public void setLastDiveTime(String lastDiveTime) {
        this.lastDiveTime = lastDiveTime;
    }

    public void setNumberOfStoredProfiles(Integer numberOfStoredProfiles) {
        this.numberOfStoredProfiles = numberOfStoredProfiles;
    }
}
