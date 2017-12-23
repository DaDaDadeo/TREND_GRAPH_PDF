# TREND_GRAPH_PDF
Convert Tab or Comma Delimited File to PDF trend graph using LIBHARU


The Libharu line draw feature is used. https://github.com/libharu/libharu/blob/master/demo/line_demo.pdf

A database table file in this example is a tab delimited text file that is created by an automation controller. This file is a record of temperatures and pressures with timed intervals. The first 5 records provide details for report purposes. The remaining records provide the data points to write the trend lines for the graph. 

Using Libharu libraries, the automated creation of a trend graph with headers, labels, keys, axis and timelines, and trend lines is possible. The main component of the graph uses the line draw feature. A single trend line is a compilation of individual lines connecting one point to another. Each point on the page is calculated by scaling a temperature or pressure to a timeline location. Each record in the database will calculate a new start and end point for each line. When all records are scanned, the compiled trend line based on a single column of data is created.

Any delimited file can be used. Simply change the delimiter character in the C program to read and parse each record.
