import sys
import matplotlib.pyplot as plt
import matplotlib.ticker as plticker
import csv

t = []
fc_tmp = []
bmp_press = []
bmp_tmp = []
N2_press = []
fuel_press = []
lox_press = []
fuel_inj_press = []
lox_inj_press = []
chamber_press = []

with open(sys.argv[1], 'r') as csvfile:
    lines = csv.reader(csvfile, delimiter = ',')
    next(lines)
    next(lines)
    for row in lines:
        t.append(int(row[1])/1000)
        fc_tmp.append(float(row[4]))
        bmp_press.append(float(row[53]) / 100.0)
        bmp_tmp.append(float(row[52]))
        N2_press.append(float(row[54]))
        fuel_press.append(float(row[55]))
        lox_press.append(float(row[56]))
        lox_inj_press.append(float(row[58]))
        fuel_inj_press.append(float(row[59]))
        chamber_press.append(float(row[60]))

bmp_alt = []
P0 = bmp_press[100]
T0 = 288.15
L = 0.0065
g = 9.80665
R = 287.05
for i in range(len(bmp_press)):
    if bmp_press[i] < 100:
        bmp_press[i] = P0
    bmp_alt.append((T0 / L) * (1 - pow(bmp_press[i] / P0, R * L / g)))


lox_press[55550] = 10.2297


#fig5, ax5 = plt.subplots()
#ax5.set_xlabel('time (s)')
#ax5.set_ylabel('AGL altitude (m)')
#ax5.plot(t, bmp_alt)
#fig5.suptitle('BMP390')

fig6, ax7 = plt.subplots()
ax7.set_xlabel('Time [s]')
ax7.set_ylabel('Temperature [°C]')
ax7.plot(t, fc_tmp, label='raw_fc_temp')
ax7.plot(t, bmp_tmp, label='raw_amb_temp')
ax7.legend()
ax7.grid()
fig6.suptitle('Flight Computer temperature VS ambient temperature\nIn nominal operation')

#bigfig, axalt = plt.subplots()
#color = 'k'
#axalt.set_xlabel('time (s)')
#axalt.set_ylabel('AGL altitude (m)', color=color)
#axalt.plot(t, bmp_alt, color=color)
#axalt.tick_params(axis='y', labelcolor=color)
#axalt.grid(True, linestyle='--')

#axacc = axalt.twinx()

#color = 'tab:red'
#axacc.set_ylabel('acceleration (m/s^2)', color=color)
#axacc.plot(t, bmi1_z, color=color)
#axacc.tick_params(axis='y', colors=color)
#axacc.spines["right"].set_position(("axes", 1.2))

#fig3, ax6 = plt.subplots()
#ax6.set_xlabel('timestamp')
#ax6.set_ylabel('HPB voltage (V)')
#ax6.plot(t, ina_hpb)
#fig3.suptitle('High Power Battery Voltage')

fig, ax = plt.subplots()
ax.set_xlabel('time [s]')
ax.set_ylabel('Pressure [bar]')
ax.plot(t, N2_press, label='N2', color='g')
ax.plot(t, fuel_press, label='Fuel', color='r')
ax.plot(t, lox_press, label='LOx', color='b')
#ax.plot(t, fuel_inj_press, '--b', label='Fuel_inj')
#ax.plot(t, lox_inj_press, '--r', label='LOx_inj')
#ax.plot(t, chamber_press, color='orange', linestyle='--')
ax.legend()
ax.grid()
fig.suptitle('Dry Run 03/09\nTanks Pressure over time')

plt.show()
