package hu.diveino.droid.model.json;

import java.util.ArrayList;
import java.util.List;

public class Profile {

    private ProfileSummary profileSummary;
    private List<ProfileItem> profileItems = new ArrayList<>();

    public void setProfileSummary(ProfileSummary profileSummary) {
        this.profileSummary = profileSummary;
    }

    public ProfileSummary getProfileSummary() {

        return profileSummary;
    }

    public List<ProfileItem> getProfileItems() {
        return profileItems;
    }
}
