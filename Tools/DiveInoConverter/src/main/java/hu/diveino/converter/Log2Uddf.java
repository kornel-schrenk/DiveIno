package hu.diveino.converter;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.Writer;
import java.text.SimpleDateFormat;
import java.util.Date;

import hu.diveino.converter.data.DiveProfileData;
import hu.diveino.converter.data.Profile;

/**
 * Check the UDCF - Universal Dive Data Format here: http://www.uddf.org/
 */
public class Log2Uddf {

    private static SimpleDateFormat uddfDateFormat = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss");

    public static void main(String[] args) {
        Log2Uddf log2Uddf = new Log2Uddf();
        if (args.length ==2) {
            File diveInoLogFile =  new File(args[0]);
            File uddfLogFile = new File(args[1]);

            LogFileParser logFileParser = new LogFileParser();
            log2Uddf.createUddfFile(logFileParser.parseDiveInoLogFile(diveInoLogFile), diveInoLogFile, uddfLogFile);
        } else {
            System.out.println("Usage: java Log2Uddf <DiveIno LogFile> <UDDF LogFile>");
            System.exit(-1);
        }
    }

    public void createUddfFile(DiveProfileData diveProfileData, File diveInoLogFile, File uddfFile) {

        if (diveProfileData != null && uddfFile != null) {
            try {
                if (uddfFile.exists()) {
                    uddfFile.delete();
                }
                uddfFile.createNewFile();

                Writer out = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(uddfFile), "UTF8"));

                out.write(this.generateUddfFileHeader().concat(this.generateUddfFileDiveEntry(diveProfileData, diveInoLogFile.getName())).concat(this.generateUddfFileFooter()));

                out.flush();
                out.close();
            } catch (IOException  e) {
                e.printStackTrace();
            }
        }
    }

    private String generateUddfFileHeader() {
        return "<uddf xmlns=\"http://www.streit.cc/uddf/3.2/\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"uddf_3.2.0.xsd\">\n" +
                "    <generator>\n" +
                "        <name>DiveIno Converter</name>\n" +
                "        <type>converter</type>\n" +
                "        <link ref=\"divecomputer\"/>\n" +
                "        <manufacturer id=\"manufacturer-1\">\n" +
                "            <name>www.diveino.hu</name>\n" +
                "        </manufacturer>\n" +
                "        <version>0.9.6</version>\n" +
                "        <datetime>" + uddfDateFormat.format(new Date()) + "</datetime>\n" +
                "    </generator>\n" +
                "    <mediadata/>\n" +
                "    <diver>\n" +
                "        <owner id=\"owner-1\">\n" +
                "            <personal>\n" +
                "                <firstname/>\n" +
                "                <lastname/>\n" +
                "            </personal>\n" +
                "            <equipment>\n" +
                "                <divecomputer id=\"divecomputer\">\n" +
                "                    <name>DiveIno</name>\n" +
                "                </divecomputer>\n" +
                "            </equipment>\n" +
                "        </owner>\n" +
                "    </diver>\n" +
                "    <profiledata>\n" +
                "        <repetitiongroup id=\"group-1\">\n";
    }

    private String generateUddfFileFooter() {
        return "        </repetitiongroup>\n" +
                "    </profiledata>\n" +
                "</uddf>\n";
    }

    private String generateUddfFileDiveEntry(DiveProfileData diveProfileData, String fileName) {
    	diveProfileData.getSummary().convertDiveDateData();
    	
        String text = "            <dive id=\"dive-1\">\n" +
                "                <informationbeforedive>\n" +
                "                    <divenumber>1</divenumber>\n" +
                "                    <datetime>"+  uddfDateFormat.format(diveProfileData.getSummary().getDiveDateData()) + "</datetime>\n" +
                "                    <surfaceintervalbeforedive>\n" +
                "                        <infinity/>\n" +
                "                    </surfaceintervalbeforedive>\n" +
                "                    <altitude>0.0</altitude>\n" +
                "                    <surfacepressure>103025.0</surfacepressure>\n" +
                "                </informationbeforedive>\n" +
                "                <samples>\n";
        for (Profile diveProfileItem : diveProfileData.getProfile()) {
            text = text.concat(
                "                  <waypoint>\n" +
                "                    <depth>" + diveProfileItem.getDepth() + "</depth>\n" +
                "                    <divetime>" + diveProfileItem.getDuration() + "</divetime>\n" +
                "                  </waypoint>\n");
        }
        text = text.concat(
                "                </samples>\n" +
                "                <informationafterdive>\n" +
                "                    <lowesttemperature>" + (diveProfileData.getSummary().getMinTemperature() + 273.15) + "</lowesttemperature>\n" +
                "                    <greatestdepth>" + diveProfileData.getSummary().getMaxDepth() + "</greatestdepth>\n" +
                "                    <diveduration>" + diveProfileData.getSummary().getDiveDuration() + "</diveduration>\n" +
                "                    <notes>\n" +
                "                        <para>\n" +
                "                            " + fileName + "\n" +
                "                        </para>\n" +
                "                    </notes>\n" +
                "                    <rating>\n" +
                "                        <ratingvalue>5</ratingvalue>\n" +
                "                    </rating>\n" +
                "                </informationafterdive>\n" +
                "            </dive>");
        return text;
    }
}
