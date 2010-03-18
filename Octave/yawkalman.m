# Kalman Filter
# Uses variable names as described on http://en.wikipedia.org/wiki/Kalman_filter

# --- Constants ---

# state vector is [angle; vel; veloffset]
# observation vector is [angle; vel]

DT = 5/1000;

# state transition model
# or: how the state is expected to change over time
# futurestate = F * state

F = [
	1, DT, 0; # angle = angle + vel*dt
	0, 1, 0;  # vel = vel
	0, 0, 1;  # veloffset = veloffset
];

# state covariance
# or: how much is the actual state expected to deviate from the above predictions

Qvec = [
	0.00001; # angle
	0.001;    # vel
	0.00001;  # veloffset (the gyro's rate of change of drift is pretty low, even though it builds up over time)
];
Q = Qvec*transpose(Qvec); 

# observation model
# or: how our state maps to an observation
# observation = H * state

H = [
	1, 0, 0;  # compass = angle
	0, 1, -1; # gyro = vel - veloffset
];

# observation covariance
# or: how much our sensors are expected to deviate from reality

R = [
	1, 0; # compass
	0, 0.0001; # gyro
]; 

### Generate Graphs ###

load "yaw.dat" samples;
samplecount = size(samples,1);
yawkalmanfilter;

figure(1);
plot(mags, 'r', angles, 'b', gyroangles, 'g');
title("Yaw test")
xlabel("Sample (200 samples/sec)")
ylabel("Roll (rad)")
axis([0, samplecount, -3, 3])
print -dsvg "-S3000,800" yaw.svg


