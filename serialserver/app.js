var createError = require('http-errors');
var express = require('express');
var path = require('path');
var bodyParser = require('body-parser');
var cookieParser = require('cookie-parser');
var logger = require('morgan');

var fs = require('fs');
var http = require('http');


var app = express();

// view engine setup
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'jade');

app.use(logger('dev'));
app.use(express.json());
app.use(bodyParser.urlencoded({ extended: true }));
app.use(cookieParser());
app.use(express.static(path.join(__dirname, 'public')));

// SYSTEMSPEZIFISCH!
const SERIAL = '/dev/cu.wchusbserial1420';

const SerialPort = require('serialport');
const port = new SerialPort(SERIAL, () => {});

r = express.Router();

r.post('/', function(req, res) {
  var call = ["","","",""];

  if(req.body.hasOwnProperty('cmd') && '' != req.body.cmd) {
    call[0] = req.body.cmd;
  }
    if(req.body.hasOwnProperty('p1') && '' != req.body.p1) {
        call[1] = req.body.p1;
    }
    if(req.body.hasOwnProperty('p2') && '' != req.body.p2) {
        call[2] = req.body.p2;
    }
    if(req.body.hasOwnProperty('p3') && '' != req.body.p3) {
        call[3] = req.body.p3;
    }

    port.write(call.join(' ') + "\n");

    res.status(200);
    res.send("OK");
    res.end();
    return;
});

app.use('/command', r);

// catch 404 and forward to error handler
app.use(function(req, res, next) {
    next(createError(404));
});

// error handler
app.use(function(err, req, res, next) {
    // set locals, only providing error in development
    res.locals.message = err.message;
    res.locals.error = req.app.get('env') === 'development' ? err : {};

    // render the error page
    res.status(err.status || 500);
    res.render('error');
});

// Serial "file"

port.on('data', (data)=>{
    console.log(data.toString('ascii'));
});


http.createServer(app).listen(8080, function(){
    console.log('Express server listening on port 8080');
});

module.exports = app;
