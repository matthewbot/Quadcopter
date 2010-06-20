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

Qvec = [1E-5; 8E-3; 5E-4];

Q = Qvec * transpose(Qvec);
for i=1:2
	Q(i, 3) = 0;
	Q(3, i) = 0;
endfor

# observation model
# or: how our state maps to an observation
# observation = H * state

H = [
	1, 0, 0;  # accel = angle
	0, 1, 1; # gyro = vel + veloffset
];

# observation covariance
# or: how much our sensors are expected to deviate from reality

R = [
	6.7716e-4, -4.8451e-4; # accel
	-4.8451e-4, 3.9255e-3; # gyro
]; 

### Generate Graphs ###

#load "samples.dat" samples;
rollpitchkalmanfilter;

figure(1);
plot(accels, 'r', angles, 'b', gyroangles, 'g', intveloffsets, 'c');
title("Airborne roll test")
xlabel("Sample (200hz)")
ylabel("Roll (rad)")
axis([0, samplecount, -0.5, 0.5])
print -dpng "-S600,500" roll.png

figure(2);
plot(gyros, 'r', vels, 'b', veloffsets, 'g');
xlabel("Sample (200hz)")
ylabel("Roll velocity (rad/sec)")
axis([0, samplecount, -1, 1])


