
const noble = require('noble-mac');
const readline = require('readline');

var writeChar = null;
var readChar = null;

var cmdbuffer = [];
var ts = 0;
var reply = true;

readline.emitKeypressEvents(process.stdin);
process.stdin.setRawMode(true);

process.stdin.on('keypress', (str, key) => {
    var cts = Date.now();
    if (key.ctrl && key.name === 'c') {
        process.exit();
    }
    if(cts > ts) {
        switch (key.name) {
            case 'up':
                cmdbuffer.push("FORWARD 720 100");
                break;
            case 'down':
                cmdbuffer.push("BACK 720 100");
                break;
            case 'left':
                cmdbuffer.push("LCIRCLE 720 100");
                break;
            case 'right':
                cmdbuffer.push("RCIRCLE 720 100");
                break;
            case 'a':
                cmdbuffer.push("LEFT 720 100");
                break;
            case 'd':
                cmdbuffer.push("RIGHT 720 100");
                break;
            case 'c':
                cmdbuffer.push("CLEAR");
                break;

        }

        ts = cts + 700;
    }
});

function executeCommand() {
    var command = cmdbuffer.shift();
    if(command /* && reply == true */) {
        var buffer = new Buffer(command + "\n");
        console.log(command);
        writeChar.write(buffer, true);
        reply = false;
    }
}

function checkCmdbuffer() {
    if(cmdbuffer.length == 0) {
        cmdbuffer.push("CLEAR");
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
    setInterval(checkCmdbuffer, 4000);
    setInterval(executeCommand, 700);
}

function handleService(service) {
    service.discoverCharacteristics([], function(error, characteristics) {
        for(var i = 0, l = characteristics.length; i < l; i++) {
            console.log(characteristics[i].uuid);
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
    /*
    if('00:1b:10:0e:0c:9a' !== peripherial.address && '00-1b-10-0e-0c-9a' !== peripherial.address) {
        return;
    }
    */
    if('00:1b:10:fa:c3:a2' !== peripherial.address && '00-1b-10-fa-c3-a2' !== peripherial.address) {
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
