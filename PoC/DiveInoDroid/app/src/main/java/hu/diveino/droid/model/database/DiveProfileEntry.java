package hu.diveino.droid.model.database;

import java.util.ArrayList;
import java.util.List;

public class DiveProfileEntry {

    private Long id;

    private Integer diveDuration;       //In seconds
    private Double maxDepth;            //In meters
    private Double minTemperature;      //In celsius degrees
    private Double oxygenPercentage;
    private String diveDateTime;        //yyyy-MM-dd HH:mm

    private String diveDate;            //yyyy-MM-dd
    private String diveTime;            //HH:mm

    private List<ProfileItemEntry> profileItems = new ArrayList<>();

    public DiveProfileEntry(Long id, Integer diveDuration, Double maxDepth, Double minTemperature, Double oxygenPercentage, String diveDateTime) {
        this.id = id;

        this.diveDuration = diveDuration;
        this.maxDepth = maxDepth;
        this.minTemperature = minTemperature;
        this.oxygenPercentage = oxygenPercentage;
        this.diveDateTime = diveDateTime;

        this.diveDate = "";
        this.diveTime = "";
        if (this.diveDateTime != null) {
            String[] splitDiveDateTime = this.diveDateTime.split(" ");
            if (splitDiveDateTime.length >= 2) {
                this.diveDate = splitDiveDateTime[0];
                this.diveTime = splitDiveDateTime[1];
            }
        }
    }

    public Long getId() {
        return id;
    }

    public Integer getDiveDuration() {
        return diveDuration;
    }

    public Double getMaxDepth() {
        return maxDepth;
    }

    public Double getMinTemperature() {
        return minTemperature;
    }

    public Double getOxygenPercentage() {
        return oxygenPercentage;
    }

    public String getDiveDateTime() {
        return diveDateTime;
    }

    public String getDiveDate() {
        return diveDate;
    }

    public String getDiveTime() {
        return diveTime;
    }

    public List<ProfileItemEntry> getProfileItems() {
        return profileItems;
    }
}
