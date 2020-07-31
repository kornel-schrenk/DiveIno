package hu.diveino.droid.model.database;

import hu.diveino.droid.util.TypeConverter;

public class LogbookEntry {

    private Integer numberOfDives;
    private Integer loggedDiveHours;
    private Integer loggedDiveMinutes;
    private Double maxDepth;
    private String lastDiveDateTime;    //yyyy-MM-dd HH:mm

    private String lastDiveDate;        //yyyy-MM-dd
    private String lastDiveTime;        //HH:mm

    public LogbookEntry(Integer numberOfDives,
                        Integer loggedDiveHours, Integer loggedDiveMinutes,
                        Double maxDepth, String lastDiveDateTime) {

        this.numberOfDives = numberOfDives;
        this.loggedDiveHours = loggedDiveHours;
        this.loggedDiveMinutes = loggedDiveMinutes;
        this.maxDepth = maxDepth;
        this.lastDiveDateTime = lastDiveDateTime;

        this.lastDiveDate = "";
        this.lastDiveTime = "";
        if (this.lastDiveDateTime != null) {
            String[] splitDiveDateTime = this.lastDiveDateTime.split(" ");
            if (splitDiveDateTime.length >= 2) {
                this.lastDiveDate = splitDiveDateTime[0];
                this.lastDiveTime = splitDiveDateTime[1];
            }
        }
    }

    public Integer getNumberOfDives() {
        return numberOfDives;
    }

    public String getNumberOfDivesText() {
        return TypeConverter.convertIntegerToText(this.numberOfDives);
    }

    public Integer getLoggedDiveHours() {
        return loggedDiveHours;
    }

    public String getLoggedDiveHoursText() {
        return TypeConverter.convertIntegerToTwoCharacterText(this.loggedDiveHours);
    }

    public Integer getLoggedDiveMinutes() {
        return loggedDiveMinutes;
    }

    public String getLoggedDiveMinutesText() {
        return TypeConverter.convertIntegerToTwoCharacterText(this.loggedDiveMinutes);
    }

    public Double getMaxDepth() {
        return maxDepth;
    }

    public String getMaxDepthText() {
        return TypeConverter.convertDoubleToText(this.maxDepth);
    }

    public String getLastDiveDateTime() {
        return lastDiveDateTime;
    }

    public String getLastDiveDate() {
        return lastDiveDate;
    }

    public String getLastDiveTime() {
        return lastDiveTime;
    }
}
