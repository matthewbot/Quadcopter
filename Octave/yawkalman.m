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
	0.005*DT; # angle
	0.005;    # vel
	0.0001;  # veloffset (the gyro's rate of change of drift is pretty low, even though it builds up over time)
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
	.0001, 0; # compass
	0, 0.005; # gyro
]; 

global mags;
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
		10, 0, 0;
		0, 10, 0;
		0, 0, 10;
	];

	# --- Processing ---

	# clear logging variables

	global mags = zeros(samplecount, 1);
	global angles = zeros(samplecount, 1);
	global gyroangles = zeros(samplecount, 1);
	cur_gyroangle = 0;

	for samplenum = 1:samplecount
		# calculate a sample
		sample = samples(samplenum,1:2);
		mag = sample(1);
		gyro = sample(2); 
		z = [mag; gyro]; 
	
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
		mags(samplenum) = mag;
		angles(samplenum) = x(1);
		cur_gyroangle += gyro*DT;
		gyroangles(samplenum) = cur_gyroangle;
	endfor
endfunction

load "yawsamples.dat" samples;
samplecount = size(samples,1);
kalmanfilter(samples, F, Q, H, R, DT)

figure(1);
plot(mags, 'r', angles, 'b', gyroangles, 'g');
title("Yaw test")
xlabel("Sample (200 samples/sec)")
ylabel("Roll (rad)")
print -dsvg "-S3000,800" recovery.svg


