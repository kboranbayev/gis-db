# gis-db

Geographic Information Systems Database implementation in C++.

The program parses GIS record files that can be obtained from the website for the USGS Board on Geographic
Names ([http://geonames.usgs.gov](http://geonames.usgs.gov)).

The program accepts 3 arguments:

1. database file name  (write-read)
2. script file name    (read-only)
3. log file name       (write-only)

Available commands:

**world<tab><westLong><tab><eastLong><tab><southLat><tab><northLat>**
  
**import<tab><GIS record file>**
  
**debug<tab>[ quad | hash | pool | world ]**
  
Shows current contents of the imported records.
  
**quit**
 
Terminates program execution.
 
**what_is_at<tab><geographic coordinate latitude><tab><geographic coordinate longitude>**
  
A single search feature that can return more than 1 entry if the coordinates are exact match. The results will be shown in terminal output and written into a log file.
  
**what_is<tab><feature name><tab><state abbreviation>**
  
For every GIS record in the database file that matches the given <feature name> nd <state abbreviation>, log the offset at which the record was found, and the county name, the primary latitude, and the primary longitude. The results will be shown in terminal output and written into a log file.
 
**what_is_in<tab><geographic coordinate><tab><half-height><tab><half-width>**
 
For every GIS record in the database file whose coordinates fall within the closed rectangle with the specified height and width, centered at the <geographic coordinate>, log the offset at which the record was found, and the feature name, the state name, and the primary latitude and primary longitude. The half-height and half-width are specified as total seconds.
  
**what_is_in<tab>-long<tab><geographic coordinate><tab><half-height><tab><half-width>**
  
 Shows long detailed information on the search results.
 
 **what_is_in<tab>-filter [ pop | water | structure ]<geographic coordinate><tab><half-height><tab><half-width>**
  
 Search feature with filter match:
 
 1. pop -- populated places
 2. water -- water class locations
 3. structure -- structures or buildings such as airports, churches, and schools.
 
 
