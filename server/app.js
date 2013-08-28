var express = require('express'),
	app = express(),
	server = require('http').createServer(app),
	sockets = require('socket.io').listen(server, { log: false }),
	api = require('./routes/api'),
	path = require('path');

var ARDUINO_MAC_ADDRESS = "78:C4:E:1:99:48";

// all environments
app.set('port', process.env.PORT || 3000);
app.set('views', __dirname + '/views');
app.set('view engine', 'jade');
app.use(express.favicon());
app.use(express.logger('dev'));
app.use(express.bodyParser());
app.use(express.methodOverride());
app.use(express.cookieParser('your secret here'));
//app.use(express.session());
app.use(app.router);
//app.use(require('less-middleware')({ src: __dirname + '/public' }));
app.use(express.static(path.join(__dirname, 'public')));



server.listen(app.get('port'), function () {
	console.log('Express server listening on port ' + app.get('port'));
});


// development only
if ('development' == app.get('env')) {
	app.use(express.errorHandler());
}


var STATUSES = ["vacant", "occupied", "reserved"];
var CLEAR_RESERVE_TIMER = (1000 * 60) * 3; // 3 minute reserve timer
var toiletStatus = 0;
var isReservable = function () {
	return !(toiletStatus == 1 || toiletStatus == 2);
};

/****** Global Methods ********/
global.sensorStatus = {
	motion: false,
	door: false
};

global.setStatus = function (status) {
	console.log("status: " + status + " isReservable: " + isReservable());
	if (status == 2) {
		if (!isReservable()) {
			console.log('isReservable check false, sending false for reserve request');
			return false;
		}
		setTimeout(function () {
			if (getStatus() == 2) {
				setStatus(0);
			}
		}, CLEAR_RESERVE_TIMER); // If status changes to reserve this checks that the reserve was used in CLEAR_RESERVE_TIMER, if not clear reserve
	}
	toiletStatus = status;
	console.log("New status:", status);
	api.emitStatusToSocket(openClientConnections);
	return toiletStatus;
};

global.getStatus = function (verbose) {
	return verbose ? STATUSES[toiletStatus] : toiletStatus;
};

global.refreshStatus = function () {
	var newStatus = getStatus();
	if (sensorStatus.door && sensorStatus.motion) {
		newStatus = 1;
	}
	if (!sensorStatus.door) {
		newStatus = 0;
	}
	return (newStatus === 0 && toiletStatus == 2) ? toiletStatus : setStatus(newStatus);
};

/****** HTTP routes ********/

app.get('/', api.updateSensors);

app.post('/reserve', api.reserve);

app.get('/status', api.getStatus);

var openClientConnections = sockets.of('/socket/status').on("connection", api.socketConnection);

//http.createServer(app).listen(app.get('port'), function(){
//  console.log('Express server listening on port ' + app.get('port'));
//});
