package hu.diveino.converter;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.Writer;
import java.text.DecimalFormat;
import java.text.NumberFormat;

import hu.diveino.converter.data.DiveProfileData;
import hu.diveino.converter.data.Profile;

/**
 * Check the UDCF - Universal Dive Computer Format here: http://www.streit.cc/extern/udcf/udcf_doc_eng.html#toc2
 */
public class Log2Udcf {

    public static void main(String[] args) {
        Log2Udcf log2Udcf = new Log2Udcf();
        if (args.length ==2) {
            File diveInoLogFile =  new File(args[0]);
            File udcfLogFile = new File(args[1]);

            LogFileParser logFileParser = new LogFileParser();
            log2Udcf.createUdcfFile(logFileParser.parseDiveInoLogFile(diveInoLogFile), udcfLogFile);
        } else {
            System.out.println("Usage: java Log2Udcf <DiveIno LogFile> <UDCF LogFile>");
            System.exit(-1);
        }
    }

    public void createUdcfFile(DiveProfileData diveProfileData, File udcfFile) {

        if (diveProfileData != null && udcfFile != null) {
            try {
                if (udcfFile.exists()) {
                    udcfFile.delete();
                }
                udcfFile.createNewFile();

                Writer out = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(udcfFile), "UTF8"));

                out.write(this.generateUdcfFileHeader().concat(this.generateUdcfFileDiveEntry(diveProfileData)).concat(this.generateUdcfFileFooter()));

                out.flush();
                out.close();
            } catch (IOException  e) {
                e.printStackTrace();
            }
        }
    }

    private String generateUdcfFileHeader() {
        return "<PROFILE UDCF=\"1\">\n" +
                "    <UNITS>Metric</UNITS>\n" +
                "    <DEVICE>\n" +
                "        <VENDOR>www.diveino.hu</VENDOR>\n" +
                "        <MODEL>DiveIno</MODEL>\n" +
                "        <VERSION>0.9.6</VERSION>\n" +
                "    </DEVICE>\n" +
                "    <REPGROUP>\n";
    }

    private String generateUdcfFileFooter() {
        return "    </REPGROUP>\n" +
                "</PROFILE>";
    }

    private String generateUdcfFileDiveEntry(DiveProfileData diveProfileData) {
    	diveProfileData.getSummary().convertDiveDateData();
    	
        String text = "        <DIVE>\n";
        text = text.concat("            <DATE>\n" +
                "                <DAY>" + diveProfileData.getSummary().getDay() + "</DAY>\n" +
                "                <MONTH>" + diveProfileData.getSummary().getMonth() + "</MONTH>\n" +
                "                <YEAR>" + diveProfileData.getSummary().getYear() + "</YEAR>\n" +
                "            </DATE>\n" +
                "            <TIME>\n" +
                "                <HOUR>" + diveProfileData.getSummary().getHour() + "</HOUR>\n" +
                "                <MINUTE>" + diveProfileData.getSummary().getMinute() + "</MINUTE>\n" +
                "            </TIME>\n");
        text = text.concat("            <SURFACEINTERVAL>infinity</SURFACEINTERVAL>\n" +
                "            <TEMPERATURE>" + diveProfileData.getSummary().getMinTemperature() + "</TEMPERATURE>\n" +
                "            <DENSITY>1030.00</DENSITY>\n" +
                "            <ALTITUDE>0.0</ALTITUDE>\n" +
                "            <GASES>\n" +
                "                <MIX>\n" +
                "                    <MIXNAME>Air</MIXNAME>\n" +
                "                    <O2>" + diveProfileData.getSummary().getOxygenPercentage() / 100 + "</O2>\n" +
                "                    <N2>" + (1 - (diveProfileData.getSummary().getOxygenPercentage() / 100)) + "</N2>\n" +
                "                    <HE>0.00</HE>\n" +
                "                </MIX>\n" +
                "            </GASES>\n" +
                "            <DELTAMODE/>\n");

        text = text.concat("            <SAMPLES>\n" +
                "                <SWITCH>Air</SWITCH>\n");
        for (Profile diveProfileItem : diveProfileData.getProfile()) {
            text = text.concat("                <T>" + convertSecondsToMinutes(diveProfileItem.getDuration()) + "</T><D>" + diveProfileItem.getDepth() + "</D>\n");
        }
        text = text.concat("                <ALARM>SURFACE</ALARM>\n" +
                "            </SAMPLES>\n");
        return text.concat("        </DIVE>\n");
    }

    private String convertSecondsToMinutes(int seconds) {
        NumberFormat formatter = new DecimalFormat("#0.00");
        return formatter.format((double)seconds / 60);
    }

}
