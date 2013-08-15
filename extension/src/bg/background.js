var SERVER_BASE_ADDRESS = "http://office.gixoo.com:3000";
var RESERVE_URL = SERVER_BASE_ADDRESS + "/reserve";
var SERVER_SOCKET = SERVER_BASE_ADDRESS + "/socket/status";
var ICON_DIR = "icons/";
var BADGE_STATUS = [
	{
		caption: "Free",
//		icon: "kaki1.jpg",
		colors: "#00FF00"
	},
	{
		caption: "Busy",
//		icon: "kaki2.jpg",
		colors: "#FF0000"
	},
	{
		caption: "Res",
//		icon: "kaki2.jpg",
		colors: "#0000FF"
	},
	{
		caption: "Go!!",
//		icon: "kaki2.jpg",
		colors: "#0000FF"
	}
];
var toiletStatus = 0;
var NOOP = function(){};

// var settings = new Store("settings", {
//     "sample_setting": "This is how you use Store.js to remember values"
// });

var statusUpdatedEvent = document.createEvent("Event");
statusUpdatedEvent.initEvent("statusUpdated",true,true);

var updateStatus = function (data) {
	toiletStatus = data.status;
	var newStatus = BADGE_STATUS[toiletStatus];
	var testDetails = {
		text: newStatus.caption
	};
	var colorDetails = {
		color: newStatus.colors
	};
	var iconDetails = {
		path: ICON_DIR + newStatus.icon
	};
	chrome.browserAction.setBadgeText(testDetails);
	chrome.browserAction.setBadgeBackgroundColor(colorDetails);
//	chrome.browserAction.setIcon(iconDetails);
	statusUpdatedEvent.status = toiletStatus;
	document.dispatchEvent(statusUpdatedEvent);

};

var reserve = function(callback){
	$.post(RESERVE_URL, {}, callback);
};

var notify = function(settings){
	var options = {
		type: "basic",
		title: settings.title,
		message: settings.message,
		iconUrl: "icons/kaki1-128.png"
	};
	chrome.notifications.create(settings.name, options, function(notificationId){});
	chrome.notifications.clear(settings.name, NOOP);
	var clickHandler = function(notificationId){
		if(notificationId === settings.name){
			settings.callback();
			chrome.notifications.onClicked.removeListener(clickHandler);
		}
	};
	chrome.notifications.onClicked.addListener(clickHandler);
};

var notifyFree = function(){
	if (notifyFree.active){
		return;
	}
	var callback = function(){
		reserve(function(){
			alert("Toilet Reserved, Go!");
			updateToStatus3();
		});
	};
	var handler = function(event){
		if(event.status === 0){
			notify({
				callback: callback,
				name: "updateFree",
				title: "The Toilet is Free",
				message: "Go!!! (...or click to Reserve for 3 minutes)"
			});
			document.removeEventListener("statusUpdated", handler);
			notifyFree.active = false;
		}
	};
	document.addEventListener("statusUpdated", handler);
	notifyFree.active = true;
};

var updateToStatus3 = function(){
	updateStatus({status:3});
};
//actions = {
//	notifyFree: notifyFree
//};
//
//chrome.extension.onMessage.addListener(
//	function (request, sender, sendResponse) {
//		actions[request.action](request);
//	});
//

document.addEventListener("DOMContentLoaded", function (event) {
	var socket = io.connect(SERVER_SOCKET);
	socket.on('statusUpdate', function (data) {
		console.log(data);
		updateStatus(data);
	});
});
