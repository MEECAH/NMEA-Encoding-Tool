#include "stdafx.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <windows.h>
#include <iomanip>      // std::setprecision
using namespace std;

class nmeaHandler {

	//see nmeaMessageStructures.docx for information on how each sentence type is composed

private:

	// port fields for serial communication
	LPCSTR portName;
	int baudRate;

	// message fields
	double global_utc;
	double global_lat;
	string global_directionLat;
	double global_lon;
	string global_directionLon;
	int global_numSats;
	double global_orthoHeight;

	string global_status;
	double global_speedKnots;
	double global_speedOverGroundKPH;
	double global_trackAngle;
	string global_relativeTrueNorth = "T";
	int global_date;

public:

	//error variables
	int errorType; //-1 means invalid argument, -2 means serial communications error
	string errorMessage;

	//this class supports the encoding of three NMEA sentence types, GGA, RMC, and VTG
	enum messageTypes { GGA, RMC, VTG };
	messageTypes messageType;

	// port fields for serial communication
	HANDLE hSerial;

	//default constructor
	nmeaHandler(){

		errorType = 0;

		// message type fields, 1 = gga, 2 = rmc, 3 = vtg
		messageType = GGA;

		// port fields
		hSerial = 0;
		portName = "";
		baudRate = 0;

		// sentence fields
		global_utc = 0;
		global_lat = 0;
		global_directionLat = "N";
		global_lon = 0;
		global_directionLon = "E";
		global_numSats = 8;
		global_orthoHeight = 0;

		global_status = "A";
		global_speedKnots = 0;
		global_speedOverGroundKPH = 0;
		global_trackAngle = 0;
		string global_relativeTrueNorth = "T";
		int global_date = 0;

	}

	//functions below

	//setter functions

	int setPortName(LPCSTR port){
		portName = port;
		return 0;
	}

	int setBaudRate(int rate) {
		//baudRate input is validated in OpenPort();
		baudRate = rate;
		return 0;
	}

	int setMessageType(messageTypes type){
		messageType = type;
		return 0;
	}

	int setUTC(double utc) {
		// 225446 = Time of fix 22:54:46 UTC

		if (utc < 0){
			//throw std::invalid_argument("received negative value");
			errorType = -1;
			errorMessage = "received negative value";
			return errorType;
		}

		if (utc > 999999.0){
			//throw std::invalid_argument("value too large");
			errorType = -1;
			errorMessage = "value too large";
			return errorType;
		}

		global_utc = utc;
		
		return 0;
	}

	int setLat(double lat){

		if (lat > 90 || lat < -90){
			//throw std::invalid_argument("lat must be between -90 and 90 degrees");
			errorType = -1;
			errorMessage = "lat must be between -90 and 90 degrees";
			return errorType;
		}

		global_lat = lat;

		return 0;
	}

	int setLon(double lon){

		if (lon > 180 || lon < -180){
			//throw std::invalid_argument("lon must be between -180 and 180 degrees");
			errorType = -1;
			errorMessage = "lon must be between -180 and 180 degrees";
			return errorType;
		}

		global_lon = lon;

		return 0;
	}

	int setNumSats(int num){

		if (num < 0){
			//throw std::invalid_argument("negative value");
			errorType = -1;
			errorMessage = "negative value";
			return errorType;
		}

		global_numSats = num;

		return 0;
	}

	int setOrthoHeight(double alt){

		if (alt < 0){
			//throw std::invalid_argument("negative value");
			errorType = -1;
			errorMessage = "negative value";
			return errorType;
		}

		global_orthoHeight = alt;

		return 0;
	}

	int setSpeedKnots(double speedKnots){

		if (speedKnots < 0){
			//throw std::invalid_argument("negative value");
			errorType = -1;
			errorMessage = "negative value";
			return errorType;
		}

		global_speedKnots = speedKnots;

		return 0;
	}

	int setSpeedOverGround(double sogKPH){

		if (sogKPH < 0){
			//throw std::invalid_argument("negative value");
			errorType = -1;
			errorMessage = "negative value";
			return errorType;
		}

		global_speedOverGroundKPH = sogKPH;

		return 0;
	}

	int setTrackAngle(double angle){
		global_trackAngle = angle;

		return 0;
	}

	int setDate(int date){

		// 191194 = Date of fix  19 November 1994

		if (date < 0){
			//throw std::invalid_argument("negative value");
			errorType = -1;
			errorMessage = "negative value";
			return errorType;
		}

		if (date > 999999){
			//throw std::invalid_argument("value too large");
			errorType = -1;
			errorMessage = "value too large";
			return errorType;
		}

		global_date = date;

		return 0;
	}

	//getter functions

	LPCSTR getPortName(){
		return portName;
	}

	int getBaudRate(){
		return baudRate;
	}

	messageTypes getMessageType(){
		return messageType;
	}

	double getUTC(){
		return global_utc;
	}

	double getLat(){
		return global_lat;
	}

	double getLon(){
		return global_lon;
	}

	int getNumSats(){
		return global_numSats;
	}

	double getSpeedKnots(){
		return global_speedKnots;
	}

	double getSpeedOverGround(){
		return global_speedOverGroundKPH;
	}

	double getTrackAngle(){
		return global_trackAngle;
	}

	int getDate(){
		return global_date;
	}

	//other functions

	void printError(){
		cout << errorMessage << endl;
	}

	//for parsing lla data from csv to use for testing
	vector<vector<string>> parseCSV(){
		std::ifstream  data("testLog.csv");
		std::string line;
		std::vector<std::vector<std::string> > parsedCsv;
		while (std::getline(data, line))
		{
			std::stringstream lineStream(line);
			std::string cell;
			std::vector<std::string> parsedRow;
			while (std::getline(lineStream, cell, ','))
			{
				parsedRow.push_back(cell);
			}

			parsedCsv.push_back(parsedRow);
		}
		return parsedCsv;
	}

	//simple concatenate integer function
	int concat(int a, int b)
	{

		// Convert both the integers to string 
		string s1 = to_string(a);
		string s2 = to_string(b);

		// Concatenate both strings 
		string s = s1 + s2;

		// Convert the concatenated string 
		// to integer 
		int c = stoi(s);

		// return the formed integer 
		return c;
	}

	//convert decimal degrees into degrees minutes, and remainder decimal degrees to conform to NMEA protocol
	double deg2dmLat(double angle){
		double d = trunc(angle);
		double m = trunc(60 * abs(angle - d));
		double s = 3600 * abs(angle - d) - 60 * m;
		double p = (angle - d) * 60 - m;
		double result = d;

		if (m < 10){
			result = concat(d, 0);
			result = concat(result, m);
		}
		else{
			result = concat(result, m);
		}

		result += p;

		stringstream stream;
		stream << std::setprecision(8) << result;
		string ret = stream.str();
		result = atof(ret.c_str());

		return result;
	}

	//convert decimal degrees into degrees minutes, and remainder decimal degrees to conform to NMEA protocol
	double deg2dmLon(double angle){
		double d = trunc(angle);
		double m = trunc(60 * abs(angle - d));
		double s = 3600 * abs(angle - d) - 60 * m;
		double p = (angle - d) * 60 - m;
		double result = d;

		if (m < 10){
			result = concat(d, 0);
			result = concat(result, m);
		}
		else{
			result = concat(result, m);
		}

		if (d < 100){
			result = concat(0, result);
		}

		result += p;

		stringstream stream;
		stream << std::setprecision(8) << result;
		string ret = stream.str();
		result = atof(ret.c_str());

		return result;
	}

	//convert from degree, minute, remainder back to decimal degrees for testing purposes
	double back2Deg(double input){
		double d = trunc(input / 100);
		double m = (input / 100) - d;
		double mDec = (m * 100);
		m = trunc(mDec);
		double a = m / 60;
		double b = (mDec - m) / 60;
		double decDeg = d + a + b;
		return decDeg;

	}

	//checksum generator, code taken from NMEA wikipedia page
	//https://en.wikipedia.org/wiki/NMEA_0183

	int checksum(const char *s) {
		int c = 0;

		while (*s)
			c ^= *s++;

		return c;
	}

	//appends sentence fields in order to construct a GGA NMEA sentence and returns as a string
	string buildGGA(){

		auto time = std::to_string(global_utc); //use system time?
		auto latitude = std::to_string(deg2dmLat(global_lat)); //4124.8963,N = 41d 24.8963' N or 41d 24' 54" N
		latitude.pop_back();
		latitude.pop_back();
		auto longitude = std::to_string(deg2dmLon(global_lon));
		longitude.pop_back();
		longitude.pop_back();
		auto numSats = std::to_string(global_numSats);
		auto orthoHeight = std::to_string(global_orthoHeight);
		string dollar = "$"; // dollar
		string message = "GPGGA,"; // message ID
		message.append(time); // utc time
		message.append(",");
		message.append(latitude); // latitude
		message.append(",");
		message.append(global_directionLat); // direction of latitude
		message.append(",");
		if (global_lon<100.0){ message.append("0"); }
		message.append(longitude);
		message.append(",");
		message.append(global_directionLon);
		message.append(",");
		message.append("1"); //gps quality
		message.append(",");
		message.append(numSats);
		message.append(",");
		message.append(orthoHeight);
		orthoHeight.pop_back();
		orthoHeight.pop_back();
		orthoHeight.pop_back();
		orthoHeight.pop_back();
		orthoHeight.pop_back();
		message.append(",");
		message.append("M");
		message.append(",");
		message.append(",");
		message.append(",");
		message.append(",");
		message.append(","); 


		int n = message.length();

		char* char_array = (char*)malloc((n)* sizeof(char));

		memcpy(char_array, message.c_str(), (n + 1) * sizeof(char));

		int cs = checksum(char_array);

		std::stringstream stream;
		stream << std::uppercase << std::hex << cs;
		std::string hexCS(stream.str());

		auto result = message.append("*" + hexCS);

		result = dollar.append(result);

		cout << result << endl;

		return result;

	}

	//appends sentence fields in order to construct a RMC NMEA sentence and returns as a string
	string buildRMC(){
		auto time = std::to_string(global_utc); //use system time?
		auto velocity = std::to_string(global_speedKnots);
		velocity.pop_back();
		velocity.pop_back();
		velocity.pop_back();
		velocity.pop_back();
		auto trackAngle = std::to_string(global_trackAngle);
		trackAngle.pop_back();
		trackAngle.pop_back();
		trackAngle.pop_back();
		trackAngle.pop_back();
		auto date = std::to_string(global_date);
		auto latitude = std::to_string(deg2dmLat(global_lat)); //4124.8963,N = 41d 24.8963' N or 41d 24' 54" N
		latitude.pop_back();
		latitude.pop_back();
		auto longitude = std::to_string(deg2dmLon(global_lon));
		longitude.pop_back();
		longitude.pop_back();
		string dollar = "$"; // dollar
		string message = "GPRMC,"; // message ID
		message.append(time); // utc time
		message.append(",");
		message.append(global_status);
		message.append(",");
		message.append(latitude); // latitude
		message.append(",");
		message.append(global_directionLat); // direction of latitude
		message.append(",");
		if (global_lon<100.0){ message.append("0"); }
		message.append(longitude);
		message.append(",");
		message.append(global_directionLon);
		message.append(",");
		message.append(velocity);
		message.append(",");
		message.append(trackAngle);
		message.append(",");
		message.append(date);
		message.append(",");
		message.append(","); // effectively leave magnetic variation null

		int n = message.length();

		char* char_array = (char*)malloc((n)* sizeof(char));

		memcpy(char_array, message.c_str(), (n + 1) * sizeof(char));

		int cs = checksum(char_array);

		std::stringstream stream;
		stream << std::uppercase << std::hex << cs;
		std::string hexCS(stream.str());

		auto result = message.append("*" + hexCS);

		result = dollar.append(result);

		cout << result << endl;

		return result;
	}

	//appends sentence fields in order to construct a VTG NMEA sentence and returns as a string
	string buildVTG(){
		auto time = std::to_string(global_utc); //use system time?
		auto velocity = std::to_string(global_speedKnots);
		auto speedOverGround = std::to_string(global_speedOverGroundKPH);
		auto trackAngle = std::to_string(global_trackAngle);
		auto date = std::to_string(global_date);
		string dollar = "$"; // dollar
		string message = "GPVTG,"; // message ID
		message.append(trackAngle);
		message.append(",");
		message.append(global_relativeTrueNorth);
		message.append(",");
		message.append(","); // effectively leave magnetic north null
		message.append("M");
		message.append(",");
		message.append(velocity);
		message.append(",");
		message.append("N");
		message.append(",");
		message.append(speedOverGround);
		message.append(",");
		message.append("K");

		int n = message.length();

		char* char_array = (char*)malloc((n)* sizeof(char));

		memcpy(char_array, message.c_str(), (n + 1) * sizeof(char));

		int cs = checksum(char_array);

		std::stringstream stream;
		stream << std::uppercase << std::hex << cs;
		std::string hexCS(stream.str());

		auto result = message.append("*" + hexCS);

		result = dollar.append(result);

		cout << result << endl;

		return result;
	}

	//function to open serial port
	int openPort(){
		hSerial = CreateFile(portName,
			GENERIC_WRITE,
			0,
			0,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			0);
		if (hSerial == INVALID_HANDLE_VALUE){
			if (GetLastError() == ERROR_FILE_NOT_FOUND){
				//cout << "//serial port does not exist. Inform user. \n";
				errorType = -2;
				errorMessage = "serial port does not exist.Inform user.";
				return errorType;
			}
			//cout << "//some other error occurred with hSerial. Inform user. \n";
			errorType = -2;
			errorMessage = "some other error occurred with hSerial. Inform user.";
			return errorType;
		}

		DCB dcbSerialParams = { 0 };
		dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
		if (!GetCommState(hSerial, &dcbSerialParams)) {
			//cout << "//error getting state \n";
			errorType = -2;
			errorMessage = "//error getting state \n";
			return errorType;
		}
		if (baudRate = 4800){
			dcbSerialParams.BaudRate = CBR_4800;
		}
		if (baudRate = 9600){
			dcbSerialParams.BaudRate = CBR_9600;
		}
		if (baudRate = 19200){
			dcbSerialParams.BaudRate = CBR_19200;
		}
		if (baudRate = 38400){
			dcbSerialParams.BaudRate = CBR_38400;
		}
		if (baudRate = 115200){
			dcbSerialParams.BaudRate = CBR_115200;
		}
		else{
			//cout << "invalid baud rate";
			errorType = -2;
			errorMessage = "invalid baud rate";
			return errorType;
		}
		dcbSerialParams.ByteSize = 8;
		dcbSerialParams.StopBits = ONESTOPBIT;
		dcbSerialParams.Parity = NOPARITY;
		if (!SetCommState(hSerial, &dcbSerialParams)){
			//cout << "//error setting serial port state \n";
			errorType = -2;
			errorMessage = "//error setting serial port state \n";
			return errorType;
		}

		COMMTIMEOUTS timeouts = { 0 };
		timeouts.ReadIntervalTimeout = 50;
		timeouts.ReadTotalTimeoutConstant = 50;
		timeouts.ReadTotalTimeoutMultiplier = 10;
		timeouts.WriteTotalTimeoutConstant = 50;
		timeouts.WriteTotalTimeoutMultiplier = 10;
		if (!SetCommTimeouts(hSerial, &timeouts)){
			//cout << "//error occureed with setcommtimeouts. Inform user \n";
			errorType = -2;
			errorMessage = "//error occureed with setcommtimeouts. Inform user \n";
			return errorType;
		}
	}

	//function to close serial port
	int closePort(){
		CloseHandle(hSerial);
		return 0;
	}

	//function which allows for updating all sentence fields at once
	int updateParams(
		double utc,
		double lat,
		double lon,
		string status,
		double speedKnots,
		double speedOverGroundKPH,
		double trackAngle,
		string relativeTrueNorth,
		int date
		){
		setUTC(utc);
		setLat(lat);
		setLon(lon);
		setSpeedKnots(speedKnots);
		setSpeedOverGround(speedOverGroundKPH);
		setTrackAngle(trackAngle);
		setDate(date);
		return 0;
	}

	//send nmea sentence over the serial port
	int sendMessage(){

		string message;

		if (messageType == GGA){
			message = buildGGA();
		}
		if (messageType == RMC){
			message = buildRMC();
		}
		if (messageType == VTG){
			message = buildVTG();
		}

		message.append(" \n");

		int n = message.length();

		// declaring character array 
		char* char_array = (char*)malloc(n * sizeof(char));

		// copying the contents of the 
		// string to char array

		if (!(message.empty())){

			memcpy(char_array, message.c_str(), (n + 1) * sizeof(char));
			//strcpy_s(char_array, 100, message.c_str());

			DWORD dwBytesToWrite = strlen(char_array);
			DWORD dNoOfBytesWritten = strlen(char_array);

			if (!WriteFile(hSerial, (LPCVOID*)char_array, dwBytesToWrite, &dNoOfBytesWritten, NULL)){
				//error occurred. Report to user.
				//cout << "failed to write \n";
				errorType = -2;
				errorMessage = "failed to write \n";
				return errorType;
				
			}
		}

		return 0;
	}

};


int main() {

	//main method demonstrates intended uses of nmeaHandler object by parsing log data from a CSV to treat as nav system input

	nmeaHandler n;

	vector<vector<string>> matrix = n.parseCSV();
	double la = -1.0;
	double lo = -1.0;

	int msgSent = 0;

	n.setPortName("COM1");
	n.setBaudRate(19200);
	n.openPort();

	for (int k = 0; k < matrix.size(); k++) {
		for (int l = 0; l < matrix[k].size(); l++) {
			//cout << matrix[k][l] << ' ';
			if (l == 0 || l % 2 == 0){
				la = atof(matrix[k][l].c_str());
				//cout << n.deg2dmsLat(la) << ' ';
			}
			else{
				lo = atof(matrix[k][l].c_str());
				//cout << n.deg2dmsLon(lo);
				//cout << matrix[k][l].c_str() << " ";}
				//cout << matrix[k][l].c_str() << " ";
			}
		}
		//cout << endl;
		if (k != 0){

			//need to rewrite this using set methods
			//n.global_utc = 0.0;
			n.setUTC(0.0);
			n.setLat(la);
			n.setLon(lo);
			//n.global_speedKnots = rand() % 100;
			//n.global_trackAngle = rand() % 100;
			double madeUpSpeed = rand() % 100;
			double madeUpAngle = rand() % 100;
			n.setSpeedKnots(madeUpSpeed);
			n.setTrackAngle(madeUpAngle);
			n.setMessageType(n.GGA);
			n.sendMessage();
			//n.messageType = n.RMC;
			//n.sendMessage();
			//n.messageType = n.VTG;
			//n.sendMessage();

		}
		msgSent++;

	}

	n.closePort();

	cout << "program ended successfully" << endl;

	return n.errorType;

}
