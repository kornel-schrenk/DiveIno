package hu.diveino.converter.data;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.GregorianCalendar;

public class Summary {

    private static SimpleDateFormat diveDateFormat = new SimpleDateFormat("yyyy-MM-dd HH:mm");
	
	private int diveDuration;
	private double maxDepth;
	private double minTemperature;
	private double oxygenPercentage;
	private String diveDate;
	private String diveTime;
	
	private Date diveDateData;
    private Calendar diveDateCalendar;

	public Summary() {
		this.diveDuration = 0;
		this.maxDepth = 0.0;
		this.minTemperature = -20;
		this.oxygenPercentage = 21.0;
		this.diveDate = "";
		this.diveTime = "";
	}
	
	public int getDiveDuration() {
		return diveDuration;
	}

	public void setDiveDuration(int diveDuration) {
		this.diveDuration = diveDuration;
	}

	public double getMaxDepth() {
		return maxDepth;
	}

	public void setMaxDepth(double maxDepth) {
		this.maxDepth = maxDepth;
	}

	public double getMinTemperature() {
		return minTemperature;
	}

	public void setMinTemperature(double minTemperature) {
		this.minTemperature = minTemperature;
	}

	public double getOxygenPercentage() {
		return oxygenPercentage;
	}

	public void setOxygenPercentage(double oxigenPercentage) {
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
	
	public void convertDiveDateData() {
        try {	
            this.diveDateData = diveDateFormat.parse(this.diveDate + " " + this.diveTime);
            this.diveDateCalendar = GregorianCalendar.getInstance();
            this.diveDateCalendar.setTime(diveDateData);
        } catch (ParseException e) {
            System.err.println("The following dive date is invalid: " + this.diveDate + " " + this.diveTime);
        	e.printStackTrace();
        }
	}
	
	public Date getDiveDateData() {
		return this.diveDateCalendar.getTime();
	}

    public int getYear() {
        return this.diveDateCalendar.get(Calendar.YEAR);
    }

    public int getMonth() {
        return this.diveDateCalendar.get(Calendar.MONTH)+1;
    }

    public int getDay() {
        return this.diveDateCalendar.get(Calendar.DATE);
    }

    public int getHour() {
        return this.diveDateCalendar.get(Calendar.HOUR_OF_DAY);
    }

    public int getMinute() {
        return this.diveDateCalendar.get(Calendar.MINUTE);
    }

}
