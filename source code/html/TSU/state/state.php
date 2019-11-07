<?php

$state =  $_GET["state"];
if($state != "")
	file_put_contents("state.log", $state,LOCK_EX);

$status = $_GET["status"];
if($status == "query")
	echo file_get_contents("state.log");

$temperature = $_GET["threshold"];
if(isset($temperature))
{
	if(is_numeric($temperature))
	{
		$temperature += 0.0;
		if($temperature > -100.0 && $temperature < 200.0)
			file_put_contents("threshold.log",$temperature,LOCK_EX);
	}
}

// if you change text in temp.log
// make sure first 3 characters are 'F' ' ' and ':' for sanity checks

$temp_f = $_GET["Fahrenheit"];
$temp_h = $_GET["Humidity"];
$temp_c = $_GET["Celsius"];

if($temp_f != "" || $temp_h != "" || $temp_c != "")
{
	$temp = "F : ".$temp_f."\n"."C : ".$temp_c."\n"."H : ".$temp_h."\nLast Recording : ".time()."\n";
	$temp .= "Threshold : " . file_get_contents("threshold.log")." F\n";
	file_put_contents("temp.log", $temp,LOCK_EX);
}
?>
