import struct


# RAD configs 
motorgains = [10000, 100, 4000]
rate = 2000
motmode = 0

pinmode = 3

period = 1000
angles = [1209, 3030, 3485, 3940, 300, 754]

with open("ConfigRADMotor", 'wb') as f:
    data = struct.pack('>iiiHB',*motorgains, rate, motmode)
    f.write(data)

with open("ConfigRADData", 'wb') as f:
    data = struct.pack('>BIHHHHHH',
                       pinmode, period, *angles)
    f.write(data)


# File Manager Configs
maxFileSize = 5000
maxDownlinkSize = 100000

with open("ConfigFMG", 'wb') as f:
    data = struct.pack('>HL',
                       maxFileSize,maxDownlinkSize)
    f.write(data)
  

# GPS FSW Configs  
timeout = 600
timein = 7200

with open("ConfigGPS", 'wb') as f:
    data = struct.pack('>HH',
                       timeout,timein)
    f.write(data)
  
# Scheduler Manager Configs
rebootime = 172800
comtimeout = 900

with open("ConfigSCH", 'wb') as f:
    data = struct.pack('>LL',
                       rebootime,comtimeout)
    f.write(data)


# ACS Configs
SC_I = [0.040438,-0.000117,-0.000077,-0.000117,0.038980,-0.000054,-0.000077,-0.000054,0.008485]
deadband = 0.00005
muThres = 0.15
muBang = 0.1
SPaxis = [1,0,0]
TXaxis = [-0.5, 0, -0.8660]
SCIaxis = [-0.530199, -0.698353, -0.480823]
SCIconstraint = [-0.497697,-0.202773,0.843153]
SCaxis = [0, 1, 0]
ST_weight = 0.175
gyro_weight = 0.005
dMRP = 100 * 0.00000484814
ST_ROTM = [0, 1, 0, 1, 0, 0, 0, 0, -1]
mode_4_time = 10*60000
torquer_pwm_magnitude = 0.3
torquer_x_alignment = -1
torquer_y_alignment = 1
torquer_z_alignment = -1
torquer_x_scramble = 2
torquer_y_scramble = 0
torquer_z_scramble = 1
K_det = [0.0,0.0,0.0]
P_det = [0.006,0.006,0.006]
Ki_det = [0,0,0]
Km_det = 0.0025
K_sun = [0.002,0.002,0.002]
P_sun = [0.005,0.005,0.005]
Ki_sun = [0.002,0.002,0.002]
Km_sun = 0.0025
K_sci = [0.002,0.002,0.002]
P_sci = [0.005,0.005,0.005]
Ki_sci = [0.002,0.002,0.002]
Km_sci = 0.0025
RW_Scale_X = 1.00
RW_Scale_Y = 1.00
RW_Scale_Z = 1.00
GYRO0SLOPECORRECTX = 1.070207058483
GYRO0BIASX = -7.656/1000
GYRO0SLOPECORRECTY = 1.066716067085
GYRO0BIASY = -2.8417/1000
GYRO0SLOPECORRECTZ = 1.00931523895
GYRO0BIASZ = -7.6823/1000
GYRO1SLOPECORRECTX = 1.039465010682
GYRO1BIASX = -2.6721/1000
GYRO1SLOPECORRECTY = 1.0584848111497
GYRO1BIASY = 10.8265/1000
GYRO1SLOPECORRECTZ = 1.03455973715
GYRO1BIASZ = -11.9726/1000
MAG0SLOPECORRECTX = 0.7112
MAG0BIASX = -0.3347
MAG0SLOPECORRECTY = 0.8150
MAG0BIASY = 0.3595
MAG0SLOPECORRECTZ = 0.7769
MAG0BIASZ = 0.7655
MAG1SLOPECORRECTX = 0.8132
MAG1BIASX = -0.6480
MAG1SLOPECORRECTY = 0.8143
MAG1BIASY = 0.1027
MAG1SLOPECORRECTZ = 0.7862
MAG1BIASZ = 0.7165
gyro0_weight = 1.00
gyro1_weight = 1.00
mag0_weight = 1.00
mag1_weight = 1.00
anti_windup_time = 5*60*1000
i_Genc = 0.080
i_Scic = 0.2  

with open("ConfigACS", 'wb') as f:
    data = struct.pack('>dddddddddddddddddddddddddddddddddddddddLdccccccdddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddLdd',
                       SC_I,deadband,muThres,muBang,SPaxis,TXaxis,SCIaxis,SCIconstraint,SCaxis,ST_weight,gyro_weight,dMRP,ST_ROTM,mode_4_time,torquer_pwm_magnitude,torquer_x_alignment,torquer_y_alignment,torquer_z_alignment,torquer_x_scramble,torquer_y_scramble,torquer_z_scramble,K_det,P_det,Ki_det,Km_det,K_sun,P_sun,Ki_sun,Km_sun,K_sci,P_sci,Ki_sci,Km_sci,RW_Scale_X,RW_Scale_Y,RW_Scale_Z,GYRO0SLOPECORRECTX,GYRO0BIASX,GYRO0SLOPECORRECTY,GYRO0BIASY,GYRO0SLOPECORRECTZ,GYRO0BIASZ,GYRO1SLOPECORRECTX,GYRO1BIASX,GYRO1SLOPECORRECTY,GYRO1BIASY,GYRO1SLOPECORRECTZ,GYRO1BIASZ,MAG0SLOPECORRECTX,MAG0BIASX,MAG0SLOPECORRECTY,MAG0BIASY,MAG0SLOPECORRECTZ,MAG0BIASZ,MAG1SLOPECORRECTX,MAG1BIASX,MAG1SLOPECORRECTY,MAG1BIASY,MAG1SLOPECORRECTZ,MAG1BIASZ,gyro0_weight,gyro1_weight,mag0_weight,mag1_weight,anti_windup_time,i_Genc,i_Scic,)
    f.write(data)
          
