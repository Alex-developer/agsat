<?php

$params = Array(
  'observer' => Array(
    'lat' => 0.0,
    'lon' => 0.0,
    'alt' => 0
  ),
  'mutualobserver' => Array(
    'lat' => 0.0,
    'lon' => 0.0,
    'alt' => 0
  ),
  'IIS' => Array(
      'line1' => '1 25544U 98067A   11042.01601058  .00003529  00000-0  30642-4 0  8008',
      'line2' => '2 25544 051.6449 087.8999 0004517 299.5128 132.8333 15.72288264701044',
      'model' => 'sdp',
      'start' => time(),
      'step' => 60
   ),
  'AO-07' => Array(
      'line1' => '1 07530U 74089B   11041.82313980 -.00000027  00000-0  10000-3 0    12',
      'line2' => '2 07530 101.3952 053.3461 0011735 166.3441 193.7946 12.53581184658356',
      'model' => 'sdp',
      'start' => time(),
      'step' => 60
   ),
  'GOES 3' => Array(
      'line1' => '1 10953U 78062A   11041.88263851 -.00000117  00000-0  10000-3 0  3032',
      'line2' => '2 10953 014.5263 354.9802 0005265 008.2880 351.7503 01.00279071 7272',
      'model' => 'sdp',
      'start' => time(),
      'step' => 60
   )
);

$line1 = '1 25544U 98067A   11042.01601058  .00003529  00000-0  30642-4 0  8008';
$line2 = '2 25544 051.6449 087.8999 0004517 299.5128 132.8333 15.72288264701044';



$result = agsat_track($params);

var_dump($result);
?>

                  
