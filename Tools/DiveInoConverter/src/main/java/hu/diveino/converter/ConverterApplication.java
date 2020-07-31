package hu.diveino.converter;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import javafx.application.Application;
import javafx.geometry.Insets;
import javafx.scene.Scene;
import javafx.scene.control.Alert;
import javafx.scene.control.Alert.AlertType;
import javafx.scene.control.Button;
import javafx.scene.control.ComboBox;
import javafx.scene.control.ListView;
import javafx.scene.control.SelectionMode;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.Priority;
import javafx.scene.layout.VBox;
import javafx.stage.FileChooser;
import javafx.stage.Stage;

public class ConverterApplication extends Application {

    public static void main(String[] args) {
        launch(args);
    }

    private Stage primaryStage;
    private ListView<String> list;
    private File workDirectory;

    private List<File> selectedFiles = new ArrayList<>();

    @Override
    public void start(Stage primaryStage) {
        this.workDirectory = new File(System.getProperty("user.home"));

        this.primaryStage = primaryStage;
        this.primaryStage.setTitle("DiveIno Converter");

        Image applicationIcon = new Image("logo_small.png");
        this.primaryStage.getIcons().add(applicationIcon);

        BorderPane borderPane = new BorderPane();
        borderPane.setTop(this.createHeader());
        borderPane.setCenter(this.createCenter());

        this.primaryStage.setMinWidth(500);
        this.primaryStage.setMinHeight(300);
        
        Scene scene = new Scene(borderPane, 500, 300);

        this.primaryStage.setScene(scene);
        this.primaryStage.show();
    }

    public HBox createHeader() {
        HBox header = new HBox();
        header.setPadding(new Insets(15, 12, 15, 12));
        header.setSpacing(10);

        Button addButton = new Button("Add", new ImageView(new Image("add-icon.png")));
        addButton.setPrefSize(100, 20);
        addButton.setOnAction(e -> {
            FileChooser fileChooser = new FileChooser();
            fileChooser.setInitialDirectory(this.workDirectory);
            fileChooser.setTitle("Open DiveIno log file(s)");
            fileChooser.getExtensionFilters().addAll(
                    new FileChooser.ExtensionFilter("DiveIno Logs", "Dive*.json"),
                    new FileChooser.ExtensionFilter("All Files", "*.*")
            );
            List<File> toBoOpenedFiles = fileChooser.showOpenMultipleDialog(this.primaryStage);
            if (toBoOpenedFiles != null) {
                for (File openFile : toBoOpenedFiles) {
                    if (!this.selectedFiles.contains(openFile)) {
                        this.selectedFiles.add(openFile);
                        this.list.getItems().add(openFile.getName());
                        this.workDirectory = openFile.getParentFile();
                    }
                }
            }
        });

        Button removeButton = new Button("Remove", new ImageView(new Image("remove-icon.png")));
        removeButton.setPrefSize(100, 20);
        removeButton.setOnAction(event -> {
            List<Integer> selectedIndices = this.list.getSelectionModel().getSelectedIndices();
            if (selectedIndices != null) {
                for (Integer index : selectedIndices) {
                    this.selectedFiles.remove(index.intValue());
                    this.list.getItems().remove(index.intValue());
                }
            }
        });

        final ComboBox<String> fileTypeComboBox = new ComboBox<String>();
        fileTypeComboBox.setPrefSize(100, 20);
        fileTypeComboBox.getItems().addAll("Uddf", "Udcf");
        fileTypeComboBox.getSelectionModel().select(0);

        Button convertButton = new Button("Convert", new ImageView(new Image("convert-icon.png")));
        convertButton.setPrefSize(100, 20);
        convertButton.setOnAction(event -> {
            List<Integer> selectedIndices = this.list.getSelectionModel().getSelectedIndices();
            if (selectedIndices != null) {
                for (Integer index : selectedIndices) {
                    File diveInoLogFile = this.selectedFiles.get(index.intValue());

                    LogFileParser logFileParser = new LogFileParser();
                    if (fileTypeComboBox.getSelectionModel().getSelectedIndex() == 0) {
                        //Convert to UDDF
                        Log2Uddf log2Uddf = new Log2Uddf();
                        File uddfLogFile = new File(diveInoLogFile.getPath().replaceAll("json", "UDDF"));
                        log2Uddf.createUddfFile(logFileParser.parseDiveInoLogFile(diveInoLogFile), diveInoLogFile, uddfLogFile);
                    } else {
                        //Convert to UDCF
                        Log2Udcf log2Udcf = new Log2Udcf();
                        File udcfLogFile = new File(diveInoLogFile.getPath().replaceAll("json", "UDCF"));
                        log2Udcf.createUdcfFile(logFileParser.parseDiveInoLogFile(diveInoLogFile), udcfLogFile);
                    }
                }
                
                if (selectedIndices.size() > 0) {
                    Alert alert = new Alert(AlertType.INFORMATION);
                    alert.setTitle("Successful conversion");
                    alert.setHeaderText(null);
                    alert.setContentText(selectedIndices.size() + " file(s) were converted.");
                    alert.showAndWait();                	
                } else {
                    Alert alert = new Alert(AlertType.WARNING);
                    alert.setTitle("No files are selected");
                    alert.setHeaderText(null);
                    alert.setContentText("Please select log files for conversion!");
                    alert.showAndWait();                	                	
                }
                
            }
        });        

        header.getChildren().addAll(addButton, removeButton, convertButton, fileTypeComboBox);
        return header;
    }

    public VBox createCenter() {
        VBox scrollVBox = new VBox();
        this.list = new ListView<>();
        this.list.getSelectionModel().setSelectionMode(SelectionMode.MULTIPLE);
        scrollVBox.getChildren().addAll(this.list);
        VBox.setVgrow(this.list, Priority.ALWAYS);                
        
        return scrollVBox;
    }
}
