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
	1, 0, 0;  # accel = angle
	0, 1, -1; # gyro = vel - veloffset
];

# observation covariance
# or: how much our sensors are expected to deviate from reality

R = [
	.0005, 0; # accel
	0, 0.001; # gyro
]; 

### Generate Graphs ###

load "samples.dat" samples;
rollpitchkalmanfilter;

figure(1);
plot(accels, 'r', spencer(accels), 'y', angles, 'b', gyroangles, 'g');
title("Airborne roll test")
xlabel("Sample (200hz)")
ylabel("Roll (rad)")
axis([0, samplecount, -0.5, 0.5])
print -dpng "-S600,500" roll.png

figure(2);
plot(accels, 'r', spencer(accels), 'y', angles, 'b', gyroangles, 'g');
title("Settling")
xlabel("Sample (200 samples/sec)")
ylabel("Roll (rad)")
axis([2200, samplecount, -.1, .1])
print -dsvg "-S500,250" roll_settling.svg

