# Kalman Filter
# Uses variable names as described on http://en.wikipedia.org/wiki/Kalman_filter

# --- Constants ---

# state vector is [angle; vel; veloffset]
# observation vector is [angle; vel]

global DT = 5/1000;

# state transition model
# or: how the state is expected to change over time
# futurestate = F * state

global F = [
	1, DT, 0; # angle = angle + vel*dt
	0, 1, 0;  # vel = vel
	0, 0, 1;  # veloffset = veloffset
];

# state covariance
# or: how much is the actual state expected to deviate from the above predictions

Qvec = [
	0.00001; # angle
	0.01;    # vel
	0.0001;  # veloffset (the gyro's rate of change of drift is pretty low, even though it builds up over time)
];
global Q = Qvec*transpose(Qvec); 

# observation model
# or: how our state maps to an observation
# observation = H * state

global H = [
	1, 0, 0;  # accel = angle
	0, 1, -1; # gyro = vel - veloffset
];

# observation covariance
# or: how much our sensors are expected to deviate from reality

global R = [
	.001, 0; # accel
	0, 0.001; # gyro
]; 

### Generate Graphs ###

load "rollsamples.dat" samples;
rollpitchkalmanfilter;

figure(1);
plot(accels, 'r', angles, 'b', gyroangles, 'g');
title("Recovery test")
xlabel("Sample (200 samples/sec)")
ylabel("Roll (rad)")
axis([0, samplecount, -.75, .75])
print -dsvg "-S3000,800" roll.svg

figure(2);
plot(accels, 'r', angles, 'b', gyroangles, 'g');
title("Zoom on motion")
xlabel("Sample (200 samples/sec)")
ylabel("Roll (rad)")
axis([1900, 2600, -.5, .5])
print -dsvg "-S3000,800" roll_motion.svg

