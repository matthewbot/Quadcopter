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

accels = zeros(samplecount, 1);
angles = zeros(samplecount, 1);
gyroangles = zeros(samplecount, 1);
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

