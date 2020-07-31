var config = require("./config.json");

var fs = require('fs');

var SerialPort = require("serialport");
var sp = new SerialPort(config.port, {
	baudrate : config.baudrate,
	parser : SerialPort.parsers.readline("\n")
});

sp.on("open", function () {
    console.log("*REPLAY:* " + config.port + " serial port was opened!");
    console.log("*REPLAY:* " + config.test + " test data file will be used!");

    //Log out to the console what the client sends back
    sp.on('data', function(data) {
        console.log(data);
    });

    //Read the file and send to the callback
    fs.readFile(config.test, handleFile);
    
    //Turn on replay
	setTimeout(function() {
		sp.write("@REPLAY ON#", function(err) {
			if (err) {
				return console.log("Error on write: ", err.message);
			}			
		});
	}, config.initialDelay * 1000 - 4000); //Two seconds before replay will start
});

function sendMessage(duration, pressure, depth, temperature) {
    setTimeout(function () {
        sp.write(pressure + ", " + depth + ", " + duration + ", " + temperature + ",");        
    }, (config.initialDelay * 1000) + (duration * config.interval));
}

function handleFile(err, data) {
    if (err) throw err;

    var duration = 0; //In seconds
    var pressure;
    var depth;
    var temperature;

    var data = JSON.parse(data);
    
    for (var i= 0; i < data.profile.length; i++) {
        duration = data.profile[i].duration;

        //Read and transform the numerical values to two decimal digit precision
        pressure = +data.profile[i].pressure.toFixed(2);
        depth = +data.profile[i].depth.toFixed(2);
        temperature = +data.profile[i].temperature.toFixed(2);

        if (duration != 0) {
            sendMessage(duration, pressure, depth, temperature);
        }
        
        //Turn off replay
        if (i == data.profile.length-1) {
        	setTimeout(function() {
        		sp.write("@REPLAY OFF#", function(err) {
        			if (err) {
        				return console.log("Error on write: ", err.message);
        			}        			
        		});
        	}, (config.initialDelay * 1000) + (duration * config.interval) + 4000); 
        }
    }         
}
