<?php

// Create a curl handle
//curl -H "Content-type: application/json" -H "Authorization:key=AAAAXQTDjlM:APA91bF1bwHAofO0AS9OCf8cWcH10AeeqCJoBu70wisfnqAJ8PAnRF8a2hyYsHHy_vlJW8Y2i0RtLCdq0DSWUqZIE4AtR3Z3r5HpCtY4wTraksiKGq-gdSalTF6gjegENS88vrFuGIDP"  -X POST -d '{ "to": "/topics/0001pm10","data": { "message": "This is a Firebase Cloud Messaging Topic Message!"}}' https://fcm.googleapis.com/fcm/send

//curl -X POST --header "Authorization: key=AAAAXQTDjlM:APA91bF1bwHAofO0AS9OCf8cWcH10AeeqCJoBu70wisfnqAJ8PAnRF8a2hyYsHHy_vlJW8Y2i0RtLCdq0DSWUqZIE4AtR3Z3r5HpCtY4wTraksiKGq-gdSalTF6gjegENS88vrFuGIDP" \
 //   --Header "Content-Type: application/json" \
 //   https://fcm.googleapis.com/fcm/send \
 //   -d "{\"to\":\"fpalcLIkrPs:APA91bHGRRPGvEQ5NSygFx8huGJMzikrtcM2QjNzMOc4lwL3cn4BARGdusbPAkLVZ8idsMmkkAjav4JYGxMZIu3BxrY_sGo7JQ1bni-VfOaBY9PxEzA27UEa74DS0DDl30nsuTZCm_JY\",\"notification\":{\"body\":\"Yellow\"},\"priority\":10}"


// API access key from Google API's Console
$pm1 = $_GET['pm25'];
echo $pm1;



$registrationIds = array( $_GET['id'] );

// prep the bundle
$msg = array
(
	'message' 	=> 'PM2.5='.$pm1.' ,Unhealthy for Sensitive',
	'title'		=> 'This is a title. title',
	'subtitle'	=> 'This is a subtitle. subtitle',
	'tickerText'	=> 'Ticker text here...Ticker text here...Ticker text here',
	'vibrate'	=> 1,
	'sound'		=> 1,
	'largeIcon'	=> 'large_icon',
	'smallIcon'	=> 'small_icon'
);

$fields = array
(
	'to' 	=> '/topics/0001pm25',
	'data'			=> $msg
);
 
$headers = array
(
	'Authorization: key=AAAAXQTDjlM:APA91bF1bwHAofO0AS9OCf8cWcH10AeeqCJoBu70wisfnqAJ8PAnRF8a2hyYsHHy_vlJW8Y2i0RtLCdq0DSWUqZIE4AtR3Z3r5HpCtY4wTraksiKGq-gdSalTF6gjegENS88vrFuGIDP',
	'Content-Type: application/json'
);
 
$ch = curl_init();
curl_setopt( $ch,CURLOPT_URL, 'https://android.googleapis.com/gcm/send' );
curl_setopt( $ch,CURLOPT_POST, true );
curl_setopt( $ch,CURLOPT_HTTPHEADER, $headers );
curl_setopt( $ch,CURLOPT_RETURNTRANSFER, true );
curl_setopt( $ch,CURLOPT_SSL_VERIFYPEER, false );
curl_setopt( $ch,CURLOPT_POSTFIELDS, json_encode( $fields ) );
$result = curl_exec($ch );
curl_close( $ch );

echo $result;
?>