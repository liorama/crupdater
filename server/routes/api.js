/*
 * GET home page.
 */


exports.updateSensors = function (req, res) {
	sensorStatus.motion = (req.body.motionSensor === "1");
	sensorStatus.door = (req.body.doorSensor === "1");
	console.log("POST Data", req.body);
	res.send(refreshStatus());
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