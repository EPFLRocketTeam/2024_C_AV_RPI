#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
void dump_csv_int (int &value, std::ostringstream &stream);
void dump_csv_unsigned_int (unsigned int &value, std::ostringstream &stream);
void dump_csv_unsigned (unsigned &value, std::ostringstream &stream);
void dump_csv_float (float &value, std::ostringstream &stream);
void dump_csv_double (double &value, std::ostringstream &stream);
void dump_csv_uint8_t (uint8_t &value, std::ostringstream &stream);
void dump_csv_bool (bool &value, std::ostringstream &stream);
void dump_csv_uint32_t (uint32_t &value, std::ostringstream &stream);
enum State {
	INIT = 0,
	CALIBRATION = 1,
	FILLING = 2,
	ARMED = 3,
	PRESSURIZATION = 4,
	ABORT_ON_GROUND = 5,
	IGNITION = 6,
	BURN = 7,
	ASCENT = 8,
	LANDED = 9,
	DESCENT = 10,
	ABORT_IN_FLIGHT = 11,
};
struct adxl375_data{
	float x;
	float y;
	float z;
};
struct bmi08_sensor_data_f{
	float x;
	float y;
	float z;
};
struct bmp3_sens_status{
	uint8_t cmd_rdy;
	uint8_t drdy_press;
	uint8_t drdy_temp;
};
struct bmp3_int_status{
	uint8_t fifo_wm;
	uint8_t fifo_full;
	uint8_t drdy;
};
struct bmp3_err_status{
	uint8_t fatal;
	uint8_t cmd;
	uint8_t conf;
};
struct bmp3_status{
	bmp3_int_status intr;
	bmp3_sens_status sensor;
	bmp3_err_status err;
	uint8_t pwr_on_rst;
};
struct bmp3_data{
	double temperature;
	double pressure;
};
struct UplinkCmd{
	uint8_t id;
	uint8_t value;
};
struct SensStatus{
	uint8_t adxl_status;
	uint8_t adxl_aux_status;
	uint8_t bmi_accel_status;
	uint8_t bmi_aux_accel_status;
	uint8_t bmi_gyro_status;
	uint8_t bmi_aux_gyro_status;
	bmp3_status bmp_status;
	bmp3_status bmp_aux_status;
};
struct NavSensors{
	adxl375_data adxl;
	adxl375_data adxl_aux;
	bmi08_sensor_data_f bmi_accel;
	bmi08_sensor_data_f bmi_gyro;
	bmi08_sensor_data_f bmi_aux_accel;
	bmi08_sensor_data_f bmi_aux_gyro;
	bmp3_data bmp;
	bmp3_data bmp_aux;
};
struct PropSensors{
	float N2_pressure_eth;
	float N2_pressure_lox;
	float N2_pressure;
	float fuel_pressure;
	float LOX_pressure;
	float LOX_inj_pressure;
	float fuel_inj_pressure;
	float chamber_pressure;
	float N2_temperature;
	float N2_ext_temperature;
	float fuel_temperature;
	float LOX_temperature;
	float fuel_inj_temperature;
	float LOX_fls_90;
	float LOX_fls_80;
	float LOX_fls_50;
	float LOX_fls_10;
	float LOX_fls_cap_0;
	float chamber_temperature;
	float pressure_check;
	float total_impulse;
	uint32_t PRB_state;
};
struct Valves{
	bool valve_dpr_pressure_lox;
	bool valve_dpr_pressure_fuel;
	bool valve_dpr_vent_copv;
	bool valve_dpr_vent_lox;
	bool valve_dpr_vent_fuel;
	bool valve_prb_main_lox;
	bool valve_prb_main_fuel;
};
struct Vector3{
	float x;
	float y;
	float z;
};
struct GPSCoord{
	double lat;
	double lng;
	double alt;
};
struct GPSTime{
	unsigned year;
	unsigned month;
	unsigned day;
	unsigned hour;
	unsigned minute;
	unsigned second;
	unsigned centisecond;
};
struct NavigationData{
	GPSTime time;
	GPSCoord position;
	double gnss_speed;
	Vector3 position_kalman;
	Vector3 velocity_kalman;
	float vertical_speed;
	Vector3 accel;
	Vector3 attitude;
	double course;
	double altitude;
	bmp3_data baro;
};
struct Batteries{
	float lpb_voltage;
	float lpb_current;
	float hpb_voltage_trb;
	float hpb_current_trb;
	float hpb_voltage_prb;
	float hpb_current_prb;
};
struct CamsRecording{
	bool cam_sep;
	bool cam_up;
	bool cam_down;
};
struct Event{
	bool command_updated;
	bool calibrated;
	bool dpr_eth_ready;
	bool dpr_eth_pressure_ok;
	bool dpr_lox_ready;
	bool dpr_lox_pressure_ok;
	bool prb_ready;
	bool trb_ready;
	bool ignited;
	bool ignition_failed;
	bool engine_cut_off;
	bool seperated;
	bool chute_opened;
	bool chute_unreefed;
	bool catastrophic_failure;
};
struct DataDump{
	State av_state;
	uint32_t av_timestamp;
	uint32_t av_delta_ms;
	UplinkCmd telemetry_cmd;
	float av_fc_temp;
	float av_amb_temp;
	SensStatus stat;
	NavSensors sens;
	PropSensors prop;
	Valves valves;
	NavigationData nav;
	Batteries bat;
	CamsRecording cams_recording;
	Event event;
};
void dump_csv_adxl375_data (adxl375_data &value, std::ostringstream &stream);
void dump_csv_bmi08_sensor_data_f (bmi08_sensor_data_f &value, std::ostringstream &stream);
void dump_csv_bmp3_sens_status (bmp3_sens_status &value, std::ostringstream &stream);
void dump_csv_bmp3_int_status (bmp3_int_status &value, std::ostringstream &stream);
void dump_csv_bmp3_err_status (bmp3_err_status &value, std::ostringstream &stream);
void dump_csv_bmp3_status (bmp3_status &value, std::ostringstream &stream);
void dump_csv_bmp3_data (bmp3_data &value, std::ostringstream &stream);
void dump_csv_UplinkCmd (UplinkCmd &value, std::ostringstream &stream);
void dump_csv_SensStatus (SensStatus &value, std::ostringstream &stream);
void dump_csv_NavSensors (NavSensors &value, std::ostringstream &stream);
void dump_csv_PropSensors (PropSensors &value, std::ostringstream &stream);
void dump_csv_Valves (Valves &value, std::ostringstream &stream);
void dump_csv_Vector3 (Vector3 &value, std::ostringstream &stream);
void dump_csv_GPSCoord (GPSCoord &value, std::ostringstream &stream);
void dump_csv_GPSTime (GPSTime &value, std::ostringstream &stream);
void dump_csv_NavigationData (NavigationData &value, std::ostringstream &stream);
void dump_csv_Batteries (Batteries &value, std::ostringstream &stream);
void dump_csv_CamsRecording (CamsRecording &value, std::ostringstream &stream);
void dump_csv_Event (Event &value, std::ostringstream &stream);
void dump_csv_DataDump (DataDump &value, std::ostringstream &stream);
void dump_csv_State (State &value, std::ostringstream &stream);
void dump_csv_int (int &value, std::ostringstream &stream){ stream << value << ","; }
void dump_csv_unsigned_int (unsigned int &value, std::ostringstream &stream){ stream << value << ","; }
void dump_csv_unsigned (unsigned &value, std::ostringstream &stream){ stream << value << ","; }
void dump_csv_float (float &value, std::ostringstream &stream){ stream << value << ","; }
void dump_csv_double (double &value, std::ostringstream &stream){ stream << value << ","; }
void dump_csv_uint8_t (uint8_t &value, std::ostringstream &stream){ stream << ((int) value) << ","; }
void dump_csv_uint32_t (uint32_t &value, std::ostringstream &stream){ stream << ((long long) value) << ","; }
void dump_csv_bool (bool &value, std::ostringstream &stream) {
	if(value) { stream << "true,"; } else { stream << "false,"; } }
void dump_csv_adxl375_data (adxl375_data &value, std::ostringstream &stream) {
	dump_csv_float(value.x, stream);
	dump_csv_float(value.y, stream);
	dump_csv_float(value.z, stream);
}
void dump_csv_bmi08_sensor_data_f (bmi08_sensor_data_f &value, std::ostringstream &stream) {
	dump_csv_float(value.x, stream);
	dump_csv_float(value.y, stream);
	dump_csv_float(value.z, stream);
}
void dump_csv_bmp3_sens_status (bmp3_sens_status &value, std::ostringstream &stream) {
	dump_csv_uint8_t(value.cmd_rdy, stream);
	dump_csv_uint8_t(value.drdy_press, stream);
	dump_csv_uint8_t(value.drdy_temp, stream);
}
void dump_csv_bmp3_int_status (bmp3_int_status &value, std::ostringstream &stream) {
	dump_csv_uint8_t(value.fifo_wm, stream);
	dump_csv_uint8_t(value.fifo_full, stream);
	dump_csv_uint8_t(value.drdy, stream);
}
void dump_csv_bmp3_err_status (bmp3_err_status &value, std::ostringstream &stream) {
	dump_csv_uint8_t(value.fatal, stream);
	dump_csv_uint8_t(value.cmd, stream);
	dump_csv_uint8_t(value.conf, stream);
}
void dump_csv_bmp3_status (bmp3_status &value, std::ostringstream &stream) {
	dump_csv_bmp3_int_status(value.intr, stream);
	dump_csv_bmp3_sens_status(value.sensor, stream);
	dump_csv_bmp3_err_status(value.err, stream);
	dump_csv_uint8_t(value.pwr_on_rst, stream);
}
void dump_csv_bmp3_data (bmp3_data &value, std::ostringstream &stream) {
	dump_csv_double(value.temperature, stream);
	dump_csv_double(value.pressure, stream);
}
void dump_csv_UplinkCmd (UplinkCmd &value, std::ostringstream &stream) {
	dump_csv_uint8_t(value.id, stream);
	dump_csv_uint8_t(value.value, stream);
}
void dump_csv_SensStatus (SensStatus &value, std::ostringstream &stream) {
	dump_csv_uint8_t(value.adxl_status, stream);
	dump_csv_uint8_t(value.adxl_aux_status, stream);
	dump_csv_uint8_t(value.bmi_accel_status, stream);
	dump_csv_uint8_t(value.bmi_aux_accel_status, stream);
	dump_csv_uint8_t(value.bmi_gyro_status, stream);
	dump_csv_uint8_t(value.bmi_aux_gyro_status, stream);
	dump_csv_bmp3_status(value.bmp_status, stream);
	dump_csv_bmp3_status(value.bmp_aux_status, stream);
}
void dump_csv_NavSensors (NavSensors &value, std::ostringstream &stream) {
	dump_csv_adxl375_data(value.adxl, stream);
	dump_csv_adxl375_data(value.adxl_aux, stream);
	dump_csv_bmi08_sensor_data_f(value.bmi_accel, stream);
	dump_csv_bmi08_sensor_data_f(value.bmi_gyro, stream);
	dump_csv_bmi08_sensor_data_f(value.bmi_aux_accel, stream);
	dump_csv_bmi08_sensor_data_f(value.bmi_aux_gyro, stream);
	dump_csv_bmp3_data(value.bmp, stream);
	dump_csv_bmp3_data(value.bmp_aux, stream);
}
void dump_csv_PropSensors (PropSensors &value, std::ostringstream &stream) {
	dump_csv_float(value.N2_pressure_eth, stream);
	dump_csv_float(value.N2_pressure_lox, stream);
	dump_csv_float(value.N2_pressure, stream);
	dump_csv_float(value.fuel_pressure, stream);
	dump_csv_float(value.LOX_pressure, stream);
	dump_csv_float(value.LOX_inj_pressure, stream);
	dump_csv_float(value.fuel_inj_pressure, stream);
	dump_csv_float(value.chamber_pressure, stream);
	dump_csv_float(value.N2_temperature, stream);
	dump_csv_float(value.N2_ext_temperature, stream);
	dump_csv_float(value.fuel_temperature, stream);
	dump_csv_float(value.LOX_temperature, stream);
	dump_csv_float(value.fuel_inj_temperature, stream);
	dump_csv_float(value.LOX_fls_90, stream);
	dump_csv_float(value.LOX_fls_80, stream);
	dump_csv_float(value.LOX_fls_50, stream);
	dump_csv_float(value.LOX_fls_10, stream);
	dump_csv_float(value.LOX_fls_cap_0, stream);
	dump_csv_float(value.chamber_temperature, stream);
	dump_csv_float(value.pressure_check, stream);
	dump_csv_float(value.total_impulse, stream);
	dump_csv_uint32_t(value.PRB_state, stream);
}
void dump_csv_Valves (Valves &value, std::ostringstream &stream) {
	dump_csv_bool(value.valve_dpr_pressure_lox, stream);
	dump_csv_bool(value.valve_dpr_pressure_fuel, stream);
	dump_csv_bool(value.valve_dpr_vent_copv, stream);
	dump_csv_bool(value.valve_dpr_vent_lox, stream);
	dump_csv_bool(value.valve_dpr_vent_fuel, stream);
	dump_csv_bool(value.valve_prb_main_lox, stream);
	dump_csv_bool(value.valve_prb_main_fuel, stream);
}
void dump_csv_Vector3 (Vector3 &value, std::ostringstream &stream) {
	dump_csv_float(value.x, stream);
	dump_csv_float(value.y, stream);
	dump_csv_float(value.z, stream);
}
void dump_csv_GPSCoord (GPSCoord &value, std::ostringstream &stream) {
	dump_csv_double(value.lat, stream);
	dump_csv_double(value.lng, stream);
	dump_csv_double(value.alt, stream);
}
void dump_csv_GPSTime (GPSTime &value, std::ostringstream &stream) {
	dump_csv_unsigned(value.year, stream);
	dump_csv_unsigned(value.month, stream);
	dump_csv_unsigned(value.day, stream);
	dump_csv_unsigned(value.hour, stream);
	dump_csv_unsigned(value.minute, stream);
	dump_csv_unsigned(value.second, stream);
	dump_csv_unsigned(value.centisecond, stream);
}
void dump_csv_NavigationData (NavigationData &value, std::ostringstream &stream) {
	dump_csv_GPSTime(value.time, stream);
	dump_csv_GPSCoord(value.position, stream);
	dump_csv_double(value.gnss_speed, stream);
	dump_csv_Vector3(value.position_kalman, stream);
	dump_csv_Vector3(value.velocity_kalman, stream);
	dump_csv_float(value.vertical_speed, stream);
	dump_csv_Vector3(value.accel, stream);
	dump_csv_Vector3(value.attitude, stream);
	dump_csv_double(value.course, stream);
	dump_csv_double(value.altitude, stream);
	dump_csv_bmp3_data(value.baro, stream);
}
void dump_csv_Batteries (Batteries &value, std::ostringstream &stream) {
	dump_csv_float(value.lpb_voltage, stream);
	dump_csv_float(value.lpb_current, stream);
	dump_csv_float(value.hpb_voltage_trb, stream);
	dump_csv_float(value.hpb_current_trb, stream);
	dump_csv_float(value.hpb_voltage_prb, stream);
	dump_csv_float(value.hpb_current_prb, stream);
}
void dump_csv_CamsRecording (CamsRecording &value, std::ostringstream &stream) {
	dump_csv_bool(value.cam_sep, stream);
	dump_csv_bool(value.cam_up, stream);
	dump_csv_bool(value.cam_down, stream);
}
void dump_csv_Event (Event &value, std::ostringstream &stream) {
	dump_csv_bool(value.command_updated, stream);
	dump_csv_bool(value.calibrated, stream);
	dump_csv_bool(value.dpr_eth_ready, stream);
	dump_csv_bool(value.dpr_eth_pressure_ok, stream);
	dump_csv_bool(value.dpr_lox_ready, stream);
	dump_csv_bool(value.dpr_lox_pressure_ok, stream);
	dump_csv_bool(value.prb_ready, stream);
	dump_csv_bool(value.trb_ready, stream);
	dump_csv_bool(value.ignited, stream);
	dump_csv_bool(value.ignition_failed, stream);
	dump_csv_bool(value.engine_cut_off, stream);
	dump_csv_bool(value.seperated, stream);
	dump_csv_bool(value.chute_opened, stream);
	dump_csv_bool(value.chute_unreefed, stream);
	dump_csv_bool(value.catastrophic_failure, stream);
}
void dump_csv_DataDump (DataDump &value, std::ostringstream &stream) {
	dump_csv_State(value.av_state, stream);
	dump_csv_uint32_t(value.av_timestamp, stream);
	dump_csv_uint32_t(value.av_delta_ms, stream);
	dump_csv_UplinkCmd(value.telemetry_cmd, stream);
	dump_csv_float(value.av_fc_temp, stream);
	dump_csv_float(value.av_amb_temp, stream);
	dump_csv_SensStatus(value.stat, stream);
	dump_csv_NavSensors(value.sens, stream);
	dump_csv_PropSensors(value.prop, stream);
	dump_csv_Valves(value.valves, stream);
	dump_csv_NavigationData(value.nav, stream);
	dump_csv_Batteries(value.bat, stream);
	dump_csv_CamsRecording(value.cams_recording, stream);
	dump_csv_Event(value.event, stream);
}
void dump_csv_State (State &value, std::ostringstream &stream) {
	if (((int) value) == 0) stream << "INIT,";
	else if (((int) value) == 1) stream << "CALIBRATION,";
	else if (((int) value) == 2) stream << "FILLING,";
	else if (((int) value) == 3) stream << "ARMED,";
	else if (((int) value) == 4) stream << "PRESSURIZATION,";
	else if (((int) value) == 5) stream << "ABORT_ON_GROUND,";
	else if (((int) value) == 6) stream << "IGNITION,";
	else if (((int) value) == 7) stream << "BURN,";
	else if (((int) value) == 8) stream << "ASCENT,";
	else if (((int) value) == 9) stream << "LANDED,";
	else if (((int) value) == 10) stream << "DESCENT,";
	else if (((int) value) == 11) stream << "ABORT_IN_FLIGHT,";
	else stream << "<UNKNOWN>,";
}

int main (void) {
    DataDump dump;
	char* buffer = (char*) (&dump);
	std::ifstream stream("/boot/av_log/dump_log_61.log", std::ios_base::binary);

	std::cout << "av_state,av_timestamp,av_delta_ms,telemetry_cmd.id,telemetry_cmd.value,av_fc_temp,av_amb_temp,stat.adxl_status,stat.adxl_aux_status,stat.bmi_accel_status,stat.bmi_aux_accel_status,stat.bmi_gyro_status,stat.bmi_aux_gyro_status,stat.bmp_status.intr.fifo_wm,stat.bmp_status.intr.fifo_full,stat.bmp_status.intr.drdy,stat.bmp_status.sensor.cmd_rdy,stat.bmp_status.sensor.drdy_press,stat.bmp_status.sensor.drdy_temp,stat.bmp_status.err.fatal,stat.bmp_status.err.cmd,stat.bmp_status.err.conf,stat.bmp_status.pwr_on_rst,stat.bmp_aux_status.intr.fifo_wm,stat.bmp_aux_status.intr.fifo_full,stat.bmp_aux_status.intr.drdy,stat.bmp_aux_status.sensor.cmd_rdy,stat.bmp_aux_status.sensor.drdy_press,stat.bmp_aux_status.sensor.drdy_temp,stat.bmp_aux_status.err.fatal,stat.bmp_aux_status.err.cmd,stat.bmp_aux_status.err.conf,stat.bmp_aux_status.pwr_on_rst,sens.adxl.x,sens.adxl.y,sens.adxl.z,sens.adxl_aux.x,sens.adxl_aux.y,sens.adxl_aux.z,sens.bmi_accel.x,sens.bmi_accel.y,sens.bmi_accel.z,sens.bmi_gyro.x,sens.bmi_gyro.y,sens.bmi_gyro.z,sens.bmi_aux_accel.x,sens.bmi_aux_accel.y,sens.bmi_aux_accel.z,sens.bmi_aux_gyro.x,sens.bmi_aux_gyro.y,sens.bmi_aux_gyro.z,sens.bmp.temperature,sens.bmp.pressure,sens.bmp_aux.temperature,sens.bmp_aux.pressure,prop.N2_pressure_eth,prop.N2_pressure_lox,prop.N2_pressure,prop.fuel_pressure,prop.LOX_pressure,prop.LOX_inj_pressure,prop.fuel_inj_pressure,prop.chamber_pressure,prop.N2_temperature,prop.N2_ext_temperature,prop.fuel_temperature,prop.LOX_temperature,prop.fuel_inj_temperature,prop.LOX_fls_90,prop.LOX_fls_80,prop.LOX_fls_50,prop.LOX_fls_10,prop.LOX_fls_cap_0,prop.chamber_temperature,prop.pressure_check,prop.total_impulse,prop.PRB_state,valves.valve_dpr_pressure_lox,valves.valve_dpr_pressure_fuel,valves.valve_dpr_vent_copv,valves.valve_dpr_vent_lox,valves.valve_dpr_vent_fuel,valves.valve_prb_main_lox,valves.valve_prb_main_fuel,nav.time.year,nav.time.month,nav.time.day,nav.time.hour,nav.time.minute,nav.time.second,nav.time.centisecond,nav.position.lat,nav.position.lng,nav.position.alt,nav.gnss_speed,nav.position_kalman.x,nav.position_kalman.y,nav.position_kalman.z,nav.velocity_kalman.x,nav.velocity_kalman.y,nav.velocity_kalman.z,nav.vertical_speed,nav.accel.x,nav.accel.y,nav.accel.z,nav.attitude.x,nav.attitude.y,nav.attitude.z,nav.course,nav.altitude,nav.baro.temperature,nav.baro.pressure,bat.lpb_voltage,bat.lpb_current,bat.hpb_voltage_trb,bat.hpb_current_trb,bat.hpb_voltage_prb,bat.hpb_current_prb,cams_recording.cam_sep,cams_recording.cam_up,cams_recording.cam_down,event.command_updated,event.calibrated,event.dpr_eth_ready,event.dpr_eth_pressure_ok,event.dpr_lox_ready,event.dpr_lox_pressure_ok,event.prb_ready,event.trb_ready,event.ignited,event.ignition_failed,event.engine_cut_off,event.seperated,event.chute_opened,event.chute_unreefed,event.catastrophic_failure\n";

	while (1) {
		stream.read(buffer, sizeof(DataDump));
		if (stream.eof() || stream.fail()) break ;

		std::ostringstream strm;
		dump_csv_DataDump(dump, strm);
		
		std::string res = strm.str();
		res[res.size() - 1] = '\n';
		std::cout << res;
	}

	stream.close();
}
