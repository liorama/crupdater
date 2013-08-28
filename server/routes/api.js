/*
 * API handlers.
 */
var REQUEST_FIELDS = {
	door: "d",
	motion: "m",
	timer: "t"
};

exports.updateSensors = function (req, res) {
	var data = req.query;
	console.log("Received request data:", data, "At ", new Date().toLocaleTimeString());
	if (data[REQUEST_FIELDS.motion] === undefined || data[REQUEST_FIELDS.door] === undefined){
		return res.send(400, "Bad Request");
	}
	sensorStatus.motion = (data[REQUEST_FIELDS.motion] === "1");
	sensorStatus.door = (data[REQUEST_FIELDS.door] === "1");
	var response = refreshStatus();
	res.useChunkedEncodingByDefault = false;
	res.set({
		"Transfer-Encoding": "none",
		"Connection": "close",
		"Content-Type": "text/plain"
	});
	return res.send(response);
};

exports.getStatus = function (req, res) {
	res.send({status: getStatus(), verbose: getStatus(true)});
};

exports.reserve = function (req, res) {
	var response = !!setStatus(2);
	console.log("Sending (reserve post response)", response);
	res.send(response);
};


/**** sockets ****/


var emitStatusToSocket = exports.emitStatusToSocket = function(socket) {
	socket.emit('statusUpdate', {status: getStatus(), verbose: getStatus(true)});
};

exports.socketConnection = function (socket){
	emitStatusToSocket(socket);
};