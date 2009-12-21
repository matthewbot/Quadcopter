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
	0.01*DT; # angle
	0.01;    # vel
	0.0005;  # veloffset (the gyro's rate of change of drift is pretty low, even though it builds up over time)
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
	.001, 0; # accel
	0, 0.005; # gyro
]; 

global accels;
global angles;
global gyroangles;

function kalmanfilter(samples, F, Q, H, R, DT)
	samplecount = size(samples,1);

	# state vector

	x = [
		0; # angle
		0;    # vel
		0;    # veloffset
	];

	# covariance matrix

	P = [
		1, 0, 0;
		0, 1, 0;
		0, 0, 1;
	];

	# --- Processing ---

	# clear logging variables

	global accels = zeros(samplecount, 1);
	global angles = zeros(samplecount, 1);
	global gyroangles = zeros(samplecount, 1);
	cur_gyroangle = 0;

	for samplenum = 1:samplecount
		# calculate a sample
		sample = samples(samplenum,1:3);
		accel = atan2(sample(3), sample(2))+pi/2;
		gyro = sample(1); 
		z = [accel; gyro]; 
	
		# Kalman prediction phase
		x_pred = F*x;
		P_pred = F*P*transpose(F) + Q;
	
		# Kalman correction phase
		y = z - H*x_pred;
		S = H*P_pred*transpose(H) + R;
		K = P_pred*transpose(H)*inv(S);
		x_est = x_pred + K*y;
		P_est = (eye(3) - K*H)*P_pred;
	
		# update
		x = x_est;
		P = P_est;
	
		# log for graph
		accels(samplenum) = accel;
		angles(samplenum) = x(1);
		cur_gyroangle += gyro*DT;
		gyroangles(samplenum) = cur_gyroangle;
	endfor
endfunction

load "samples.dat" samples;
samplecount = size(samples,1);
kalmanfilter(samples, F, Q, H, R, DT)

figure(1);
plot(accels, 'r', angles, 'b', gyroangles, 'g');
title("Recovery test")
xlabel("Sample (200 samples/sec)")
ylabel("Roll (rad)")
print -dsvg "-S3000,800" recovery.svg

figure(2);
plot(accels, 'r', angles, 'b')
axis([4800, 5500, -.02, .02])
title("Recovery test - stationary zoom")
xlabel("Sample (200 samples/sec)")
ylabel("Roll (rad)")
print -dsvg "-S3000,800" recoveryzoom.svg

load "tiltsamples.dat" samples;
samplecount = size(samples,1);
kalmanfilter(samples, F, Q, H, R, DT)

figure(3);
plot(accels, 'r', angles, 'b', gyroangles, 'g');
title("Tilt test")
xlabel("Sample (200 samples/sec)")
ylabel("Roll (rad)")
print -dsvg "-S3000,800" tilt.svg


