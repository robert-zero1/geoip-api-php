<?php

/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 2; tab-width: 2 -*- */
/* geoipcity.inc
 *
 * Copyright (C) 2002 MaxMind
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 */

define("FULL_RECORD_LENGTH",50);
define("RECORD_LENGTH",3);
include("geoip.inc");

class geoiprecord {
  var $country_code;
  var $country_code3;
  var $country_name;
  var $region;
  var $city;
  var $postal_code;
  var $latitude;
  var $longitude;
}

function _get_record($gi,$ipnum){
  $seek_country = _geoip_seek_country($gi,$ipnum);
  if ($seek_country == $gi->databaseSegments) {
    return NULL;
  }
  $record_pointer = $seek_country + (2 * RECORD_LENGTH - 1) * $gi->databaseSegments;

  fseek($gi->filehandle, $record_pointer, SEEK_SET);
  $record_buf = fread($gi->filehandle,FULL_RECORD_LENGTH);
  $record = new geoiprecord;
  $char = ord(substr($record_buf,$record_buf_pos,1));
  $record_buf_pos = 0;
  $record->country_code = $GLOBALS['GEOIP_COUNTRY_CODES'][$char];
  $record->country_code3 = $GLOBALS['GEOIP_COUNTRY_CODES3'][$char];
  $record->country_name = $GLOBALS['GEOIP_COUNTRY_NAMES'][$char];
  $record_buf_pos++;
  $str_length = 0;
  
  //get region
  $char = ord(substr($record_buf,$record_buf_pos+$str_length,1));
  while ($char != 0){
    $str_length++;
    $char = ord(substr($record_buf,$record_buf_pos+$str_length,1));
  }
  if ($str_length > 0){
    $record->region = substr($record_buf,$record_buf_pos,$str_length);
  }
  $record_buf_pos += $str_length + 1;
  $str_length = 0;

  //get city
  $char = ord(substr($record_buf,$record_buf_pos+$str_length,1));
  while ($char != 0){
    $str_length++;
    $char = ord(substr($record_buf,$record_buf_pos+$str_length,1));
  }
  if ($str_length > 0){
    $record->city = substr($record_buf,$record_buf_pos,$str_length);
  }
  $record_buf_pos += $str_length + 1;
  $str_length = 0;

  //get postal code
  $char = ord(substr($record_buf,$record_buf_pos+$str_length,1));
  while ($char != 0){
    $str_length++;
    $char = ord(substr($record_buf,$record_buf_pos+$str_length,1));
  }
  if ($str_length > 0){
    $record->postal_code = substr($record_buf,$record_buf_pos,$str_length);
  }
  $record_buf_pos += $str_length + 1;
  $str_length = 0;
  $latitude = 0;
  $longitude = 0;
  for ($j = 0;$j < 3; ++$j){
    $char = ord(substr($record_buf,$record_buf_pos++,1));
    $latitude += ($char << ($j * 8));
  }
  $record->latitude = ($latitude/10000) - 180;

  for ($j = 0;$j < 3; ++$j){
    $char = ord(substr($record_buf,$record_buf_pos++,1));
    $longitude += ($char << ($j * 8));
  }
  $record->longitude = ($longitude/10000) - 180;

  return $record;
}

function GeoIP_record_by_addr ($gi,$addr){
  if ($addr == NULL){
     return 0;
  }
  $ipnum = ip2long($addr);
  return _get_record($gi, $ipnum);
}
?>