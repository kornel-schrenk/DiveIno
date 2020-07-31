package hu.diveino.converter.data;

import java.util.ArrayList;
import java.util.List;

public class DiveProfileData {

    private Summary summary;
    private List<Profile> profileItems;
        
    public DiveProfileData() {
    	this.profileItems = new ArrayList<Profile>();
    }
    
    public List<Profile> getProfile() {
        return profileItems;
    }

	public Summary getSummary() {
		return summary;
	}

	public void setSummary(Summary summary) {
		this.summary = summary;
	}

}
