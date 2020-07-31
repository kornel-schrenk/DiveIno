var config = require("./config.json");

var fs = require('fs');

var SerialPort = require("serialport");
var sp = new SerialPort(config.port, {
	baudrate : config.baudrate,
	parser : SerialPort.parsers.readline("\n")
});

var startTimeStamp = Date.now();
var testData;

sp.on("open", function() {
	console.log("*EMULATOR*: " + config.port + " serial port was opened!");
	console.log("*EMULATOR*: " + config.test + " test data file will be used!");

	fs.readFile(config.test, function handleFile(err, data) {
		if (err)
			throw err;		
		testData = JSON.parse(data);				
	});

	sp.on("data", function(data) {
		if (data.startsWith("@START#")) {
			startTimeStamp = Date.now();
			console.log("*EMULATOR*: Dive started at: " + startTimeStamp);
		} else if (data.startsWith("@GET#")) {
			var durationInSeconds = (Date.now() - startTimeStamp) / 1000;			

			var pressure = 928;
			var temperature = 25.3;
			for (var i = 0; i < testData.profile.length; i++) {
				if (testData.profile[i].duration > durationInSeconds) {
					pressure = +testData.profile[i].pressure.toFixed(2);
					temperature = +testData.profile[i].temperature.toFixed(1);
					break; //The relevant data was found - stop searching!
				}
			}
			console.log("*EMULATOR*: " + durationInSeconds + " sec, " + pressure + " mbar, " + temperature + " cel\n");
			sp.write(pressure + ", " + temperature + ",");
		} else if (data.startsWith("@STOP#")) {
			sp.write("@EMULATOR OFF#", function(err) {
				if (err) {
					return console.log("Error on write: ", err.message);
				}
				console.log("*EMULATOR*: Emulator was turned OFF!");
			});
		} else {
			console.log(data);
		}
	});

	setTimeout(function() {
		sp.write("@EMULATOR ON#", function(err) {
			if (err) {
				return console.log("Error on write: ", err.message);
			}
			console.log("*EMULATOR*: Emulator was turned ON!");
		});
	}, config.initialDelay * 1000);
});
