close all;
clear all;
data = load_data('../sample_data/data1.txt');


t = (data.t - data.t(1)) / 1e9;  % convert to seconds

plot_3x1_1(t, [data.p_x - data.p_x(1), ...
    data.p_y - data.p_y(1), ...
    data.p_z - data.p_z(1)]', ...
    'Relative Position')
plot_3x1_1(t, [data.p_x, data.p_y, data.p_z]', 'Position')
plot_3x1_1(t, [data.v_x, data.v_y, data.v_z]', 'Velocity')
plot_3x1_1(t, [data.a_x, data.a_y, data.a_z]', 'Acceleration')
