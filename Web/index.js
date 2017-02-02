<!DOCTYPE HTML>
<html>
<head>
<meta charset="utf-8">
<title>Temperature &amp; Humidity Dashboard | Firebase + ESP8266 </title>
<meta name="viewport" content="width=device-width, initial-scale=1">

<!-- jQuery -->
<script src="https://cdnjs.cloudflare.com/ajax/libs/jquery/3.1.0/jquery.min.js"></script>

<!-- Firebase -->
<script src="https://www.gstatic.com/firebasejs/3.3.2/firebase.js"></script>

<!-- canvasjs -->
<script src="js/jquery.canvasjs.min.js"></script>

<!-- Material Design fonts -->
<link rel="stylesheet" href="http://fonts.googleapis.com/css?family=Roboto:300,400,500,700" />
<link rel="stylesheet" href="https://fonts.googleapis.com/icon?family=Material+Icons">
  
<!-- Bootstrap -->
<link rel="stylesheet" href="css/bootstrap.min.css">
<link rel="stylesheet" href="css/bootstrap-material-design.min.css">
<link rel="stylesheet" href="css/ripples.min.css">
<script src="js/bootstrap.min.js"></script>
<script src="js/material.min.js"></script>
<script src="js/ripples.min.js"></script>

<!-- Animate.css -->
<link rel="stylesheet" href="css/animate.min.css">

<script>
var chart, toOffline;
var newItems = false;
// Initialize Firebase
var config = {
    apiKey: "AIzaSyCiBoJkJFuJJI726-6N7DF2TSBdAUn4lqo",
    authDomain: "pointznet-c1490.firebaseapp.com",
    databaseURL: "https://pointznet-c1490.firebaseio.com",
    storageBucket: "pointznet-c1490.appspot.com",
    messagingSenderId: "399511883347"
  };


$(document).ready(function(e) {
	$.material.init()
	
	chart = new CanvasJS.Chart("chartContainer", {
		title: {
			text: "Temperature & Humidity"
		},
		axisX:{  
			valueFormatString: "HH:mm"
		},
		axisY: {
			valueFormatString: "0.0#"
		},
		toolTip: {
			shared: true,
			contentFormatter: function (e) {
				var content = CanvasJS.formatDate(e.entries[0].dataPoint.x, "HH:mm:ss") + "<br>";
				content += "<strong>Temperature</strong>: " + e.entries[0].dataPoint.y + " &deg;C<br>";
				content += "<strong>Humidity</strong>: " + e.entries[1].dataPoint.y + " %";
				return content;
			}
		},
		animationEnabled: true,
		data: [
			{
				type: "spline", //change it to line, area, column, pie, etc
				dataPoints: []
			},
			{
				type: "spline", //change it to line, area, column, pie, etc
				dataPoints: []
			}
		]
	});
	chart.render();	
	
	firebase.initializeApp(config);
		
	var logDHT = firebase.database().ref().("0001");
		
	logDHT.on("child_added", function(sanp) {
		if (!newItems) return;
		var row = sanp.val();
		var pm25 = row.lastpm25;
		var pm10 = row.lastpm10;
		
		row.time = new Date(row.time);
		chart.options.data[0].dataPoints.push({x: row.time, y: row.temperature});
		chart.options.data[1].dataPoints.push({x: row.time, y: row.humidity});
		chart.render();
		
		$("#temperature > .content").html(row.temperature + " &deg;C");
		$("#pm10 > .content").html(row.pm10 + " ug/m3");
		$("#pm25 > .content").html(row.pm25 + " ug/m3");
		//console.log(pm25);
		if(pm25>=0&&pm25<=50) text25="Good";
		else if(pm25>50&&pm25<101) text25="Moderate";
		else if(pm25>100&&pm25<151) text25="Unhealthy for Sensitive";
		else if(pm25>150&&pm25<201) text25="Unhealthy";
		else if(pm25>200&&pm25<301) text25="Very Unhealthy";
		else if(pm25>300&&pm25<501) text25="Hazardous";
		console.log(text25);

		//console.log(pm10);
		if(pm10>=0&&pm10<=50) text10="Good";
		else if(pm10>50&&pm10<101) text10="Moderate";
		else if(pm10>100&&pm10<151) text10="Unhealthy for Sensitive";
		else if(pm10>150&&pm10<201) text10="Unhealthy";
		else if(pm10>200&&pm10<301) text10="Very Unhealthy";
		else if(pm10>300&&pm10<501) text10="Hazardous";
		console.log(text10);
		//$("#pm25m > .content").html(row.pm25 + " ug/m3");
		//$("#pm25mm > ").html(row.pm25*100 + " ug/m3")
		//$("#pm25m > .content").html(if(row.pm25>20){row.pm25});
		$("#gps > .content").html(row.lat + " , " + row.lng);
		$("#humidity > .content").text(row.humidity + " %");
		
		$("#status").removeClass("danger").addClass("success");
		$("#status > .content").text("ONLINE");

		
		setTimeoffline();
	});
	
	var now = new Date();
	logDHT.orderByChild("time").startAt(now.getFullYear() + "-" + (now.getMonth() + 1) + "-" + now.getDate()).once("value", function(sanp) {
		console.log(sanp);
		newItems = true;
		var dataRows = sanp.val();
		var lastRows = 0;

		
		var Arow = lastRows;
		$("#temperature > .content").html(Arow.temperature + " &deg;C");
		$("#humidity > .content").text(Arow.humidity + " %");
		//$("#pm10 > .content").text(Arow.pm10 + " ug/m3");
		//$("#pm25 > .content").text(Arow.pm25 + " ug/m3");
		
		var now = new Date();
		Arow.time = new Date(Arow.time);
		if (Math.round(now) -  Math.round(Arow.time) < (2 * 60 * 1000)) {
			$("#status").removeClass("danger").addClass("success");
			$("#status > .content").text("ONLINE");
		} else {
			$("#status").removeClass("success").addClass("danger");
			$("#status > .content").text("OFFLINE");
		}
		
		setTimeoffline();
	});
});

var setTimeoffline = function() {
	if (typeof toOffline === "number") clearTimeout(toOffline);
	
	toOffline = setTimeout(function() {
		$("#status").removeClass("success").addClass("danger");
		$("#status > .content").text("OFFLINE");
	}, 2 * 60 * 1000);
}
</script>

<style>
.dialog {
	width: 100%;
	border-radius: 4px;
	margin-bottom: 20px;
	box-shadow: 0 1px 6px 0 rgba(0, 0, 0, 0.12), 0 1px 6px 0 rgba(0, 0, 0, 0.12);
}
.dialog > .content {
	padding: 30px 0;
	border-radius: 6px 6px 0 0;
	font-size: 64px;
	color: rgba(255,255,255, 0.84);
	text-align: center;
}
.dialog.primary > .content{
	background: #00aa9a;
}
.dialog.success > .content {
	background: #59b75c;
}
.dialog.info > .content {
	background: #03a9f4;
}
.dialog.warning > .content {
	background: #ff5722;
}

.dialog.danger > .content {
	background: #f44336;
}
.dialog > .title {
	background: #FFF;
	border-radius: 0 0 6px 6px;
	font-size: 22px;
	color: rgba(0,0,0, 0.87);
	text-align: center;
	padding: 10px 0;
	/* box-shadow: 0px 10px 8px -10px rgba(0, 0, 0, 0.4) inset; */
	font-weight: bold;
}
.nav-tabs {
	margin-bottom: 20px;
}
</style>
</head>

<body>

  <div class="container">
    <h1>Device No.0001 Dashboard <small>Firebase + ESP8266</small></h1>
    <hr />
     
    <!-- Nav tabs -->
    <!--
    <ul class="nav nav-tabs" role="tablist">
      <li role="presentation" class="active"><a href="#overview" aria-controls="home" role="tab" data-toggle="tab">Overview</a></li>
      <li role="presentation"><a href="#history" aria-controls="profile" role="tab" data-toggle="tab">History</a></li>
    </ul>
-->

    <!-- Tab panes -->
    <div class="tab-content">
      <div role="tabpanel" class="tab-pane active" id="overview">
        <div class="row">
        	<div class="col-sm-4">
             <div class="dialog success fadeIn animated" id="status">
               <div class="content">???</div>
               <div class="title">Device No.0001</div>
             </div>
           </div>
           <div class="col-sm-4">
             <div class="dialog warning fadeIn animated" id="temperature">
               <div class="content">00.0 &deg;C</div>
               <div class="title">Temperature</div>
             </div>
           </div>
           <div class="col-sm-4">
             <div class="dialog info fadeIn animated" id="humidity">
               <div class="content">00.0 %</div>
               <div class="title">Humidity</div>
             </div>
           </div>
        </div>

        <div class="row">
        	<div class="col-sm-4">
             <div class="dialog danger fadeIn animated" id="pm10">
               <div class="content">???</div>
               <div class="title">PM 10</div>
             </div>
           </div>
           <div class="col-sm-4">
             <div class="dialog primary fadeIn animated" id="pm25">
               <div class="content">00.0 &deg;C</div>
               <div class="title">PM 2.5</div>
             </div>
           </div>
           <div class="col-sm-4">
             <div class="dialog success fadeIn animated" id="gps">
               <div class="content">00.0 %</div>
               <div class="title">GPS</div>
             </div>
           </div>
        </div>
  
   

      </div>
      <div role="tabpanel" class="tab-pane" id="history">
        <div id="chartContainer" style="height: 300px; width: 100%;"></div>
      </div>
    </div>
  </div>
</body>
</html>


