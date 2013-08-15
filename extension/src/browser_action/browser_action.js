var background = chrome.extension.getBackgroundPage();
var toiletStatus = background.toiletStatus;
var currentSectionId = "reserving";
var showSection = function(sectionId){
	$("#" + currentSectionId).hide();
	$("#" + sectionId).show();
	currentSectionId = sectionId;
};

var reserveToilet = function(){
	showSection("reserving");
	background.reserve(function(data){
		if(data){
			showSection("reserved");
			return background.updateToStatus3();
		}
		showSection("notifying");
	});
};
var setNotification = function(){
	background.notifyFree();
	showSection("notifying");
};


var showUnreservable = function(){
	showSection("unreservable");
};

var reserved = function(){
	showSection("reserved");
};

statusActions = [reserveToilet, setNotification, showUnreservable, reserved];


document.addEventListener("DOMContentLoaded", function (event) {
	console.log ("Toilet Status is:", toiletStatus);
	statusActions[toiletStatus]();
});
