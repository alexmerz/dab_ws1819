const noble = require('noble-mac');
var writeChar = null;
var readChar = null;

var cmdbuffer = [];
var reply = true;

function executeCommand() {
    var command = cmdbuffer.shift();
    if(command && reply == true) {
        var buffer = new Buffer(command + "\n");
        console.log(command);
        writeChar.write(buffer);
        reply = false;
    }
}

function init() {

    readChar.subscribe((error) => {
        if(error) console.log("Error:" +error);
    });
    readChar.on('data', (value) => {
        console.log(value.toString());
        if('' != value.toString()) {
            reply = true;
        }
    });
    cmdbuffer.push("RGB 100 100 100");

    setInterval(executeCommand, 700);
}

function handleService(service) {
    service.discoverCharacteristics([], function(error, characteristics) {
        for(var i = 0, l = characteristics.length; i < l; i++) {
            switch(characteristics[i].uuid) {
                case 'ffe3':
                    writeChar = characteristics[i];
                    break;
                case 'ffe2':
                    readChar = characteristics[i];
                    break;
            }
        }
        console.log("Chars discovered");
        setTimeout(init, 2000);
    });
}

function discover(peripherial) {

    console.log("Peri: " + peripherial.address);
    if('00:1b:10:0e:0c:9a' !== peripherial.address && '00-1b-10-0e-0c-9a' !== peripherial.address) {
        return;
    }
    peripherial.connect(function(error) {
        if(error) {
            console.log(error);
            return;
        }

        peripherial.discoverServices([], function(error, services) {
            if (error) {
                console.log(error);
                return;
            }

            console.log('Services discovered');

            for (var i = 0, l = services.length; i < l; i++) {
                var service = services[i];
                if ('ffe1' == service.uuid) {
                    noble.stopScanning();
                    handleService(service);
                }
            }
        });
    });
}

noble.on('discover', discover);
noble.startScanning([], false);
console.log("Scanning started");