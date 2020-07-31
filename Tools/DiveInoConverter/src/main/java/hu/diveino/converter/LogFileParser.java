package hu.diveino.converter;

import java.io.File;
import java.io.IOException;

import com.fasterxml.jackson.core.JsonParseException;
import com.fasterxml.jackson.databind.JsonMappingException;
import com.fasterxml.jackson.databind.ObjectMapper;

import hu.diveino.converter.data.DiveProfileData;

public class LogFileParser {

    public DiveProfileData parseDiveInoLogFile(File logFile) {

        DiveProfileData diveProfileData = null;

        if (logFile != null && logFile.exists()) {
        	
          	ObjectMapper mapper = new ObjectMapper();	        	
        	try {
				diveProfileData = mapper.readValue(logFile, DiveProfileData.class);
			} catch (JsonParseException e1) {
				e1.printStackTrace();
			} catch (JsonMappingException e1) {
				e1.printStackTrace();
			} catch (IOException e1) {
				e1.printStackTrace();
			}

//            int durationInSeconds = 0;
//            double maximumDepthInMeter = 0.0;
//            double minimumWaterTemperatureInCelsius = 0.0;
//            double oxygenPercentage = 21.0;
//            String diveDateTime = "";
//
//            try (Scanner scanner = new Scanner(logFile)) {
//
//                int counter = 0;
//                String line;
//                while (scanner.hasNext()){
//                    line = scanner.nextLine();
//
//                    if (counter == 4) {
//                        durationInSeconds = readIntFromLineEnd(line);
//                    } else if (counter == 5) {
//                        maximumDepthInMeter = readDoubleFromLineEnd(line);
//                    } else if (counter == 6) {
//                        minimumWaterTemperatureInCelsius = readDoubleFromLineEnd(line);
//                    } else if (counter == 7) {
//                        oxygenPercentage = readDoubleFromLineEnd(line);
//                    } else if (counter == 8) {
//                        diveDateTime = readStringFromLineEnd(line);
//                    } else if (counter == 9) {
//                        diveDateTime = diveDateTime.concat(" ").concat(readStringFromLineEnd(line));
//                    } else if (counter == 10) {
//                        diveProfileData = new DiveProfileData(durationInSeconds, maximumDepthInMeter, minimumWaterTemperatureInCelsius, oxygenPercentage, diveDateTime);
//                    } else if (counter >= 24) {
//                        String[] values = line.split(",");
//                        DiveProfileItem diveProfileItem = new DiveProfileItem(Double.parseDouble(values[0]), Double.parseDouble(values[1]), Double.parseDouble(values[2]), Integer.parseInt(values[3]));
//                        diveProfileData.getProfileItems().add(diveProfileItem);
//                    }
//
//                    counter++;
//                }
//            } catch (IOException e) {
//                e.printStackTrace();
//            }
        }

        return diveProfileData;
    }

//    private double readDoubleFromLineEnd(String line) {
//        return Double.parseDouble(line.substring(line.indexOf('=') + 2));
//    }
//
//    private int readIntFromLineEnd(String line) {
//        return Integer.parseInt(line.substring(line.indexOf('=') + 2));
//    }
//
//    private String readStringFromLineEnd(String line) {
//        return line.substring(line.indexOf('=') + 2);
//    }
}
